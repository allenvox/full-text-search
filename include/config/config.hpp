#pragma once

#include <common/common.hpp>

struct Config {
  NgramStopWords stop_words;
  size_t min_length;
  size_t max_length;
};
using Config = struct Config;

extern Config DEFAULT_CONFIG;