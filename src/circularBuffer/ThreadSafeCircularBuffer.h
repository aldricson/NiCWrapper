#ifndef THREADSAFECIRCULARBUFFER_H
#define THREADSAFECIRCULARBUFFER_H

#include "circular_buffer.h"
#include <mutex>
#include <vector>
#include <limits>
#include <type_traits>
#include <memory>
#include <algorithm>


template <typename T>
class ThreadSafeCircularBuffer {
public:
    class locked_iterator;

    explicit ThreadSafeCircularBuffer(size_t size);
    bool push_back(const T& item);
    bool pop_front(T& item);
    size_t size() const;
    void setSize(size_t newSize);
    locked_iterator begin();
    locked_iterator end();
    std::vector<T> copy() const;
    void restore(const std::vector<T>& source);
    int getCurrentPosition() const;
    void clear();
    T operator[](size_t i) const;

private:
    circular_buffer<T> buffer_;
    int m_currentPosition = 0;
    mutable std::mutex mutex_;
};

//#include "ThreadSafeCircularBuffer.cpp"

#endif // THREADSAFECIRCULARBUFFER_H
