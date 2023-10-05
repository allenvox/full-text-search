#include <iostream>
#include <searcher/searcher.hpp>

std::string get_user_input(const std::string &message) {
    std::cout << message << '\n' << "> ";
    std::string input;
    getline(std::cin, input);
    return input;
}

int main() {
    std::string idx_path = get_user_input("Enter input index path:");
    if (idx_path.empty()) {
        idx_path = "build";
    }
    const std::filesystem::path path = idx_path;
    const std::string query = get_user_input("Enter search query:");
    const TextIndexAccessor indexAccessor({{"the", "and", "of", "on"}, 3, 6},
                                          path);
    Results results = searcher::search(query, indexAccessor);
    std::cout << "id\tscore\ttext\n";
    for (auto &result : results) {
        IndexID id = result.doc_id;
        std::cout << id << '\t' << result.score << '\t'
                  << indexAccessor.load_document(id) << '\n';
    }
    return 0;
}