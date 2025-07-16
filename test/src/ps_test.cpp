
#include <gtest/gtest.h>
#include <incstd.hpp>

#include <data_get.hpp>


using namespace incom::terminal_plot::testing;
using namespace incom::terminal_plot::dataget;
namespace incplot = incom::terminal_plot;

TEST(PS, penguins_default) {
    using enum incplot::Unexp_plotSpecs;

    auto ds = get_DS(DataSets_FN::penguins.at(0));
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};
    auto                                dp = incom::terminal_plot::DesiredPlot(dpctrs).guess_missingParams(ds.value());

    incom::terminal_plot::plot_structures::Scatter ps;

    auto ar = incom::terminal_plot::plot_structures::Scatter{}.build_self(dp.value(), ds.value());

    EXPECT_EQ(ar.value().areaWidth, 43);
    EXPECT_EQ(ar.value().areaHeight, 14);
    EXPECT_EQ(ar.value().labels_verLeftWidth, 5);
    EXPECT_EQ(ar.value().labels_verRightWidth, 9);
    EXPECT_EQ(ar.value().axis_verLeftSteps, 2);
    EXPECT_EQ(ar.value().axis_horBottomSteps, 3);
    EXPECT_EQ(ar.value().axisName_horBottom_bool, true);
    EXPECT_EQ(ar.value().axisName_verLeft_bool, true);
    EXPECT_EQ(ar.value().labels_horBottom_bool, true);


    EXPECT_EQ(true, true);
}


TEST(PS, wine_quality_default) {
    using enum incplot::Unexp_plotSpecs;

    auto ds = get_DS(DataSets_FN::wine_quality.at(0));
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{};
    auto                                dp = incom::terminal_plot::DesiredPlot(dpctrs).guess_missingParams(ds.value());

    incom::terminal_plot::plot_structures::Scatter ps;

    auto ar = incom::terminal_plot::plot_structures::Scatter{}.build_self(dp.value(), ds.value());

    EXPECT_EQ(ar.value().areaWidth, 47);
    EXPECT_EQ(ar.value().areaHeight, 15);
    EXPECT_EQ(ar.value().labels_verLeftWidth, 5);
    EXPECT_EQ(ar.value().labels_verRightWidth, 5);
    EXPECT_EQ(ar.value().axis_verLeftSteps, 2);
    EXPECT_EQ(ar.value().axis_horBottomSteps, 4);
    EXPECT_EQ(ar.value().axisName_horBottom_bool, true);
    EXPECT_EQ(ar.value().axisName_verLeft_bool, true);
    EXPECT_EQ(ar.value().labels_horBottom_bool, true);


    EXPECT_EQ(true, true);
}

