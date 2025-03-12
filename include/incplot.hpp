#pragma once
#include <algorithm>
#include <expected>
#include <iostream>
#include <more_concepts/more_concepts.hpp>
#include <nlohmann/json.hpp>
#include <oof.h>
#include <optional>
#include <string_view>
#include <vector>


namespace incom {
namespace terminal_plot {
using NLMjson = nlohmann::json;

enum class PlotType {
    barV,
    barH,
    line,
    multiline,
    scatter,
    bubble
};

struct DataStore {

    DataStore(std::vector<NLMjson> &json) {}

    std::vector<std::string>                                colNames;
    std::vector<std::pair<nlohmann::json::value_t, size_t>> colTypes; // First =  ColType, Second = ID in data vector

    std::vector<std::vector<std::string>> stringCols;
    std::vector<std::vector<double>>      doubleCols;
    



    size_t guessLabelColID(this auto const &self) {}
};


struct DesiredPlot {
    enum class SpecGuessError {
        impossibleToGuess,
    };

    std::optional<PlotType> plot_type;

    std::optional<size_t>      label_colID;
    std::optional<std::string> label_colName;

    std::vector<size_t>      values_coldIDs;
    std::vector<std::string> values_coldNames;

    // Escapsulated logic for validating the parsed data structure
    // Doesn't validate technical correctness
    // Validates whether the data is possible to plot as specified by user
    bool validateSelf(this auto const &self, DataStore const &ds) { return false; }

    std::expected<DesiredPlot, SpecGuessError> autoSpecGuess(this auto const &self, DataStore const &ds) {
        if (not self.plot_type.has_value()) {}
        else {}

        return std::unexpected(SpecGuessError::impossibleToGuess);
    }
};

struct PlotSpecs {};


// Encapsulates parsing of the input into custom data structure
// Validates 'hard' errors during parsing
// Validates that input data is not structured 'impossibly' (missing values, different value names per record, etc.)
struct Parser {

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static DataStore parse_NDJSON(T const &stringLike) {

        auto const it =
            std::ranges::find_if_not(stringLike.rbegin(), stringLike.rend(), [](auto &&chr) { return chr == '\n'; });
        std::string_view const trimmed(stringLike.begin(),
                                       stringLike.begin() + (stringLike.size() - (it - stringLike.rbegin())));

        std::vector<NLMjson> parsed;
        for (auto const &oneLine : std::views::split(trimmed, '\n') |
                                       std::views::transform([](auto const &in) { return std::string_view(in); })) {
            NLMjson oneLineJson;
            try {
                oneLineJson = NLMjson::parse(oneLine);
            }
            catch (const NLMjson::exception &e) {
                std::cout << e.what() << '\n';
            }
            parsed.push_back(std::move(oneLineJson));
        }

        // Validate that all the JSON objects parsed above have the same structure
        for (auto const &js : parsed) {

            // Different number of items in this line vs the firstline
            if (js.size() != parsed.front().size()) {} // throw something here

            auto firstLineIT = parsed.front().items().begin();
            for (size_t id = 0; auto const &[key, val] : js.items()) {

                // Key is not the same as in the first line
                if (key != firstLineIT.key()) {} // throw something here

                // Type is not the same as in the first line
                if (val.type() != firstLineIT.value().type()) {} // throw something here
                ++firstLineIT;
            }
        }

        return DataStore(parsed);
    }

    template <typename T>
    requires more_concepts::container<T> && std::is_convertible_v<typename T::value_type, std::string_view>
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
                    std::cout << e.what() << '\n';
                }
                parsed.push_back(std::move(oneLineJson));
            }
        }
        return DataStore(parsed);
    }
};

} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;