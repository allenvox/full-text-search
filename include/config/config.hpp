#pragma once

#include <common/common.hpp>

struct Config {
  NgramStopWords stop_words;
  NgramLength min_length;
  NgramLength max_length;
};
using Config = struct Config;

Config DEFAULT_CONFIG = {{"and", "are", "but", "for", "into", "not",
                                "such", "that", "the", "their", "then", "there",
                                "these", "they", "this", "was", "will", "with"},
                               3,
                               7};