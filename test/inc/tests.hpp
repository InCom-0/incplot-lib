#pragma once

#include <filesystem>
#include <fstream>
#include <incplot.hpp>
#include <ranges>
#include <tests_config.hpp>
#include <unordered_map>


namespace incom::terminal_plot::testing {

// More efficient when FP (filepath) is known at compile-time
template <std::string_view const &SV>
inline static const std::optional<std::string_view> get_data() {
    static std::ifstream                    ifs;
    static const std::optional<std::string> dataStorage{
        (ifs.open(std::filesystem::path(SV)),
         ifs.is_open()
             ? std::optional<std::string>(std::string(std::istreambuf_iterator(ifs), std::istreambuf_iterator<char>()))
             : std::optional<std::string>(std::nullopt))};

    if (ifs.is_open()) { ifs.close(); }
    return dataStorage.transform([](auto const &str) { return std::string_view(str); });
}
template <DataSetSV const &ds>
inline static DataSetSV get_oneDataSet() {
    return DataSetSV{.csv    = (ds.csv.has_value() ? get_data<ds.csv.value()>() : std::nullopt),
                     .tsv    = (ds.tsv.has_value() ? get_data<ds.tsv.value()>() : std::nullopt),
                     .json   = (ds.json.has_value() ? get_data<ds.json.value()>() : std::nullopt),
                     .ndjson = (ds.ndjson.has_value() ? get_data<ds.ndjson.value()>() : std::nullopt)};
}
template <DataSetSV const &...dss>
inline static std::vector<DataSetSV> get_dataSets() {
    std::vector<DataSetSV> res;
    (res.push_back(get_oneDataSet<dss>()), ...);
    return res;
}


// Storing the loaded text files in a static variable
inline static std::optional<std::string_view> get_data(std::string_view const &sv) {
    static std::unordered_map<std::string, std::string> storageMP;
    if (auto ele = storageMP.find(std::string(sv)); ele != storageMP.end()) { return std::string_view(ele->second); }
    else {
        std::ifstream ifs;
        ifs.open(std::filesystem::path(sv));
        if (ifs.is_open()) {
            return storageMP.insert({std::string(sv), std::string(std::from_range, std::views::istream<char>(ifs))})
                .first->second;
        }
        else { return std::nullopt; }
    };
}
inline static DataSetSV get_oneDataSet(DataSetSV const &ds) {
    return DataSetSV{.csv    = (ds.csv.has_value() ? get_data(ds.csv.value()) : std::nullopt),
                     .tsv    = (ds.tsv.has_value() ? get_data(ds.tsv.value()) : std::nullopt),
                     .json   = (ds.json.has_value() ? get_data(ds.json.value()) : std::nullopt),
                     .ndjson = (ds.ndjson.has_value() ? get_data(ds.ndjson.value()) : std::nullopt)};
}
inline static std::vector<DataSetSV> get_dataSets(auto const &containerOf_ds) {
    std::vector<DataSetSV> res;
    for (auto const &item : containerOf_ds) { res.push_back(get_oneDataSet(item)); }
    return res;
}


// Generating a tuple with of N times the supplied type
// Usage: pass the N parameter and the T type only, leave the ...Ts pack empty
// Note: If you don't adhere to the above the output tuple will have more 'member types' than you want. Beware.
// Note: Uses recursive template instantiation.
// Note: T and Ts all must be the same types.
template <size_t N, typename T, typename... Ts>
requires(std::same_as<T, Ts> && ...)
struct _c_generateTuple {
    using type = typename _c_generateTuple<N - 1, T, T, Ts...>::type;
};
template <typename T, typename... Ts>
requires(std::same_as<T, Ts> && ...)
struct _c_generateTuple<0, T, Ts...> {
    using type = std::tuple<Ts...>;
};

} // namespace incom::terminal_plot::testing