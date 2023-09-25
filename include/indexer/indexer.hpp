#pragma once

#include <common/common.hpp>
#include <filesystem>
#include <vector>

using IndexID = std::size_t;
using IndexText = std::string;
struct IndexDocument {
    IndexID id;
    IndexText text;
};
using IndexDocument = struct IndexDocument;
using IndexDocumentsVec = std::vector<IndexDocument>;

using IndexIdx = std::size_t;
struct IndexEntry {
    IndexID doc_id;
    IndexIdx pos;
};
using IndexEntry = struct IndexEntry;
using IndexEntriesVec = std::vector<IndexEntry>;

class Index {
public:
    IndexDocumentsVec docs;
    IndexEntriesVec entries;
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

using IndexHash = std::string;
using IndexTerm = std::string;
using IndexCount = std::size_t;

class TextIndexWriter : public IndexWriter {
public:
    void write(IndexPath path, Index index) override;
};