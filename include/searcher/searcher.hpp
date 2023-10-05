#pragma once

#include <config/config.hpp>
#include <indexer/indexer.hpp>
#include <vector>

struct Result {
    IndexID doc_id;
    double score;
};
using Result = struct Result;
using Results = std::vector<Result>;
using TermInfos = std::string;
using SearcherQuery = std::string;
using DocsCount = std::size_t;

class IndexAccessor {
  public:
    virtual Config config() const = 0;
    virtual TermInfos get_term_infos(const IndexTerm& term) const = 0;
    virtual IndexText load_document(IndexID doc_id) const = 0;
    virtual DocsCount total_docs() const = 0;
};

class TextIndexAccessor : public IndexAccessor {
  public:
    Config config() const;
    TermInfos get_term_infos(const IndexTerm& term) const;
    IndexText load_document(IndexID doc_id) const;
    DocsCount total_docs() const;
  private:
    IndexPath path_;
};

namespace searcher {

Results search(const SearcherQuery& query, const TextIndexAccessor& ia);

} // namespace searcher