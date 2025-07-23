#pragma once

#include <vector>
#include <mutex>
#include <algorithm>
#include <cstddef>

template <typename T>
class CircularBuffer {
public:
    CircularBuffer() : capacity_(0), writePos_(0), readPos_(0), dataSize_(0) {}

    void Resize(size_t capacity) {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.resize(capacity);
        capacity_ = capacity;
        writePos_ = 0;
        readPos_ = 0;
        dataSize_ = 0;
    }

    void Write(const T* data, size_t bytes) {
        if (bytes == 0 || capacity_ == 0) return;

        std::lock_guard<std::mutex> lock(mutex_);

        if (bytes > capacity_ - dataSize_) {
            size_t overflowBytes = bytes - (capacity_ - dataSize_);
            readPos_ = (readPos_ + overflowBytes) % capacity_;
            dataSize_ -= overflowBytes;
        }

        size_t firstChunk = std::min(bytes, capacity_ - writePos_);
        memcpy(&buffer_[writePos_], data, firstChunk);

        if (bytes > firstChunk) {
            memcpy(&buffer_[0], data + firstChunk, bytes - firstChunk);
        }

        writePos_ = (writePos_ + bytes) % capacity_;
        dataSize_ += bytes;
    }

    size_t Read(T* dest, size_t bytes) {
        if (bytes == 0 || capacity_ == 0) return 0;

        std::lock_guard<std::mutex> lock(mutex_);

        size_t bytesToRead = std::min(bytes, dataSize_);
        if (bytesToRead == 0) return 0;

        size_t firstChunk = std::min(bytesToRead, capacity_ - readPos_);

        memcpy(dest, &buffer_[readPos_], firstChunk);
        if (bytesToRead > firstChunk) {
            memcpy(dest + firstChunk, &buffer_[0], bytesToRead - firstChunk);
        }

        readPos_ = (readPos_ + bytesToRead) % capacity_;
        dataSize_ -= bytesToRead;

        return bytesToRead;
    }

    size_t GetSize() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return dataSize_;
    }

    size_t GetCapacity() const {
        return capacity_;
    }

private:
    std::vector<T> buffer_;
    mutable std::mutex mutex_;
    size_t capacity_;
    size_t writePos_;
    size_t readPos_;
    size_t dataSize_;
};