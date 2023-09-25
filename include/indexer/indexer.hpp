#pragma once

#include <common/common.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>

using IndexID = std::size_t;
using IndexText = std::string;
using IndexDocuments = std::unordered_map<IndexID, IndexText>;
using IndexIdx = std::size_t;

struct IndexEntry {
    IndexID doc_id;
    IndexIdx pos;
};
using IndexEntry = struct IndexEntry;
using IndexHash = std::string;
using IndexTerm = std::string;
using IndexEntries = std::unordered_map<IndexTerm, std::vector<IndexEntry>>;

class Index {
public:
    IndexDocuments docs;
    IndexEntries entries;
};

class IndexBuilder {
public:
    IndexBuilder(NgramStopWords stop_words, NgramLength min_length, NgramStopWords max_length);
    Index index() const;
    void add_document(IndexID id, IndexText text);
private:
    Index index_;
    NgramStopWords stop_words_;
    NgramLength min_length_;
    NgramLength max_length_;
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