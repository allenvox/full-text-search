#include <cxxopts.hpp>
#include <fts/fts.hpp>
#include <iostream>

int main(int argc, char **argv) {
    cxxopts::Options options("sum");
    try {
        // clang-format off
        options.add_options()
          ("first", "Floating-point number", cxxopts::value<double>())
          ("second", "Floating-point number", cxxopts::value<double>());
        // clang-format on

        const auto result = options.parse(argc, argv);
        if (result.count("first") != 1 && result.count("second") != 1) {
            std::cout << options.help() << "\n";
            return 0;
        }
        const auto a = result["first"].as<double>();
        const auto b = result["second"].as<double>();
        std::cout << fts::sum(a, b) << "\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}