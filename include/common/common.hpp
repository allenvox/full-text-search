#pragma once

#include <string>
#include <unordered_set>
#include <vector>

class Parser {
  public:
    virtual std::vector<std::string>
    parse(const std::string &text,
          const std::unordered_set<std::string> &stop_words,
          size_t min_ngram_length, size_t max_ngram_length) const = 0;
};

class NgramParser : public Parser {
  public:
    std::vector<std::string>
    parse(const std::string &text,
          const std::unordered_set<std::string> &stop_words,
          size_t min_ngram_length, size_t max_ngram_length) const override;

    static std::unordered_set<std::string>
    split_stop_words(const std::string &stop_words_str, const char separator) {
        std::unordered_set<std::string> stop_words;
        size_t start = 0;
        size_t end = stop_words_str.find(separator);
        while (end != std::string::npos) {
            std::string word = stop_words_str.substr(start, end - start);
            stop_words.insert(word);
            start = end + 1;
            end = stop_words_str.find(separator, start);
        }
        std::string last_word = stop_words_str.substr(start);
        stop_words.insert(last_word);
        return stop_words;
    }
};