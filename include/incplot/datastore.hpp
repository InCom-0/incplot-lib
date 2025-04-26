#pragma once

#include <cmath>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

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
    using NLMjson            = nlohmann::ordered_json;
    using vec_pr_strVarVec_t = std::vector<
        std::pair<std::string, std::variant<std::vector<std::string>, std::vector<double>, std::vector<long long>>>>;
    using vec_colVar_t = std::vector<
        std::variant<std::reference_wrapper<std::vector<std::string>>, std::reference_wrapper<std::vector<long long>>,
                     std::reference_wrapper<std::vector<double>>>>;

    // The json this was constucted with/from ... possibly not strictly necessary to keep, but whatever
    std::vector<NLMjson> constructedWith;

    // TODO: Maybe provide my own 'ColType' enum ... consider
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

    DataStore(std::vector<NLMjson> &&jsonVec) : constructedWith(std::move(jsonVec)) {

        // Create the requisite data descriptors and the structure
        for (auto const &[key, val] : constructedWith.front().items()) {
            colNames.push_back(key);

            if (val.type() == NLMjson::value_t::string) {
                colTypes.push_back({parsedVal_t::string_like, stringCols.size()});
                stringCols.push_back(std::vector<std::string>());
            }
            else if (val.type() == NLMjson::value_t::number_float) {
                colTypes.push_back({parsedVal_t::double_like, doubleCols.size()});
                doubleCols.push_back(std::vector<double>());
            }
            else if (val.type() == NLMjson::value_t::number_integer ||
                     val.type() == NLMjson::value_t::number_unsigned) {
                colTypes.push_back({parsedVal_t::signed_like, llCols.size()});
                llCols.push_back(std::vector<long long>());
            }
        }

        auto build_vecOfColVariants = [&]() {
            for (auto const &colTypesPair : colTypes) {
                if (colTypesPair.first == parsedVal_t::string_like) {
                    vec_colVariants.push_back(stringCols[colTypesPair.second]);
                }
                else if (colTypesPair.first == parsedVal_t::string_like) {
                    vec_colVariants.push_back(llCols[colTypesPair.second]);
                }
                else if (colTypesPair.first == parsedVal_t::double_like) {
                    vec_colVariants.push_back(doubleCols[colTypesPair.second]);
                }
            }
        };
        build_vecOfColVariants();

        append_data(constructedWith);
    }
    // Pair first = name of the column, Pair second = values in that column
    DataStore(vec_pr_strVarVec_t const &vecOfDataVecs) {

        // Create data descriptors and the structure
        for (auto const &[colName, dataVect] : vecOfDataVecs) {
            colNames.push_back(colName);

            if (std::holds_alternative<std::vector<std::string>>(dataVect)) {
                colTypes.push_back({parsedVal_t::string_like, stringCols.size()});
            }
            else if (std::holds_alternative<std::vector<double>>(dataVect)) {
                colTypes.push_back({parsedVal_t::double_like, doubleCols.size()});
            }
            else if (std::holds_alternative<std::vector<long long>>(dataVect)) {
                colTypes.push_back({parsedVal_t::signed_like, llCols.size()});
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

        append_data(vecOfDataVecs);
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
                stringCols.push_back(std::get<std::vector<std::string>>(vecOfDataVecs.at(id).second));
            }
            else if (colt_t == parsedVal_t::double_like) {
                doubleCols.push_back(std::get<std::vector<double>>(vecOfDataVecs.at(id).second));
            }
            else if (colt_t == parsedVal_t::signed_like) {
                llCols.push_back(std::get<std::vector<long long>>(vecOfDataVecs.at(id).second));
            }
            else {}
            ++id;
        }
    }

    void append_data(std::vector<NLMjson> const &toAppend) {
        // For each json line ...
        for (auto const &oneJson : toAppend) {
            auto oneJsonIT = oneJson.items().begin();
            // ... go across the colTypes and add data to the right vector
            for (auto const &[ct_t, colID] : colTypes) {
                if (ct_t == parsedVal_t::string_like) { stringCols[colID].push_back(oneJsonIT.value()); }
                else if (ct_t == parsedVal_t::double_like) { doubleCols[colID].push_back(oneJsonIT.value()); }
                else if (ct_t == parsedVal_t::signed_like) {
                    llCols[colID].push_back(static_cast<long long>(oneJsonIT.value()));
                }

                ++oneJsonIT;
            }
        }
    }
};

} // namespace terminal_plot
} // namespace incom