#include <algorithm>
#include <cmath>
#include <common/common.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <picosha2.h>
#include <searcher/searcher.hpp>
#include <string>

constexpr auto IDX_DOESNT_CONTAIN = "!!";

Config TextIndexAccessor::config() const { return config_; }

TermInfos TextIndexAccessor::get_term_infos(const IndexTerm &term) const {
  const IndexHash hash = picosha2::hash256_hex_string(term).substr(0, 6);
  const IndexPath fullPath = path_ / "index" / "entries" / hash;
  if (!std::filesystem::exists(fullPath)) {
    return IDX_DOESNT_CONTAIN;
  }
  std::ifstream termFile(fullPath);
  if (!termFile.is_open()) {
    throw std::runtime_error("Can't open term file");
  }
  TermInfos termInfos("");
  std::string line;
  while (std::getline(termFile, line)) {
    termInfos += line;
  }
  return termInfos;
}

IndexText TextIndexAccessor::load_document(IndexID doc_id) const {
  const std::string doc_name = std::to_string(doc_id);
  std::ifstream docFile(path_ / "index" / "docs" / doc_name);
  if (!docFile.is_open()) {
    throw std::runtime_error("Can't open document file " + doc_name);
  }
  IndexText docText("");
  std::string line;
  while (std::getline(docFile, line)) {
    docText += line;
  }
  return docText;
}

DocsCount TextIndexAccessor::total_docs() const {
  DocsCount c = 0;
  for (const auto &entry :
       std::filesystem::directory_iterator(path_ / "index" / "docs")) {
    (void)entry;
    ++c;
  }
  return c;
}

Results searcher::search(const SearcherQuery &query,
                         const TextIndexAccessor &ia) {
  const NgramParser parser;
  const Config cfg = ia.config();
  const NgramVec parsed =
      parser.parse(query, cfg.stop_words, cfg.min_length, cfg.max_length);
  std::map<IndexID, double> scores;
  for (const auto &ngram : parsed) {
    const TermInfos termInfos = ia.get_term_infos(ngram.text);
    if (termInfos == IDX_DOESNT_CONTAIN) {
      continue;
    }
    NgramWords words = parser.split_in_words(termInfos);
    const double doc_frequency = std::stod(words[1]);
    for (std::size_t i = 2; i < words.size() - 1; ++i) {
      auto doc_id = static_cast<std::size_t>(std::stoi(words[i]));
      ++i;
      const double term_frequency = std::stod(words[i]);
      i += static_cast<std::size_t>(term_frequency);
      auto N = static_cast<double>(static_cast<int>(ia.total_docs()));
      const double idf = log(N) - log(doc_frequency);
      const double tf_idf = term_frequency * idf;
      scores[doc_id] += tf_idf;
    }
  }
  Results results;
  for (auto &score : scores) {
    results.push_back({score.first, score.second});
  }
  auto compare_desc_by_score = [](const Result &a, const Result &b) {
    return a.score > b.score;
  };
  std::sort(results.begin(), results.end(), compare_desc_by_score);
  return results;
}