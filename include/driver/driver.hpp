#pragma once

#include <rapidcsv.h>

#include <config/config.hpp>
#include <indexer/indexer.hpp>
#include <searcher/searcher.hpp>

namespace driver {

void check_if_exists(const IndexPath &path);
rapidcsv::Document get_csv(const IndexPath &path);
IndexDocuments get_docs(const rapidcsv::Document &csv,
                        Config &cfg = DEFAULT_CONFIG);
void remove_unneeded_cols(rapidcsv::Document &csv);
Index generate_index(rapidcsv::Document &csv, Config &cfg);
void write_index(const IndexPath &path, const Index &idx, const IndexWriter &iw);

} // namespace driver