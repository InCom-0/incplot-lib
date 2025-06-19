#pragma once

#include <filesystem>
#include <fstream>
#include <incplot.hpp>
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


// Storing the loaded text files in a static variable
inline static std::optional<std::string_view> get_data(std::string_view const &sv) {
    static std::unordered_map<std::string, std::string> storageMP;
    if (auto ele = storageMP.find(std::string(sv)); ele != storageMP.end()) { return std::string_view(ele->second); }
    else {
        std::ifstream ifs;
        ifs.open(std::filesystem::path(sv));
        if (ifs.is_open()) {
            return storageMP
                .insert({std::string(sv), std::string(std::istreambuf_iterator(ifs), std::istreambuf_iterator<char>())})
                .first->second;
        }
        else { return std::nullopt; }
    };
}

} // namespace incom::terminal_plot::testing