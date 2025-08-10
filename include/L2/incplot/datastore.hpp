#pragma once

#include <cassert>
#include <concepts>
#include <optional>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


namespace incom {
namespace terminal_plot {

enum class parsedVal_t {
    double_like,
    unsigned_like, // unused
    signed_like,
    string_like
};

// Data storage for the actual data that are to be plotted
class DataStore {
public:
    // TYPE ALIAS

    using varCol_t        = std::variant<std::vector<std::string>, std::vector<double>, std::vector<long long>>;
    using vec_pr_varCol_t = std::vector<std::pair<std::string, varCol_t>>;

    struct DS_CtorObj {
        std::vector<std::pair<std::string, varCol_t>> data;
        std::vector<std::vector<unsigned int>>        itemFlags;
    };

    struct Column {
        std::string                                                                         name;
        parsedVal_t                                                                         colType;
        std::vector<unsigned int>                                                           itemFlags;
        std::variant<std::vector<std::string>, std::vector<long long>, std::vector<double>> variant_data;

        template <typename CT>
        const auto &get_data() const {
            assert(std::holds_alternative<CT>(variant_data));
            return std::get<CT>(variant_data);
        }

        const auto &get_variantData() const { return variant_data; }

        auto get_filteredVariantData() const {
            auto fltr = [&, i = 0uz](auto &&_) mutable { return itemFlags[i++] == 0b0; };

            using res_t = std::variant<decltype(std::views::filter(std::get<0>(variant_data), fltr)),
                                       decltype(std::views::filter(std::get<1>(variant_data), fltr)),
                                       decltype(std::views::filter(std::get<2>(variant_data), fltr))>;

            auto visi = [&](auto const &vari) {
                if constexpr (std::same_as<std::remove_cvref_t<decltype(vari)>, std::vector<std::string>>) {
                    return res_t(std::views::filter(std::get<0>(variant_data), fltr));
                }
                else if constexpr (std::same_as<std::remove_cvref_t<decltype(vari)>, std::vector<long long>>) {
                    return res_t(std::views::filter(std::get<1>(variant_data), fltr));
                }
                else if constexpr (std::same_as<std::remove_cvref_t<decltype(vari)>, std::vector<double>>) {
                    return res_t(std::views::filter(std::get<2>(variant_data), fltr));
                }

                else { static_assert(false); }
                std::unreachable();
            };

            return std::visit(visi, variant_data);
        }

        auto get_filteredVariantData(std::vector<unsigned int> const &itemFlags_ext) const {
            if (itemFlags_ext.size() != itemFlags.size()) { assert(false); }

            auto fltr   = [](auto &&a) { return std::get<0>(a) == 0; };
            auto transf = [](auto &&b) { return std::get<1>(b); };

            using res_t = std::variant<decltype(std::views::zip(itemFlags_ext, std::get<0>(variant_data)) |
                                                std::views::filter(fltr) | std::views::transform(transf)),
                                       decltype(std::views::zip(itemFlags_ext, std::get<1>(variant_data)) |
                                                std::views::filter(fltr) | std::views::transform(transf)),
                                       decltype(std::views::zip(itemFlags_ext, std::get<2>(variant_data)) |
                                                std::views::filter(fltr) | std::views::transform(transf))>;

            auto visi = [&](auto const &vari) {
                if constexpr (std::same_as<std::remove_cvref_t<decltype(vari)>, std::vector<std::string>>) {
                    return res_t(std::views::zip(itemFlags_ext, std::get<0>(variant_data)) | std::views::filter(fltr) |
                                 std::views::transform(transf));
                }
                else if constexpr (std::same_as<std::remove_cvref_t<decltype(vari)>, std::vector<long long>>) {
                    return res_t(std::views::zip(itemFlags_ext, std::get<1>(variant_data)) | std::views::filter(fltr) |
                                 std::views::transform(transf));
                }
                else if constexpr (std::same_as<std::remove_cvref_t<decltype(vari)>, std::vector<double>>) {
                    return res_t(std::views::zip(itemFlags_ext, std::get<2>(variant_data)) | std::views::filter(fltr) |
                                 std::views::transform(transf));
                }

                else { static_assert(false); }
                std::unreachable();
            };

            return std::visit(visi, variant_data);
        }
    };


    // Data descriptors
    // std::vector<std::string>                    colNames;
    // std::vector<std::pair<parsedVal_t, size_t>> colTypes;
    // std::vector<std::vector<unsigned char>>     itemFlags;

    // Actual data storage
    // std::vector<std::vector<std::string>> stringCols;
    // std::vector<std::vector<long long>>   llCols;
    // std::vector<std::vector<double>>      doubleCols;

    std::vector<Column> m_data;


    // CONSTRUCTION
    DataStore() : DataStore(DS_CtorObj()) {}

    // Pair first = name of the column, Pair second = values in that column
    DataStore(DS_CtorObj const &vecOfDataVecs);
    DataStore(DS_CtorObj const &&vecOfDataVecs) : DataStore(vecOfDataVecs) {};

    // COMPARISON
    bool operator==(const DataStore &other) const { return true; }

    // APPENDING
    void append_data(DS_CtorObj const &vecOfDataVecs);

    void append_fakeLabelCol(size_t const sz);


    // VIEWING
    const auto get_filteredViewOfData(std::vector<size_t> const       &colsToGet,
                                      std::vector<unsigned int> const &itemFlags_ext) const {

        using vec_val_t = decltype(Column().get_filteredVariantData(std::vector<unsigned int>()));
        std::vector<vec_val_t> res;

        for (auto const &oneCol : colsToGet) {
            res.push_back(m_data.at(oneCol).get_filteredVariantData(itemFlags_ext));
        }
        return res;
    }

    const auto get_filteredViewOfData(std::vector<size_t> const      &&colsToGet,
                                      std::vector<unsigned int> const &itemFlags_ext) const {
        return get_filteredViewOfData(colsToGet, itemFlags_ext);
    }

    const auto get_filteredViewOfData(std::vector<size_t> const &colsToGet, double const stdDeviation) const {
        auto flags = compute_filterFlags(colsToGet, stdDeviation);
        return get_filteredViewOfData(colsToGet, flags);
    }
    const auto get_filteredViewOfData(std::vector<size_t> const &&colsToGet, double const stdDeviation) const {
        return get_filteredViewOfData(colsToGet, stdDeviation);
    }

    const auto get_filteredViewOfData(size_t const &colToGet, std::vector<unsigned int> const &itemFlags_ext) const {
        return m_data.at(colToGet).get_filteredVariantData(itemFlags_ext);
    }
    const auto get_filteredViewOfData(size_t const &&colToGet, std::vector<unsigned int> const &itemFlags_ext) const {
        return get_filteredViewOfData(colToGet, itemFlags_ext);
    }


    std::vector<unsigned int> compute_filterFlags(std::vector<size_t> const  &colsToGet,
                                                  std::optional<double> const stdDeviation) const;
    std::vector<unsigned int> compute_filterFlags(std::vector<size_t> const &&colsToGet,
                                                  std::optional<double> const stdDeviation) const {
        return compute_filterFlags(colsToGet, stdDeviation);
    }

    // STATIC
    static std::optional<std::reference_wrapper<const DataStore>> get_DS(std::string_view const &sv);
};

} // namespace terminal_plot
} // namespace incom