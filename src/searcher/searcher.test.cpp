#include <gtest/gtest.h>
#include <fstream>
#include <searcher/searcher.hpp>

IndexPath testPath = "build/test";
IndexID testID = 363;
IndexTerm testTerm = "hi";
TermInfos testTermInfos = "hi 1 363 1 0 ";
Config testConfig = {{}, 1, 3};
DocsCount testDocsCount = 2;
SearcherQuery testSearcherQuery = "hi";

TEST(SearcherTest, GetConfig) {
  const TextIndexAccessor indexAccessor(testConfig, testPath);
  const Config cfg = indexAccessor.config();
  EXPECT_EQ(cfg.min_length, testConfig.min_length);
  EXPECT_EQ(cfg.max_length, testConfig.max_length);
}

TEST(SearcherTest, GetTermInfos) {
  IndexBuilder indexBuilder(testConfig.stop_words, testConfig.min_length,
                            testConfig.max_length);
  indexBuilder.add_document(testID, testTerm);
  indexBuilder.add_document(testID + 1, "hello");
  const Index index = indexBuilder.index();
  const TextIndexWriter writer;
  writer.write(testPath, index);
  const TextIndexAccessor indexAccessor(testConfig, testPath);
  const TermInfos termInfos = indexAccessor.get_term_infos(testTerm);
  EXPECT_EQ(testTermInfos, termInfos);
}

TEST(SearcherTest, LoadDocument) {
  const TextIndexAccessor indexAccessor(testConfig, testPath);
  const IndexText docText = indexAccessor.load_document(testID);
  EXPECT_EQ(docText, testTerm);
}

TEST(SearcherTest, TotalDocs) {
  const TextIndexAccessor indexAccessor(testConfig, testPath);
  EXPECT_EQ(indexAccessor.total_docs(), testDocsCount);
}

TEST(SearcherTest, Search) {
  const TextIndexAccessor indexAccessor(testConfig, testPath);
  Results results = searcher::search(testSearcherQuery, indexAccessor);
  EXPECT_EQ(results[0].doc_id, testID);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}