#include <iostream>
#include <unordered_map>

#include <incplot/config.hpp>
#include <incplot/datastore.hpp>
#include <incplot/parsers_inc.hpp>
#include <incstd_all.hpp>
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

                for (auto const &item : std::get<std::remove_cvref_t<decltype(dataVari)>>(ctorObj.data.at(id).second)) {
                    dataVari.push_back(item);
                }
                // Once implemented in libc++
                // dataVari.append_range(std::get<std::remove_cvref_t<decltype(dataVari)>>(ctorObj.data.at(id).second));
            }
            else { assert(false); }
        };
        std::visit(visi, data);

        for (auto const &item : ctorObj.itemFlags.at(id)) { flags.push_back(item); }
        // Once implemented in libc++
        // flags.append_range(ctorObj.itemFlags.at(id));
        ++id;
    }
}

void DataStore::append_fakeLabelCol(size_t const sz) {
    m_data.push_back(Column{std::string(Config::noLabel), parsedVal_t::string_like, std::vector(sz, 0u),
                            std::vector<std::string>(sz, "")});
}

std::vector<unsigned int> DataStore::compute_filterFlags(std::vector<size_t> const  &colsToGet,
                                                         std::optional<double> const allowedStdDevitation) const {
    if (m_data.size() < 1) { assert(false); }

    std::vector<unsigned int> res(m_data.front().itemFlags.size(), 0u);

    // Filter flags for 'null' values based just on the selected columns
    for (auto const &selID : colsToGet) {
        // Non existent column ID or itemFlag sizes do not match
        if (selID >= m_data.size() || m_data.at(selID).itemFlags.size() != m_data.front().itemFlags.size()) {
            assert(false);
        }
        for (size_t i = 0; auto const &flag : m_data.at(selID).itemFlags) { res[i++] |= flag; }
    }

    // Filter based on standard deviation (excluding extreme values)
    if (allowedStdDevitation.has_value() && allowedStdDevitation.value() != 0.0) {
        std::vector<unsigned int> res2(m_data.front().itemFlags.size(), 0u);
        for (auto const &selID : colsToGet) {

            auto lam = [&](auto const &varVec) -> void {
                using v_t = std::remove_cvref_t<decltype(varVec)>::value_type;

                if constexpr (not std::is_arithmetic_v<v_t>) { return; }
                else {

                    auto v = std::views::zip(res, varVec) |
                             std::views::filter([](auto &&tpl) { return std::get<0>(tpl) == 0u; }) |
                             std::views::transform([](auto &&tpl2) { return std::get<1>(tpl2); });

                    v_t    sum   = 0;
                    size_t count = 0;
                    for (auto const &item : v) {
                        sum += item;
                        count++;
                    }
                    double avg = static_cast<double>(sum) / count;

                    auto stddev  = incom::standard::numeric::compute_stdDeviation(v);
                    stddev      *= allowedStdDevitation.value();
                    for (size_t i = 0; i < res.size(); ++i) {
                        if (res[i] == 0u) { res2[i] |= (std::abs(varVec[i] - avg) < stddev) ? 0b00 : 0b10; }
                    }
                    return;
                }
            };

            std::visit(lam, m_data.at(selID).variant_data);
        }
        for (size_t i = 0; i < res.size(); ++i) { res[i] |= res2[i]; }
    }
    return res;
}

std::optional<std::reference_wrapper<const DataStore>> DataStore::get_DS(std::string_view const &sv) {
    static std::unordered_map<std::string, const DataStore> storageMP;
    if (auto ele = storageMP.find(std::string(sv)); ele != storageMP.end()) { return ele->second; }
    else {
        auto data_sv = incstd::filesys::get_file_textual(sv);
        if (not data_sv.has_value()) { return std::nullopt; }

        auto newDS = incom::terminal_plot::parsers::Parser::parse(data_sv.value());
        if (newDS.has_value()) { return storageMP.insert({std::string(sv), std::move(newDS.value())}).first->second; }
        else { return std::nullopt; }
    };
}

} // namespace terminal_plot
} // namespace incom
