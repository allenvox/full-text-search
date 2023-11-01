#include <rapidcsv.h>

#include <driver/driver.hpp>
#include <indexer/indexer.hpp>

#include <iostream>
#include <unordered_map>

namespace driver {

void check_if_exists(const IndexPath &path) {
  if (!std::filesystem::exists(path)) {
    std::string err =
        "error: " + static_cast<std::string>(path) + " doesn't exist\n";
    throw std::runtime_error(err);
  }
}

rapidcsv::Document get_csv(const IndexPath &path) {
  rapidcsv::Document doc(path);
  return doc;
}

void remove_unneeded_cols(rapidcsv::Document &csv) {
  for (std::size_t i = 2; i < csv.GetColumnCount(); ++i) {
    csv.RemoveColumn(i);
  }
}

Index generate_index(IndexPath &csv_path, Config &cfg) {
  rapidcsv::Document csv(csv_path);
  IndexBuilder indexBuilder(cfg);
  remove_unneeded_cols(csv);
  for (std::size_t i = 1; i < csv.GetRowCount(); ++i) {
    auto id = csv.GetCell<IndexID>(0, i);
    auto txt = csv.GetCell<IndexText>(1, i);
    indexBuilder.add_document(id, txt);
  }
  return indexBuilder.index();
}

} // namespace driver