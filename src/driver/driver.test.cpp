#include <gtest/gtest.h>

#include <driver/driver.hpp>

#include <iostream>

IndexPath testPath = "test.csv";
IndexPath fakePath = "fake.csv";

rapidcsv::Document testDoc() {
  rapidcsv::Document doc;
  doc.InsertColumn(0, std::vector<std::size_t>{101, 202, 303}, "id");
  doc.InsertColumn(
      1, std::vector<std::string>{"Welcome", "Hello", "Good morning"}, "name");
  doc.InsertColumn(2, std::vector<int>{-1, 3, 1}, "random");
  doc.RemoveColumn(3);
  return doc;
}

TEST(DriverTest, CheckExist) {
  testDoc().Save(testPath);
  EXPECT_THROW(driver::check_if_exists(fakePath), std::runtime_error);
  EXPECT_NO_THROW(driver::check_if_exists(testPath));
}

TEST(DriverTest, GetCSV) {
  testDoc().Save(testPath);
  const rapidcsv::Document doc = driver::get_csv(testPath);
  EXPECT_EQ(doc.GetColumnCount(), 3);
}

TEST(DriverTest, GenerateIdx) {
  testDoc().Save(testPath);
  const Index index = driver::generate_index(testPath);
  EXPECT_EQ(index.docs.size(), 3);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}