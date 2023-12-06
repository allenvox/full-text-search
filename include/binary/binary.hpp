#pragma once

#include <cstdlib>
#include <cstring>
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

  BinaryHandler(char *data, size_t size)
      : capacity_(size), size_(size), data_(data), cursor_(data),
        is_allocated(false) {
    if (data_ == nullptr) {
      throw std::invalid_argument("Buffer is null");
    }
  }

  BinaryHandler(BinaryHandler &other) = default;
  BinaryHandler(BinaryHandler &&other) = default;
  BinaryHandler &operator=(const BinaryHandler &other) = default;
  BinaryHandler &operator=(BinaryHandler &&other) = default;

  ~BinaryHandler() {
    if (is_allocated) {
      free(data_);
    }
  }

  template <typename T> void write(T value) {
    if (capacity_ - (cursor_ - data_) < sizeof(value)) {
      increase_capacity(capacity_ * 2 + sizeof(value));
    }
    std::memcpy(cursor_, &value, sizeof(value));
    cursor_ += sizeof(value);
    size_ += sizeof(value);
  }

  template <typename StringType> void write_string(const StringType &value) {
    if (capacity_ - (cursor_ - data_) < value.size()) {
      increase_capacity(capacity_ * 2 + value.size());
    }
    write<uint8_t>(value.size());
    std::memcpy(cursor_, value.data(), value.size());
    cursor_ += value.size();
    size_ += value.size();
  }

  template <typename T> void read(T &value) {
    std::memcpy(&value, cursor_, sizeof(value));
    cursor_ += sizeof(value);
  }

  template <typename StringType> void read_string(StringType &value) {
    uint8_t length = 0;
    read(length);
    value = StringType(cursor_, cursor_ + length);
    cursor_ += length;
  }

  template <typename T> void skip(size_t count) {
    cursor_ += count * sizeof(T);
    if (static_cast<std::size_t>(cursor_ - data_) >= capacity_) {
      cursor_ = data_ + capacity_ - 1;
    }
  }

  void skip_string(size_t count) {
    for (std::size_t i = 0; i < count; i++) {
      uint8_t length = 0;
      read(length);
      cursor_ += length;
      if (static_cast<std::size_t>(cursor_ - data_) >= capacity_) {
        cursor_ = data_ + capacity_ - 1;
        break;
      }
    }
  }

  void set_data(char *data, size_t size) {
    if (data == nullptr) {
      throw std::invalid_argument("Buffer is null");
    }
    if (is_allocated) {
      free(data_);
      is_allocated = false;
    }
    capacity_ = size;
    size_ = size;
    data_ = data;
    cursor_ = data;
  }

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
  void increase_capacity(size_t new_capacity) {
    if (new_capacity <= capacity_) {
      return;
    }
    const size_t position = cursor_ - data_;
    capacity_ = new_capacity;
    char *tmp = (char*)realloc(data_, capacity_);
    if (tmp != nullptr) {
      data_ = tmp;
    } else {
      throw std::runtime_error("Not enough memory");
    }
    /*data_ = static_cast<char *>(realloc(data_, capacity_));
    if (data_ == nullptr) {
      throw std::runtime_error("Not enough memory");
    }*/
    cursor_ = data_ + position;
  }
};
