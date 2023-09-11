
#include <common/common.hpp>

#include <algorithm>
#include <cctype>
#include <string>

std::string NgramParser::clear_text(const std::string &source) const {
    std::string text; // buffer for formatted text
    // remove punctuation marks
    for (char c : source) {
        if (std::ispunct(c) == 0) {
            text += c;
        }
    }
    // make lowercase
    std::transform(text.begin(), text.end(), text.begin(),
                   static_cast<int (*)(int)>(std::tolower));
    return text;
}

std::vector<std::string>
NgramParser::split_in_words(const std::string &text,
                            const char separator) const {
    std::vector<std::string> words;
    size_t start = 0;
    size_t end = text.find(separator);
    while (end != std::string::npos) {
        std::string word = text.substr(start, end - start);
        words.push_back(word);
        start = end + 1;
        end = text.find(' ', start);
    }
    words.push_back(text.substr(start));
    return words;
}

std::vector<std::string> NgramParser::remove_stop_words(
    const std::vector<std::string> &words,
    const std::vector<std::string> &stop_words) const {
    std::vector<std::string> removed_stop_words;
    for (const std::string &word : words) {
        if (std::find(stop_words.begin(), stop_words.end(), word) ==
            stop_words.end()) {
            removed_stop_words.push_back(word);
        }
    }
    return removed_stop_words;
}

std::vector<std::string>
NgramParser::generate_ngrams(const std::vector<std::string> &words,
                             const std::vector<std::string> &stop_words,
                             const size_t ngram_min_length,
                             const size_t ngram_max_length) const {
    std::vector<std::string> ngrams;
    for (const std::string &word : words) {
        for (size_t length = ngram_min_length;
             length <= ngram_max_length && length <= word.length(); length++) {
            std::string ngram = word.substr(0, length);
            if (std::find(stop_words.begin(), stop_words.end(), ngram) ==
                stop_words.end()) {
                ngrams.push_back(ngram + ' ' +
                                 std::to_string(&word - &words[0]));
            }
        }
    }
    return ngrams;
}

std::vector<std::string>
NgramParser::parse(const std::string &text,
                   const std::vector<std::string> &stop_words,
                   size_t ngram_min_length, size_t ngram_max_length) const {
    std::string clear = clear_text(text);
    std::vector<std::string> words =
        remove_stop_words(split_in_words(clear, ' '), stop_words);

    /*
    // split text in words
    std::vector<std::string> words;
    size_t start = 0;
    size_t end = clean_text.find(' ');
    while (end != std::string::npos) {
        std::string word = clean_text.substr(start, end - start);
        if (stop_words.find(word) == stop_words.end()) {
            words.push_back(word);
        }
        start = end + 1;
        end = clean_text.find(' ', start);
    }
    std::string last_word = clean_text.substr(start);
    if (stop_words.find(last_word) == stop_words.end()) {
        words.push_back(last_word);
    }*/

    // generate a vector of ngrams
    std::vector<std::string> ngrams =
        generate_ngrams(words, stop_words, ngram_min_length, ngram_max_length);
    return ngrams;
}