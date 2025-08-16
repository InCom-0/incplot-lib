
#include <functional>
#include <gtest/gtest.h>
#include <incstd.hpp>

#include <incplot.hpp>
#include <tests_config.hpp>
#include <typeindex>


using namespace incom::terminal_plot::testing;
namespace incplot = incom::terminal_plot;


TEST(DP_compute_colAssessment, penguins_csv) {
    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
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
    auto ds = incplot::DataStore::get_DS(sourceFN);
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
    auto ds = incplot::DataStore::get_DS(sourceFN);
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
    auto ds = incplot::DataStore::get_DS(sourceFN);
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
    auto ds = incplot::DataStore::get_DS(sourceFN);
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
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.lts_colName = "non-existent_col"};

    auto dp_res = incplot::DesiredPlot::transform_namedColsIntoIDs(dpctrs, ds.value());
    EXPECT_FALSE(dp_res.has_value());
    EXPECT_EQ(dp_res.error(), TNCII_colByNameNotExist);
}
TEST(DP_transform_namedColsIntoIDs, TScol_nile_fake) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::nile.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.lts_colName = "non-existent_col"};

    auto dp_res = incplot::DesiredPlot::transform_namedColsIntoIDs(dpctrs, ds.value());
    EXPECT_FALSE(dp_res.has_value());
    EXPECT_EQ(dp_res.error(), TNCII_colByNameNotExist);
}

TEST(DP_transform_namedColsIntoIDs, valCols_wine_quality_real) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{
        .v_colNames{"fixed_acidity", "volatile_acidity", "citric_acid", "residual_sugar"}};

    auto dp_res = incplot::DesiredPlot::transform_namedColsIntoIDs(dpctrs, ds.value());
    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.values_colIDs.size(), 4);

    std::vector val_colIDS_exp{0uz, 1uz, 2uz, 3uz};
    EXPECT_EQ(dp.values_colIDs, val_colIDS_exp);
}
TEST(DP_transform_namedColsIntoIDs, valCols_penguins_real) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.v_colNames{"flipper_length_mm", "body_mass_g"}};

    auto dp_res = incplot::DesiredPlot::transform_namedColsIntoIDs(dpctrs, ds.value());
    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.values_colIDs.size(), 2);

    std::vector val_colIDS_exp{4uz, 5uz};
    EXPECT_EQ(dp.values_colIDs, val_colIDS_exp);
}


TEST(DP_guess_TSCol, penguins_possible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.v_colIDs{4, 5}};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.values_colIDs.size(), 2);
    EXPECT_EQ(dp.labelTS_colID.has_value(), true);
    EXPECT_EQ(dp.labelTS_colID.value(), 0);
}
TEST(DP_guess_TSCol, penguins_impossible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.v_colIDs{2, 3, 4, 5}, .c_colID = 7};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_catCol, ds.value()));

    EXPECT_FALSE(dp_res.has_value());
    EXPECT_EQ(dp_res.error(), GCC_cantSpecifyCategoryForOtherThanScatter);
}

TEST(DP_guess_TSCol, wine_quality_possible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.v_colIDs{4, 5, 0}};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.values_colIDs.size(), 3);
    EXPECT_EQ(dp.labelTS_colID.has_value(), true);
    EXPECT_EQ(dp.labelTS_colID.value(), 12);
}
TEST(DP_guess_TSCol, wine_quality_impossible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs;
    for (auto i : std::views::iota(0uz, 12uz)) { dpctrs.v_colIDs.push_back(i); }

    // TODO: Use the thing below once C++23 is properly supported on CI (ie. with GCC 15)
    // incplot::DesiredPlot::DP_CtorStruct dpctrs2{.v_colIDs{std::from_range, std::views::iota(0uz, 12uz)}};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_catCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_valueCols, ds.value()));

    EXPECT_FALSE(dp_res.has_value());
    EXPECT_EQ(dp_res.error(), GVC_selectedMoreThan6YvalColForBarXM);
}


TEST(DP_guess_catCol, penguins_possible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_catCol, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.values_colIDs.size(), 0);
    EXPECT_EQ(dp.labelTS_colID.has_value(), true);
    EXPECT_EQ(dp.labelTS_colID.value(), 0);
    EXPECT_EQ(dp.cat_colID.has_value(), false);
}
TEST(DP_guess_catCol, penguins_impossible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.v_colIDs{0, 1, 6, 7}};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_catCol, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.values_colIDs.size(), 4);
    EXPECT_EQ(dp.labelTS_colID.has_value(), true);
    EXPECT_EQ(dp.labelTS_colID.value(), 0);
    EXPECT_EQ(dp.cat_colID.has_value(), false);
}

TEST(DP_guess_catCol, wine_quality_possible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_catCol, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.values_colIDs.size(), 0);
    EXPECT_EQ(dp.labelTS_colID.has_value(), true);
    EXPECT_EQ(dp.labelTS_colID.value(), 12);
    EXPECT_EQ(dp.cat_colID.has_value(), false);
}
TEST(DP_guess_catCol, wine_quality_impossible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.lts_colID = 12};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_catCol, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.values_colIDs.size(), 0);
    EXPECT_EQ(dp.labelTS_colID.has_value(), true);
    EXPECT_EQ(dp.labelTS_colID.value(), 12);
    EXPECT_EQ(dp.cat_colID.has_value(), false);
}


TEST(DP_guess_valueCols, penguins_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_catCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_valueCols, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    std::vector<size_t> exp_res{2,3,4,5,7};

    EXPECT_EQ(dp.values_colIDs.size(), 5);
    EXPECT_EQ(dp.values_colIDs, exp_res);
}
TEST(DP_guess_valueCols, wine_quality_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::transform_namedColsIntoIDs, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_TSCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_catCol, ds.value()))
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_valueCols, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    std::vector<size_t> exp_res{0,1,2,3,4,5};

    EXPECT_EQ(dp.values_colIDs.size(), 6);
    EXPECT_EQ(dp.values_colIDs, exp_res);
}


TEST(DP_guess_sizes, penguins_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_sizes, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.targetWidth.has_value(), true);
    EXPECT_EQ(dp.targetWidth.value(), 64uz);
}
TEST(DP_guess_sizes, wine_quality_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_sizes, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.targetWidth.has_value(), true);
    EXPECT_EQ(dp.targetWidth.value(), 64uz);
}
TEST(DP_guess_sizes, penguins_possible_available_possible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.availableWidth = 250};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_sizes, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.targetWidth.has_value(), true);
    EXPECT_EQ(dp.targetWidth.value(), static_cast<size_t>(incplot::Config::scale_availablePlotWidth * 250));
}
TEST(DP_guess_sizes, wine_quality_available_possible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.availableWidth = 50};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_sizes, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    auto dp = dp_res.value();

    EXPECT_EQ(dp.targetWidth.has_value(), true);
    EXPECT_EQ(dp.targetWidth.value(), static_cast<size_t>(incplot::Config::scale_availablePlotWidth * 50));
}
TEST(DP_guess_sizes, penguins_possible_available_impossible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.availableWidth = 30};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_sizes, ds.value()));

    EXPECT_EQ(dp_res.has_value(), false);
    EXPECT_EQ(dp_res.error(), GZS_widthTooSmall);
}
TEST(DP_guess_sizes, wine_quality_available_impossible) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{.availableWidth = 0};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_sizes, ds.value()));

    EXPECT_EQ(dp_res.has_value(), false);
    EXPECT_EQ(dp_res.error(), GZS_widthTooSmall);
}

TEST(DP_guess_plotTypes, plotTypeName_nile_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::nile.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    EXPECT_TRUE(dp_res->plot_type_name.has_value());
    //TODO: Need to correctly reflect the type_id instead of hardcoding plot type name as string.
    EXPECT_EQ(dp_res->plot_type_name.value(), std::type_index(typeid(incom::terminal_plot::plot_structures::Multiline)));
}
TEST(DP_guess_plotTypes, plotTypeName_flights_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::flights.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    EXPECT_TRUE(dp_res->plot_type_name.has_value());
    //TODO: Need to correctly reflect the type_id instead of hardcoding plot type name as string.
    EXPECT_EQ(dp_res->plot_type_name.value(), std::type_index(typeid(incom::terminal_plot::plot_structures::Multiline)));
}
TEST(DP_guess_plotTypes, plotTypeName_penguins_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::penguins.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    EXPECT_TRUE(dp_res->plot_type_name.has_value());
    //TODO: Need to correctly reflect the type_id instead of hardcoding plot type name as string.
    EXPECT_EQ(dp_res->plot_type_name.value(), std::type_index(typeid(incom::terminal_plot::plot_structures::BarHM)));
}
TEST(DP_guess_plotTypes, plotTypeName_wine_quality_default) {
    using enum incplot::Unexp_plotSpecs;

    auto sourceFN{DataSets_FN::wine_quality.at(0)};
    auto ds = incplot::DataStore::get_DS(sourceFN);
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};

    auto dp_res = incplot::DesiredPlot::compute_colAssessments(dpctrs, ds.value())
                      .and_then(std::bind_back(incplot::DesiredPlot::guess_plotType, ds.value()));

    EXPECT_TRUE(dp_res.has_value());
    EXPECT_TRUE(dp_res->plot_type_name.has_value());
    //TODO: Need to correctly reflect the type_id instead of hardcoding plot type name as string.
    EXPECT_EQ(dp_res->plot_type_name.value(), std::type_index(typeid(incom::terminal_plot::plot_structures::BarHM)));
}