#include <searcher-c/searcher-c.h>

#include <searcher/searcher.hpp>

#include <stdexcept>

struct TextIndexAccessorWrapper {
  TextIndexAccessor *accessor;
};

struct BinaryIndexAccessorWrapper {
  BinaryIndexAccessor *accessor;
};

void *initialize_text_searcher(const char *index_path) {
  // create a TextIndexAccessor instance and store it in the wrapper
  TextIndexAccessorWrapper *wrapper = new TextIndexAccessorWrapper;
  wrapper->accessor =
      new TextIndexAccessor((std::filesystem::path &)index_path);
  return reinterpret_cast<void *>(wrapper);
}

void *initialize_binary_searcher(const char *index_path) {
  // create a TextIndexAccessor instance and store it in the wrapper
  BinaryIndexAccessorWrapper *wrapper = new BinaryIndexAccessorWrapper;
  wrapper->accessor =
      new BinaryIndexAccessor((std::filesystem::path &)index_path);
  return reinterpret_cast<void *>(wrapper);
}

void finalize_text_searcher(void *accessor) {
  if (accessor != NULL) {
    TextIndexAccessorWrapper *wrapper =
        reinterpret_cast<TextIndexAccessorWrapper *>(accessor);
    delete wrapper->accessor;
    delete wrapper;
  }
}

void finalize_binary_searcher(void *accessor) {
  if (accessor != NULL) {
    BinaryIndexAccessorWrapper *wrapper =
        reinterpret_cast<BinaryIndexAccessorWrapper *>(accessor);
    delete wrapper->accessor;
    delete wrapper;
  }
}

void search_text(void *accessor, const char *query, unsigned int *doc_ids,
                 double *scores, unsigned int *result_count) {
  if (accessor == NULL || query == NULL || doc_ids == NULL || scores == NULL ||
      result_count == NULL) {
    return;
  }

  TextIndexAccessorWrapper *wrapper =
      reinterpret_cast<TextIndexAccessorWrapper *>(accessor);
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

void search_binary(void *accessor, const char *query, unsigned int *doc_ids,
                 double *scores, unsigned int *result_count) {
  if (accessor == NULL || query == NULL || doc_ids == NULL || scores == NULL ||
      result_count == NULL) {
    return;
  }

  BinaryIndexAccessorWrapper *wrapper =
      reinterpret_cast<BinaryIndexAccessorWrapper *>(accessor);
  BinaryIndexAccessor &ia = *(wrapper->accessor);

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
