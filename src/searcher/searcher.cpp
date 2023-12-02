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
#include <sys/stat.h>
#include <unistd.h>

constexpr auto IDX_DOESNT_CONTAIN = "!!";

TermInfos TextIndexAccessor::get_term_infos(const std::string &term) const {
  const std::string hash = picosha2::hash256_hex_string(term).substr(0, 6);
  const std::filesystem::path fullPath = path_ / "index" / "entries" / hash;
  if (!std::filesystem::exists(fullPath)) {
    return IDX_DOESNT_CONTAIN;
  }
  std::ifstream termFile(fullPath);
  if (!termFile.is_open()) {
    throw std::runtime_error("Can't open term file");
  }
  TermInfos termInfos("");
  std::string line;
  while (std::getline(termFile, line)) {
    termInfos += line;
  }
  return termInfos;
}

std::string TextIndexAccessor::load_document(std::size_t doc_id) const {
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

DocsCount TextIndexAccessor::total_docs() const {
  DocsCount c = 0;
  for (const auto &entry :
       std::filesystem::directory_iterator(path_ / "index" / "docs")) {
    (void)entry;
    ++c;
  }
  return c;
}

Results searcher::search(const SearcherQuery &query,
                         const IndexAccessor &ia) {
  const NgramParser parser;
  const Config cfg = ia.config();
  const NgramVec parsed =
      parser.parse(query, cfg.stop_words, cfg.min_length, cfg.max_length);
  std::map<std::size_t, double> scores;
  for (const auto &ngram : parsed) {
    const TermInfos termInfos = ia.get_term_infos(ngram.text);
    if (termInfos == IDX_DOESNT_CONTAIN) {
      continue;
    }
    NgramWords words = parser.split_in_words(termInfos);
    const double doc_frequency = std::stod(words[1]);
    for (std::size_t i = 2; i < words.size() - 1; ++i) {
      auto doc_id = static_cast<std::size_t>(std::stoi(words[i]));
      ++i;
      const double term_frequency = std::stod(words[i]);
      i += static_cast<std::size_t>(term_frequency);
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

uint32_t get_u32_from_u8s(uint32_t start, const std::vector<uint8_t> &bytes) {
  if (start + 3 >= bytes.size()) {
    throw std::runtime_error("Invalid start position for uint32_t extraction");
  }
  uint32_t result = 0;
  for (int i = 0; i < 4; ++i) {
    result |= static_cast<uint32_t>(bytes[start + i]) << (8 * i);
  }
  return result;
}

BinaryIndexAccessor::BinaryIndexAccessor(std::filesystem::path &path,
                                         Config &config) {
  config_ = std::move(config);

  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error("Failed to open the index file");
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    close(fd);
    throw std::runtime_error("Failed to get file size");
  }
  fsize_ = sb.st_size;

  // load file to mem with mmap
  void* mapped_data = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (mapped_data == MAP_FAILED) {
    close(fd);
    throw std::runtime_error("Failed to mmap the index file");
  }
  data_ = std::vector<uint8_t>(static_cast<uint8_t*>(mapped_data),
                               static_cast<uint8_t*>(mapped_data) + fsize_);
  close(fd);

  uint32_t di_offset = get_u32_from_u8s(1 + sizeof("dictionary"), data_);
  uint32_t e_offset = get_u32_from_u8s(4 + di_offset + sizeof("entries"), data_);
  uint32_t docs_offset = get_u32_from_u8s(4 + e_offset + sizeof("documents"), data_);

  dia_ = new DictionaryAccessor(di_offset, e_offset - 1, data_);
  ea_ = new EntriesAccessor(e_offset, docs_offset - 1, data_);
  doa_ = new DocumentsAccessor(docs_offset, data_.size() - 1, data_);
}

void BinaryIndexAccessor::print_data() const {
  for (const auto& byte : data_) {
    std::cout << std::hex << byte << " (" << std::dec << byte << ")\n";
  }
}

Config BinaryIndexAccessor::config() const {
  return {};
}

TermInfos EntriesAccessor::get_term_infos(const uint32_t offset) const {
  return 0;
}

std::string DocumentsAccessor::load_document(uint32_t doc_id) const {
  return std::to_string(doc_id);
}

uint32_t DocumentsAccessor::total_docs() const {
  return get_u32_from_u8s(0, data_);
}