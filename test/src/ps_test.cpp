
#include <gtest/gtest.h>
#include <incstd.hpp>

#include <incplot.hpp>
#include <tests_config.hpp>
#include <typeindex>


using namespace incom::terminal_plot::testing;
namespace incplot = incom::terminal_plot;

TEST(PS, penguins_default) {
    using enum incplot::Unexp_plotSpecs;

    auto ds = incplot::DataStore::get_DS(DataSets_FN::penguins.at(0));
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{
        .plot_type_name = std::type_index(typeid(incom::terminal_plot::plot_structures::Scatter))};
    auto dp = incom::terminal_plot::DesiredPlot(dpctrs).guess_missingParams(ds.value());

    auto ps = incom::terminal_plot::plot_structures::Scatter(dp.value(), ds.value());
    auto ar = ps.build_self();

    EXPECT_TRUE(ar.has_value());

    EXPECT_EQ(ar.value().get().areaWidth, 59);
    EXPECT_EQ(ar.value().get().areaHeight, 19);
    EXPECT_EQ(ar.value().get().labels_verLeftWidth, 5);
    EXPECT_EQ(ar.value().get().labels_verRightWidth, 9);
    EXPECT_EQ(ar.value().get().axis_verLeftSteps, 3);
    EXPECT_EQ(ar.value().get().axis_horBottomSteps, 5);
    EXPECT_EQ(ar.value().get().axisName_horBottom_bool, true);
    EXPECT_EQ(ar.value().get().axisName_verLeft_bool, true);
    EXPECT_EQ(ar.value().get().labels_horBottom.size(), 1uz);


    EXPECT_EQ(true, true);
}


TEST(PS, wine_quality_default) {
    using enum incplot::Unexp_plotSpecs;

    auto ds = incplot::DataStore::get_DS(DataSets_FN::wine_quality.at(0));
    EXPECT_TRUE(ds.has_value());

    incplot::DesiredPlot::DP_CtorStruct dpctrs{
        .plot_type_name = std::type_index(typeid(incom::terminal_plot::plot_structures::Scatter))};
    auto dp = incom::terminal_plot::DesiredPlot(dpctrs).guess_missingParams(ds.value());

    auto ps = incom::terminal_plot::plot_structures::Scatter(dp.value(), ds.value());
    auto ar = ps.build_self();

    EXPECT_TRUE(ar.has_value());

    EXPECT_EQ(ar.value().get().areaWidth, 63);
    EXPECT_EQ(ar.value().get().areaHeight, 20);
    EXPECT_EQ(ar.value().get().labels_verLeftWidth, 5);
    EXPECT_EQ(ar.value().get().labels_verRightWidth, 5);
    EXPECT_EQ(ar.value().get().axis_verLeftSteps, 3);
    EXPECT_EQ(ar.value().get().axis_horBottomSteps, 5);
    EXPECT_EQ(ar.value().get().axisName_horBottom_bool, true);
    EXPECT_EQ(ar.value().get().axisName_verLeft_bool, true);
    EXPECT_EQ(ar.value().get().labels_horBottom.size(), 1uz);


    EXPECT_EQ(true, true);
}
