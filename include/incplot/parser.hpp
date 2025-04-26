#pragma once

#include <algorithm>
#include <expected>
#include <print>

#include <csv2/reader.hpp>
#include <nlohmann/json.hpp>

#include <incplot/datastore.hpp>


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
        ndjsonNotFlat
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

        // This heuristic shoud be fine as '\t' is hardly every used in contexts outside of TSV
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

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_NDJSON(T const &sv_like) {

        std::vector<NLMjson> parsed;
        for (auto const &oneLine : std::views::split(sv_like, '\n') |
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
                else if (val.type() != temp_firstLineTypes[i]) {
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


        for (int colID = 0; colID < res.size(); ++colID) {
            size_t curVarIDX = res.at(colID).second.index();

            for (int lineID = 0; lineID < parsed.size(); ++lineID) {
                if (colID >= parsed[lineID].size()) { return std::unexpected(Unexp_parser::JSONObjectsNotOfSameSize); }
                else if ((parsed[lineID].begin() + colID)->type() != parsed.front().begin()->type()) {
                    return std::unexpected(Unexp_parser::valueTypeInsideJSONdoesntMatch);
                }
                else {
                    switch (curVarIDX) {
                        case 0uz: std::get<0>(res[colID].second).push_back(*(parsed[lineID].begin() + colID)); break;
                        case 1uz: std::get<1>(res[colID].second).push_back(*(parsed[lineID].begin() + colID)); break;
                        case 2uz: std::get<2>(res[colID].second).push_back(*(parsed[lineID].begin() + colID)); break;
                        default:  std::unreachable();
                    }
                }
            }
        }

        return res;
    }

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_JSON(T const &sv_like) {

        // Trim input 'string like' of all 'newline' chars at the end
        std::string_view const trimmed(
            sv_like.begin(), sv_like.end() - (std::ranges::find_if_not(sv_like.rbegin(), sv_like.rend(),
                                                                       [](auto &&chr) { return chr == '\n'; }) -
                                              sv_like.rbegin()));

        std::vector<NLMjson> parsed;

        NLMjson wholeJson;
        try {
            wholeJson = NLMjson::parse(trimmed);
        }
        catch (const NLMjson::exception &e) {
            // TODO: Finally figure out how to handle exceptions somewhat professionally
            std::print("{}\n", e.what());
        }
        parsed.push_back(std::move(wholeJson));

        return DataStore::vec_pr_strVarVec_t();
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
        return DataStore::vec_pr_strVarVec_t();
    }

    static std::expected<DataStore::vec_pr_strVarVec_t, Unexp_parser> parse_TSV(std::string_view const sv_like) {
        return DataStore::vec_pr_strVarVec_t();
    }
};
} // namespace terminal_plot
} // namespace incom