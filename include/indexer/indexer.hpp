#pragma once

#include <binary/binary.hpp>
#include <common/common.hpp>
#include <config/config.hpp>

#include <filesystem>
#include <unordered_map>
#include <vector>

struct IndexDocToPos {
  size_t doc_id;
  size_t pos;
};
using IndexDocuments = std::unordered_map<size_t, std::string>;
using IndexPath = std::filesystem::path;
using IndexEntries =
    std::unordered_map<std::string, std::vector<IndexDocToPos>>;

class Index {
public:
  IndexDocuments docs;
  IndexEntries entries;
};

class IndexBuilder {
public:
  IndexBuilder(const NgramStopWords &stop_words, size_t min_length,
               size_t max_length)
      : config_({stop_words, min_length, max_length}){};
  explicit IndexBuilder(Config &cfg = DEFAULT_CONFIG)
      : config_(std::move(cfg)){};
  Index index() const { return index_; };
  void add_document(size_t id, const std::string &text);

private:
  Index index_;
  Config config_;
};

class IndexWriter {
public:
  virtual void write(IndexPath &path, Index &index) = 0;
};

class TextIndexWriter : public IndexWriter {
public:
  void write(IndexPath &path, Index &index) override;
  std::string convert_to_entry_output(
      const std::string &term,
      const std::vector<IndexDocToPos> &doc_to_pos_vec) const;
  void create_index_directories(const IndexPath &path) const;
  std::string term_to_hash(const std::string &term) const;

private:
  void write_docs(const IndexPath &path, const IndexDocuments &docs) const;
  void write_entries(const IndexPath &path, const IndexEntries &entries) const;
};

struct TrieNode {
  char data;
  bool is_leaf{false};
  TrieNode *parent{nullptr};
  std::unordered_map<char, TrieNode *> children;
  explicit TrieNode(char c) : data(c) {}
};

class Trie {
public:
  Trie() {
    root_ = new TrieNode('\0');
    root_->is_leaf = false;
    root_->parent = nullptr;
  }
  Trie(Trie &other) = default;
  Trie(Trie &&other) noexcept = default;
  Trie &operator=(const Trie &other) = default;
  Trie &operator=(Trie &&other) noexcept = default;
  ~Trie() { clear(root_); }
  void insert(const std::string &word);
  std::string retrieve(const TrieNode *node);
  TrieNode *root() const { return root_; }
  size_t size() const { return size_; }

private:
  size_t size_{0};
  TrieNode *root_{nullptr};
  void clear(TrieNode *current) {
    for (auto &[c, node] : current->children) {
      clear(node);
    }
    delete current;
  }
};

class BinaryIndexWriter : public IndexWriter {
public:
  void write(IndexPath &path, Index &index) override;

private:
  enum { Docs, Entries, Dictionary };
  const std::unordered_map<size_t, std::string> sections_ = {
      {Docs, "docs"}, {Entries, "entries"}, {Dictionary, "dictionary"}};
  std::unordered_map<size_t, size_t> id_to_offset_;
  std::unordered_map<std::string, size_t> term_to_offset_;
  std::unordered_map<TrieNode *, std::pair<size_t, size_t>> children_offsets_;

  struct Header {
    std::unordered_map<std::string, size_t> section_offsets;
  } header_;

  void write_header(BinaryHandler &buf);
  void write_docs(BinaryHandler &buf, const Index &index);
  void write_entries(BinaryHandler &buf, const Index &index);
  void build_dictionary(BinaryHandler &buf, Trie &trie, TrieNode *current,
                        uint32_t begin);
  void write_dictionary(BinaryHandler &buf, const Index &index);
};