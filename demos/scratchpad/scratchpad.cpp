#include <incplot.hpp>
#include <iostream>
#include <print>

#include <ranges>
#include <string_view>

using json = nlohmann::json;


int main() {
    std::string cont((std::istreambuf_iterator(std::cin)), std::istreambuf_iterator<char>());

    while (cont.back() == '\n') { cont.pop_back(); }

    for (auto oneLine :
         std::views::split(cont, '\n') | std::views::transform([](auto const &in) { return std::string_view(in); })) {

        std::cout << oneLine << '\n';
        nlohmann::basic_json<> yyy;

        try {
            yyy = json::parse(oneLine);
        }
        catch (const json::exception &e) {
            std::cout << e.what() << '\n';
            return 1;
        }

        for (auto &[key, val] : yyy.items()) { std::cout << key << ": " << val.type_name() << ": " << val << '\n'; }
        std::cout << '\n';
    }
    // std::cout << std::string_view(sv);

    return 0;
}