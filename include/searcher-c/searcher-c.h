#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void *initialize_text_searcher(const char *index_path);
void *initialize_binary_searcher(const char *index_path);
void finalize_text_searcher(void *accessor);
void finalize_binary_searcher(void *accessor);
void text_search(void *accessor, const char *query, unsigned int *doc_ids,
            double *scores, unsigned int *result_count);

#ifdef __cplusplus
}
#endif