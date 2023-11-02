#include <rapidcsv.h>

#include <driver/driver.hpp>
#include <indexer/indexer.hpp>

#include <iostream>

namespace driver {

void check_if_exists(const IndexPath &path) {
  if (!std::filesystem::exists(path)) {
    const std::string err =
        "error: " + static_cast<std::string>(path) + " doesn't exist\n";
    throw std::runtime_error(err);
  }
}

rapidcsv::Document get_csv(const IndexPath &path) {
  rapidcsv::Document doc(path);
  return doc;
}

enum ColumnHeader {
  ID_column,
  Text_column
};

Index generate_index(IndexPath &csv_path, Config &cfg) {
  rapidcsv::Document csv(csv_path);
  IndexBuilder indexBuilder(cfg);
  for (std::size_t i = 0; i < csv.GetRowCount(); ++i) {
    auto id = csv.GetCell<IndexID>(ID_column, i);
    auto txt = csv.GetCell<IndexText>(Text_column, i);
    indexBuilder.add_document(id, txt);
  }
  return indexBuilder.index();
}

} // namespace driver