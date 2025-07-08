#include <iostream>

#include <incplot/config.hpp>
#include <incplot/datastore.hpp>
#include <utility>

namespace incom {
namespace terminal_plot {

// Data storage for the actual data that are to be plotted
DataStore::DataStore(DataStore::DS_CtorObj const &ctorObj) {

    // Create data descriptors and the structure
    for (auto const &[colName, dataVect] : ctorObj.data) {
        DataStore::Column toInsert;
        if (colName == "0" || colName == "" || colName == " ") { toInsert.name = Config::noLabel; }
        else { toInsert.name = colName; }

        if (std::holds_alternative<std::vector<std::string>>(dataVect)) {
            toInsert.colType      = parsedVal_t::string_like;
            toInsert.variant_data = std::vector<std::string>{};
        }
        else if (std::holds_alternative<std::vector<double>>(dataVect)) {
            toInsert.colType      = parsedVal_t::double_like;
            toInsert.variant_data = std::vector<double>{};
        }
        else if (std::holds_alternative<std::vector<long long>>(dataVect)) {
            toInsert.colType      = parsedVal_t::signed_like;
            toInsert.variant_data = std::vector<long long>{};
        }
        m_data.push_back(std::move(toInsert));
    }

    append_data(ctorObj);
    // Append 'fake' label column of strings if there is just one val column
    if (m_data.size() == 1) {
        auto getSize = [&](auto const &vec) -> size_t { return vec.size(); };
        if (m_data.at(0).colType != parsedVal_t::string_like) {
            append_fakeLabelCol(std::visit(getSize, ctorObj.data.at(0).second));
        }
    }
}

void DataStore::append_data(DataStore::DS_CtorObj const &ctorObj) {

    if (m_data.size() != ctorObj.data.size()) {
        std::cerr << "Impossible to append data to DataStore.\n";
        std::cerr << "m_data.size in DataStore object = " + std::to_string(m_data.size()) + ".\n" +
                         "vecOfDataVecs.size() = " + std::to_string(ctorObj.data.size()) + ".";
        std::exit(1);
    }

    if (m_data.size() != ctorObj.data.size()) {
        std::cerr << "Impossible to append data to DataStore.\n";
        std::cerr << "m_data.size in DataStore object = " + std::to_string(m_data.size()) + ".\n" +
                         "vecOfDataVecs.size() = " + std::to_string(ctorObj.data.size()) + ".";
        std::exit(1);
    }

    for (int id = 0; auto &[_, colt_t, flags, data] : m_data) {
        auto visi = [&](auto &&dataVari) {
            if (std::holds_alternative<std::remove_cvref_t<decltype(dataVari)>>(ctorObj.data.at(id).second)) {
                dataVari.append_range(std::get<std::remove_cvref_t<decltype(dataVari)>>(ctorObj.data.at(id).second));
            }
            else { assert(false); }
        };

        std::visit(visi, data);
        flags.append_range(ctorObj.itemFlags.at(id));
        ++id;
    }
}

void DataStore::append_fakeLabelCol(size_t const sz) {
    m_data.push_back(
        Column{Config::noLabel, parsedVal_t::string_like, std::vector(sz, 0u), std::vector<std::string>(sz, "")});
}

} // namespace terminal_plot
} // namespace incom