
#include <common/common.hpp>

#include <algorithm>
#include <cctype>
#include <string>

NgramText NgramParser::clear_text(const NgramText &source) const {
    NgramText text; // buffer for formatted text
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

NgramWords NgramParser::split_in_words(const NgramText &text,
                                       const char separator) const {
    NgramWords words;
    NgramLength start = 0;
    NgramLength end = text.find(separator);
    while (end != std::string::npos) {
        const NgramWord word = text.substr(start, end - start);
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
    NgramWords removed_stop_words;
    for (const NgramWord &word : words) {
        if (std::find(stop_words.begin(), stop_words.end(), word) ==
            stop_words.end()) {
            removed_stop_words.push_back(word);
        }
    }
    return removed_stop_words;
}

NgramWords
NgramParser::generate_ngrams(const NgramWords &words,
                             const NgramStopWords &stop_words,
                             const NgramLength ngram_min_length,
                             const NgramLength ngram_max_length) const {
    NgramWords ngrams;
    for (const NgramWord &word : words) {
        for (NgramLength length = ngram_min_length;
             length <= ngram_max_length && length <= word.length(); length++) {
            const NgramWord ngram = word.substr(0, length);
            if (std::find(stop_words.begin(), stop_words.end(), ngram) ==
                stop_words.end()) {
                ngrams.push_back(ngram + ' ' +
                                 std::to_string(&word - &words[0]));
            }
        }
    }
    return ngrams;
}

NgramWords NgramParser::parse(const std::string &text,
                              const NgramStopWords &stop_words,
                              NgramLength ngram_min_length,
                              NgramLength ngram_max_length) const {
    const NgramText clear = clear_text(text);
    const NgramWords words =
        remove_stop_words(split_in_words(clear, ' '), stop_words);

    // generate a vector of ngrams
    NgramWords ngrams =
        generate_ngrams(words, stop_words, ngram_min_length, ngram_max_length);
    return ngrams;
}