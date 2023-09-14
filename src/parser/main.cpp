#include <common/common.hpp>

#include <iostream>

NgramText get_user_input(const NgramText &message) {
    std::cout << message << '\n' << "> ";
    NgramText input;
    getline(std::cin, input);
    return input;
}

int main() {
    const NgramText text = get_user_input("Enter the text to be parsed:");
    const NgramParser parser;
    const NgramStopWords stop_words = parser.split_in_words(
        get_user_input("Enter the stop words separated with spaces:"), ' ');
    const NgramLength minlen = static_cast<size_t>(
        std::stoi(get_user_input("Enter minimum ngram length:")));
    const NgramLength maxlen = static_cast<size_t>(
        std::stoi(get_user_input("Enter maximum ngram length:")));
    for (const Ngram &ngram : parser.parse(text, stop_words, minlen, maxlen)) {
        std::cout << ngram.text << ' ' << ngram.pos << ' ';
    }
    std::cout << std::endl;
    return 0;
}