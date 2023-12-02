#pragma once

#include <config/config.hpp>

#include <filesystem>
#include <sys/mman.h>
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
  explicit TextIndexAccessor(std::filesystem::path &path,
                             Config &config = DEFAULT_CONFIG)
      : config_(std::move(config)), path_(std::move(path)){};
  Config config() const override { return config_; };
  TermInfos get_term_infos(const std::string &term) const override;
  std::string load_document(std::size_t doc_id) const override;
  DocsCount total_docs() const override;

private:
  Config config_;
  std::filesystem::path path_;
};

class DictionaryAccessor {
public:
  explicit DictionaryAccessor(uint32_t begin, uint32_t end, std::vector<uint8_t> data)
      : data_(data.begin() + begin, data.begin() + end){};
  uint32_t retrieve(std::string word) const;
private:
  std::vector<uint8_t> data_;
};

class EntriesAccessor {
public:
  explicit EntriesAccessor(uint32_t begin, uint32_t end, std::vector<uint8_t> data)
      : data_(data.begin() + begin, data.begin() + end){};
  TermInfos get_term_infos(uint32_t offset) const;
private:
  std::vector<uint8_t> data_;
};

class DocumentsAccessor {
public:
  explicit DocumentsAccessor(uint32_t begin, uint32_t end, std::vector<uint8_t> data)
      : data_(data.begin() + begin, data.begin() + end){};
  std::string load_document(uint32_t id) const;
  uint32_t total_docs() const;
private:
  std::vector<uint8_t> data_;
};

class BinaryIndexAccessor : public IndexAccessor {
public:
  explicit BinaryIndexAccessor(std::filesystem::path &path,
                               Config &config = DEFAULT_CONFIG);
  void print_data() const;
  Config config() const override { return config_; };
  TermInfos get_term_infos(const std::string &term) const override {
    return ea_->get_term_infos(term.size());
  };
  std::string load_document(std::size_t doc_id) const override {
      return doa_->load_document(doc_id);
  };
  DocsCount total_docs() const override {
      return doa_->total_docs();
  };

private:
  uint32_t get_u32_from_u8s(uint32_t start, const std::vector<uint8_t> &bytes);
  Config config_;
  std::vector<uint8_t> data_;
  std::size_t fsize_;
  DictionaryAccessor *dia_;
  EntriesAccessor *ea_;
  DocumentsAccessor *doa_;
};

namespace searcher {

Results search(const SearcherQuery &query, const IndexAccessor &ia);

} // namespace searcher