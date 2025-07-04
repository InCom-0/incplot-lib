#include <iostream>

#include <incplot/config.hpp>
#include <incplot/datastore.hpp>

namespace incom {
namespace terminal_plot {

// Data storage for the actual data that are to be plotted
DataStore::DataStore(DataStore::DS_CtorObj const &ctorObj) {

    // Create data descriptors and the structure
    for (auto const &[colName, dataVect] : ctorObj.data) {
        if (colName == "0" || colName == "" || colName == " ") { colNames.push_back(Config::noLabel); }
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

        itemFlags.push_back({});
    }

    append_data(ctorObj.data);
    // Append 'fake' label column of strings if there is just one val column
    if (colTypes.size() == 1) {
        auto getSize = [&](auto const &vec) -> size_t { return vec.size(); };
        if (colTypes.at(0).first != parsedVal_t::string_like) {
            append_fakeLabelCol(std::visit(getSize, ctorObj.data.at(0).second));
            itemFlags.push_back(std::vector<unsigned char>(std::visit(getSize, ctorObj.data.at(0).second), 0));
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
void DataStore::append_data(vec_pr_strVarVec_t const &vecOfDataVecs) {

    if (colTypes.size() != vecOfDataVecs.size()) {
        std::cerr << "Impossible to append data to DataStore.\n";
        std::cerr << "colTypes.size in DataStore object = " + std::to_string(colTypes.size()) + ".\n" +
                         "vecOfDataVecs.size() = " + std::to_string(vecOfDataVecs.size()) + ".";
        std::exit(1);
    }

    for (int id = 0; auto const &[colt_t, idInDataVec] : colTypes) {
        if (colt_t == parsedVal_t::string_like) {
            stringCols.at(idInDataVec).append_range(std::get<std::vector<std::string>>(vecOfDataVecs.at(id).second));
        }
        else if (colt_t == parsedVal_t::double_like) {
            doubleCols.at(idInDataVec).append_range(std::get<std::vector<double>>(vecOfDataVecs.at(id).second));
        }
        else if (colt_t == parsedVal_t::signed_like) {
            llCols.at(idInDataVec).append_range(std::get<std::vector<long long>>(vecOfDataVecs.at(id).second));
        }
        else {}
        ++id;
    }
}

void DataStore::append_data(DataStore::DS_CtorObj const &ctorObj) {

    if (colTypes.size() != ctorObj.data.size()) {
        std::cerr << "Impossible to append data to DataStore.\n";
        std::cerr << "colTypes.size in DataStore object = " + std::to_string(colTypes.size()) + ".\n" +
                         "vecOfDataVecs.size() = " + std::to_string(ctorObj.data.size()) + ".";
        std::exit(1);
    }

    auto app = [&] (auto const &inp) {
        
    };

    for (int id = 0; auto const &[colt_t, idInDataVec] : colTypes) {
        if (colt_t == parsedVal_t::string_like) {
            stringCols.at(idInDataVec).append_range(std::get<std::vector<std::string>>(ctorObj.data.at(id).second));
        }
        else if (colt_t == parsedVal_t::double_like) {
            doubleCols.at(idInDataVec).append_range(std::get<std::vector<double>>(ctorObj.data.at(id).second));
        }
        else if (colt_t == parsedVal_t::signed_like) {
            llCols.at(idInDataVec).append_range(std::get<std::vector<long long>>(ctorObj.data.at(id).second));
        }
        else {}

        itemFlags.at(id).append_range(ctorObj.itemFlags.at(id));
        ++id;
    }
}

void DataStore::append_fakeLabelCol(size_t const sz) {

    colNames.push_back(Config::noLabel);
    colTypes.push_back({parsedVal_t::string_like, stringCols.size()});

    stringCols.push_back(decltype(stringCols)::value_type(sz, ""));
}

} // namespace terminal_plot
} // namespace incom