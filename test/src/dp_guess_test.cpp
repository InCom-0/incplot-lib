
#include "incplot/desired_plot.hpp"
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
