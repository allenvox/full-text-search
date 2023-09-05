#include <cxxopts.hpp>
#include <fts/fts.hpp>
#include <iostream>

int main(int argc, char **argv) {
    cxxopts::Options options("sqrt");
    try {
        // clang-format off
        options.add_options()
          ("a", "Floating-point number", cxxopts::value<double>())
          ("b", "Floating-point number", cxxopts::value<double>());
        // clang-format on

        const auto result = options.parse(argc, argv);
        if (argc != 2) {
            std::cout << options.help() << "\n";
            return 0;
        }
        const auto a = result["a"].as<double>();
        const auto b = result["b"].as<double>();
        std::cout << fts::sum(a, b) << "\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}