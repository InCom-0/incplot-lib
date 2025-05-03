#pragma once

#include <algorithm>
#include <concepts>
#include <cstdarg>
#include <cstddef>
#include <expected>
#include <print>

#include <csv2/reader.hpp>
#include <nlohmann/json.hpp>

#include <incplot/datastore.hpp>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


namespace incom {
namespace terminal_plot {
// Encapsulates parsing of the input into DataStore
// Validates 'hard' errors during parsing
// Validates that input data is not structured 'impossibly' (missing values, different value names per record, etc.)
class Parser {
    enum class input_t {
        NDJSON,
        JSON,
        CSV,
        TSV
    };
    enum class Unexp_parser {
        malformatted_array_like,
        braceCountDoesntMatch,
        braceCountDoesntMatchNLcount,
        parsedNDJSONisEmpty,
        JSONObjectsNotOfSameSize,
        valueTypeInsideJSONdoesntMatch,
        keyNameInsideJSONdoesntMatch,
        JSON_empty,
        JSON_topLevelEleNotArrayOrObject,
        JSONunhandledType,
        ndjsonNotFlat,
        CSV_headerHasMoreItemsThanDataRow,
        CSV_headerHasLessItemsThanDataRow,
        CSV_cellTypeIsDifferentThanExpected,
    };

    enum class csvCellType {
        double_like,
        ll_like,
        string_like
    };

    using NLMjson = nlohmann::ordered_json;

    // HLPRS
    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static std::string_view get_trimmedSV(T const &stringLike) {
        return std::string_view(stringLike.begin(),
                                stringLike.end() -
                                    (std::ranges::find_if_not(stringLike.rbegin(), stringLike.rend(),
                                                              [](auto &&chr) { return (chr == '\n' || chr == ' '); }) -
                                     stringLike.rbegin()));
    }

    static bool validate_jsonSameness(std::vector<NLMjson> const &jsonVec) {
        // Validate that all the JSON objects parsed above have the same structure
        for (auto const &js : std::views::drop(jsonVec, 1)) {
            if (not validate_jsonSameness(js, jsonVec.front())) { return false; }
        }
        return true;
    }

    static bool validate_jsonSameness(NLMjson const &json_A, NLMjson const &json_B) {
        if (json_A.size() != json_B.size()) { return false; }

        auto json_A_IT = json_A.items().begin();
        auto json_B_IT = json_B.items().begin();
        for (size_t i = 0; i < json_A.size(); ++i) {
            if (json_A_IT.key() != json_B_IT.key()) { return false; }
            if (json_B_IT.value().type() != json_B_IT.value().type()) { return false; }
            json_A_IT++, json_B_IT++;
        }
        return true;
    }

    static csvCellType assess_cellType(auto const &csvCell) { return csvCellType::string_like; }

    static double conv_cellToDouble(auto const &csvCell) {
        std::string_view rv    = csvCell.read_view();
        const char      *first = rv.data();
        char            *next{};
        errno = 0;

        double res = std::strtod(first, &next);
        if (next != rv.end()) {
            std::print("{}\n{}{}\n", "Error in assess_cellType, some characters left unparsed",
                       "Encountered while parsing: ", rv);
            errno = 0;
            std::exit(1);
        }
        else if (errno == ERANGE) {
            std::print("{}\n{}{}\n", "Error in assess_cellType, range error in std::strtod",
                       "Encountered while parsing: ", rv);
            errno = 0;
            std::exit(1);
        }
        return res;
    }
    static long long conv_cellToLongLong(auto const &csvCell) {
        std::string_view rv    = csvCell.read_view();
        const char      *first = rv.data();
        char            *next{};
        errno = 0;

        long long res = std::strtoll(first, &next, 10);
        if (next != rv.end()) {
            std::print("{}\n{}{}\n", "Error in assess_cellType, some characters left unparsed",
                       "Encountered while parsing: ", rv);
            errno = 0;
            std::exit(1);
        }
        else if (errno == ERANGE) {
            std::print("{}\n{}{}\n", "Error in assess_cellType, range error in std::strtod",
                       "Encountered while parsing: ", rv);
            errno = 0;
            std::exit(1);
        }
        return res;
    }
    static std::string conv_cellToString(auto const &csvCell) { return std::string(csvCell.read_view()); }


    // COMPOSITION METHODS
    // TODO: Maybe make this public for later use?
    static std::expected<input_t, Unexp_parser> assess_inputType(std::string_view const &sv) {
        size_t begBrcCount = 0, endBrcCount = 0;
        for (auto it = sv.begin(); it != sv.end(); ++it) {
            if (*it == '{') { begBrcCount++; }
            else { break; }
        }
        for (auto it = sv.rbegin(); it != sv.rend(); ++it) {
            if (*it == '}') { endBrcCount++; }
            else { break; }
        }

        std::tuple arr(1, 2, 3, 4, 5);

        // '{', '}', '\n', ',', '\t'
        auto count_symbols =
            std::ranges::fold_left(sv, std::tuple(0uz, 0uz, 0uz, 0uz, 0uz), [](auto &&accu, auto const &a) {
                std::get<0>(accu) += (a == '{');
                std::get<1>(accu) += (a == '}');
                std::get<2>(accu) += (a == '\n');
                std::get<3>(accu) += (a == ',');
                std::get<4>(accu) += (a == '\t');
                return accu;
            });

        // CSV / TSV?
        if ((std::get<3>(count_symbols) % (std::get<2>(count_symbols) + 1)) == 0) {
            // No brace at the beginning ... not idea but good enough
            // TODO: Possibly improve this heuristic
            if (sv.front() != '{' && sv.front() != '[') { return input_t::CSV; }
        }

        // This heuristic shoud be fine as '\t' is hardly ever used in contexts outside of TSV
        else if ((std::get<4>(count_symbols) % (std::get<2>(count_symbols) + 1)) == 0) {
            if (sv.front() != '{' && sv.front() != '[') { return input_t::TSV; }
        }

        // JSON / NDJSON
        if (sv.front() == '[') {
            if (sv.back() == ']') { return input_t::JSON; }
            else { return std::unexpected(Unexp_parser::malformatted_array_like); }
        }
        else if (sv.back() == ']') { return std::unexpected(Unexp_parser::malformatted_array_like); }
        else if (begBrcCount == endBrcCount && begBrcCount == 1) {

            if (std::get<0>(count_symbols) != std::get<1>(count_symbols)) {
                return std::unexpected(Unexp_parser::braceCountDoesntMatch);
            }
            else if (std::get<0>(count_symbols) != (std::get<2>(count_symbols) + 1)) {
                return std::unexpected(Unexp_parser::braceCountDoesntMatchNLcount);
            }
            else { return input_t::NDJSON; }
        }
        else if (begBrcCount == endBrcCount && begBrcCount > 1) { return input_t::JSON; }
        else { return std::unexpected(Unexp_parser::ndjsonNotFlat); }
    }

    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> dispatch_toParsers(input_t const          &inp_t,
                                                                                         std::string_view const &sv) {
        switch (inp_t) {
            case input_t::NDJSON: return parse_NDJSON(sv);
            case input_t::JSON:   return parse_JSON(sv);
            case input_t::CSV:    return parse_CSV(sv);
            case input_t::TSV:    return parse_TSV(sv);
            default:              std::unreachable();
        }
        std::unreachable();
    }


    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> create_fromVerifiedJSON(
        auto const &someJsonStructure) {
        return std::unexpected(Unexp_parser::JSON_empty);
    }

    // PARSE USING RANAV::CSV2
    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_usingCSV2(auto                 &&csv2Reader,
                                                                                      std::string_view const trimmed) {

        if (not csv2Reader.parse_view(trimmed)) {
            std::print("{}\n", "Parser error when parsing as CSV, please check input formatting.");
            std::exit(1);
        }

        // Header 'size' for verification later
        size_t hdr_sz = 0;
        for (auto const &hdrItem : csv2Reader.header()) { hdr_sz++; }


        DataStore::vec_pr_strVarVec_t res;
        std::vector<csvCellType>      cellTypes;
        if (csv2Reader.rows() < 1) { return std::unexpected(Unexp_parser::JSON_empty); }

        // Set US locale for use in parsing CSV
        std::string orig_loc = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, "en_US.UTF-8");

        // Use first row to initialize the 'DataStore::vec_pr_strVarVec_t' data structure
        // Enclosed because not so nice iterator and id 'leakage'
        {
            auto   headerItem = csv2Reader.header().begin();
            size_t id         = 0;
            
            for (auto const &firstRow : csv2Reader) {
                for (auto const &cell : firstRow) {
                    if (not(id < hdr_sz)) { return std::unexpected(Unexp_parser::CSV_headerHasLessItemsThanDataRow); }

                    switch (assess_cellType(cell)) {
                        case csvCellType::double_like:
                            res.push_back(std::make_pair(
                                std::string((*headerItem).read_view()),
                                DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<double>())));
                            cellTypes.push_back(csvCellType::double_like);
                            break;

                        case csvCellType::ll_like:
                            res.push_back(std::make_pair(
                                std::string((*headerItem).read_view()),
                                DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<long long>())));
                            cellTypes.push_back(csvCellType::ll_like);
                            break;

                        case csvCellType::string_like:
                            res.push_back(std::make_pair(
                                std::string((*headerItem).read_view()),
                                DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<std::string>())));
                            cellTypes.push_back(csvCellType::string_like);
                            break;
                        default: std::unreachable();
                    }
                    id++;
                    ++headerItem;
                }
                if (id != hdr_sz) { return std::unexpected(Unexp_parser::CSV_headerHasMoreItemsThanDataRow); }
                // Taking just the first row
                // TODO: This is wierd ... need to look into what I am doing wrong
                break;
            }
        }

        for (auto const &row : csv2Reader) {
            size_t i = 0;
            for (auto const &cell : row) {
                if (not(i < hdr_sz)) { return std::unexpected(Unexp_parser::CSV_headerHasLessItemsThanDataRow); }
                if (assess_cellType(cell) != cellTypes[i]) {
                    return std::unexpected(Unexp_parser::CSV_cellTypeIsDifferentThanExpected);
                }

                auto vis = [&](auto variVec) -> void {
                    // Selecting the right conversion based on the type inside the variant
                    if constexpr (std::same_as<std::decay_t<decltype(variVec)>, std::vector<double>>) {
                        variVec.push_back(conv_cellToDouble(cell));
                    }
                    else if constexpr (std::same_as<std::decay_t<decltype(variVec)>, std::vector<long long>>) {
                        variVec.push_back(conv_cellToLongLong(cell));
                    }
                    else if constexpr (std::same_as<std::decay_t<decltype(variVec)>, std::vector<std::string>>) {
                        variVec.push_back(conv_cellToString(cell));
                    }
                    // This shoudl be impossible to instantiate
                    else { static_assert(false); };
                };

                std::visit(vis, res.at(i).second);
                ++i;
            }
            if (i != hdr_sz) { return std::unexpected(Unexp_parser::CSV_headerHasMoreItemsThanDataRow); }
        }

        // Restore original locale so that we 'clean up' after ourselves
        std::setlocale(LC_ALL, orig_loc.c_str());
        return res;
    }


public:
    // MAIN INTENDED INTERFACE METHOD
    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static std::expected<DataStore, Unexp_parser> parse(T const &stringLike) {
        std::string_view const trimmed = get_trimmedSV(stringLike);

        auto d_tprsrs = [&](auto const &&inp_t) { return dispatch_toParsers(inp_t, trimmed); };
        auto c_dstr   = [&](auto const &&data) { return DataStore(data); };

        return assess_inputType(trimmed).and_then(d_tprsrs).transform(c_dstr);
    }


    // JSON AND NDJSON - RELATED

    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_NDJSON(std::string_view const &trimmed) {

        std::vector<NLMjson> parsed;
        for (auto const &oneLine : std::views::split(trimmed, '\n') |
                                       std::views::transform([](auto const &in) { return std::string_view(in); })) {
            NLMjson oneLineJson;
            try {
                oneLineJson = NLMjson::parse(oneLine);
            }
            catch (const NLMjson::exception &e) {
                // TODO: Finally figure out how to handle exceptions somewhat professionally
                std::print("{}\n", e.what());
                std::exit(1);
            }
            // TODO: Consider if and how we need to flatten each line of NDJSON
            // oneLineJson = oneLineJson.flatten();
            parsed.push_back(std::move(oneLineJson));
        }

        if (parsed.size() == 0) { return std::unexpected(Unexp_parser::parsedNDJSONisEmpty); }


        // Construct 'vec_pr_strVarVec_t' based on the first line
        DataStore::vec_pr_strVarVec_t res;
        std::vector<NLMjson::value_t> temp_firstLineTypes;

        for (auto const &[key, val] : parsed.front().items()) {
            if (val.type() == NLMjson::value_t::string) {
                res.push_back(std::make_pair(
                    key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<std::string>())));
            }
            else if (val.type() == NLMjson::value_t::number_float) {
                res.push_back(
                    std::make_pair(key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<double>())));
            }
            else if (val.type() == NLMjson::value_t::number_integer ||
                     val.type() == NLMjson::value_t::number_unsigned) {
                res.push_back(std::make_pair(
                    key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<long long>())));
            }
            else {}
            temp_firstLineTypes.push_back(val.type());
        }

        for (auto const &parsedLine : parsed) {
            if (parsedLine.size() != res.size()) { return std::unexpected(Unexp_parser::JSONObjectsNotOfSameSize); }
            for (int i = 0; auto const &[key, val] : parsedLine.items()) {
                if (key != res[i].first) { return std::unexpected(Unexp_parser::keyNameInsideJSONdoesntMatch); }
                // TODO: Problem with type checking different NDJSON lines
                else if (val.type() != temp_firstLineTypes[i] && (val.type() == NLMjson::value_t::string ||
                                                                  temp_firstLineTypes[i] == NLMjson::value_t::string)) {
                    return std::unexpected(Unexp_parser::valueTypeInsideJSONdoesntMatch);
                }
                else {
                    switch (res[i].second.index()) {
                        case 0uz: std::get<0>(res[i].second).push_back(val); break;
                        case 1uz: std::get<1>(res[i].second).push_back(val); break;
                        case 2uz: std::get<2>(res[i].second).push_back(val); break;
                        default:  std::unreachable();
                    }
                }
                ++i;
            }
        }

        return res;
    }

    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_JSON(std::string_view const &trimmed) {

        NLMjson wholeJson;
        try {
            wholeJson = NLMjson::parse(trimmed);
        }
        catch (const NLMjson::exception &e) {
            // TODO: Finally figure out how to handle exceptions somewhat professionally
            std::print("{}\n", e.what());
            std::exit(1);
        }

        // UNEXP CHECKS
        if (wholeJson.empty()) { return std::unexpected(Unexp_parser::JSON_empty); }
        if (not wholeJson.is_structured()) { return std::unexpected(Unexp_parser::JSON_topLevelEleNotArrayOrObject); }
        if (wholeJson.items().begin().value().empty()) { return std::unexpected(Unexp_parser::JSON_empty); }

        // The 'second level' is structured
        if (wholeJson.items().begin().value().is_structured()) {
            if (std::ranges::any_of(wholeJson.items().begin().value().items(),
                                    [](auto const &a) { return a.value().is_structured(); })) {
                // Flatten any 'structuring on the 'third level' down
                for (auto &oneRecord : wholeJson) {
                    for (auto &oneItemInRecord : oneRecord) {
                        if (oneItemInRecord.is_structured()) { oneItemInRecord = oneItemInRecord.flatten(); }
                    }
                }
            }

            DataStore::vec_pr_strVarVec_t res;
            std::vector<NLMjson::value_t> temp_firstLineTypes;

            // First 'record' determines the structure of each record.
            for (auto const &[key, val] : wholeJson.items().begin().value().items()) {
                if (val.type() == NLMjson::value_t::string) {
                    res.push_back(std::make_pair(
                        key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<std::string>())));
                }
                else if (val.type() == NLMjson::value_t::number_float) {
                    res.push_back(std::make_pair(
                        key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<double>())));
                }
                else if (val.type() == NLMjson::value_t::number_integer ||
                         val.type() == NLMjson::value_t::number_unsigned) {
                    res.push_back(std::make_pair(
                        key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<long long>())));
                }
                else {}
                temp_firstLineTypes.push_back(val.type());
            }

            for (auto const &[key_l1, val_l1] : wholeJson.items()) {
                if (val_l1.size() != res.size()) { return std::unexpected(Unexp_parser::JSONObjectsNotOfSameSize); }

                for (int i = 0; auto const &[key, val] : val_l1.items()) {
                    // CHECKS
                    // Check keys are the same and valTypes are the same on (flattened) 3rd level
                    if (key != res[i].first) { return std::unexpected(Unexp_parser::keyNameInsideJSONdoesntMatch); }
                    // TODO: Problem with type checking different NDJSON lines
                    else if (val.type() != temp_firstLineTypes[i] &&
                             (val.type() == NLMjson::value_t::string ||
                              temp_firstLineTypes[i] == NLMjson::value_t::string)) {
                        return std::unexpected(Unexp_parser::valueTypeInsideJSONdoesntMatch);
                    }
                    // 'CORRECT' PATH
                    else {
                        std::visit([&](auto &vari) { vari.push_back(val); }, res[i].second);
                    }
                    ++i;
                }
            }

            return res;
        }
        // The 'second level' isn't structured ... that is it is actually just one column of values
        else {
            for (auto &oneItem : std::views::drop(wholeJson, 1)) {
                // If any JSON type on 'second level' doesn't match the type of the first record
                if (oneItem.type() != wholeJson.items().begin().value().type()) {
                    return std::unexpected(Unexp_parser::valueTypeInsideJSONdoesntMatch);
                }
            }

            DataStore::vec_pr_strVarVec_t res;
            std::vector<NLMjson::value_t> temp_firstLineTypes;

            // First 'record' determines the structure of each record.
            for (auto const &[key, val] : std::views::take(wholeJson.items(), 1)) {
                if (val.type() == NLMjson::value_t::string) {
                    res.push_back(std::make_pair(
                        key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<std::string>())));
                }
                else if (val.type() == NLMjson::value_t::number_float) {
                    res.push_back(std::make_pair(
                        key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<double>())));
                }
                else if (val.type() == NLMjson::value_t::number_integer ||
                         val.type() == NLMjson::value_t::number_unsigned) {
                    res.push_back(std::make_pair(
                        key, DataStore::vec_pr_strVarVec_t::value_type::second_type(std::vector<long long>())));
                }
                else { return std::unexpected(Unexp_parser::JSONunhandledType); }
                temp_firstLineTypes.push_back(val.type());
            }

            auto &resFront = res.at(0);
            for (auto const &[key_l1, val_l1] : wholeJson.items()) {
                if (val_l1.size() != res.size()) { return std::unexpected(Unexp_parser::JSONObjectsNotOfSameSize); }

                // CHECKS
                // Check keys are the same and valTypes are the same on (flattened) 3rd level
                if (wholeJson.is_object()) {
                    if (key_l1 != resFront.first) {
                        return std::unexpected(Unexp_parser::keyNameInsideJSONdoesntMatch);
                    }
                }

                // TODO: Problem with type checking different NDJSON lines
                else if (val_l1.type() != wholeJson.at(0).type()) {
                    return std::unexpected(Unexp_parser::valueTypeInsideJSONdoesntMatch);
                }

                // 'CORRECT' PATH
                else {
                    std::visit([&](auto &vari) { vari.push_back(val_l1); }, resFront.second);
                }
            }

            return res;
        }
        std::unreachable();
    }


    template <typename T>
    requires std::is_convertible_v<typename T::value_type, std::string_view>
    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_NDJSON(T containerOfStringLike) {

        for (auto &oneStr : containerOfStringLike) {
            while (oneStr.back() == '\n') { oneStr.popback(); }
        }


        return DataStore::vec_pr_strVarVec_t();
    }

    // CSV AND TSV - RELATED
    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_CSV(std::string_view const sv_like) {
        return parse_usingCSV2(csv2::Reader<csv2::delimiter<','>, csv2::quote_character<'"'>,
                                            csv2::first_row_is_header<true>, csv2::trim_policy::trim_whitespace>{},
                               sv_like);
    }

    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_TSV(std::string_view const sv_like) {
        return parse_usingCSV2(csv2::Reader<csv2::delimiter<','>, csv2::quote_character<'"'>,
                                            csv2::first_row_is_header<true>, csv2::trim_policy::trim_whitespace>{},
                               sv_like);
    }
};
} // namespace terminal_plot
} // namespace incom