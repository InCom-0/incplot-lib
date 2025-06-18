
#include <algorithm>
#include <cstddef>
#include <functional>
#include <gtest/gtest.h>
#include <tests.hpp>
#include <tuple>
#include <type_traits>
#include <vector>

#include <kcomb.hpp>
#include <print>


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

    auto k_comb_view = []<size_t COMB, typename C, typename Z>(C &container, Z &fn) {};

    auto apply_onCombinations = []<size_t COMB, typename C, typename Z>(C &container, Z &fn) {
        auto app = []<typename T, typename CMP, size_t... I>(T const &c2, CMP &cmp, std::index_sequence<I...>) {
            size_t                                         sz_cur = combSz;
            std::vector<int>                               res;
            std::vector<std::reference_wrapper<contVal_t>> refStack;

            auto recur = [&](this auto &self, int const startID) -> void {
                if (sz_cur > 0) {
                    for (int i = startID; i < (c2.size() - combSz + 1); i++) {
                        refStack.push_back(c2.at(i));
                        sz_cur--;
                        self(i + 1);
                        sz_cur++;
                        refStack.pop_back();
                    }
                }
                else {
                    auto tied = std::tie(refStack[I].get()...);
                    res.push_back(cmp(tied));
                }
            };
            recur(0);
            return res;
        };

        return app.template operator()<decltype(container), decltype(fn)>(container, fn,
                                                                          std::make_index_sequence<COMB>{});
    };

    auto dataConsistentInEachSet = std::ranges::all_of(vOfv_ds, [&](auto &&vOf_ds) {
        return std::ranges::all_of(apply_onCombinations.template operator()<2uz>(vOf_ds, cmp),
                                   [](auto &&bv) { return bv; });
    });

    EXPECT_TRUE(dataConsistentInEachSet);
}

TEST(aaa, A1) {

    std::vector<int> vec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (auto const &[a, b] : incom::terminal_plot::testing::combinations_k<2>(vec)) { std::print("{} {}\n", a, b); }


    EXPECT_EQ(true, true);
}

TEST(aaa, A2) {
    EXPECT_EQ(true, true);
}