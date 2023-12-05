#pragma once

#include <cstdlib>
#include <iostream>

class BinaryHandler {
public:
  BinaryHandler() : is_allocated(true) {
    data_ = static_cast<char *>(malloc(capacity_));
    if (data_ == nullptr) {
      throw std::runtime_error("Not enough memory");
    }
    cursor_ = data_;
  }

  BinaryHandler(char *data, size_t size);
  BinaryHandler(BinaryHandler &other) = default;
  BinaryHandler(BinaryHandler &&other) = default;
  BinaryHandler &operator=(const BinaryHandler &other) = default;
  BinaryHandler &operator=(BinaryHandler &&other) = default;

  ~BinaryHandler() {
    if (is_allocated) {
      free(data_);
    }
  }

  template <typename T> void write(T value);
  template <typename StringType> void write_string(const StringType &value);
  template <typename T> void read(T &value);
  template <typename StringType> void read_string(StringType &value);
  template <typename T> void skip(std::size_t count);
  void skip_string(size_t count);
  void set_data(char *data, size_t size);
  void set_current_position(size_t position) { cursor_ = data_ + position; }
  size_t get_current_position() { return (cursor_ - data_); }
  char *data() { return data_; }
  char *current() const { return cursor_; }
  size_t size() const { return size_; }

private:
  size_t capacity_{10};
  size_t size_{0};
  char *data_;
  char *cursor_;
  bool is_allocated;
  void increase_capacity(size_t new_capacity);
};
