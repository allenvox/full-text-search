#pragma once

#include <common/common.hpp>

struct Config {
  NgramStopWords stop_words;
  NgramLength min_length;
  NgramLength max_length;
};
using Config = struct Config;