#include <gtest/gtest.h>

#include <driver/driver.hpp>

std::filesystem::path testPath = "build/test.csv";
std::filesystem::path fakePath = "fake";

TEST(DriverTest, CheckExist) {
  EXPECT_THROW(driver::check_if_exists(fakePath), std::runtime_error);
  EXPECT_NO_THROW(driver::check_if_exists(testPath));
}

TEST(DriverTest, GetCSV) {
  rapidcsv::Document doc = driver::get_csv(testPath);
  EXPECT_EQ(doc.GetColumnCount(), 3);
}

TEST(DriverTest, RemoveCols) {
  rapidcsv::Document doc = driver::get_csv(testPath);
  driver::remove_unneeded_cols(doc);
  EXPECT_EQ(doc.GetColumnCount(), 2);
}

TEST(DriverTest, GenerateIdx) {
  Index index = driver::generate_index(testPath);
  EXPECT_EQ(index.docs.size(), 3);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}