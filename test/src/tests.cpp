
#include <gtest/gtest.h>
#include <incstd.hpp>

#include <tests.hpp>


using namespace incom::terminal_plot::testing;
namespace incplot = incom::terminal_plot;


TEST(ParserTest, parsing_csv) {
    auto sourceFileTypeSet{DataSets_FN_transposed::csv};
    auto dataSet   = std::views::transform(sourceFileTypeSet, [](auto const &oneFN) { return get_data(oneFN); });
    auto allParsed = std::ranges::all_of(dataSet, [](auto const &opt) { return true; });
    EXPECT_TRUE(allParsed);
}
TEST(ParserTest, parsing_tsv) {
    auto sourceFileTypeSet{DataSets_FN_transposed::tsv};
    auto dataSet   = std::views::transform(sourceFileTypeSet, [](auto const &oneFN) { return get_data(oneFN); });
    auto allParsed = std::ranges::all_of(dataSet, [](auto const &opt) { return true; });
    EXPECT_TRUE(allParsed);
}
TEST(ParserTest, parsing_json) {
    auto sourceFileTypeSet{DataSets_FN_transposed::json};
    auto dataSet   = std::views::transform(sourceFileTypeSet, [](auto const &oneFN) { return get_data(oneFN); });
    auto allParsed = std::ranges::all_of(dataSet, [](auto const &opt) { return true; });
    EXPECT_TRUE(allParsed);
}
TEST(ParserTest, parsing_ndjson) {
    auto sourceFileTypeSet{DataSets_FN_transposed::ndjson};
    auto dataSet   = std::views::transform(sourceFileTypeSet, [](auto const &oneFN) { return get_data(oneFN); });
    auto allParsed = std::ranges::all_of(dataSet, [](auto const &opt) { return true; });
    EXPECT_TRUE(allParsed);
}


TEST(ParserTest, identicalDS_regardlessOfFileType) {
    constexpr std::array allDataSets{DataSets_FN::flights, DataSets_FN::penguins, DataSets_FN::nile};

    std::vector<std::vector<incplot::DataStore>> vOfv_ds;
    for (auto const &oneSet : allDataSets) {
        vOfv_ds.push_back(std::vector<incplot::DataStore>{});
        for (auto const &oneFN : oneSet) {
            auto dt = get_data(oneFN);
            if (dt.has_value()) {
                auto parsedDS = incom::terminal_plot::parsers::Parser::parse(dt.value());
                if (parsedDS.has_value()) { vOfv_ds.back().push_back(std::move(parsedDS.value())); }
            }
        }
    }

    using contVal_t = std::remove_cvref_t<typename decltype(vOfv_ds)::value_type>::value_type;
    using tuple_t   = incstd::typegen::c_generateTuple<2uz, std::add_lvalue_reference_t<contVal_t>>::type;

    auto cmp = [](tuple_t a) {
        auto res1 = std::get<0>(a) == std::get<1>(a);
        return res1;
    };

    auto dataConsistentInEachSet = std::ranges::all_of(vOfv_ds, [&](auto &&vOf_ds) {
        auto res =
            std::ranges::all_of(incstd::views::combinations_k<2uz>(vOf_ds), [&](auto &&tpl) { return cmp(tpl); });
        return res;
    });

    EXPECT_TRUE(dataConsistentInEachSet);
}

TEST(aaa, A1) {


    EXPECT_EQ(true, true);
}

TEST(aaa, A2) {
    EXPECT_EQ(true, true);
}