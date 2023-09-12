#pragma once

#include <string>
#include <unordered_set>
#include <vector>

using NgramStopWords = std::vector<std::string>;
using NgramWords = std::vector<std::string>;
using NgramLength = std::size_t;
using NgramText = std::string;
using NgramWord = std::string;

class Parser {
  public:
    virtual NgramWords parse(const NgramText &text,
                             const NgramStopWords &stop_words,
                             NgramLength ngram_min_length,
                             NgramLength ngram_max_length) const = 0;
};

class NgramParser : public Parser {
  public:
    NgramText clear_text(const NgramText &source) const;

    NgramWords split_in_words(const NgramText &text,
                              const char separator) const;

    NgramStopWords remove_stop_words(const NgramWords &words,
                                     const NgramStopWords &stop_words) const;

    NgramWords generate_ngrams(const NgramWords &words,
                               const NgramStopWords &stop_words,
                               const NgramLength ngram_min_length,
                               const NgramLength ngram_max_length) const;

    NgramWords parse(const NgramText &text, const NgramStopWords &stop_words,
                     const NgramLength ngram_min_length,
                     const NgramLength ngram_max_length) const override;
};