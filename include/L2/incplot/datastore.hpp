#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
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

    struct DS_CtorObj {
        std::vector<
            std::pair<std::string, std::variant<std::vector<std::string>, std::vector<double>, std::vector<long long>>>>
                                                data;
        std::vector<std::vector<unsigned char>> itemFlags;
    };
    using vec_pr_strVarVec_t = std::vector<
        std::pair<std::string, std::variant<std::vector<std::string>, std::vector<double>, std::vector<long long>>>>;
    using vec_colVar_t = std::vector<
        std::variant<std::reference_wrapper<std::vector<std::string>>, std::reference_wrapper<std::vector<double>>,
                     std::reference_wrapper<std::vector<long long>>>>;


    struct Column {
        std::string                                                                         name;
        parsedVal_t                                                                         colType;
        std::vector<unsigned char>                                                          itemFlags;
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

        auto get_filteredVariantData(std::vector<unsigned char> const &itemFlags_ext) const {
            if (itemFlags_ext.size() != itemFlags.size()) { assert(false); }
            auto fltr = [&, i = 0uz](auto &&_) mutable { return itemFlags_ext[i++] == 0b0; };

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
    };

    // Data descriptors
    std::vector<std::string>                    colNames;
    std::vector<std::pair<parsedVal_t, size_t>> colTypes; // First =  ColType, Second = ID in data vector
    std::vector<std::vector<unsigned char>>     itemFlags;

    // Actual data storage
    std::vector<std::vector<std::string>> stringCols;
    std::vector<std::vector<long long>>   llCols; // Don't care about signed unsigned, etc. ... all will be long long
    std::vector<std::vector<double>>      doubleCols;

    // DataStore can be accessed using dynamic polymorphism with this vector of variants reference to each collumn in
    // the data storage
    vec_colVar_t vec_colVariants;

    std::vector<Column> m_data;


    // CONSTRUCTION
    DataStore() : DataStore(DS_CtorObj()) {}

    // Pair first = name of the column, Pair second = values in that column
    DataStore(DS_CtorObj const &vecOfDataVecs);
    DataStore(DS_CtorObj const &&vecOfDataVecs) : DataStore(vecOfDataVecs) {};

    // COMPARISON
    bool operator==(const DataStore &other) const {
        if (colNames != other.colNames) { return false; }
        if (colTypes != other.colTypes) { return false; }
        if (stringCols != other.stringCols) { return false; }
        if (llCols != other.llCols) { return false; }
        if (doubleCols != other.doubleCols) { return false; }
        return true;
    }

    // APPENDING
    void append_data(DS_CtorObj const &vecOfDataVecs);

    void append_fakeLabelCol(size_t const sz);


    // VIEWING
    const auto get_filteredViewOfData(std::vector<size_t> const &colsToGet) const {
        auto flags = compute_filterFlags(colsToGet);

        using vec_val_t = decltype(std::declval<Column>().get_filteredVariantData(flags));
        std::vector<vec_val_t> res;

        for (auto const &oneCol : colsToGet) { res.push_back(m_data.at(oneCol).get_filteredVariantData(flags)); }
        return res;
    }


private:
    std::vector<unsigned char> compute_filterFlags(std::vector<size_t> const &colsToGet) const {
        if (m_data.size() < 1) { assert(false); }

        std::vector<unsigned char> res(m_data.front().itemFlags.size(), 0b0);

        for (auto const &selID : colsToGet) {
            // Non existent column ID or itemFlag sizes do not match
            if (selID >= m_data.size() || m_data.at(selID).itemFlags.size() != m_data.front().itemFlags.size()) {
                assert(false);
            }
            for (size_t i = 0; auto const &flag : m_data.at(selID).itemFlags) { res[i++] |= flag; }
        }
        return res;
    }
    std::vector<unsigned char> compute_filterFlags(std::vector<size_t> const &&colsToGet) const {
        return compute_filterFlags(colsToGet);
    }
};

} // namespace terminal_plot
} // namespace incom