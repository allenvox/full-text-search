#include <binary/binary.hpp>

#include <cstring>

void BinaryHandler::increase_capacity(size_t new_capacity) {
  if (new_capacity <= capacity_) {
    return;
  }
  const size_t position = cursor_ - data_;
  capacity_ = new_capacity;
  data_ = static_cast<char *>(realloc(data_, capacity_));
  if (data_ == nullptr) {
    throw std::runtime_error("Not enough memory");
  }
  cursor_ = data_ + position;
}

BinaryHandler::BinaryHandler(char *data, size_t size)
    : capacity_(size), size_(size), data_(data), cursor_(data),
      is_allocated(false) {
  if (data_ == nullptr) {
    throw std::invalid_argument("Buffer is null");
  }
}

template <typename T> void BinaryHandler::write(T value) {
  if (capacity_ - (cursor_ - data_) < sizeof(value)) {
    increase_capacity(capacity_ * 2 + sizeof(value));
  }
  std::memcpy(cursor_, &value, sizeof(value));
  cursor_ += sizeof(value);
  size_ += sizeof(value);
}

template <typename StringType> void BinaryHandler::write_string(const StringType &value) {
  if (capacity_ - (cursor_ - data_) < value.size()) {
    increase_capacity(capacity_ * 2 + value.size());
  }
  write<uint8_t>(value.size());
  std::memcpy(cursor_, value.data(), value.size());
  cursor_ += value.size();
  size_ += value.size();
}

template <typename T> void BinaryHandler::read(T &value) {
  std::memcpy(&value, cursor_, sizeof(value));
  cursor_ += sizeof(value);
}

template <typename StringType> void BinaryHandler::read_string(StringType &value) {
  uint8_t length = 0;
  read(length);
  value = StringType(cursor_, cursor_ + length);
  cursor_ += length;
}

template <typename T> void BinaryHandler::skip(size_t count) {
  cursor_ += count * sizeof(T);
  if (static_cast<std::size_t>(cursor_ - data_) >= capacity_) {
    cursor_ = data_ + capacity_ - 1;
  }
}

void BinaryHandler::skip_string(size_t count) {
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

void BinaryHandler::set_data(char *data, size_t size) {
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