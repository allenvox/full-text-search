#pragma once

#include <string>
#include <unordered_set>
#include <vector>

class Parser {
  public:
    virtual std::vector<std::string>
    parse(const std::string &text,
          const std::vector<std::string> &stop_words,
          size_t ngram_min_length, size_t ngram_max_length) const = 0;
};

class NgramParser : public Parser {
  public:
    std::string clear_text(const std::string &source) const;

    std::vector<std::string> split_in_words(const std::string &text,
                                            const char separator) const;

    std::vector<std::string>
    remove_stop_words(const std::vector<std::string> &words,
                      const std::vector<std::string> &stop_words) const;

    std::vector<std::string>
    generate_ngrams(const std::vector<std::string> &words,
                    const std::vector<std::string> &stop_words,
                    const size_t ngram_min_length,
                    const size_t ngram_max_length) const;

    std::vector<std::string>
    parse(const std::string &text,
          const std::vector<std::string> &stop_words,
          const size_t ngram_min_length,
          const size_t ngram_max_length) const override;
};