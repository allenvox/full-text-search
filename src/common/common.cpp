
#include <common/common.hpp>

#include <algorithm>
#include <cctype>
#include <string>

std::vector<std::string>
NgramParser::parse(const std::string &text,
                   const std::unordered_set<std::string> &stop_words,
                   size_t min_ngram_length, size_t max_ngram_length) const {
    // buffer for formatted text
    std::string clean_text;

    // remove punctuation marks
    for (char c : text) {
        if (std::ispunct(c) == 0) {
            clean_text += c;
        }
    }

    // make lowercase
    std::transform(clean_text.begin(), clean_text.end(), clean_text.begin(),
                   static_cast<int (*)(int)>(std::tolower));

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
    }

    // generate a vector of ngrams
    std::vector<std::string> ngrams;
    for (const std::string &word : words) {
        for (size_t length = min_ngram_length;
             length <= max_ngram_length && length <= word.length(); length++) {
            std::string ngram = word.substr(0, length);
            ngrams.push_back(ngram + ' ' + std::to_string(&word - &words[0]));
        }
    }

    // generate a result vector
    std::vector<std::string> result;
    for (const std::string &ngram : ngrams) {
        if (stop_words.find(ngram) == stop_words.end()) {
            result.push_back(ngram);
        }
    }
    return result;
}