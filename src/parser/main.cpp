#include <common/common.hpp>

#include <iostream>

std::string get_user_input(const std::string &message) {
    std::cout << message << "\n"
              << "> ";
    std::string input;
    getline(std::cin, input);
    return input;
}

int main() {
    std::string text = get_user_input("Enter the text to be parsed:");
    std::unordered_set<std::string> stop_words = NgramParser::split_stop_words(
        get_user_input("Enter the stop words separated with spaces:"), ' ');
    size_t minlen = static_cast<size_t>(
        std::stoi(get_user_input("Enter minimum ngram length:")));
    size_t maxlen = static_cast<size_t>(
        std::stoi(get_user_input("Enter maximum ngram length:")));
    NgramParser parser;
    for (const std::string &token :
         parser.parse(text, stop_words, minlen, maxlen)) {
        std::cout << token << ' ';
    }
    std::cout << "\n";
    return 0;
}