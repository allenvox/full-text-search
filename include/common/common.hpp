#pragma once

#include <string>
#include <utility>
#include <vector>

using NgramStopWords = std::vector<std::string>;
using NgramWords = std::vector<std::string>;
using NgramLength = std::size_t;
using NgramIndex = std::size_t;
using NgramText = std::string;
using NgramWord = std::string;
using NgramPair = std::pair<NgramWord, NgramIndex>;
using NgramPairs = std::vector<NgramPair>;

class Parser {
  public:
    virtual NgramPairs parse(const NgramText &text,
                             const NgramStopWords &stop_words,
                             NgramLength ngram_min_length,
                             NgramLength ngram_max_length) const = 0;
};

class NgramParser : public Parser {
  public:
    bool is_stop_word(const NgramWord &word,
                      const NgramStopWords &stop_words) const;

    NgramText clear_text(const NgramText &source) const;

    NgramWords split_in_words(const NgramText &text,
                              const char separator = ' ') const;

    NgramWords remove_stop_words(const NgramWords &words,
                                 const NgramStopWords &stop_words) const;

    NgramPairs generate_ngrams(const NgramWords &words,
                               const NgramStopWords &stop_words,
                               const NgramLength ngram_min_length,
                               const NgramLength ngram_max_length) const;

    NgramPairs parse(const NgramText &text, const NgramStopWords &stop_words,
                     const NgramLength ngram_min_length,
                     const NgramLength ngram_max_length) const override;
};