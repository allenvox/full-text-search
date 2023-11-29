#pragma once

#include <string>
#include <vector>

using NgramStopWords = std::vector<std::string>;
using NgramWords = std::vector<std::string>;
using NgramLength = std::size_t;
using NgramIndex = std::size_t;
using NgramText = std::string;
using NgramWord = std::string;
struct Ngram {
  std::string text;
  std::size_t pos;
};
using Ngram = struct Ngram;
using NgramVec = std::vector<Ngram>;

class Parser {
public:
  virtual NgramVec parse(const NgramText &text,
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
                            char separator = ' ') const;

  NgramWords remove_stop_words(const NgramWords &words,
                               const NgramStopWords &stop_words) const;

  NgramVec generate_ngrams(const NgramWords &words,
                           const NgramStopWords &stop_words,
                           NgramLength ngram_min_length,
                           NgramLength ngram_max_length) const;

  NgramVec parse(const NgramText &text, const NgramStopWords &stop_words,
                 NgramLength ngram_min_length,
                 NgramLength ngram_max_length) const override;
};