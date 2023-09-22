#include "ThreadSafeCircularBuffer.h"

template <typename T>
class ThreadSafeCircularBuffer<T>::locked_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    locked_iterator(pointer ptr, std::unique_ptr<std::lock_guard<std::mutex>> lock)
        : m_ptr(ptr), m_lock(std::move(lock)) {}

    locked_iterator(const locked_iterator&) = delete;

    locked_iterator(locked_iterator&& other) noexcept
        : m_ptr(other.m_ptr), m_lock(std::move(other.m_lock)) {}

    reference operator*() const { return *m_ptr; }
    pointer operator->() { return m_ptr; }
    locked_iterator& operator++() { m_ptr++; return *this; }
    locked_iterator operator++(int) { locked_iterator tmp = std::move(*this); ++(*this); return tmp; }
    friend bool operator== (const locked_iterator& a, const locked_iterator& b) { return a.m_ptr == b.m_ptr; }
    friend bool operator!= (const locked_iterator& a, const locked_iterator& b) { return a.m_ptr != b.m_ptr; }

private:
    pointer m_ptr;
    std::unique_ptr<std::lock_guard<std::mutex>> m_lock;
};

template <typename T>
ThreadSafeCircularBuffer<T>::ThreadSafeCircularBuffer(size_t size)
    : buffer_(size) {}

template <typename T>
bool ThreadSafeCircularBuffer<T>::push_back(const T& item) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.full()) buffer_.pop_front();
    buffer_.push_back(item);
    m_currentPosition++;
    return true;
}

template <typename T>
bool ThreadSafeCircularBuffer<T>::pop_front(T& item) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.empty()) {
        return false;
    }
    item = buffer_.front();
    buffer_.pop_front();
    m_currentPosition--;
    return true;
}

template <typename T>
size_t ThreadSafeCircularBuffer<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
}

template <typename T>
void ThreadSafeCircularBuffer<T>::setSize(size_t newSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.set_capacity(newSize);
}

template <typename T>
typename ThreadSafeCircularBuffer<T>::locked_iterator ThreadSafeCircularBuffer<T>::begin() {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(mutex_);
    return locked_iterator(&buffer_.front(), std::move(lock));
}

template <typename T>
typename ThreadSafeCircularBuffer<T>::locked_iterator ThreadSafeCircularBuffer<T>::end() {
    auto lock = std::make_unique<std::lock_guard<std::mutex>>(mutex_);
    return locked_iterator(&buffer_.back() + 1, std::move(lock));
}

template <typename T>
std::vector<T> ThreadSafeCircularBuffer<T>::copy() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::vector<T>(buffer_.begin(), buffer_.end());
}

template <typename T>
void ThreadSafeCircularBuffer<T>::restore(const std::vector<T>& source) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.clear();
    buffer_.set_capacity(source.size());
    for (const T& item : source) {
        buffer_.push_back(item);
    }
}

template <typename T>
int ThreadSafeCircularBuffer<T>::getCurrentPosition() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return m_currentPosition;
}

template <typename T>
void ThreadSafeCircularBuffer<T>::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.clear();
}

template <typename T>
T ThreadSafeCircularBuffer<T>::operator[](size_t i) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_[i];
}
