/**
 * @file RingBuffer.h
 * @brief Implements a generic fixed-size ring buffer with serialization support.
 *
 * This file defines a templated RingBuffer class that stores a fixed number of elements
 * in a circular buffer. It supports basic operations like push, clear, and mean calculation.
 * The buffer can be serialized using Boost.Serialization.
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_


#include <vector>
#include <numeric>

#include <boost/serialization/vector.hpp>

/**
 * @brief A fixed-size ring buffer (circular buffer) for storing elements of type T.
 *
 * The buffer overwrites the oldest elements when full. Provides a method to calculate
 * the mean of the stored elements and supports Boost serialization.
 *
 * @tparam T Type of elements stored in the buffer.
 */
template <typename T>
class RingBuffer {
public:
    /**
     * @brief Default constructor, creates an empty buffer with size 0.
     */
    RingBuffer()
        : maxSize(0), head(0) {}

    /**
     * @brief Constructs a RingBuffer with a given maximum size and initial value.
     * @param maxSize Maximum number of elements in the buffer.
     * @param initialValue Value to initialize all elements.
     */
    RingBuffer(std::size_t maxSize, const T& initialValue) 
        : maxSize(maxSize), initialValue(initialValue) {
        
        clear();
    }

    /**
     * @brief Adds a new element to the buffer.
     *
     * If the buffer is full, the oldest element is overwritten.
     *
     * @param value Element to add.
     */
    void push(const T& value) {
        buffer[head] = value;
        head = (head + 1) % maxSize;
    }

    /**
     * @brief Calculates the mean of the elements in the buffer.
     *
     * Specialization exists for `size_t` to avoid type issues.
     *
     * @return Mean value of elements.
     */
    T calculateMean() const {
        T sum = std::accumulate(buffer.begin(), buffer.end(), T());
        return sum / static_cast<double>(maxSize);
    }

    /**
     * @brief Clears the buffer and resets all elements to the initial value.
     */
    void clear() {
        buffer.clear();
        buffer.resize(maxSize, initialValue);
        head = 0;
    }

    /**
     * @brief Boost.Serialization method to serialize or deserialize the ring buffer.
     *
     * @tparam Archive Type of Boost archive.
     * @param ar Archive to read/write.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & maxSize;
        ar & buffer;
        ar & head;
        ar & initialValue;
    }

private:
    std::size_t maxSize;   /**< Maximum number of elements in the buffer */
    std::vector<T> buffer; /**< Storage for buffer elements */
    std::size_t head;      /**< Index of the next element to overwrite */
    T initialValue;        /**< Value used to initialize buffer elements */
};

/**
 * @brief Specialization of calculateMean for size_t type.
 *
 * @return Mean of elements in the buffer as size_t.
 */
template <>
inline size_t RingBuffer<size_t>::calculateMean() const {
    size_t sum = std::accumulate(buffer.begin(), buffer.end(), static_cast<size_t>(0u));
    return sum / maxSize;
}


#endif // RING_BUFFER_H_
