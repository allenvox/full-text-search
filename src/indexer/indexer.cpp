#include <indexer/indexer.hpp>
#include <picosha2>

IndexBuilder::IndexBuilder(NgramStopWords stop_words, NgramLength min_length, NgramStopWords max_length) {
    stop_words_ = std::move(stop_words);
    min_length_ = std::move(min_length);
    max_length_ = std::move(max_length);
}

Index IndexBuilder::index() {
    return index_;
}

void IndexBuilder::add_document(IndexID id, IndexText text) {
    index_.docs.insert({id, text});
    NgramParser parser;
    NgramVec ngrams = parser.parse(text, stop_words_, min_length_, max_length_);
    for (NgramIndex pos = 0, end = ngrams.size(); pos != end; pos++) {
        for (const auto& term : ngrams[pos]) {
            index_.entries_[term].insert({id, pos});
        }
    }
}

IndexHash term_to_hash(const IndexTerm term) const {
    return picosha2::hash256_hex_string(term).substr(0, 6);
}

void create_index_directories(const IndexPath& path) {
    std::filesystem::create_directories(path / "index" / "docs");
    std::filesystem::create_directories(path / "index" / "entries");
    if (!std::filesystem::exists(path / "index")) {
        throw_index_fs_error();
    }
}

void write_docs(const IndexPath& path, const IndexDocuments& docs) {
    for (const auto& [id, text] : docs) {
        std::ofstream out_file(path / "index" / "docs" / std::to_string(id));
        if (!out_file.is_open()) {
            throw_index_fs_error();
        }
        out_file << text << '\n';
        out_file.close();
    }
}

IndexText convert_to_entry_output(const IndexTerm& term, const IndexEntry& entry) {

}

void write_entries(const IndexPath& path, const IndexEntries& entries) {
    for (const auto& [term, entries_vec] : entries) {
        std::ofstream out_file(path / "index" / "entries" / term_to_hash(term));
        if (!out_file.is_open()) {
            throw_index_fs_error();
        }
        const IndexText out_text = convert_to_entry_output(term, entries_vec);
        out_file << out_text;
        out_file.close();
    }
}

void throw_index_fs_error() {
    throw std::runtime_error("Can't create index file");
}

void TextIndexWriter::write(IndexPath path, Index index) const override {
    create_index_directories(path);
    write_docs(path, index.docs_);
    write_entries(path, index.entries_);
}