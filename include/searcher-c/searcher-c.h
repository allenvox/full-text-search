#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void *initialize_searcher(const char *index_path);
void finalize_searcher(void *accessor);
void search(void *accessor, const char *query, unsigned int *doc_ids,
            double *scores, unsigned int *result_count);

#ifdef __cplusplus
}
#endif