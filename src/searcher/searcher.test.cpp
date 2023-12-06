#include <gtest/gtest.h>

#include <binary/binary.hpp>
#include <indexer/indexer.hpp>
#include <searcher/searcher.hpp>

std::filesystem::path testPath = "build/test";
std::filesystem::path testBinaryPath = "index/index.bin";
size_t testID = 363;
size_t testOffset = 10;
std::string testTerm = "hi";
std::string testTermInfos = "hi 1 363 1 0 ";
std::string testBinaryTermInfos = "hi 1 10 1 0 ";
Config testConfig = {{}, 1, 3};
size_t testDocsCount = 2;
std::string testSearcherQuery = "hi";

TEST(SearcherTest, TextGetConfig) {
  const TextIndexAccessor indexAccessor(testPath, testConfig);
  const Config cfg = indexAccessor.config();
  EXPECT_EQ(cfg.min_length, testConfig.min_length);
  EXPECT_EQ(cfg.max_length, testConfig.max_length);
}

TEST(SearcherTest, TextGetTermInfos) {
  IndexBuilder indexBuilder(testConfig.stop_words, testConfig.min_length,
                            testConfig.max_length);
  indexBuilder.add_document(testID, testTerm);
  indexBuilder.add_document(testID + 1, "hello");
  Index index = indexBuilder.index();
  TextIndexWriter writer;
  writer.write(testPath, index);
  const TextIndexAccessor indexAccessor(testPath, testConfig);
  const std::string termInfos = indexAccessor.get_term_infos(testTerm);
  EXPECT_EQ(testTermInfos, termInfos);
}

TEST(SearcherTest, TextLoadDocument) {
  const TextIndexAccessor indexAccessor(testPath, testConfig);
  const std::string docText = indexAccessor.load_document(testID);
  EXPECT_EQ(docText, testTerm);
}

TEST(SearcherTest, TextTotalDocs) {
  const TextIndexAccessor indexAccessor(testPath, testConfig);
  EXPECT_EQ(indexAccessor.total_docs(), testDocsCount);
}

TEST(SearcherTest, TextSearch) {
  const TextIndexAccessor indexAccessor(testPath, testConfig);
  Results results = searcher::search(testSearcherQuery, indexAccessor);
  EXPECT_EQ(results[0].doc_id, testID);
}

TEST(SearcherTest, BinaryGetConfig) {
  IndexBuilder indexBuilder(testConfig.stop_words, testConfig.min_length,
                            testConfig.max_length);
  indexBuilder.add_document(testID, testTerm);
  indexBuilder.add_document(testID + 1, "hello");
  Index index = indexBuilder.index();
  BinaryIndexWriter writer;
  writer.write(testPath, index);
  const BinaryIndexAccessor indexAccessor(testBinaryPath, testConfig);
  const Config cfg = indexAccessor.config();
  EXPECT_EQ(cfg.min_length, testConfig.min_length);
  EXPECT_EQ(cfg.max_length, testConfig.max_length);
}

TEST(SearcherTest, BinaryGetTermInfos) {
  IndexBuilder indexBuilder(testConfig.stop_words, testConfig.min_length,
                            testConfig.max_length);
  indexBuilder.add_document(testID, testTerm);
  indexBuilder.add_document(testID + 1, "hello");
  Index index = indexBuilder.index();
  BinaryIndexWriter writer;
  writer.write(testPath, index);
  const BinaryIndexAccessor indexAccessor(testBinaryPath, testConfig);
  const std::string termInfos = indexAccessor.get_term_infos(testTerm);
  EXPECT_EQ(testBinaryTermInfos, termInfos);
}

TEST(SearcherTest, BinaryLoadDocument) {
  IndexBuilder indexBuilder(testConfig.stop_words, testConfig.min_length,
                            testConfig.max_length);
  indexBuilder.add_document(testID, testTerm);
  indexBuilder.add_document(testID + 1, "hello");
  Index index = indexBuilder.index();
  BinaryIndexWriter writer;
  writer.write(testPath, index);
  const BinaryIndexAccessor indexAccessor(testBinaryPath, testConfig);
  const std::string docText = indexAccessor.load_document(testOffset);
  EXPECT_EQ(docText, testTerm);
}

TEST(SearcherTest, BinaryTotalDocs) {
  IndexBuilder indexBuilder(testConfig.stop_words, testConfig.min_length,
                            testConfig.max_length);
  indexBuilder.add_document(testID, testTerm);
  indexBuilder.add_document(testID + 1, "hello");
  Index index = indexBuilder.index();
  BinaryIndexWriter writer;
  writer.write(testPath, index);
  const BinaryIndexAccessor indexAccessor(testBinaryPath, testConfig);
  EXPECT_EQ(indexAccessor.total_docs(), testDocsCount);
}

TEST(SearcherTest, BinarySearch) {
  IndexBuilder indexBuilder(testConfig.stop_words, testConfig.min_length,
                            testConfig.max_length);
  indexBuilder.add_document(testID, testTerm);
  indexBuilder.add_document(testID + 1, "hello");
  Index index = indexBuilder.index();
  BinaryIndexWriter writer;
  writer.write(testPath, index);
  const BinaryIndexAccessor indexAccessor(testBinaryPath, testConfig);
  Results results = searcher::search(testSearcherQuery, indexAccessor);
  EXPECT_EQ(results[0].doc_id, testOffset);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}