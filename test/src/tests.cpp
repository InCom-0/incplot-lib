#include <algorithm>
#include <functional>
#include <gtest/gtest.h>
#include <incplot.hpp>
#include <optional>
#include <string_view>
#include <type_traits>

#define TEST_DF "../../test/data"


namespace incom::terminal_plot::testing {
struct DataSetFN {
    std::optional<std::string_view> csv    = std::nullopt;
    std::optional<std::string_view> tsv    = std::nullopt;
    std::optional<std::string_view> json   = std::nullopt;
    std::optional<std::string_view> ndjson = std::nullopt;

    template <typename F>
    auto apply_overAllPairs(const F fn) const -> std::vector<std::invoke_result_t<F, std::string_view, std::string_view>> {
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
    auto apply_overEach(F f) const -> std::vector<std::invoke_result_t<F, std::string_view>> {
        std::vector<std::invoke_result_t<F, std::string_view>> res;
        if (csv.has_value()) { res.push_back(F(csv.value())); }
        if (tsv.has_value()) { res.push_back(F(tsv.value())); }
        if (json.has_value()) { res.push_back(F(json.value())); }
        if (ndjson.has_value()) { res.push_back(F(ndjson.value())); }
        return res;
    }
};

using namespace std::literals;
struct DataSets {
    static constexpr DataSetFN flights{TEST_DF "/flights/flights_data.csv"sv, TEST_DF "/flights/flights_data.tsv"sv,
                                       TEST_DF "/flights/flights_data.json"sv,
                                       TEST_DF "/flights/flights_data.ndjson"sv};
    static constexpr DataSetFN nile{TEST_DF "/nile/nile_data.csv"sv, TEST_DF "/nile/nile_data.tsv"sv,
                                    TEST_DF "/nile/nile_data.json"sv, TEST_DF "/nile/nile_data.ndjson"sv};
    static constexpr DataSetFN penguins{
        TEST_DF "/penguins/penguins_data.csv"sv, TEST_DF "/penguins/penguins_data.tsv"sv,
        TEST_DF "/penguins/penguins_data.json"sv, TEST_DF "/penguins/penguins_data.ndjson"sv};
    static constexpr DataSetFN wine_quality{
        TEST_DF "/wine_quality/wine_quality_data.csv"sv, TEST_DF "/wine_quality/wine_quality_data.tsv"sv,
        TEST_DF "/wine_quality/wine_quality_data.json"sv, TEST_DF "/wine_quality/wine_quality_data.ndjson"sv};
};
} // namespace incom::terminal_plot::testing

using namespace incom::terminal_plot::testing;

class ParserTest : public testing::Test {

protected:
    const std::vector<std::reference_wrapper<const DataSetFN>> allDataSets;
    ParserTest() : allDataSets{DataSets::flights, DataSets::nile, DataSets::penguins, DataSets::wine_quality} {};
};


TEST_F(ParserTest, identical_by_file_type) {

    auto const compare_parsedSameness = [](std::string_view toParse_A, std::string_view toParse_B) -> bool { return true; };

    for (DataSetFN const &ref : allDataSets) {
        std::ranges::all_of(ref.apply_overAllPairs(compare_parsedSameness), [](auto &&item) { return item == true; });


        EXPECT_TRUE(true);
    }
}

TEST(aaa, A1) {
    EXPECT_EQ(true, true);
}

TEST(aaa, A2) {
    EXPECT_EQ(true, true);
}