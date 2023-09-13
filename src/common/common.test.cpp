#include <common/common.hpp>

#include <gtest/gtest.h>

TEST(ClearTextTest, Uppercase) {
    NgramParser parser;
    NgramText input = "DR JEKYLL AND MR HYDE";
    NgramText result = parser.clear_text(input);
    NgramText expected = "dr jekyll and mr hyde";
    EXPECT_EQ(expected, result);
}

TEST(ClearTextTest, Punctuation) {
    NgramParser parser;
    NgramText input = "dr. jekyll, mr. hyde";
    NgramText result = parser.clear_text(input);
    NgramText expected = "dr jekyll mr hyde";
    EXPECT_EQ(expected, result);
}

TEST(ClearTextTest, Combined) {
    NgramParser parser;
    NgramText input = "Dr. Jekyll, Mr. Hyde";
    NgramText result = parser.clear_text(input);
    NgramText expected = "dr jekyll mr hyde";
    EXPECT_EQ(expected, result);
}

TEST(SplitInWordsTest, Trivial) {
    NgramParser parser;
    NgramText input = "Dr. Jekyll and Mr. Hyde";
    NgramWords result = parser.split_in_words(input);
    NgramWords expected{"Dr.", "Jekyll", "and", "Mr.", "Hyde"};
    EXPECT_EQ(expected, result);
}

TEST(RemoveStopWordsTest, Trivial) {
    NgramParser parser;
    NgramWords input_arg1{"dr", "jekyll", "and", "mr", "hyde"};
    NgramStopWords input_arg2{"and", "dr", "mr"};
    NgramWords result = parser.remove_stop_words(input_arg1, input_arg2);
    NgramWords expected{"jekyll", "hyde"};
    EXPECT_EQ(expected, result);
}

TEST(GenerateNgramsTest, Trivial) {
    NgramParser parser;
    NgramWords input{"jekyll", "hyde"};
    NgramLength minlen = 3;
    NgramLength maxlen = 6;
    NgramWords result = parser.generate_ngrams(input, {}, minlen, maxlen);
    NgramWords expected{"jek 0",    "jeky 0", "jekyl 0",
                        "jekyll 0", "hyd 1",  "hyde 1"};
    EXPECT_EQ(expected, result);
}

TEST(ParseTest, Trivial) {
    NgramParser parser;
    NgramText input = "Dr. Jekyll and Mr. Hyde";
    NgramStopWords stop_words{"and", "dr", "mr"};
    NgramLength minlen = 3;
    NgramLength maxlen = 6;
    NgramWords result = parser.parse(input, stop_words, minlen, maxlen);
    NgramWords expected{"jek 0",    "jeky 0", "jekyl 0",
                        "jekyll 0", "hyd 1",  "hyde 1"};
    EXPECT_EQ(expected, result);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}