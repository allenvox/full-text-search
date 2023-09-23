#pragma once

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
    Index index;
    void add_document(IndexDocument doc);
    void add_document(IndexID id, IndexText text);
};

using IndexPath = std::string;

class IndexWriter {
public:
    virtual void write(IndexPath path, Index index);
};

class TextIndexWriter : public IndexWriter {
public:
    void write(IndexPath path, Index index) override;
};

using IndexHash = std::string;
using IndexTerm = std::string;
using IndexCount = std::size_t;