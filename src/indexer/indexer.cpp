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
  vec.push_back(val & 0xFFU);
  vec.push_back((val >> 8U) & 0xFFU);
  vec.push_back((val >> 16U) & 0xFFU);
  vec.push_back((val >> 24U) & 0xFFU);
}

BytesVec indexer::serialize_string(const std::string &str) {
  BytesVec serialized;
  serialized.push_back(static_cast<uint8_t>(str.size())); // str length
  for (char c : str) { // serialize each char
    serialized.push_back(static_cast<uint8_t>(c));
  }
  return serialized;
}

BytesVec indexer::serialize_docs(const IndexDocuments &docs,
                                 IdToOffset &id_to_offset) {
  BytesVec serialized_docs;
  uint32_t current_offset = 0;
  auto docs_count = static_cast<uint32_t>(docs.size());
  // write docs (titles) count in first 4 bytes
  push_u32_to_u8(docs_count, serialized_docs);
  for (const auto& [id, title] : docs) {
    // get offset of doc
    id_to_offset[id] = current_offset;
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

BytesVec indexer::serialize_entries(const IndexEntries &entries,
                                    const IdToOffset &id_to_offset,
                                    TermToOffset &term_to_offset) {
  BytesVec serialized_entries;
  for (const auto& [term, doc_to_pos] : entries) {
    // filling occurrences for term [ doc_offset: { pos_count, pos1, pos2... } ]
    std::unordered_map<uint32_t, std::vector<uint32_t>> occurrences;
    for (const auto& entry : doc_to_pos) {
      uint32_t pos = entry.pos;
      uint32_t offset = get_offset(entry.doc_id, id_to_offset);
      auto occurrences_it = occurrences.find(offset);
      if (occurrences_it == occurrences.end()) {
        occurrences[offset] = {1, pos};
      } else {
        occurrences[offset][0]++;
        occurrences[offset].push_back(pos);
      }
    }
    term_to_offset[term] = serialized_entries.size();

    // serialize count of docs where entry appears
    push_u32_to_u8(occurrences.size(), serialized_entries);

    for (const auto& [doc_offset, pos_vec] : occurrences) {
      // serialize doc offset
      push_u32_to_u8(doc_offset, serialized_entries);

      // serialize pos_count
      uint32_t pos_count = pos_vec[0];
      push_u32_to_u8(pos_count, serialized_entries);

      // serialize positions
      for (const auto& pos : pos_vec) {
        push_u32_to_u8(pos, serialized_entries);
      }
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

BytesVec indexer::serialize_dictionary(const IndexEntries &entries, const TermToOffset &term_to_offset) {
  auto* root = new TrieNode();
  BytesVec serialized_dictionary;

  // add all terms to trie
  for (const auto& [term, doc_to_pos_vec] : entries) {
    insertWord(root, term, term_to_offset.at(term));
  }

  // recursive serialization with offsets
  std::function<void(TrieNode*)> serializeNode = [&](TrieNode* node) {
    // serialize children_count
    auto childrenCount = static_cast<uint32_t>(node->children.size());
    push_u32_to_u8(childrenCount, serialized_dictionary);

    // serialize children nodes letters and their offsets
    for (const auto& [letter, childNode] : node->children) {
      serialized_dictionary.push_back(static_cast<uint8_t>(letter));
      uint32_t childOffset = childNode->entry_offset;
      push_u32_to_u8(childOffset, serialized_dictionary);
    }

    // serialize is_leaf
    uint8_t isLeaf = node->is_leaf ? 1 : 0;
    serialized_dictionary.push_back(isLeaf);

    if (node->is_leaf) {
      // serialize entry_offset
      uint32_t entryOffset = node->entry_offset;
      push_u32_to_u8(entryOffset, serialized_dictionary);
    }

    for (const auto& [letter, childNode] : node->children) {
      serializeNode(childNode);
    }
  };

  serializeNode(root); // start serialization from root
  delete root; // delete trie after serialization

  return serialized_dictionary;
}

void indexer::change_offset(BytesVec &header, uint32_t pos, uint32_t val) {
  header.at(pos) = val & 0xFFU;
  header.at(pos + 1) = (val >> 8U) & 0xFFU;
  header.at(pos + 2) = (val >> 16U) & 0xFFU;
  header.at(pos + 3) = (val >> 24U) & 0xFFU;
}

BytesVec indexer::serialize_header(const BytesVec &dictionary,
                                   const BytesVec &entries) {
  BytesVec serialized_header{4};
  BytesVec hname = serialize_string("header");
  serialized_header.insert(serialized_header.end(), hname.begin(), hname.end());
  push_u32_to_u8(0, serialized_header);

  BytesVec diname = serialize_string("dictionary");
  serialized_header.insert(serialized_header.end(), diname.begin(), diname.end());
  uint32_t dioffset = serialized_header.size();
  push_u32_to_u8(0, serialized_header);

  BytesVec ename = serialize_string("entries");
  serialized_header.insert(serialized_header.end(), ename.begin(), ename.end());
  uint32_t eoffset = serialized_header.size();
  push_u32_to_u8(0, serialized_header);

  BytesVec doname = serialize_string("docs");
  serialized_header.insert(serialized_header.end(), doname.begin(), doname.end());
  uint32_t dooffset = serialized_header.size();
  push_u32_to_u8(0, serialized_header);

  uint32_t cur_offset = serialized_header.size();
  change_offset(serialized_header, dioffset, cur_offset);
  cur_offset += dictionary.size();
  change_offset(serialized_header, eoffset, cur_offset);
  cur_offset += entries.size();
  change_offset(serialized_header, dooffset, cur_offset);

  return serialized_header;
}

void BinaryIndexWriter::write(IndexPath path, Index index) const {
  indexer::IdToOffset id_to_offset;
  indexer::TermToOffset term_to_offset;
  std::vector<BytesVec> sections(4);

  sections[3] = indexer::serialize_docs(index.docs, id_to_offset);
  sections[2] = indexer::serialize_entries(index.entries, id_to_offset, term_to_offset);
  sections[1] = indexer::serialize_dictionary(index.entries, term_to_offset);
  sections[0] = indexer::serialize_header(sections[1], sections[2]);

  std::ofstream out_file(path / "index.bin", std::ios::binary);
  if (!out_file.is_open()) {
    throw std::runtime_error("Failed to open the output file.");
  }

  try {
    for (const auto& section : sections) {
      out_file.write(reinterpret_cast<const char*>(section.data()),
                     static_cast<std::streamsize>(section.size()));
    }
    out_file.close();
  } catch (const std::exception& e) {
    out_file.close();
    throw e;
  }
}