#include <fstream>
#include <indexer/indexer.hpp>
#include <picosha2.h>
#include <string>

void IndexBuilder::add_document(IndexID id, const IndexText &text) {
  index_.docs.insert({id, text});
  const NgramParser parser;
  const NgramVec ngrams = parser.parse(text, config_.stop_words,
                                       config_.min_length, config_.max_length);
  for (const auto &ngram : ngrams) {
    index_.entries[ngram.text].push_back({id, ngram.pos});
  }
}

IndexHash indexer::term_to_hash(const IndexTerm &term) {
  return picosha2::hash256_hex_string(term).substr(0, 6);
}

void indexer::create_index_directories(const IndexPath &path) {
  std::filesystem::path base_path = path / "index";
  std::filesystem::create_directories(base_path / "docs");
  std::filesystem::create_directories(base_path / "entries");
  if (!std::filesystem::exists(base_path) ||
      !std::filesystem::exists(base_path / "docs") ||
      !std::filesystem::exists(base_path / "entries")) {
    throw std::runtime_error("Index folder doesn't exist");
  }
}

void indexer::write_docs(const IndexPath &path, const IndexDocuments &docs) {
  for (const auto &[id, text] : docs) {
    std::ofstream out_file(path / "index" / "docs" / std::to_string(id));
    if (!out_file.is_open()) {
      throw std::runtime_error("Can't open document with id " + std::to_string(id));
    }
    out_file << text << '\n';
    out_file.close();
  }
}

IndexText indexer::convert_to_entry_output(
    const IndexTerm &term, const std::vector<IndexDocToPos> &doc_to_pos_vec) {
  IndexText output(term + ' ' + std::to_string(doc_to_pos_vec.size()) + ' ');
  std::vector<IndexID> already_outputed;
  for (const auto &doc_to_pos1 : doc_to_pos_vec) {
    const auto &id = doc_to_pos1.doc_id;
    if (std::count(already_outputed.begin(), already_outputed.end(), id) == 1) {
      continue;
    }
    output.append(std::to_string(id) + ' ' +
                  std::to_string(std::count_if(
                      doc_to_pos_vec.begin(), doc_to_pos_vec.end(),
                      [&](const IndexDocToPos &s) { return s.doc_id == id; })) +
                  ' ');
    for (const auto &doc_to_pos2 : doc_to_pos_vec) {
      if (doc_to_pos2.doc_id == id) {
        output.append(std::to_string(doc_to_pos2.pos) + ' ');
      }
    }
    already_outputed.push_back(id);
  }
  output.append("\n");
  return output;
}

void indexer::write_entries(const IndexPath &path,
                            const IndexEntries &entries) {
  for (const auto &[term, doc_to_pos_vec] : entries) {
    std::string hash = indexer::term_to_hash(term);
    std::ofstream out_file(path / "index" / "entries" / hash);
    if (!out_file.is_open()) {
      throw std::runtime_error("Can't open entry " + hash);
    }
    out_file << convert_to_entry_output(term, doc_to_pos_vec);
    out_file.close();
  }
}

void TextIndexWriter::write(IndexPath path, Index index) const {
  indexer::create_index_directories(path);
  indexer::write_docs(path, index.docs);
  indexer::write_entries(path, index.entries);
}