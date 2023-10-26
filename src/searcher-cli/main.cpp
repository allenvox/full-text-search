#include <cxxopts.hpp>

#include <driver/driver.hpp>
#include <searcher/searcher.hpp>

#include <iostream>

std::string get_user_input(const std::string &message) {
  std::cout << message << '\n' << "> ";
  std::string input;
  getline(std::cin, input);
  return input;
}

const std::string index_description = "path to folder with saved index";
const std::string query_description = "query to searcher";

int main(int argc, char **argv) {
  std::cout.setf(std::ios::fixed);
  std::cout.precision(4);
  IndexPath index_path;
  std::string query;

  cxxopts::Options options("searcher-cli");
  // clang-format off
  options.add_options()
    ("index", index_description, cxxopts::value<IndexPath>())
    ("query", query_description, cxxopts::value<std::string>());
  // clang-format on

  const auto result = options.parse(argc, argv);

  if (result.count("index") == 1) {
    index_path = result["index"].as<IndexPath>();
  } else {
    index_path = get_user_input("Enter " + index_description + ':');
  }
  driver::check_if_exists(index_path);

  if (result.count("query") == 1) {
    query = result["query"].as<std::string>();
  } else {
    query = get_user_input("Enter " + query_description + ':');
  }

  const TextIndexAccessor indexAccessor(index_path);
  const Results results = searcher::search(query, indexAccessor);
  std::cout << "id\tscore\ttext\n";
  for (const auto &result : results) {
    const IndexID id = result.doc_id;
    std::cout << id << '\t' << result.score << '\t'
              << indexAccessor.load_document(id) << '\n';
  }
  return 0;
}