#pragma once

#include <common/common.hpp>
#include <config/config.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>

using IndexID = std::size_t;
using IndexText = std::string;
using IndexDocuments = std::unordered_map<IndexID, IndexText>;
using IndexIdx = std::size_t;
struct IndexDocToPos {
  IndexID doc_id;
  IndexIdx pos;
};
using IndexDocToPos = struct IndexDocToPos;
using IndexHash = std::string;
using IndexTerm = std::string;
using IndexEntries = std::unordered_map<IndexTerm, std::vector<IndexDocToPos>>;
using BytesVec = std::vector<uint8_t>;

class Index {
public:
  IndexDocuments docs;
  IndexEntries entries;
};

class IndexBuilder {
public:
  IndexBuilder(NgramStopWords stop_words, NgramLength min_length,
               NgramLength max_length)
      : config_({std::move(stop_words), min_length, max_length}){};
  explicit IndexBuilder(Config &cfg = DEFAULT_CONFIG) : config_(std::move(cfg)){};
  Index index() const { return index_; };
  void add_document(IndexID id, const IndexText &text);

private:
  Index index_;
  Config config_;
};

using IndexPath = std::filesystem::path;

class IndexWriter {
public:
  virtual void write(IndexPath path, Index index) const = 0;
};

class TextIndexWriter : public IndexWriter {
public:
  void write(IndexPath path, Index index) const override;
};

class BinaryIndexWriter : public IndexWriter {
public:
  void write(IndexPath path, Index index) const override;
};

namespace indexer {

// TextIndexWriter
IndexHash term_to_hash(const IndexTerm &term);
void create_index_directories(const IndexPath &path);
void write_docs(const IndexPath &path, const IndexDocuments &docs);
IndexText
convert_to_entry_output(const IndexTerm &term,
                        const std::vector<IndexDocToPos> &doc_to_pos_vec);
void write_entries(const IndexPath &path, const IndexEntries &entries);

// BinaryIndexWriter
using IdToOffset = std::unordered_map<uint32_t, uint32_t>;
using TermToOffset = std::unordered_map<std::string, uint32_t>;
uint32_t get_offset(uint32_t id, const IdToOffset &id_to_offset);
void push_u32_to_u8(uint32_t val, BytesVec &vec);
BytesVec serialize_string(const std::string &str);
BytesVec serialize_docs(const IndexDocuments &docs, IdToOffset &id_to_offset);
BytesVec serialize_entries(const IndexEntries &entries,
                           const IdToOffset &id_to_offset,
                           TermToOffset &term_to_offset);
struct TrieNode {
  std::unordered_map<char, TrieNode*> children;
  bool is_leaf;
  uint32_t entry_offset;
};
using TrieNode = struct TrieNode;
void insertWord(TrieNode* root, const std::string &word, uint32_t entry_offset);
BytesVec serialize_dictionary(const IndexEntries &entries, const TermToOffset &term_to_offset);
void change_offset(BytesVec &header, uint32_t pos, uint32_t val);
BytesVec serialize_header(const BytesVec &dictionary, const BytesVec &entries);

} // namespace indexer