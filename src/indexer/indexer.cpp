#include <indexer/indexer.hpp>
#include <picosha2.h>

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

IndexHash term_to_hash(const IndexTerm& term) const {
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

IndexText convert_to_entry_output(const IndexTerm& term, const std::vector<IndexDocToPos>& doc_to_pos_vec) {
    IndexText output(term + ' ' + std::to_string(doc_to_pos_vec.size()) + ' ');
    for (const auto& doc_to_pos : doc_to_pos_vec) {
        const auto doc_id = doc_to_pos.doc_id;
        const auto pos = doc_to_pos.pos;
        output.append(std::to_string(doc_id) + ' ' + std::to_string(doc_to_pos_vec.count(doc_id)) + ' ');
        for (auto [beg_entries, end_entries] = doc_to_pos_vec.equal_range(doc_id);
             beg_entries != end_entries; ++beg_entries) {
            output.append(std::to_string(beg_entries->second) + ' ');
        }
    }
    output.append('\n');
    return output;
}

void write_entries(const IndexPath& path, const IndexEntries& entries) {
    for (const auto& [term, doc_to_pos_vec] : entries) {
        std::ofstream out_file(path / "index" / "entries" / term_to_hash(term));
        if (!out_file.is_open()) {
            throw_index_fs_error();
        }
        const IndexText out_text = convert_to_entry_output(term, doc_to_pos_vec);
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