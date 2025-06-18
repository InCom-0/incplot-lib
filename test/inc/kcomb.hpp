#pragma once

#include <array>
#include <cstddef>
#include <incplot.hpp>
#include <iterator>
#include <limits>
#include <ranges>
#include <tests.hpp>
#include <utility>

namespace incom::utils::packs {

// Shamelessly 'borrowed' from https://godbolt.org/z/6nbfsbTz1 ... from a comment on SO
// https://stackoverflow.com/questions/72706224/how-do-i-reverse-the-order-of-the-integers-in-a-stdinteger-sequenceint-4

template <auto View, typename Int, Int... Is>
auto build_sequence(std::integer_sequence<Int, Is...>) {
    // We build an array holding the input sequence of integers
    constexpr std::array<Int, sizeof...(Is)> input = {{Is...}};

    // We get the size of the output sequence of integers
    constexpr auto N = std::size(std::views::iota(size_t{0}, sizeof...(Is)) | View);

    // We build the array holding the output sequence of integers
    constexpr std::array<Int, N> values = [&] {
        std::array<Int, N> res;
        auto               vw = std::views::iota(size_t{0}, sizeof...(Is)) | View;
        for (auto [i, x] : vw | std::views::enumerate) { res[i] = input[x]; }
        return res;
    }();

    // We build the output std::integer_sequence object (with a different pack Os...)
    return [&]<std::size_t... Os>(std::index_sequence<Os...>) {
        return std::integer_sequence<Int, values[Os]...>{};
    }(std::make_index_sequence<N>());
}

// We define an alias that builds the std::integer_sequence result from a call to build_sequence
template <auto view, typename Sequence>
using view_t = decltype(build_sequence<view>(Sequence{}));
} // namespace incom::utils::packs

namespace incom::terminal_plot::testing {
using namespace incom::utils::packs;

template <std::ranges::forward_range RANGE>
struct inc_sentinel {};

template <std::ranges::forward_range RANGE, size_t K>
class inc_iterator {
    using base_iterator   = std::ranges::iterator_t<RANGE>;
    using base_sentinel   = std::ranges::sentinel_t<RANGE>;
    using base_value_type = std::ranges::range_value_t<RANGE>;
    using base_reference  = std::ranges::range_reference_t<RANGE>;

    _c_generateTuple<K, base_iterator>::type iters;
    base_iterator                            n_{};
    base_iterator                            m_{};
    _c_generateTuple<K, base_sentinel>::type end_iters;
    base_sentinel                            end_{};

    static constexpr auto idxSeq     = std::make_index_sequence<K>{};
    static constexpr auto idxSeq_rev = view_t<std::views::reverse, decltype(idxSeq)>{};

private:
    template <size_t... I>
    constexpr inc_iterator(std::index_sequence<I...>, base_iterator begin, base_sentinel end)
        : iters{(std::next(begin, I))...}, end_iters{std::next(begin, I + (end - begin) - (K - 1))...}, end_{end} {}

public:
    using value_type        = std::pair<base_value_type, base_value_type>;
    using reference         = _c_generateTuple<K, base_reference>::type;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    [[nodiscard]] constexpr inc_iterator() = default;

    [[nodiscard]] constexpr inc_iterator(base_iterator begin, base_sentinel end) : inc_iterator(idxSeq, begin, end) {}

    constexpr auto operator++() -> inc_iterator & {
        auto lam = [&]<size_t... I>(std::integer_sequence<size_t, I...> seq) -> void {
            auto lamRev = [&]<size_t... J>(std::integer_sequence<size_t, J...> revSeq) -> void {
                std::array<bool, sizeof...(I)> idAtLastPos{(I == std::numeric_limits<size_t>::max())...};

                if (((std::next(std::get<J>(iters)) == std::get<J>(end_iters) ? (idAtLastPos[J] = true, true)
                                                                              : (std::get<J>(iters)++, false)) &&
                     ...)) {
                    ((std::get<I>(iters) = std::get<I>(end_iters)), ...);
                    return;
                }

                base_iterator *ptr = &(std::get<0>(iters));

                (((idAtLastPos[I] == true && I > 0)
                      ? std::get<I>(iters) = std::next(*ptr), ptr = &(std::get<I>(iters))
                      : ptr = &std::get<I>(iters)),
                 ...);


                // (((idAtLastPos[I])
                //       ? (ptr = &(std::get<I>(iters)), bp = &idAtLastPos[I],
                //          ((((idAtLastPos[J]) ? (true) : ((*ptr) = std::next(std::get<J>(iters)), *bp = false, false)) &&
                //            ...)))
                //       : (true)) &&
                //  ...);
            };

            lamRev(idxSeq_rev);
        };
        lam(idxSeq);
        return *this;
    }

    [[nodiscard]] constexpr auto operator++(int) -> inc_iterator {
        const auto pre = *this;
        ++(*this);
        return pre;
    }

    [[nodiscard]] constexpr auto operator*() const -> reference {
        auto lam = [&]<size_t... I>(std::integer_sequence<size_t, I...> seq) -> reference {
            return {(*(std::get<I>(iters)))...};
        };
        return lam(idxSeq);
    }

    [[nodiscard]] constexpr auto operator<=>(const inc_iterator &) const = default;

    [[nodiscard]] constexpr auto operator==(const inc_sentinel<RANGE> & /*unused*/) const -> bool {
        auto lam = [&]<size_t... I>(std::integer_sequence<size_t, I...> seq) -> bool {
            return ((std::get<I>(iters) == std::get<I>(end_iters) ? (true) : false) && ...);
        };
        return lam(idxSeq);
    }
};

template <std::ranges::forward_range RANGE, size_t K>
requires std::ranges::view<RANGE>
class combinations_k_view : public std::ranges::view_interface<combinations_k_view<RANGE, K>> {
    RANGE base_;

public:
    [[nodiscard]] constexpr combinations_k_view() = default;

    [[nodiscard]] constexpr explicit combinations_k_view(RANGE range) : base_{std::move(range)} {}

    [[nodiscard]] constexpr auto begin() const -> inc_iterator<RANGE, K> {
        return inc_iterator<RANGE, K>{std::ranges::begin(base_), std::ranges::end(base_)};
    }

    [[nodiscard]] constexpr auto end() const -> inc_sentinel<RANGE> { return {}; }
};

// template <std::ranges::sized_range RANGE>
// combinations_k_view(RANGE &&) -> combinations_k_view<std::views::all_t<RANGE>, 2>;

template <size_t K>
struct combinations_k_fn : std::ranges::range_adaptor_closure<combinations_k_fn<K>> {
    template <typename RANGE>
    constexpr auto operator()(RANGE &&range) const {
        return combinations_k_view<RANGE, K>::combinations_k_view(std::forward<RANGE>(range));
    }
};

template <size_t K>
constexpr inline combinations_k_fn<K> combinations_k;


} // namespace incom::terminal_plot::testing