#include <fstream>
#include <functional>
#include <indexer/indexer.hpp>
#include <picosha2.h>
#include <string>

void IndexBuilder::add_document(size_t id, const std::string &text) {
  index_.docs.insert({id, text});
  const NgramParser parser;
  const Ngrams ngrams = parser.parse(text, config_.stop_words,
                                     config_.min_length, config_.max_length);
  for (const auto &ngram : ngrams) {
    index_.entries[ngram.text].push_back({id, ngram.pos});
  }
}

std::string TextIndexWriter::term_to_hash(const std::string &term) const {
  return picosha2::hash256_hex_string(term).substr(0, 6);
}

void TextIndexWriter::create_index_directories(const IndexPath &path) const {
  const std::filesystem::path base_path = path / "index";
  std::filesystem::create_directories(base_path / "docs");
  std::filesystem::create_directories(base_path / "entries");
  if (!std::filesystem::exists(base_path) ||
      !std::filesystem::exists(base_path / "docs") ||
      !std::filesystem::exists(base_path / "entries")) {
    throw std::runtime_error("Index folder doesn't exist");
  }
}

void TextIndexWriter::write_docs(const IndexPath &path,
                                 const IndexDocuments &docs) const {
  for (const auto &[id, text] : docs) {
    std::ofstream out_file(path / "index" / "docs" / std::to_string(id));
    if (!out_file.is_open()) {
      throw std::runtime_error("Can't open document with id " +
                               std::to_string(id));
    }
    out_file << text << '\n';
    out_file.close();
  }
}

std::string TextIndexWriter::convert_to_entry_output(
    const std::string &term,
    const std::vector<IndexDocToPos> &doc_to_pos_vec) const {
  std::string output(term + ' ' + std::to_string(doc_to_pos_vec.size()) + ' ');
  std::vector<size_t> already_outputted;
  for (const auto &doc_to_pos1 : doc_to_pos_vec) {
    const auto &id = doc_to_pos1.doc_id;
    if (std::count(already_outputted.begin(), already_outputted.end(), id) ==
        1) {
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
    already_outputted.push_back(id);
  }
  output.append("\n");
  return output;
}

void TextIndexWriter::write_entries(const IndexPath &path,
                                    const IndexEntries &entries) const {
  for (const auto &[term, doc_to_pos_vec] : entries) {
    const std::string hash = term_to_hash(term);
    std::ofstream out_file(path / "index" / "entries" / hash);
    if (!out_file.is_open()) {
      throw std::runtime_error("Can't open entry " + hash);
    }
    out_file << convert_to_entry_output(term, doc_to_pos_vec);
    out_file.close();
  }
}

void TextIndexWriter::write(IndexPath &path, Index &index) {
  create_index_directories(path);
  write_docs(path, index.docs);
  write_entries(path, index.entries);
}

void Trie::insert(const std::string &word) {
  TrieNode *current = root_;
  TrieNode *previous = current;
  for (char c : word) {
    c = static_cast<char>(std::tolower(c));
    if (current->children.count(c) == 0) {
      current = new TrieNode(c);
      current->parent = previous;
      previous->children[c] = current;
    } else {
      current = current->children[c];
    }
    previous = current;
  }
  current->is_leaf = true;
  size_++;
}

std::string Trie::retrieve(const TrieNode *node) {
  if (!node->is_leaf) {
    throw std::invalid_argument("Node is not a leaf");
  }
  std::string word;
  while (node != root_) {
    word.insert(0, 1, node->data);
    node = node->parent;
  }
  return word;
}

void BinaryIndexWriter::write_header(BinaryHandler &buf) {
  buf.write<uint8_t>(sections_.size());
  for (const auto &[id, name] : sections_) {
    header_.section_offsets[name] = buf.get_current_position();
    buf.write_string(name);
    buf.write<uint32_t>(0);
  }
}

void BinaryIndexWriter::write_docs(BinaryHandler &buf, const Index &index) {
  const uint32_t begin = buf.get_current_position();
  buf.set_current_position(
      header_.section_offsets.at(sections_.at(Section::Docs)));
  buf.skip_string(1);
  buf.write(begin);
  buf.set_current_position(begin);
  buf.write<uint32_t>(index.docs.size());
  for (const auto &[document_id, title] : index.docs) {
    id_to_offset_[document_id] = buf.get_current_position() - begin;
    buf.write_string(title);
  }
}

void BinaryIndexWriter::write_entries(BinaryHandler &buf, const Index &index) {
  using IdToPos = std::unordered_map<size_t, std::vector<size_t>>;
  using Occurrences = std::unordered_map<std::string, IdToPos>;
  Occurrences occur;
  // term: [
  //   id: { pos1, pos2 ... }
  //   ...
  // ] ...
  for (const auto &[term, doc_to_pos_vec] : index.entries) {
    for (const auto &doc_to_pos : doc_to_pos_vec) {
      size_t pos = doc_to_pos.pos;
      size_t id = doc_to_pos.doc_id;
      occur[term][id].push_back(pos);
    }
  }

  const uint32_t entries_offset = buf.get_current_position();
  buf.set_current_position(
      header_.section_offsets.at(sections_.at(Section::Entries)));
  buf.skip_string(1);
  buf.write(entries_offset);
  buf.set_current_position(entries_offset);
  for (const auto &[term, id_to_pos] : occur) {
    term_to_offset_[term] = buf.get_current_position() - entries_offset;
    buf.write<uint32_t>(occur.at(term).size());
    for (const auto &[id, positions] : id_to_pos) {
      buf.write<uint32_t>(id_to_offset_[id]);
      buf.write<uint32_t>(positions.size());
      for (const auto &pos : positions) {
        buf.write<uint32_t>(pos);
      }
    }
  }
}

void BinaryIndexWriter::build_dictionary(BinaryHandler &buf, Trie &trie,
                                         TrieNode *current, uint32_t begin) {
  if (children_offsets_.count(current) > 0) {
    children_offsets_[current].second = buf.get_current_position() - begin;
  }
  buf.write<uint32_t>(current->children.size());
  for (const auto &[c, node] : current->children) {
    buf.write(c);
  }
  for (const auto &[c, node] : current->children) {
    children_offsets_[node] =
        std::pair<size_t, size_t>(buf.get_current_position() - begin, 0);
    buf.write<uint32_t>(0);
  }
  buf.write(static_cast<uint8_t>(current->is_leaf));
  if (current->is_leaf) {
    buf.write<uint32_t>(term_to_offset_[trie.retrieve(current)]);
  }
  for (const auto &[c, node] : current->children) {
    build_dictionary(buf, trie, node, begin);
  }
}

void BinaryIndexWriter::write_dictionary(BinaryHandler &buf,
                                         const Index &index) {
  const uint32_t begin = buf.get_current_position();
  buf.set_current_position(
      header_.section_offsets.at(sections_.at(Section::Dictionary)));
  buf.skip_string(1);
  buf.write(begin);
  buf.set_current_position(begin);
  Trie trie;
  for (const auto &[term, entries] : index.entries) {
    trie.insert(term);
  }
  build_dictionary(buf, trie, trie.root(), begin);
  for (const auto &[node, offsets] : children_offsets_) {
    buf.set_current_position(begin + offsets.first);
    buf.write<uint32_t>(offsets.second);
  }
}

void BinaryIndexWriter::write(IndexPath &path, Index &index) {
  BinaryHandler buf;
  write_header(buf);
  write_docs(buf, index);
  write_entries(buf, index);
  write_dictionary(buf, index);
  std::filesystem::current_path(path);
  std::filesystem::create_directory("index");
  std::ofstream f("index/index.bin", std::ios::binary);
  f.write(buf.data(), static_cast<std::streamsize>(buf.size()));
  f.close();
}