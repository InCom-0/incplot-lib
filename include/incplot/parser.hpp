#pragma once

#include <algorithm>
#include <expected>
#include <print>

#include <csv2/reader.hpp>
#include <nlohmann/json.hpp>

#include <incplot/datastore.hpp>
#include <string_view>


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
        ndjsonNotFlat
    };

    using NLMjson = nlohmann::ordered_json;

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static std::string_view get_trimmedSV(T const &stringLike) {
        return std::string_view(stringLike.begin(),
                                stringLike.end() -
                                    (std::ranges::find_if_not(stringLike.rbegin(), stringLike.rend(),
                                                              [](auto &&chr) { return (chr == '\n' || chr == ' '); }) -
                                     stringLike.rbegin()));
    }

    static std::expected<input_t, Unexp_parser> assess_inputType(std::string_view const stringLike) {
        std::string_view trimmed     = get_trimmedSV(stringLike);
        size_t           begBrcCount = 0, endBrcCount = 0;
        for (auto it = trimmed.begin(); it != trimmed.end(); ++it) {
            if (*it == '{') { begBrcCount++; }
            else { break; }
        }
        for (auto it = trimmed.rbegin(); it != trimmed.rend(); ++it) {
            if (*it == '}') { endBrcCount++; }
            else { break; }
        }
        // '{', '}', '\n', ',', '\t'
        auto count_symbols =
            std::ranges::fold_left(trimmed, std::tuple(0uz, 0uz, 0uz), [](auto &&accu, auto const &&a) {
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
            if (trimmed.front() != '{' && trimmed.front() != '[') { return input_t::CSV; }
        }

        // This heuristic shoud be fine as '\t' is hardly every used in contexts outside of TSV
        else if ((std::get<4>(count_symbols) % (std::get<2>(count_symbols) + 1)) == 0) {
            if (trimmed.front() != '{' && trimmed.front() != '[') { return input_t::TSV; }
        }

        // JSON / NDJSON
        if (trimmed.front() == '[') {
            if (trimmed.back() == ']') { return input_t::JSON; }
            else { return std::unexpected(Unexp_parser::malformatted_array_like); }
        }
        else if (trimmed.back() == ']') { return std::unexpected(Unexp_parser::malformatted_array_like); }
        else if (begBrcCount == endBrcCount && begBrcCount == 1) {

            if (std::get<0>(count_symbols) != std::get<1>(count_symbols)) {
                return std::unexpected(Unexp_parser::braceCountDoesntMatch);
            }
            else if (std::get<0>(count_symbols) != (std::get<2>(count_symbols) - 1)) {
                return std::unexpected(Unexp_parser::braceCountDoesntMatchNLcount);
            }
            else { return input_t::NDJSON; }
        }
        else if (begBrcCount == endBrcCount && begBrcCount > 1) { return input_t::JSON; }
        else { return std::unexpected(Unexp_parser::ndjsonNotFlat); }
    }

    static std::expected<DataStore, Unexp_parser> dispatch_toParsers(input_t              &&inp_t,
                                                                     std::string_view const stringLike) {
        switch (inp_t) {
            case input_t::NDJSON: return parse_NDJSON_intoDS(stringLike); break;
            case input_t::JSON:   break;
            case input_t::CSV:    break;
            case input_t::TSV:    break;
        }


        return std::unexpected(Unexp_parser::ndjsonNotFlat);
    }


public:
    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    std::expected<DataStore, Unexp_parser> parse(T const &stringLike) {
        std::string_view trimmed = get_trimmedSV(stringLike);
    }


    static bool validate_jsonSameness(std::vector<NLMjson> const &jsonVec) {
        // Validate that all the JSON objects parsed above have the same structure
        for (auto const &js : jsonVec) {
            // Different number of items in this line vs the firstline
            if (js.size() != jsonVec.front().size()) { return false; }

            auto firstLineIT = jsonVec.front().items().begin();
            for (auto const &[key, val] : js.items()) {

                // Key is not the same as in the first line
                if (key != firstLineIT.key()) { return false; }

                // Type is not the same as in the first line
                if (val.type() != firstLineIT.value().type()) { return false; }
                ++firstLineIT;
            }
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

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static std::vector<NLMjson> parse_NDJSON(T const &stringLike) {

        // Trim input 'string like' of all 'newline' chars at the end
        auto const it =
            std::ranges::find_if_not(stringLike.rbegin(), stringLike.rend(), [](auto &&chr) { return chr == '\n'; });
        std::string_view const trimmed(stringLike.begin(), stringLike.end() - (it - stringLike.rbegin()));

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
            }
            parsed.push_back(std::move(oneLineJson));
        }
        return parsed;
    }

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static void parse_NDJSON_andAddTo(T const &stringLike, DataStore &out_DS_toAppend) {

        auto parsed = parse_NDJSON(stringLike);
        if (not validate_jsonSameness(parsed)) {} // Throw something here
        if (not validate_jsonSameness(parsed.front(), out_DS_toAppend.constructedWith.front())) {
        } // Throw something here

        out_DS_toAppend.append_jsonAndData(parsed);
    }


    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static DataStore parse_NDJSON_intoDS(T const &stringLike) {
        auto parsed = parse_NDJSON(stringLike);
        if (not validate_jsonSameness(parsed)) {} // Throw something here
        return DataStore(std::move(parsed));
    }

    template <typename T>
    requires std::is_convertible_v<typename T::value_type, std::string_view>
    static DataStore parse_NDJSON(T containerOfStringLike) {

        for (auto &oneStr : containerOfStringLike) {
            while (oneStr.back() == '\n') { oneStr.popback(); }
        }

        std::vector<NLMjson> parsed;
        for (auto const &contItem : containerOfStringLike) {
            for (auto const oneLine : std::views::split(contItem, '\n') |
                                          std::views::transform([](auto const &in) { return std::string_view(in); })) {

                NLMjson oneLineJson;
                try {
                    oneLineJson = NLMjson::parse(oneLine);
                }
                catch (const NLMjson::exception &e) {
                    std::print("{}\n", e.what());
                }
                parsed.push_back(std::move(oneLineJson));
            }
        }
        return DataStore(std::move(parsed));
    }
};
} // namespace terminal_plot
} // namespace incom