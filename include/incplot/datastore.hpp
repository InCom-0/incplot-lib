#pragma once

#include <string>
#include <variant>
#include <vector>


namespace incom {
namespace terminal_plot {

enum class parsedVal_t {
    double_like,
    unsigned_like,
    signed_like,
    string_like
};

// Data storage for the actual data that are to be plotted
struct DataStore {
    // TYPE ALIAS
    using vec_pr_strVarVec_t = std::vector<
        std::pair<std::string, std::variant<std::vector<std::string>, std::vector<double>, std::vector<long long>>>>;
    using vec_colVar_t = std::vector<
        std::variant<std::reference_wrapper<std::vector<std::string>>, std::reference_wrapper<std::vector<long long>>,
                     std::reference_wrapper<std::vector<double>>>>;

    // Data descriptors
    std::vector<std::string>                    colNames;
    std::vector<std::pair<parsedVal_t, size_t>> colTypes; // First =  ColType, Second = ID in data vector

    // Actual data storage
    std::vector<std::vector<std::string>> stringCols;
    std::vector<std::vector<long long>>   llCols; // Don't care about signed unsigned, etc. ... all will be long long
    std::vector<std::vector<double>>      doubleCols;

    // DataStore can be accessed using dynamic polymorphism with this vector of variants reference to each collumn in
    // the data storage
    vec_colVar_t vec_colVariants;


    // CONSTRUCTION
    DataStore() : DataStore(vec_pr_strVarVec_t()) {}

    // Pair first = name of the column, Pair second = values in that column
    DataStore(vec_pr_strVarVec_t const &vecOfDataVecs);

    // APPENDING
    void append_data(vec_pr_strVarVec_t const &vecOfDataVecs);

    void append_fakeLabelCol(size_t const sz);
};

} // namespace terminal_plot
} // namespace incom