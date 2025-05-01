#pragma once

#include "incplot/config.hpp"
#include <iostream>
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
    DataStore(vec_pr_strVarVec_t const &vecOfDataVecs) {

        // Create data descriptors and the structure
        for (auto const &[colName, dataVect] : vecOfDataVecs) {
            if (colName == "0" || colName == "") { colNames.push_back(Config::noLabel); }
            else { colNames.push_back(colName); }

            if (std::holds_alternative<std::vector<std::string>>(dataVect)) {
                colTypes.push_back({parsedVal_t::string_like, stringCols.size()});
                stringCols.push_back(decltype(stringCols)::value_type());
            }
            else if (std::holds_alternative<std::vector<double>>(dataVect)) {
                colTypes.push_back({parsedVal_t::double_like, doubleCols.size()});
                doubleCols.push_back(decltype(doubleCols)::value_type());
            }
            else if (std::holds_alternative<std::vector<long long>>(dataVect)) {
                colTypes.push_back({parsedVal_t::signed_like, llCols.size()});
                llCols.push_back(decltype(llCols)::value_type());
            }
        }

        append_data(vecOfDataVecs);
        // Append 'fake' label column of strings if there is just one val column
        if (colTypes.size() == 1) {
            auto getSize = [&](auto const &vec) -> size_t { return vec.size(); };
            if (colTypes.at(0).first != parsedVal_t::string_like) {
                append_fakeLabelCol(std::visit(getSize, vecOfDataVecs.at(0).second));
            }
        }

        auto build_vecOfColVariants = [&]() {
            for (auto const &[ct_t, colID] : colTypes) {
                if (ct_t == parsedVal_t::string_like) { vec_colVariants.push_back(stringCols[colID]); }
                else if (ct_t == parsedVal_t::double_like) { vec_colVariants.push_back(doubleCols[colID]); }
                else if (ct_t == parsedVal_t::signed_like) { vec_colVariants.push_back(llCols[colID]); }
                else {}
            }
        };
        build_vecOfColVariants();
    }

    // APPENDING
    void append_data(vec_pr_strVarVec_t const &vecOfDataVecs) {

        if (colTypes.size() != vecOfDataVecs.size()) {
            std::cerr << "Impossible to append data to DataStore.\n";
            std::cerr << "colTypes.size in DataStore object = " + std::to_string(colTypes.size()) + ".\n" +
                             "vecOfDataVecs.size() = " + std::to_string(vecOfDataVecs.size()) + ".";
            std::exit(1);
        }

        for (int id = 0; auto const &[colt_t, idInDataVec] : colTypes) {
            if (colt_t == parsedVal_t::string_like) {
                stringCols.at(idInDataVec) = std::get<std::vector<std::string>>(vecOfDataVecs.at(id).second);
            }
            else if (colt_t == parsedVal_t::double_like) {
                doubleCols.at(idInDataVec) = std::get<std::vector<double>>(vecOfDataVecs.at(id).second);
            }
            else if (colt_t == parsedVal_t::signed_like) {
                llCols.at(idInDataVec) = std::get<std::vector<long long>>(vecOfDataVecs.at(id).second);
            }
            else {}
            ++id;
        }
    }

    void append_fakeLabelCol(size_t const sz) {

        colNames.push_back(Config::noLabel);
        colTypes.push_back({parsedVal_t::string_like, stringCols.size()});

        stringCols.push_back(decltype(stringCols)::value_type(sz, ""));
    }
};

} // namespace terminal_plot
} // namespace incom