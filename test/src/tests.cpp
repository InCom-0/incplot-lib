
#include <algorithm>
#include <cstddef>
#include <functional>
#include <gtest/gtest.h>
#include <tests.hpp>
#include <tuple>
#include <type_traits>
#include <vector>


using namespace incom::terminal_plot::testing;
namespace incplot = incom::terminal_plot;


TEST(ParserTest, identical_by_file_type) {
    auto allDataSets =
        get_dataSets<DataSets_FN::flights, DataSets_FN::nile, DataSets_FN::penguins, DataSets_FN::wine_quality>();

    using parsed_t = std::expected<incplot::DataStore, incplot::incerr_c>;
    std::vector<std::tuple<parsed_t, parsed_t, parsed_t, parsed_t>> vOftpl_ds;
    std::vector<std::vector<incplot::DataStore>>                    vOfv_ds;

    for (auto &DS_sv : allDataSets) {
        vOfv_ds.push_back(std::vector{incplot::parsers::Parser::parse(DS_sv.csv.value()).value(),
                                      incplot::parsers::Parser::parse(DS_sv.tsv.value()).value(),
                                      incplot::parsers::Parser::parse(DS_sv.json.value()).value(),
                                      incplot::parsers::Parser::parse(DS_sv.ndjson.value()).value()});
    }

    size_t const combSz = 2;
    using contVal_t     = std::remove_cvref_t<typename decltype(vOfv_ds)::value_type>::value_type;
    using tuple_t       = _c_generateTuple<combSz, std::add_lvalue_reference_t<contVal_t>>::type;

    auto cmp = [](tuple_t a) { return a == a; };


    auto applyOnCom = []<typename T, typename CMP, size_t... I>(T const &container, CMP &fn,
                                                                std::index_sequence<I...>) {
        size_t            sz_cur = combSz;
        std::vector<bool> res;


        std::vector<std::reference_wrapper<contVal_t>> refStack;

        auto recur = [&](this auto &self, int const startID) -> void {
            if (sz_cur > 0) {
                for (int i = startID; i < (container.size() - combSz + 1); i++) {
                    refStack.push_back(container.at(i));
                    sz_cur--;
                    self(i + 1);
                    sz_cur++;
                    refStack.pop_back();
                }
            }
            else {
                tuple_t tt{refStack[I]...};
                res.push_back(fn(tt));
            }
        };
        recur(0);

        return res;
    };

    auto dataConsistentInEachSet = std::ranges::all_of(vOfv_ds, [&](auto &&vOf_ds) {
        return std::ranges::all_of(applyOnCom.template operator()<decltype(vOf_ds), decltype(cmp)>(
                                       vOf_ds, cmp, std::make_index_sequence<2uz>{}),
                                   [](auto &&bv) { return bv; });
    });

    EXPECT_TRUE(dataConsistentInEachSet);
}

TEST(aaa, A1) {
    EXPECT_EQ(true, true);
}

TEST(aaa, A2) {
    EXPECT_EQ(true, true);
}