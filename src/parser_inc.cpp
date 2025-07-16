#include <algorithm>
#include <cassert>
#include <charconv>
#include <concepts>
#include <expected>
#include <print>

#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <csv2/reader.hpp>
#include <nlohmann/json.hpp>

// #include <cppcoro/filesystem.hpp>
// #include <cppcoro/io_service.hpp>
// #include <cppcoro/read_only_file.hpp>
// #include <cppcoro/static_thread_pool.hpp>
// #include <cppcoro/sync_wait.hpp>
// #include <cppcoro/task.hpp>
// #include <cppcoro/when_all.hpp>

#include <incplot/parsers_inc.hpp>

namespace incom {
namespace terminal_plot {
// Encapsulates parsing of the input into DataStore
// Validates 'hard' errors during parsing
// Validates that input data is not structured 'impossibly' (missing values, different value names per record, etc.)
// TODO: Would it be possible to cleave he parser from the library so that it is easier to customize later?
// TODO: How would one do the above in a reasonable manner?
namespace parsers {
using NLMjson  = nlohmann::ordered_json;
using incerr_c = incerr::incerr_code;
using enum Unexp_parser;

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

static bool validate_jsonSameness(std::vector<NLMjson> const &jsonVec) {
    // Validate that all the JSON objects parsed above have the same structure
    for (auto const &js : std::views::drop(jsonVec, 1)) {
        if (not validate_jsonSameness(js, jsonVec.front())) { return false; }
    }
    return true;
}

// 'Trims' a string_view by removing 'new line' and/or '[space]' characters from the end of it
// This is useful in order to sort of 'standardize' structure of input
std::string_view Parser::get_trimmedSV(std::string_view const &sv) {
    return std::string_view(
        sv.begin(),
        sv.end() - (std::ranges::find_if_not(sv.rbegin(), sv.rend(),
                                             [](auto &&chr) { return (chr == '\n' || chr == ' ' || chr == '\r'); }) -
                    sv.rbegin()));
}


CellType Parser::assess_cellType(auto const &csvCell) {
    std::string_view rv = csvCell.read_view();
    if (rv.empty()) { return CellType::null_like; }

    long long llOut = 0ll;
    auto [ptr, _]   = std::from_chars(rv.data(), rv.data() + rv.size(), llOut);

    if (ptr == (rv.data() + rv.size())) { return CellType::ll_like; }

    double dblOut   = 0.0;
    auto [ptr2, _2] = std::from_chars(rv.data(), rv.data() + rv.size(), dblOut);
    if (ptr2 == (rv.data() + rv.size())) { return CellType::double_like; }

    return CellType::string_like;
}

double Parser::conv_cellToDouble(auto const &csvCell) {
    std::string_view rv  = csvCell.read_view();
    double           res = 0.0;

    auto [ptr, ec] = std::from_chars(rv.data(), rv.data() + rv.size(), res);

    if (ec == std::errc()) { return res; }
    else if (ec == std::errc::invalid_argument) {
        std::print("{}\n{}{}\n", "Error in conv_cellToDouble, not convertable to double",
                   "Encountered while parsing: ", rv);
        std::exit(1);
    }
    else if (ec == std::errc::result_out_of_range) {
        std::print("{}\n{}{}\n", "Error in conv_cellToDouble, range error in std::from_chars",
                   "Encountered while parsing: ", rv);
        std::exit(1);
    }
    else { assert(false); }
    std::unreachable();
}
long long Parser::conv_cellToLongLong(auto const &csvCell) {
    std::string_view rv  = csvCell.read_view();
    long long        res = 0.0;

    auto [ptr, ec] = std::from_chars(rv.data(), rv.data() + rv.size(), res);

    if (ec == std::errc()) { return res; }
    else if (ec == std::errc::invalid_argument) {
        std::print("{}\n{}{}\n", "Error in conv_cellToLongLong, not convertable to long long",
                   "Encountered while parsing: ", rv);
        std::exit(1);
    }
    else if (ec == std::errc::result_out_of_range) {
        std::print("{}\n{}{}\n", "Error in conv_cellToLongLong, range error in std::from_chars",
                   "Encountered while parsing: ", rv);
        std::exit(1);
    }
    else { assert(false); }
    std::unreachable();
}
std::string Parser::conv_cellToString(auto const &csvCell) {
    return std::string(csvCell.read_view());
}


// COMPOSITION METHODS
std::expected<input_t, incerr_c> Parser::assess_inputType(std::string_view const &sv) {
    size_t begBrcCount = 0, endBrcCount = 0;

    for (auto it : sv) {
        if (it == '{') { begBrcCount++; }
        else { break; }
    }
    for (auto it : sv | std::views::reverse) {
        if (it == '}') { endBrcCount++; }
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

    if (std::get<2>(count_symbols) == 0) { return std::unexpected(incerr_c::make(CSV_containsZeroNewLineChars)); }

    // CSV / TSV?
    if ((std::get<3>(count_symbols) != 0) && (std::get<3>(count_symbols) % (std::get<2>(count_symbols) + 1)) == 0) {
        // No brace at the beginning ... not idea but good enough
        // TODO: Possibly improve this heuristic
        if (sv.front() != '{' && sv.front() != '[') { return input_t::CSV; }
    }

    // This heuristic shoud be fine as '\t' is hardly ever used in contexts outside of TSV
    else if ((std::get<4>(count_symbols) != 0) &&
             (std::get<4>(count_symbols) % (std::get<2>(count_symbols) + 1)) == 0) {
        if (sv.front() != '{' && sv.front() != '[') { return input_t::TSV; }
    }

    // JSON / NDJSON
    if (sv.front() == '[') {
        if (sv.back() == ']') { return input_t::JSON; }
        else { return std::unexpected(incerr_c::make(JSON_malformattedArrayLike)); }
    }
    else if (sv.back() == ']') { return std::unexpected(incerr_c::make(JSON_malformattedArrayLike)); }
    else if (begBrcCount == endBrcCount && begBrcCount == 1) {

        if (std::get<0>(count_symbols) != std::get<1>(count_symbols)) {
            return std::unexpected(incerr_c::make(NDJSON_braceCountDoesntMatch));
        }
        else if (std::get<0>(count_symbols) != (std::get<2>(count_symbols) + 1)) {
            return std::unexpected(incerr_c::make(NDJSON_braceCountDoesntMatchNLcount));
        }
        else { return input_t::NDJSON; }
    }
    else if (begBrcCount == endBrcCount && begBrcCount > 1) { return input_t::JSON; }
    else { return std::unexpected(incerr_c::make(NDJSON_isNotFlat)); }
}

std::expected<DataStore::DS_CtorObj, incerr_c> Parser::dispatch_toParsers(input_t const          &inp_t,
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

// MAIN INTENDED INTERFACE METHOD
// Dispatches the string_view to the right parser and constructs DataStore
std::expected<DataStore, incerr_c> Parser::parse(std::string_view const sv) {
    std::string_view const trimmed = get_trimmedSV(sv);

    auto c_dstr = [](auto const &&data) { return DataStore(data); };
    return assess_inputType(trimmed).and_then(std::bind_back(dispatch_toParsers, trimmed)).transform(c_dstr);
}

// PARSE USING RANAV::CSV2
std::expected<DataStore::DS_CtorObj, incerr_c> Parser::parse_usingCSV2(auto                 &&csv2Reader,
                                                                       std::string_view const trimmed) {

    if (not csv2Reader.parse_view(trimmed)) { return std::unexpected(incerr_c::make(CSV_parserBackendError)); }

    // Header 'size' for verification later
    size_t hdr_sz = 0;
    for (auto const &hdrItem : csv2Reader.header()) { hdr_sz++; }

    DataStore::DS_CtorObj res;
    std::vector<CellType> cellTypes;
    if (csv2Reader.rows() < 1) { return std::unexpected(incerr_c::make(JSON_isEmpty)); }

    // Set US locale for use in parsing CSV
    std::string orig_loc = std::setlocale(LC_ALL, nullptr);
    std::setlocale(LC_ALL, "en_US.UTF-8");

    {
        using varVec_t    = DataStore::vec_pr_varCol_t::value_type::second_type;
        auto   headerItem = csv2Reader.header().begin();
        size_t id         = 0;

        auto const &rowIter = *csv2Reader.begin();
        // auto const &aaa = (*rowIter);

        for (auto const &firstRow : csv2Reader) {
            for (auto const &cell : firstRow) {
                if (not(id < hdr_sz)) { return std::unexpected(incerr_c::make(CSV_headerHasLessItemsThanDataRow)); }

                switch (assess_cellType(cell)) {
                    case CellType::double_like:
                        res.data.push_back(
                            std::make_pair(std::string((*headerItem).read_view()), varVec_t(std::vector<double>())));
                        cellTypes.push_back(CellType::double_like);
                        break;

                    case CellType::ll_like:
                        res.data.push_back(
                            std::make_pair(std::string((*headerItem).read_view()), varVec_t(std::vector<long long>())));
                        cellTypes.push_back(CellType::ll_like);
                        break;

                    case CellType::string_like:
                        res.data.push_back(std::make_pair(std::string((*headerItem).read_view()),
                                                          varVec_t(std::vector<std::string>())));
                        cellTypes.push_back(CellType::string_like);
                        break;
                    default: return std::unexpected(incerr_c::make(CSV_unhandledCellType));
                }
                res.itemFlags.push_back({});
                ++id;
                ++headerItem;
            }
            if (id != hdr_sz) { return std::unexpected(incerr_c::make(CSV_headerHasMoreItemsThanDataRow)); }
            // Taking just the first row
            // TODO: This is wierd ... looked into it ... seems the only reasonable way lol ... I must be doing
            // something wrong :-)
            break;
        }
    }

    for (auto const &row : csv2Reader) {
        size_t i = 0;
        for (auto const &cell : row) {
            if (not(i < hdr_sz)) { return std::unexpected(incerr_c::make(CSV_headerHasLessItemsThanDataRow)); }
            // Error when not the same type
            // However if trying to parse 'something which looks like long long' into double ... then that's fine

            auto assessed_ct = assess_cellType(cell);
            if (assessed_ct != cellTypes[i] && assessed_ct != CellType::null_like &&
                (not((assessed_ct == CellType::ll_like) && cellTypes[i] == CellType::double_like))) {

                auto testSV = cell.read_view();
                // TODO: Finally fix the problem of 'mixed types' by pre-evaluating the whole data file
                return std::unexpected(incerr_c::make(CSV_valueTypeDoesntMatch));
            }

            auto vis = [&](auto &variVec) -> void {
                // Selecting the right conversion based on the type inside the variant
                if constexpr (std::same_as<std::decay_t<decltype(variVec)>, std::vector<double>>) {
                    if (assessed_ct == CellType::null_like) { variVec.push_back(0.0); }
                    else { variVec.push_back(conv_cellToDouble(cell)); }
                }
                else if constexpr (std::same_as<std::decay_t<decltype(variVec)>, std::vector<long long>>) {
                    if (assessed_ct == CellType::null_like) { variVec.push_back(0ll); }
                    else { variVec.push_back(conv_cellToLongLong(cell)); }
                }
                else if constexpr (std::same_as<std::decay_t<decltype(variVec)>, std::vector<std::string>>) {
                    if (assessed_ct == CellType::null_like) {}
                    variVec.push_back(conv_cellToString(cell));
                }
                // This should be impossible to instantiate
                else { static_assert(false); };

                res.itemFlags[i].push_back(assessed_ct == CellType::null_like ? 0b1 : 0b0);
            };

            std::visit(vis, res.data.at(i).second);
            ++i;
        }
        if (i != hdr_sz) { return std::unexpected(incerr_c::make(CSV_headerHasMoreItemsThanDataRow)); }
    }

    // Restore original locale so that we 'clean up' after ourselves
    std::setlocale(LC_ALL, orig_loc.c_str());
    return res;
}

// JSON AND NDJSON
std::expected<DataStore::DS_CtorObj, incerr_c> Parser::parse_NDJSON(std::string_view const &trimmed) {

    // cppcoro::static_thread_pool threadPool(4);

    // using retType = cppcoro::task<NLMjson>;


    // auto process_chunk_async = [](cppcoro::static_thread_pool &tp, auto const inputData) -> retType {
    //     co_await tp.schedule();
    //     co_return NLMjson::parse(inputData);
    // };

    // auto run = [&](cppcoro::static_thread_pool &tp,
    //                std::string_view const &trimmed) -> cppcoro::task<std::vector<NLMjson>> {
    //     std::vector<retType> tasks;

    //     auto ssv = std::views::split(trimmed, '\n') |
    //                std::views::transform([](auto const &in) { return std::string_view(in); });

    //     for (auto const &item : ssv) { tasks.push_back(process_chunk_async(tp, item)); }
    //     auto aa = co_await cppcoro::when_all(std::move(tasks));

    //     co_return aa;
    // };

    // = cppcoro::sync_wait(run(threadPool, trimmed));

    std::vector<NLMjson> parsed;


    for (auto const &oneLine : std::views::split(trimmed, '\n') |
                                   std::views::transform([](auto const &in) { return std::string_view(in); })) {
        NLMjson oneLineJson;
        try {
            oneLineJson = NLMjson::parse(oneLine);
        }
        catch (const NLMjson::exception &e) {
            // TODO: Finally figure out how to handle exceptions somewhat professionally
            return std::unexpected(incerr_c::make(JSON_parserBackendError));
        }
        // TODO: Consider if and how we need to flatten each line of NDJSON
        // oneLineJson = oneLineJson.flatten();
        parsed.push_back(std::move(oneLineJson));
    }

    if (parsed.size() == 0) { return std::unexpected(incerr_c::make(NDJSON_isEmpty)); }


    // Construct 'vec_pr_strVarVec_t' based on the first line
    DataStore::DS_CtorObj         res;
    std::vector<NLMjson::value_t> temp_firstLineTypes;

    for (auto const &[key, val] : parsed.front().items()) {
        if (val.type() == NLMjson::value_t::string) {
            res.data.push_back(
                std::make_pair(key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<std::string>())));
        }
        else if (val.type() == NLMjson::value_t::number_float) {
            res.data.push_back(
                std::make_pair(key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<double>())));
        }
        else if (val.type() == NLMjson::value_t::number_integer || val.type() == NLMjson::value_t::number_unsigned) {
            res.data.push_back(
                std::make_pair(key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<long long>())));
        }
        else {}
        temp_firstLineTypes.push_back(val.type());
        res.itemFlags.push_back({});
    }

    for (auto const &parsedLine : parsed) {
        if (parsedLine.size() != res.data.size()) { return std::unexpected(incerr_c::make(JSON_objectsNotOfSameSize)); }
        for (int i = 0; auto const &[key, val] : parsedLine.items()) {
            if (key != res.data[i].first) { return std::unexpected(incerr_c::make((JSON_keyNameDoesntMatch))); }
            // TODO: Problem with type checking different NDJSON lines

            bool isNullLike = val.is_null() || (val.is_string() && (val.template get<std::string>() == ""));
            if (isNullLike) { res.itemFlags[i].push_back(0b1); }
            else if (val.type() != temp_firstLineTypes[i] &&
                     (val.type() == NLMjson::value_t::string || temp_firstLineTypes[i] == NLMjson::value_t::string)) {
                return std::unexpected(incerr_c::make(JSON_valueTypeDoesntMatch));
            }
            else { res.itemFlags[i].push_back(0b0); }

            switch (res.data[i].second.index()) {
                case 0uz:
                    std::get<0>(res.data[i].second).push_back(isNullLike ? "" : val.template get<std::string>());
                    break;
                case 1uz:
                    std::get<1>(res.data[i].second).push_back(isNullLike ? 0.0 : val.template get<double>());
                    break;
                case 2uz:
                    std::get<2>(res.data[i].second).push_back(isNullLike ? 0ll : val.template get<long long>());
                    break;
                default: std::unreachable();
            }

            ++i;
        }
    }
    return res;
}
std::expected<DataStore::DS_CtorObj, incerr_c> Parser::parse_JSON(std::string_view const &trimmed) {

    NLMjson wholeJson;
    try {
        wholeJson = NLMjson::parse(trimmed);
    }
    catch (const NLMjson::exception &e) {
        // TODO: Finally figure out how to handle exceptions somewhat professionally
        return std::unexpected(incerr_c::make(JSON_parserBackendError));
    }

    // UNEXP CHECKS
    if (wholeJson.empty()) { return std::unexpected(incerr_c::make(JSON_isEmpty)); }
    if (not wholeJson.is_structured()) { return std::unexpected(incerr_c::make(JSON_topLevelEleNotArrayOrObject)); }
    if (wholeJson.items().begin().value().empty()) { return std::unexpected(incerr_c::make(JSON_isEmpty)); }

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

        DataStore::DS_CtorObj         res;
        std::vector<NLMjson::value_t> temp_firstLineTypes;

        // First 'record' determines the structure of each record.
        for (auto const &[key, val] : wholeJson.items().begin().value().items()) {
            if (val.type() == NLMjson::value_t::string) {
                res.data.push_back(std::make_pair(
                    key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<std::string>())));
            }
            else if (val.type() == NLMjson::value_t::number_float) {
                res.data.push_back(
                    std::make_pair(key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<double>())));
            }
            else if (val.type() == NLMjson::value_t::number_integer ||
                     val.type() == NLMjson::value_t::number_unsigned) {
                res.data.push_back(
                    std::make_pair(key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<long long>())));
            }
            else {}
            temp_firstLineTypes.push_back(val.type());
            res.itemFlags.push_back({});
        }

        for (auto const &[key_l1, val_l1] : wholeJson.items()) {
            if (val_l1.size() != res.data.size()) { return std::unexpected(incerr_c::make(JSON_objectsNotOfSameSize)); }

            for (int i = 0; auto const &[key, val] : val_l1.items()) {
                // CHECKS
                if (key != res.data[i].first) { return std::unexpected(incerr_c::make(JSON_keyNameDoesntMatch)); }

                // Check keys are the same and valTypes are the same on (flattened) 3rd level
                bool isNullLike = val.is_null() || (val.is_string() && (val.template get<std::string>() == ""));
                if (isNullLike) { res.itemFlags[i].push_back(0b1); }
                else if (val.type() != temp_firstLineTypes[i] && (val.type() == NLMjson::value_t::string ||
                                                                  temp_firstLineTypes[i] == NLMjson::value_t::string)) {
                    return std::unexpected(incerr_c::make(JSON_valueTypeDoesntMatch));
                }
                else { res.itemFlags[i].push_back(0b0); }

                // 'CORRECT' PATH
                switch (res.data[i].second.index()) {
                    case 0uz:
                        std::get<0>(res.data[i].second).push_back(isNullLike ? "" : val.template get<std::string>());
                        break;
                    case 1uz:
                        std::get<1>(res.data[i].second).push_back(isNullLike ? 0.0 : val.template get<double>());
                        break;
                    case 2uz:
                        std::get<2>(res.data[i].second).push_back(isNullLike ? 0ll : val.template get<long long>());
                        break;
                    default: assert(false); std::unreachable();
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
            if (oneItem.type() != wholeJson.items().begin().value().type() &&
                not(oneItem.is_null() || (oneItem.is_string() && (oneItem.template get<std::string>() == "")))) {
                return std::unexpected(incerr_c::make(JSON_valueTypeDoesntMatch));
            }
        }
        DataStore::DS_CtorObj         res2;
        std::vector<NLMjson::value_t> temp_firstLineTypes;

        // First 'record' determines the structure of each record.
        for (auto const &[key, val] : std::views::take(wholeJson.items(), 1)) {
            if (val.type() == NLMjson::value_t::string) {
                res2.data.push_back(std::make_pair(
                    key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<std::string>())));
            }
            else if (val.type() == NLMjson::value_t::number_float) {
                res2.data.push_back(
                    std::make_pair(key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<double>())));
            }
            else if (val.type() == NLMjson::value_t::number_integer ||
                     val.type() == NLMjson::value_t::number_unsigned) {
                res2.data.push_back(
                    std::make_pair(key, DataStore::vec_pr_varCol_t::value_type::second_type(std::vector<long long>())));
            }
            else { return std::unexpected(incerr_c::make(JSON_unhandledCellType)); }
            temp_firstLineTypes.push_back(val.type());
            res2.itemFlags.push_back({});
        }

        for (auto const &[key, val] : wholeJson.items()) {
            // CHECKS
            if (key != res2.data[0].first) { return std::unexpected(incerr_c::make(JSON_keyNameDoesntMatch)); }

            // Check keys are the same and valTypes are the same on (flattened) 3rd level
            bool isNullLike = val.is_null() || (val.is_string() && (val.template get<std::string>() == ""));
            if (isNullLike) { res2.itemFlags[0].push_back(0b1); }
            else if (val.type() != temp_firstLineTypes[0] &&
                     (val.type() == NLMjson::value_t::string || temp_firstLineTypes[0] == NLMjson::value_t::string)) {
                return std::unexpected(incerr_c::make(JSON_valueTypeDoesntMatch));
            }
            else { res2.itemFlags[0].push_back(0b0); }

            // 'CORRECT' PATH
            switch (res2.data[0].second.index()) {
                case 0uz:
                    std::get<0>(res2.data[0].second).push_back(isNullLike ? "" : val.template get<std::string>());
                    break;
                case 1uz:
                    std::get<1>(res2.data[0].second).push_back(isNullLike ? 0.0 : val.template get<double>());
                    break;
                case 2uz:
                    std::get<2>(res2.data[0].second).push_back(isNullLike ? 0ll : val.template get<long long>());
                    break;
                default: assert(false); std::unreachable();
            }
        }
        return res2;
    }
    std::unreachable();
}

// CSV AND TSV
std::expected<DataStore::DS_CtorObj, incerr_c> Parser::parse_CSV(std::string_view const sv_like) {
    return parse_usingCSV2(csv2::Reader<csv2::delimiter<','>, csv2::quote_character<'"'>,
                                        csv2::first_row_is_header<true>, csv2::trim_policy::trim_whitespace>{},
                           sv_like);
}

std::expected<DataStore::DS_CtorObj, incerr_c> Parser::parse_TSV(std::string_view const sv_like) {
    return parse_usingCSV2(csv2::Reader<csv2::delimiter<'\t'>, csv2::quote_character<'"'>,
                                        csv2::first_row_is_header<true>, csv2::trim_policy::trim_whitespace>{},
                           sv_like);
}

} // namespace parsers
} // namespace terminal_plot
} // namespace incom