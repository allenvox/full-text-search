#include <fts/fts.hpp>

#include <gtest/gtest.h>

TEST(SumTest, Positive) {
    EXPECT_EQ(46.23561, fts::sum(12.1256, 34.11001));
    EXPECT_EQ(11.07, fts::sum(3.33, 7.74));
}

TEST(SumTest, Negative) {
    EXPECT_EQ(-5.3, fts::sum(-2.3, -3.0));
    EXPECT_EQ(-4123, fts::sum(-2023, -2100));
}

TEST(SumTest, PositiveNegative) {
    EXPECT_EQ(1.1, fts::sum(-5.1, 6.2));
    EXPECT_EQ(0.0, fts::sum(-12.4, 12.4));
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}