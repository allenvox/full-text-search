#include <searcher-c/searcher-c.h>
#include <searcher/searcher.hpp>

#include <stdexcept>

// struct to hold the C++ TextIndexAccessor instance
struct IndexAccessorWrapper {
  TextIndexAccessor *accessor;
};

void *initialize_searcher(const char *index_path) {
  // create a TextIndexAccessor instance and store it in the wrapper
  IndexAccessorWrapper *wrapper = new IndexAccessorWrapper;
  wrapper->accessor = new TextIndexAccessor(index_path);
  return reinterpret_cast<void *>(wrapper);
}

void finalize_searcher(void *accessor) {
  if (accessor != NULL) {
    IndexAccessorWrapper *wrapper =
        reinterpret_cast<IndexAccessorWrapper *>(accessor);
    delete wrapper->accessor;
    delete wrapper;
  }
}

void search(void *accessor, const char *query, unsigned int *doc_ids,
            double *scores, unsigned int *result_count) {
  if (accessor == NULL || query == NULL || doc_ids == NULL || scores == NULL ||
      result_count == NULL) {
    return;
  }

  IndexAccessorWrapper *wrapper =
      reinterpret_cast<IndexAccessorWrapper *>(accessor);
  TextIndexAccessor &ia = *(wrapper->accessor);

  // perform the search using C++ searcher
  std::string query_str(query);
  Results results = searcher::search(query_str, ia);

  // copy the results to the output arrays
  unsigned int count = static_cast<unsigned int>(results.size());
  *result_count = count;
  for (unsigned int i = 0; i < count; ++i) {
    doc_ids[i] = results[i].doc_id;
    scores[i] = results[i].score;
  }
}
