#include <gtest/gtest.h>

#include <indexer/indexer.hpp>

#include <fstream>

TEST(IndexBuilderTest, AddDocument) {
  IndexBuilder builder({"the"}, 3, 6);
  const IndexID id = 1;
  const IndexText text = "The Matrix";
  builder.add_document(id, text);
  const Index index = builder.index();
  ASSERT_EQ(index.docs.size(), 1);
  ASSERT_EQ(index.entries.size(), 4); // 4 ngrams
  IndexText doc_text = "";
  auto it = index.docs.find(id);
  if (it != index.docs.end()) {
    doc_text = it->second;
  }
  EXPECT_EQ(doc_text, text);
}

TEST(IndexerUtilsTest, TermToHashSize) {
  const IndexTerm term = "example_term";
  const IndexHash hash = indexer::term_to_hash(term);
  const std::size_t expected = 6;
  EXPECT_EQ(expected, hash.size());
}

TEST(IndexerUtilsTest, CreateIndexDirectories) {
  const IndexPath testPath = "test_index";
  indexer::create_index_directories(testPath);
  EXPECT_TRUE(std::filesystem::exists(testPath / "index"));
  EXPECT_TRUE(std::filesystem::exists(testPath / "index" / "docs"));
  EXPECT_TRUE(std::filesystem::exists(testPath / "index" / "entries"));
}

TEST(IndexerUtilsTest, ConvertToEntryOutput) {
  const IndexTerm term = "matrix";
  const std::vector<IndexDocToPos> doc_to_pos_vec = {{11, 0}, {22, 0}, {22, 1}};
  const IndexText output =
      indexer::convert_to_entry_output(term, doc_to_pos_vec);
  const std::string expected_output = "matrix 3 11 1 0 22 2 0 1 \n";
  EXPECT_EQ(output, expected_output);
}

TEST(TextIndexWriterTest, Write) {
  IndexBuilder indexbuilder({"the"}, 3, 6);
  indexbuilder.add_document(199903, "The Matrix");
  indexbuilder.add_document(200305, "The Matrix Reloaded");
  indexbuilder.add_document(200311, "The Matrix Revolution");
  const Index index = indexbuilder.index();
  const IndexPath testPath = "./build/test_index";
  const TextIndexWriter writer;
  writer.write(testPath, index);

  // get text from doc1 and check it
  std::ifstream docsFile(testPath / "index" / "docs" / "199903");
  ASSERT_TRUE(docsFile.is_open());
  std::string docText;
  std::getline(docsFile, docText);
  EXPECT_EQ(docText, "The Matrix");
  docsFile.close();

  // get text from entry of term1 and check
  std::ifstream entriesFile(testPath / "index" / "entries" /
                            indexer::term_to_hash("matrix"));
  ASSERT_TRUE(entriesFile.is_open());
  std::string entryText;
  std::getline(entriesFile, entryText);
  EXPECT_EQ(entryText, "matrix 3 199903 1 0 200305 1 0 200311 1 0 ");
  entriesFile.close();
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}