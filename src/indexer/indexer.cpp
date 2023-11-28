#include <fstream>
#include <functional>
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
  std::vector<IndexID> already_outputted;
  for (const auto &doc_to_pos1 : doc_to_pos_vec) {
    const auto &id = doc_to_pos1.doc_id;
    if (std::count(already_outputted.begin(), already_outputted.end(), id) == 1) {
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

uint32_t indexer::get_offset(const uint32_t id, const IdToOffset &id_to_offset) {
  auto offset_it = id_to_offset.find(id);
  if (offset_it == id_to_offset.end()) {
    throw std::runtime_error("No offset of " + std::to_string(id));
  }
  return offset_it->second;
}

void indexer::push_u32_to_u8(const uint32_t val, BytesVec &vec) {
  vec.push_back(val & 0xFF);
  vec.push_back((val >> 8) & 0xFF);
  vec.push_back((val >> 16) & 0xFF);
  vec.push_back((val >> 24) & 0xFF);
}

BytesVec indexer::serialize_string(const std::string &str) {
  BytesVec serialized;
  serialized.push_back(static_cast<uint8_t>(str.size())); // str length
  for (char c : str) { // serialize each char
    serialized.push_back(static_cast<uint8_t>(c));
  }
  return serialized;
}

BytesVec indexer::serialize_docs(const std::vector<std::string> &titles,
                                 IdToOffset &id_to_offset) {
  BytesVec serialized_docs;
  uint32_t current_offset = 0;
  uint32_t titles_count = static_cast<uint32_t>(titles.size());
  // write docs (titles) count in first 4 bytes
  push_u32_to_u8(titles_count, serialized_docs);
  for (const auto& title : titles) {
    // get offset of doc
    id_to_offset[static_cast<uint32_t>(id_to_offset.size())] = current_offset;
    BytesVec serialized_title = serialize_string(title);
    // add serialized title bytes to docs
    serialized_docs.insert(serialized_docs.end(),
                           serialized_title.begin(),
                           serialized_title.end());
    // next offset
    current_offset += static_cast<uint32_t>(serialized_title.size());
  }
  return serialized_docs;
}

BytesVec indexer::serialize_entries(const std::vector<IndexDocToPos> &entries,
                                    const IdToOffset &id_to_offset) {
  // filling occurrences [ doc_offset: { pos_count, pos1, pos2... } ]
  std::unordered_map<uint32_t, std::vector<uint32_t>> occurrences;
  for (const auto& entry : entries) {
    uint32_t pos = entry.pos;
    uint32_t offset = get_offset(entry.doc_id, id_to_offset);
    auto occurrences_it = occurrences.find(offset);
    auto entry_vec = occurrences_it->second;
    if (occurrences_it == occurrences.end()) {
      entry_vec = {1, pos};
    } else {
      entry_vec[0]++;
      entry_vec.push_back(pos);
    }
  }

  BytesVec serialized_entries;
  // serialize count of docs where entry appears
  uint32_t docs_count = occurrences.size();
  push_u32_to_u8(docs_count, serialized_entries);
  for (const auto& [offset, pos_vec] : occurrences) {
    push_u32_to_u8(offset, serialized_entries); // serialize doc offset
    // serialize pos_count
    uint32_t pos_count = pos_vec[0];
    push_u32_to_u8(pos_count, serialized_entries);
    for (std::size_t i = 1; i < pos_vec.size(); ++i) { // serialize positions
      push_u32_to_u8(pos_vec[i], serialized_entries);
    }
  }
  return serialized_entries;
}

void indexer::insertWord(TrieNode* root, const std::string &word,
                         uint32_t entry_offset) {
  TrieNode* node = root;
  for (char ch : word) {
    if (node->children.find(ch) == node->children.end()) {
      node->children[ch] = new TrieNode(); // if node doesn't exist, create
    }
    node = node->children[ch];
  }
  // mark leaf node, save entry offset
  node->is_leaf = true;
  node->entry_offset = entry_offset;
}

BytesVec indexer::serialize_dictionary(TrieNode* root,
                                       uint32_t &next_entry_offset) {
  std::vector<uint8_t> serialized_dictionary;
  // for recursive serialization of trie
  std::function<void(TrieNode*)> serializeTrie = [&](TrieNode* node) {
    if (node == nullptr) {
      return;
    }
    // serialize children_count
    uint32_t children_count = static_cast<uint32_t>(node->children.size());
    push_u32_to_u8(children_count, serialized_dictionary);
    // serialize children letters & its offsets
    for (const auto& entry : node->children) {
      char letter = entry.first;
      TrieNode* child = entry.second;
      uint32_t child_offset = next_entry_offset; // save current child offset
      // serialize letter
      serialized_dictionary.push_back(static_cast<uint8_t>(letter));
      // serialize current child offset
      push_u32_to_u8(child_offset, serialized_dictionary);
      serializeTrie(child); // recursively serialize child
    }
    serialized_dictionary.push_back(node->is_leaf ? 1 : 0);
    if (node->is_leaf) { // if leaf node - serialize entry_offset
      push_u32_to_u8(node->entry_offset, serialized_dictionary);
    }
  };
  serializeTrie(root); // start serialization from root
  return serialized_dictionary;
}

BytesVec indexer::serialize_header(const std::vector<std::vector<uint8_t>> &sections) {
  BytesVec serialized_header{4};
  std::vector<std::string> names{"header", "dictionary", "entries", "docs"};
  uint32_t current_offset = 0;
  for (std::size_t i = 0; i < 4; ++i) {
    BytesVec name = serialize_string(names[i]);
    serialized_header.insert(serialized_header.end(), name.begin(), name.end());
    push_u32_to_u8(static_cast<uint32_t>(current_offset), serialized_header);
    current_offset += static_cast<uint32_t>(sections[i].size());
  }
  return serialized_header;
}