
#include "incplot/desired_plot.hpp"
#include <functional>
#include <gtest/gtest.h>
#include <incstd.hpp>

#include <tests.hpp>


using namespace incom::terminal_plot::testing;
namespace incplot = incom::terminal_plot;


TEST(DP_compute_colAssessment, penguins_csv) {
    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(incplot::DesiredPlot::DP_CtorStruct{}, ds.value());
    EXPECT_TRUE(dp_res.has_value());

    incplot::DesiredPlot &dp = dp_res.value();


    EXPECT_EQ(dp.m_colAssessments.at(0).categoryCount, 3);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_categoryLike, true);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_timeSeriesLikeIndex, false);

    EXPECT_EQ(dp.m_colAssessments.at(1).categoryCount, 3);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_categoryLike, true);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_timeSeriesLikeIndex, false);

    EXPECT_EQ(dp.m_colAssessments.at(2).is_categoryLike, false);
    EXPECT_EQ(dp.m_colAssessments.at(2).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(2).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(2).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(2).is_timeSeriesLikeIndex, false);

    EXPECT_EQ(dp.m_colAssessments.at(3).is_categoryLike, false);
    EXPECT_EQ(dp.m_colAssessments.at(3).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(3).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(3).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(3).is_timeSeriesLikeIndex, false);

    EXPECT_EQ(dp.m_colAssessments.at(4).is_categoryLike, false);
    EXPECT_EQ(dp.m_colAssessments.at(4).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(4).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(4).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(4).is_timeSeriesLikeIndex, false);

    EXPECT_EQ(dp.m_colAssessments.at(5).is_categoryLike, false);
    EXPECT_EQ(dp.m_colAssessments.at(5).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(5).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(5).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(5).is_timeSeriesLikeIndex, false);

    EXPECT_EQ(dp.m_colAssessments.at(6).categoryCount, 2);
    EXPECT_EQ(dp.m_colAssessments.at(6).is_categoryLike, true);
    EXPECT_EQ(dp.m_colAssessments.at(6).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(6).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(6).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(6).is_timeSeriesLikeIndex, false);

    EXPECT_EQ(dp.m_colAssessments.at(7).categoryCount, 3);
    EXPECT_EQ(dp.m_colAssessments.at(7).is_categoryLike, true);
    EXPECT_EQ(dp.m_colAssessments.at(7).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(7).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(7).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(7).is_timeSeriesLikeIndex, false);
}
TEST(DP_compute_colAssessment, nile_csv) {
    auto sourceFN{DataSets_FN::nile.at(0)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(incplot::DesiredPlot::DP_CtorStruct{}, ds.value());
    EXPECT_TRUE(dp_res.has_value());

    incplot::DesiredPlot &dp = dp_res.value();

    EXPECT_EQ(dp.m_colAssessments.at(0).categoryCount, 100);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_categoryLike, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_categoriesSameSize, true);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_timeSeriesLikeIndex, true);

    EXPECT_EQ(dp.m_colAssessments.at(1).is_categoryLike, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_timeSeriesLikeIndex, false);
}
TEST(DP_compute_colAssessment, flights_ndjson) {
    auto sourceFN{DataSets_FN::flights.at(3)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(incplot::DesiredPlot::DP_CtorStruct{}, ds.value());
    EXPECT_TRUE(dp_res.has_value());

    incplot::DesiredPlot &dp = dp_res.value();

    EXPECT_EQ(dp.m_colAssessments.at(0).categoryCount, 144);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_categoryLike, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_categoriesSameSize, true);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(0).is_timeSeriesLikeIndex, true);

    EXPECT_EQ(dp.m_colAssessments.at(1).is_categoryLike, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_categoriesSameSize, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_sameRepeatingSubsequences, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_sameRepeatingSubsequences_whole, false);
    EXPECT_EQ(dp.m_colAssessments.at(1).is_timeSeriesLikeIndex, false);
}


TEST(DP_transform_namedColsIntoIDs, TScol_flights_real) {
    auto sourceFN{DataSets_FN::flights.at(3)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.lts_colName = "period"};

    auto dp_res = incplot::DesiredPlot::transform_namedColsIntoIDs(dpctrs, ds.value());
    EXPECT_TRUE(dp_res.has_value());

    incplot::DesiredPlot &dp = dp_res.value();


    EXPECT_FALSE(dp.labelTS_colName.has_value());
    EXPECT_TRUE(dp.labelTS_colID.has_value());
    EXPECT_EQ(dp.labelTS_colID.value(), 0);
}
TEST(DP_transform_namedColsIntoIDs, TScol_nile_real) {
    auto sourceFN{DataSets_FN::nile.at(1)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.lts_colName = "time"};

    auto dp_res = incplot::DesiredPlot::transform_namedColsIntoIDs(dpctrs, ds.value());
    EXPECT_TRUE(dp_res.has_value());

    incplot::DesiredPlot &dp = dp_res.value();


    EXPECT_FALSE(dp.labelTS_colName.has_value());
    EXPECT_TRUE(dp.labelTS_colID.has_value());
    EXPECT_EQ(dp.labelTS_colID.value(), 0);
}

TEST(DP_transform_namedColsIntoIDs, TScol_flights_fake) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::flights.at(2)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.lts_colName = "non-existent_col"};

    auto dp_res = incplot::DesiredPlot::transform_namedColsIntoIDs(dpctrs, ds.value());
    EXPECT_FALSE(dp_res.has_value());
    EXPECT_EQ(dp_res.error(), TNCII_colByNameNotExist);
}
TEST(DP_transform_namedColsIntoIDs, TScol_nile_fake) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::nile.at(0)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.lts_colName = "non-existent_col"};

    auto dp_res = incplot::DesiredPlot::transform_namedColsIntoIDs(dpctrs, ds.value());
    EXPECT_FALSE(dp_res.has_value());
    EXPECT_EQ(dp_res.error(), TNCII_colByNameNotExist);
}


TEST(DP_guess_plotTypes, plotTypeName_nile_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::nile.at(0)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    EXPECT_TRUE(dp_res->plot_type_name.has_value());
    EXPECT_EQ(dp_res->plot_type_name.value(), "Multiline");
}
TEST(DP_guess_plotTypes, plotTypeName_flights_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::flights.at(0)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    EXPECT_TRUE(dp_res->plot_type_name.has_value());
    EXPECT_EQ(dp_res->plot_type_name.value(), "Multiline");
}
TEST(DP_guess_plotTypes, plotTypeName_penguins_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    EXPECT_TRUE(dp_res->plot_type_name.has_value());
    EXPECT_EQ(dp_res->plot_type_name.value(), "Scatter");
}
TEST(DP_guess_plotTypes, plotTypeName_wine_quality_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    EXPECT_TRUE(dp_res->plot_type_name.has_value());
    EXPECT_EQ(dp_res->plot_type_name.value(), "Scatter");
}