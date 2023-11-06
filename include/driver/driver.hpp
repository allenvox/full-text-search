#pragma once

#include <rapidcsv.h>

#include <config/config.hpp>
#include <indexer/indexer.hpp>
#include <searcher/searcher.hpp>

namespace driver {

void check_if_exists(const IndexPath &path);
rapidcsv::Document get_csv(const IndexPath &path);
void remove_unneeded_cols(rapidcsv::Document &csv);
Index generate_index(IndexPath &csv_path, Config &cfg = DEFAULT_CONFIG);

} // namespace driver