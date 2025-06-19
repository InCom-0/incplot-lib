#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <vector>


#define TEST_DF "../../test/data"

namespace incom::terminal_plot::testing {

using namespace std::literals;

// This is used both for 'file names' (as compile time statics inside DataSets struct) as well as getting the loaded
// data itself from the Meyer's singleton inside get_data<>()
struct DataSetSV {
    std::optional<std::string_view> csv    = std::nullopt;
    std::optional<std::string_view> tsv    = std::nullopt;
    std::optional<std::string_view> json   = std::nullopt;
    std::optional<std::string_view> ndjson = std::nullopt;

    template <typename F>
    constexpr auto apply_overAllPairs(F const fn) const
        -> std::vector<std::invoke_result_t<F, std::string_view, std::string_view>> {
        std::vector<std::invoke_result_t<F, std::string_view, std::string_view>> res;
        if (csv.has_value()) {
            if (tsv.has_value()) { res.push_back(fn(csv.value(), tsv.value())); }
            if (json.has_value()) { res.push_back(fn(csv.value(), json.value())); }
            if (ndjson.has_value()) { res.push_back(fn(csv.value(), ndjson.value())); }
        }
        if (tsv.has_value()) {
            if (json.has_value()) { res.push_back(fn(tsv.value(), json.value())); }
            if (ndjson.has_value()) { res.push_back(fn(tsv.value(), ndjson.value())); }
        }
        if (json.has_value()) {
            if (ndjson.has_value()) { res.push_back(fn(json.value(), ndjson.value())); }
        }
        return res;
    }

    template <typename F>
    constexpr auto apply_overEach(F const f) const -> std::vector<std::invoke_result_t<F, std::string_view>> {
        std::vector<std::invoke_result_t<F, std::string_view>> res;
        if (csv.has_value()) { res.push_back(F(csv.value())); }
        if (tsv.has_value()) { res.push_back(F(tsv.value())); }
        if (json.has_value()) { res.push_back(F(json.value())); }
        if (ndjson.has_value()) { res.push_back(F(ndjson.value())); }
        return res;
    }
};

struct DataSets_FN {
    using svArray = std::array<std::string_view, 4>;
    static constexpr svArray flights{TEST_DF "/flights/flights_data.csv"sv, TEST_DF "/flights/flights_data.tsv"sv,
                                     TEST_DF "/flights/flights_data.json"sv, TEST_DF "/flights/flights_data.ndjson"sv};

    static constexpr svArray nile{TEST_DF "/nile/nile_data.csv"sv, TEST_DF "/nile/nile_data.tsv"sv,
                                  TEST_DF "/nile/nile_data.json"sv, TEST_DF "/nile/nile_data.ndjson"sv};

    static constexpr svArray penguins{TEST_DF "/penguins/penguins_data.csv"sv, TEST_DF "/penguins/penguins_data.tsv"sv,
                                      TEST_DF "/penguins/penguins_data.json"sv,
                                      TEST_DF "/penguins/penguins_data.ndjson"sv};

    static constexpr svArray wine_quality{
        TEST_DF "/wine_quality/wine_quality_data.csv"sv, TEST_DF "/wine_quality/wine_quality_data.tsv"sv,
        TEST_DF "/wine_quality/wine_quality_data.json"sv, TEST_DF "/wine_quality/wine_quality_data.ndjson"sv};
};

struct DataSets_FN_transposed {
    using svArray = std::array<std::string_view, 4>;
    static constexpr svArray csv{DataSets_FN::flights[0], DataSets_FN::nile[0],
                                                         DataSets_FN::penguins[0], DataSets_FN::wine_quality[0]};
    static constexpr svArray tsv{DataSets_FN::flights[1], DataSets_FN::nile[1],
                                                         DataSets_FN::penguins[1], DataSets_FN::wine_quality[1]};
    static constexpr svArray json{DataSets_FN::flights[2], DataSets_FN::nile[2],
                                                         DataSets_FN::penguins[2], DataSets_FN::wine_quality[2]};
    static constexpr svArray ndjson{DataSets_FN::flights[3], DataSets_FN::nile[3],
                                                         DataSets_FN::penguins[3], DataSets_FN::wine_quality[3]};
};


} // namespace incom::terminal_plot::testing
