#include <fts/fts.hpp>

#include <gtest/gtest.h>

TEST(SumTest, Trivial) { EXPECT_EQ(3, fts::sum(1, 2)); }

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}