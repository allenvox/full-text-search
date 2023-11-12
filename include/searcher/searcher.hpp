#pragma once

#include <config/config.hpp>

#include <filesystem>
#include <vector>

struct Result {
  std::size_t doc_id;
  double score;
};
using Result = struct Result;
using Results = std::vector<Result>;
using TermInfos = std::string;
using SearcherQuery = std::string;
using DocsCount = std::size_t;

class IndexAccessor {
public:
  virtual Config config() const = 0;
  virtual TermInfos get_term_infos(const std::string &term) const = 0;
  virtual std::string load_document(std::size_t doc_id) const = 0;
  virtual DocsCount total_docs() const = 0;
};

class TextIndexAccessor : public IndexAccessor {
public:
  TextIndexAccessor(const std::filesystem::path &path,
                    Config &config = DEFAULT_CONFIG)
      : config_(std::move(config)), path_(std::move(path)){};
  Config config() const;
  TermInfos get_term_infos(const std::string &term) const;
  std::string load_document(std::size_t doc_id) const;
  DocsCount total_docs() const;

private:
  Config config_;
  std::filesystem::path path_;
};

namespace searcher {

Results search(const SearcherQuery &query, const TextIndexAccessor &ia);

} // namespace searcher