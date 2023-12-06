#include <picosha2.h>

#include <common/common.hpp>
#include <searcher/searcher.hpp>

#include <algorithm>
#include <cmath>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

constexpr auto IDX_DOESNT_CONTAIN = "!!";

std::string TextIndexAccessor::get_term_infos(const std::string &term) const {
  const std::string hash = picosha2::hash256_hex_string(term).substr(0, 6);
  const std::filesystem::path fullPath = path_ / "index" / "entries" / hash;
  if (!std::filesystem::exists(fullPath)) {
    return IDX_DOESNT_CONTAIN;
  }
  std::ifstream termFile(fullPath);
  if (!termFile.is_open()) {
    throw std::runtime_error("Can't open term file");
  }
  std::string termInfos("");
  std::string line;
  while (std::getline(termFile, line)) {
    termInfos += line;
  }
  return termInfos;
}

std::string TextIndexAccessor::load_document(size_t doc_id) const {
  const std::string doc_name = std::to_string(doc_id);
  std::ifstream docFile(path_ / "index" / "docs" / doc_name);
  if (!docFile.is_open()) {
    throw std::runtime_error("Can't open document file " + doc_name);
  }
  std::string docText("");
  std::string line;
  while (std::getline(docFile, line)) {
    docText += line;
  }
  return docText;
}

size_t TextIndexAccessor::total_docs() const {
  size_t c = 0;
  for (const auto &entry :
       std::filesystem::directory_iterator(path_ / "index" / "docs")) {
    (void)entry;
    ++c;
  }
  return c;
}

Results searcher::search(const std::string &query, const IndexAccessor &ia) {
  const NgramParser parser;
  const Config cfg = ia.config();
  const Ngrams parsed =
      parser.parse(query, cfg.stop_words, cfg.min_length, cfg.max_length);
  std::map<size_t, double> scores;
  for (const auto &ngram : parsed) {
    const std::string termInfos = ia.get_term_infos(ngram.text);
    if (termInfos == IDX_DOESNT_CONTAIN) {
      continue;
    }
    NgramWords words = parser.split_in_words(termInfos);
    const double doc_frequency = std::stod(words[1]);
    for (size_t i = 2; i < words.size() - 1; ++i) {
      auto doc_id = static_cast<size_t>(std::stoi(words[i]));
      ++i;
      const double term_frequency = std::stod(words[i]);
      i += static_cast<size_t>(term_frequency);
      auto N = static_cast<double>(static_cast<int>(ia.total_docs()));
      const double idf = log(N) - log(doc_frequency);
      const double tf_idf = term_frequency * idf;
      scores[doc_id] += tf_idf;
    }
  }
  Results results;
  for (auto &score : scores) {
    results.push_back({score.first, score.second});
  }
  auto compare_desc_by_score = [](const Result &a, const Result &b) {
    return a.score > b.score;
  };
  std::sort(results.begin(), results.end(), compare_desc_by_score);
  return results;
}

std::string DocumentsAccessor::load_document(size_t offset) {
  data_.set_current_position(section_offsets_->at("docs") + offset);
  std::string title;
  data_.read_string(title);
  return title;
}

std::string EntriesAccessor::get_term_infos(const std::string &term) {
  size_t this_term_offset = dia_->retrieve(term);
  std::string term_info = term + ' ';
  uint32_t docs_count = 0;
  data_.set_current_position(section_offsets_->at("entries") +
                             this_term_offset);
  data_.read(docs_count);
  term_info += std::to_string(docs_count) + ' ';
  for (uint32_t i = 0; i < docs_count; i++) {
    uint32_t doc_offset = 0;
    uint32_t pos_count = 0;
    data_.read(doc_offset);
    data_.read(pos_count);
    term_info +=
        std::to_string(doc_offset) + ' ' + std::to_string(pos_count) + ' ';
    for (uint32_t j = 0; j < pos_count; ++j) {
      uint32_t cur_pos = 0;
      data_.read(cur_pos);
      term_info += std::to_string(cur_pos) + ' ';
    }
  }
  return term_info;
}

size_t DictionaryAccessor::retrieve(const std::string &word) {
  const size_t dictionary_offset = section_offsets_->at("dictionary");
  data_.set_current_position(dictionary_offset);
  uint32_t children_count = 0;
  char child = 0;
  for (const auto c : word) {
    data_.read(children_count);
    uint32_t i = 0;
    for (; i < children_count; i++) {
      data_.read(child);
      if (child == c) {
        break;
      }
    }
    if (child != c) {
      throw std::invalid_argument("Term not found");
    }
    data_.skip<uint8_t>(children_count - (i + 1));
    data_.skip<uint32_t>(i);
    uint32_t child_offset = 0;
    data_.read(child_offset);
    data_.set_current_position(dictionary_offset + child_offset);
  }
  data_.read(children_count);
  size_t leaf_info_pos = data_.get_current_position() + children_count +
                         children_count * sizeof(uint32_t);
  data_.set_current_position(leaf_info_pos);
  uint8_t is_leaf = 0;
  data_.read(is_leaf);
  if (is_leaf == 0) {
    throw std::invalid_argument("Term not found");
  }
  uint32_t term_info_offset = 0;
  data_.read(term_info_offset);
  return term_info_offset;
}

BinaryIndexAccessor::BinaryIndexAccessor(const std::filesystem::path &path,
                                         Config &config) {
  config_ = config;
  fd_ = open(path.c_str(), O_RDONLY);
  if (fd_ < 0) {
    throw std::invalid_argument("File reading error");
  }
  const auto size = std::filesystem::file_size(path);
  data_.set_data(
      static_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd_, 0)),
      size);
  uint8_t sections = 0;
  data_.read(sections);
  std::string section;
  uint32_t offset = 0;
  for (int i = 0; i < sections; i++) {
    data_.read_string(section);
    data_.read(offset);
    section_offsets_[section] = offset;
  }
  doa_ = new DocumentsAccessor(data_, section_offsets_);
  ea_ = new EntriesAccessor(data_, section_offsets_);
}

BinaryIndexAccessor::~BinaryIndexAccessor() {
  munmap(data_.data(), data_.size());
  close(fd_);
  delete doa_;
  delete ea_;
}