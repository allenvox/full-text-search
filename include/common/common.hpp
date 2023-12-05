#pragma once

#include <string>
#include <vector>

using NgramStopWords = std::vector<std::string>;
using NgramWords = std::vector<std::string>;
struct Ngram {
  std::string text;
  size_t pos;
};
using Ngrams = std::vector<Ngram>;

class Parser {
public:
  virtual Ngrams parse(const std::string &text,
                       const NgramStopWords &stop_words,
                       size_t ngram_min_length,
                       size_t ngram_max_length) const = 0;
};

class NgramParser : public Parser {
public:
  bool is_stop_word(const std::string &word,
                    const NgramStopWords &stop_words) const;

  std::string clear_text(const std::string &source) const;

  NgramWords split_in_words(const std::string &text,
                            char separator = ' ') const;

  NgramWords remove_stop_words(const NgramWords &words,
                               const NgramStopWords &stop_words) const;

  Ngrams generate_ngrams(const NgramWords &words,
                         const NgramStopWords &stop_words,
                         size_t ngram_min_length,
                         size_t ngram_max_length) const;

  Ngrams parse(const std::string &text, const NgramStopWords &stop_words,
                 size_t ngram_min_length,
                 size_t ngram_max_length) const override;
};