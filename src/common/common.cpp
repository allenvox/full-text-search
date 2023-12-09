#include <common/common.hpp>

#include <algorithm>
#include <cctype>
#include <string>

bool NgramParser::is_stop_word(const std::string &word,
                               const NgramStopWords &stop_words) const {
  return std::find(stop_words.begin(), stop_words.end(), word) !=
         stop_words.end();
}

std::string NgramParser::clear_text(const std::string &source) const {
  std::string text; // buffer for formatted text
  text.reserve(source.length());

  // remove punctuation marks
  for (const char c : source) {
    if (std::ispunct(c) == 0) {
      text += c;
    }
  }

  // make lowercase
  std::transform(text.begin(), text.end(), text.begin(),
                 static_cast<int (*)(int)>(std::tolower));
  return text;
}

NgramWords NgramParser::split_in_words(const std::string &text,
                                       const char separator) const {
  NgramWords words;
  size_t start = 0;
  size_t end = text.find(separator);
  while (end != std::string::npos) {
    const std::string word = text.substr(start, end - start);
    words.push_back(word);
    start = end + 1;
    end = text.find(' ', start);
  }
  words.push_back(text.substr(start));
  return words;
}

NgramWords
NgramParser::remove_stop_words(const NgramWords &words,
                               const NgramStopWords &stop_words) const {
  NgramWords words_without_stops;
  for (const std::string &word : words) {
    if (!is_stop_word(word, stop_words)) {
      words_without_stops.push_back(word);
    }
  }
  return words_without_stops;
}

Ngrams NgramParser::generate_ngrams(const NgramWords &words,
                                    const NgramStopWords &stop_words,
                                    const size_t ngram_min_length,
                                    const size_t ngram_max_length) const {
  Ngrams ngrams;
  for (const auto &word : words) {
    for (size_t length = ngram_min_length;
         length <= ngram_max_length && length <= word.length(); length++) {
      const std::string ngram_text = word.substr(0, length);
      if (!is_stop_word(ngram_text, stop_words)) {
        Ngram ngram;
        ngram.text = ngram_text;
        ngram.pos = &word - words.data();
        ngrams.push_back(ngram);
      }
    }
  }
  return ngrams;
}

Ngrams NgramParser::parse(const std::string &text,
                          const NgramStopWords &stop_words,
                          size_t ngram_min_length,
                          size_t ngram_max_length) const {
  const std::string clear = clear_text(text);
  const NgramWords words = remove_stop_words(split_in_words(clear), stop_words);

  // generate a vector of ngrams
  Ngrams ngrams =
      generate_ngrams(words, stop_words, ngram_min_length, ngram_max_length);
  return ngrams;
}