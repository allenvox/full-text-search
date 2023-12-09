#include <cxxopts.hpp>

#include <driver/driver.hpp>
#include <indexer/indexer.hpp>

#include <iostream>

std::string get_user_input(const std::string &message) {
  std::cout << message << '\n' << "> ";
  std::string input;
  getline(std::cin, input);
  return input;
}

const std::string csv_description = "path to CSV-file to index";
const std::string index_description = "path to folder to save index to";

int main(int argc, char **argv) {
  cxxopts::Options options("indexer-cli");
  // clang-format off
  options.add_options()
    ("csv", csv_description, cxxopts::value<IndexPath>())
    ("index", index_description, cxxopts::value<IndexPath>());
  // clang-format on

  const auto result = options.parse(argc, argv);
  IndexPath csv_path;
  IndexPath index_path;

  if (result.count("csv") == 1) {
    csv_path = result["csv"].as<IndexPath>();
  } else {
    csv_path = get_user_input("Enter " + csv_description + ':');
  }
  driver::check_if_exists(csv_path);

  if (result.count("index") == 1) {
    index_path = result["index"].as<IndexPath>();
  } else {
    index_path = get_user_input("Enter " + index_description + ':');
  }
  driver::check_if_exists(index_path);

  Index idx = driver::generate_index(csv_path);
  BinaryIndexWriter writer;
  writer.write(index_path, idx);
  return 0;
}