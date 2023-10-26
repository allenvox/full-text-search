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
  IndexBuilder(Config& cfg = DEFAULT_CONFIG) : config_(std::move(cfg)){};
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
  void write(IndexPath path, Index index) const;
};

namespace indexer {

IndexHash term_to_hash(const IndexTerm &term);
void throw_index_fs_error();
void create_index_directories(const IndexPath &path);
void write_docs(const IndexPath &path, const IndexDocuments &docs);
IndexText
convert_to_entry_output(const IndexTerm &term,
                        const std::vector<IndexDocToPos> &doc_to_pos_vec);
void write_entries(const IndexPath &path, const IndexEntries &entries);

} // namespace indexer