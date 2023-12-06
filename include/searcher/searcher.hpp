#pragma once

#include <binary/binary.hpp>
#include <config/config.hpp>

#include <filesystem>
#include <sys/mman.h>
#include <unordered_map>
#include <vector>

struct Result {
  size_t doc_id;
  double score;
};
using Results = std::vector<Result>;

class IndexAccessor {
public:
  virtual Config config() const = 0;
  virtual std::string get_term_infos(const std::string &term) const = 0;
  virtual std::string load_document(size_t doc_id) const = 0;
  virtual size_t total_docs() const = 0;
};

class TextIndexAccessor : public IndexAccessor {
public:
  explicit TextIndexAccessor(std::filesystem::path &path,
                             Config &config = DEFAULT_CONFIG)
      : config_(std::move(config)), path_(std::move(path)){};
  Config config() const override { return config_; };
  std::string get_term_infos(const std::string &term) const override;
  std::string load_document(size_t doc_id) const override;
  size_t total_docs() const override;

private:
  Config config_;
  std::filesystem::path path_;
};

class DocumentsAccessor {
public:
  explicit DocumentsAccessor(
      BinaryHandler &data,
      std::unordered_map<std::string, size_t> &section_offsets)
      : data_(data), section_offsets_(&section_offsets) {
    data_.set_current_position(section_offsets_->at("docs"));
    data_.read(total_docs_);
  }
  size_t total_docs() const { return total_docs_; }
  std::string load_document(size_t offset);

private:
  BinaryHandler data_;
  std::unordered_map<std::string, size_t> *section_offsets_;
  uint32_t total_docs_{0};
};

class DictionaryAccessor {
public:
  explicit DictionaryAccessor(
      BinaryHandler &data,
      std::unordered_map<std::string, size_t> &section_offsets)
      : data_(data), section_offsets_(&section_offsets) {}
  size_t retrieve(const std::string &word);

private:
  BinaryHandler data_;
  std::unordered_map<std::string, size_t> *section_offsets_;
};

class EntriesAccessor {
public:
  explicit EntriesAccessor(
      BinaryHandler &data,
      std::unordered_map<std::string, size_t> &section_offsets)
      : data_(data), section_offsets_(&section_offsets) {
    dia_ = new DictionaryAccessor(data_, section_offsets);
  }
  std::string get_term_infos(const std::string &term);
  ~EntriesAccessor() { delete dia_; }

private:
  BinaryHandler data_;
  std::unordered_map<std::string, size_t> *section_offsets_;
  DictionaryAccessor *dia_;
};

class BinaryIndexAccessor : public IndexAccessor {
public:
  explicit BinaryIndexAccessor(const std::filesystem::path &path,
                               Config &config = DEFAULT_CONFIG);
  BinaryIndexAccessor(BinaryIndexAccessor &other) = default;
  BinaryIndexAccessor(BinaryIndexAccessor &&other) = default;
  ~BinaryIndexAccessor();
  BinaryIndexAccessor &operator=(const BinaryIndexAccessor &other) = default;
  BinaryIndexAccessor &operator=(BinaryIndexAccessor &&other) = default;
  Config config() const override { return config_; }

  size_t total_docs() const override { return doa_->total_docs(); }

  std::string load_document(size_t id) const override {
    return doa_->load_document(id);
  }

  std::string get_term_infos(const std::string &term) const override {
    return ea_->get_term_infos(term);
  }

private:
  int fd_;
  BinaryHandler data_;
  std::unordered_map<std::string, size_t> section_offsets_;
  DocumentsAccessor *doa_;
  EntriesAccessor *ea_;
  Config config_;
};

namespace searcher {

Results search(const std::string &query, const IndexAccessor &ia);

} // namespace searcher