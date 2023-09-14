#include <common/common.hpp>

#include <gtest/gtest.h>

TEST(ClearTextTest, Uppercase) {
    const NgramParser parser;
    const NgramText input = "DR JEKYLL AND MR HYDE";
    const NgramText result = parser.clear_text(input);
    const NgramText expected = "dr jekyll and mr hyde";
    EXPECT_EQ(expected, result);
}

TEST(ClearTextTest, Punctuation) {
    const NgramParser parser;
    const NgramText input = "dr. jekyll, mr. hyde";
    const NgramText result = parser.clear_text(input);
    const NgramText expected = "dr jekyll mr hyde";
    EXPECT_EQ(expected, result);
}

TEST(ClearTextTest, Combined) {
    const NgramParser parser;
    const NgramText input = "Dr. Jekyll, Mr. Hyde";
    const NgramText result = parser.clear_text(input);
    const NgramText expected = "dr jekyll mr hyde";
    EXPECT_EQ(expected, result);
}

TEST(ClearTextTest, EmptyInput) {
    const NgramParser parser;
    const NgramText input = "";
    const NgramText result = parser.clear_text(input);
    const NgramText expected = "";
    EXPECT_EQ(expected, result);
}

TEST(SplitInWordsTest, Trivial) {
    const NgramParser parser;
    const NgramText input = "Dr. Jekyll and Mr. Hyde";
    const NgramWords result = parser.split_in_words(input);
    const NgramWords expected{"Dr.", "Jekyll", "and", "Mr.", "Hyde"};
    EXPECT_EQ(expected, result);
}

TEST(SplitInWordsTest, EmptyInput) {
    const NgramParser parser;
    const NgramText input = "";
    const NgramWords result = parser.split_in_words(input);
    const NgramWords expected{""};
    EXPECT_EQ(expected, result);
}

TEST(RemoveStopWordsTest, Trivial) {
    const NgramParser parser;
    const NgramWords input_arg1{"dr", "jekyll", "and", "mr", "hyde"};
    const NgramStopWords input_arg2{"and", "dr", "mr"};
    const NgramWords result = parser.remove_stop_words(input_arg1, input_arg2);
    const NgramWords expected{"jekyll", "hyde"};
    EXPECT_EQ(expected, result);
}

TEST(RemoveStopWordsTest, NoStopWords) {
    const NgramParser parser;
    const NgramWords input_arg1{"jekyll", "hyde"};
    const NgramStopWords input_arg2{};
    const NgramWords result = parser.remove_stop_words(input_arg1, input_arg2);
    const NgramWords expected{"jekyll", "hyde"};
    EXPECT_EQ(expected, result);
}

TEST(GenerateNgramsTest, Trivial) {
    const NgramParser parser;
    const NgramWords input{"jekyll", "hyde"};
    const NgramLength minlen = 3;
    const NgramLength maxlen = 6;
    const NgramWords result = parser.generate_ngrams(input, {}, minlen, maxlen);
    const NgramWords expected{"jek 0",    "jeky 0", "jekyl 0",
                              "jekyll 0", "hyd 1",  "hyde 1"};
    EXPECT_EQ(expected, result);
}

TEST(GenerateNgramsTest, NoWords) {
    const NgramParser parser;
    const NgramWords input{};
    const NgramLength minlen = 3;
    const NgramLength maxlen = 6;
    const NgramWords result = parser.generate_ngrams(input, {}, minlen, maxlen);
    const NgramWords expected;
    EXPECT_EQ(expected, result);
}

TEST(ParseTest, Trivial) {
    const NgramParser parser;
    const NgramText input = "Dr. Jekyll and Mr. Hyde";
    const NgramStopWords stop_words{"and", "dr", "mr"};
    const NgramLength minlen = 3;
    const NgramLength maxlen = 6;
    const NgramWords result = parser.parse(input, stop_words, minlen, maxlen);
    const NgramWords expected{"jek 0",    "jeky 0", "jekyl 0",
                              "jekyll 0", "hyd 1",  "hyde 1"};
    EXPECT_EQ(expected, result);
}

TEST(ParseTest, NoWords) {
    const NgramParser parser;
    const NgramText input = "";
    const NgramStopWords stop_words{"and", "dr", "mr"};
    const NgramLength minlen = 3;
    const NgramLength maxlen = 6;
    const NgramWords result = parser.parse(input, stop_words, minlen, maxlen);
    const NgramWords expected;
    EXPECT_EQ(expected, result);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}