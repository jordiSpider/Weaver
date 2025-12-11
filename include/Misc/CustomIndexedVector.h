/**
 * @file CustomIndexedVector.h
 * @brief Template class providing a vector indexed by a custom type with bounds checking and Boost.Serialization support.
 *
 * This class wraps around `std::vector` and allows indexing using a custom type (`IndexType`) while enforcing bounds checks.
 * It supports typical vector operations such as push_back, resize, iteration, and access. It also provides
 * a specialization for `bool` to save memory and behave like `std::vector<bool>`.
 */

#ifndef CUSTOM_INDEXED_VECTOR_H_
#define CUSTOM_INDEXED_VECTOR_H_


#include <vector>
#include <boost/serialization/vector.hpp>


/**
 * @class CustomIndexedVector
 * @brief A vector container indexed by a custom type with bounds checking.
 *
 * @tparam IndexType The type used to index the vector (e.g., enum or integer type).
 * @tparam ContentType The type of elements stored in the vector.
 *
 * @note Throws `std::out_of_range` if the index exceeds the vector size.
 */
template <typename IndexType, typename ContentType>
class CustomIndexedVector {
public:
    /// Default constructor, creates an empty vector.
    CustomIndexedVector()
        : data()
    {

    }

    /// Constructs a vector of size n with default-initialized elements.
    CustomIndexedVector(const size_t n)
        : data(n)
    {

    }

    /// Constructs a vector of size n initialized with a given value.
    CustomIndexedVector(const size_t n, const ContentType& val)
        : data(n, val)
    {

    }

    /// Constructs a vector from an existing std::vector.
    CustomIndexedVector(const std::vector<ContentType> &values)
        : data(values)
    {
        
    }

    /// Returns a const reference to the internal vector.
    const std::vector<ContentType>& getData() const
    {
        return data;
    }

    /// Resizes the vector to contain n elements.
    void resize(const size_t n)
    {
        data.resize(n);
    }

    /// Resizes the vector to contain n elements, initializing new elements with val.
    void resize(const size_t n, const ContentType& val)
    {
        data.resize(n, val);
    }

    /// Reserves memory for at least n elements.
    void reserve(const size_t n)
    {
        data.reserve(n);
    }

    /// Returns the number of elements in the vector.
    size_t size() const
    {
        return data.size();
    }

    /// Returns true if the vector is empty.
    bool empty() const noexcept
    {
        return data.empty();
    }

    /// Clears all elements in the vector.
    void clear()
    {
        data.clear();
    }

    /**
     * @brief Access element by index (non-const).
     * 
     * Provides unchecked access to the element at the given index. Throws
     * std::out_of_range if the index is out of bounds.
     * 
     * @param index Index of the element.
     * @return Reference to the element at the specified index.
     * @throws std::out_of_range if index >= size().
     */
    ContentType& operator[](const IndexType& index)
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data[static_cast<size_t>(index)];
    }

    /**
     * @brief Access element by index (const version).
     * 
     * Provides unchecked access to the element at the given index for
     * const objects. Throws std::out_of_range if the index is out of bounds.
     * 
     * @param index Index of the element.
     * @return Const reference to the element at the specified index.
     * @throws std::out_of_range if index >= size().
     */
    const ContentType& operator[](const IndexType& index) const
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data[static_cast<size_t>(index)];
    }

    /**
     * @brief Access element by index with bounds checking (non-const).
     * 
     * Checks if the index is within bounds and returns a reference to the
     * element. Throws std::out_of_range if the index is invalid.
     * 
     * @param index Index of the element.
     * @return Reference to the element.
     * @throws std::out_of_range if index >= size().
     */
    ContentType& at(const IndexType& index)
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(static_cast<size_t>(index));
    }

    /**
     * @brief Access element by index with bounds checking (const version).
     * 
     * Checks if the index is within bounds and returns a const reference
     * to the element. Throws std::out_of_range if the index is invalid.
     * 
     * @param index Index of the element.
     * @return Const reference to the element.
     * @throws std::out_of_range if index >= size().
     */
    const ContentType& at(const IndexType& index) const
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(static_cast<size_t>(index));
    }

    /**
     * @brief Access element by numeric index (non-const).
     * 
     * Provides bounds-checked access using a size_t index.
     * 
     * @param index Numeric index of the element.
     * @return Reference to the element at the specified index.
     * @throws std::out_of_range if index >= size().
     */
    ContentType& getValue(const size_t& index)
    {
        if(index >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(index);
    }

    /**
     * @brief Access element by numeric index (const version).
     * 
     * Provides bounds-checked access using a size_t index for const objects.
     * 
     * @param index Numeric index of the element.
     * @return Const reference to the element at the specified index.
     * @throws std::out_of_range if index >= size().
     */
    const ContentType& getValue(const size_t& index) const
    {
        if(index >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(index);
    }

    /// Adds an element to the end of the vector.
    void push_back(const ContentType &val)
    {
        data.push_back(val);
    }

    /// Constructs an element in-place at the end of the vector.
    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        data.emplace_back(std::forward<Args>(args)...);
    }

    /**
     * @brief Accesses the last element in the container (non-const).
     * 
     * @return Reference to the last element.
     * @throws std::out_of_range if the container is empty.
     */
    typename std::vector<ContentType>::reference back() {
        return data.back();
    }

    /**
     * @brief Accesses the last element in the container (const version).
     * 
     * @return Const reference to the last element.
     * @throws std::out_of_range if the container is empty.
     */
    typename std::vector<ContentType>::const_reference back() const {
        return data.back();
    }

    /**
     * @brief Returns an iterator to the beginning of the container (non-const).
     * 
     * Allows range-based for loops and other iterator-based access.
     * 
     * @return Iterator pointing to the first element.
     */
    typename std::vector<ContentType>::iterator begin() noexcept {
        return data.begin();
    }

    /**
     * @brief Returns a const iterator to the beginning of the container.
     * 
     * @return Const iterator pointing to the first element.
     */
    typename std::vector<ContentType>::const_iterator begin() const noexcept {
        return data.cbegin();
    }

    /**
     * @brief Returns a const iterator to the beginning of the container.
     * 
     * Equivalent to `begin() const`.
     * 
     * @return Const iterator pointing to the first element.
     */
    typename std::vector<ContentType>::const_iterator cbegin() const noexcept {
        return data.cbegin();
    }

    /**
     * @brief Returns an iterator to the end of the container (non-const).
     * 
     * The iterator points past the last element.
     * 
     * @return Iterator pointing past the last element.
     */
    typename std::vector<ContentType>::iterator end() noexcept {
        return data.end();
    }

    /**
     * @brief Returns a const iterator to the end of the container.
     * 
     * @return Const iterator pointing past the last element.
     */
    typename std::vector<ContentType>::const_iterator end() const noexcept {
        return data.cend();
    }

    /**
     * @brief Returns a const iterator to the end of the container.
     * 
     * Equivalent to `end() const`.
     * 
     * @return Const iterator pointing past the last element.
     */
    typename std::vector<ContentType>::const_iterator cend() const noexcept {
        return data.cend();
    }

    /**
     * @brief Serializes the vector using Boost.Serialization.
     * @tparam Archive Archive type.
     * @param ar Archive to serialize to or from.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & data;
    }

private:
    std::vector<ContentType> data; /**< Internal storage of elements */
};



/**
 * @brief Specialization of CustomIndexedVector for `bool` values.
 *
 * This specialization stores bools efficiently and returns `std::vector<bool>::reference` 
 * from operator[] and at() to maintain bit-level storage like std::vector<bool>.
 *
 * @tparam IndexType The type used to index the vector.
 */
template <typename IndexType>
class CustomIndexedVector<IndexType, bool> {
public:
    /// Default constructor, creates an empty vector.
    CustomIndexedVector()
        : data()
    {

    }

    /// Constructs a vector of size n with default-initialized elements.
    CustomIndexedVector(const size_t n)
        : data(n)
    {

    }

    /// Constructs a vector of size n initialized with a given value.
    CustomIndexedVector(const size_t n, const bool& val)
        : data(n, val)
    {

    }

    /// Constructs a vector from an existing std::vector.
    CustomIndexedVector(const std::vector<bool> &values)
        : data(values)
    {
        
    }

    /// Returns a const reference to the internal vector.
    const std::vector<bool>& getData() const
    {
        return data;
    }

    /// Resizes the vector to contain n elements.
    void resize(const size_t n)
    {
        data.resize(n);
    }

    /// Resizes the vector to contain n elements, initializing new elements with val.
    void resize(const size_t n, const bool& val)
    {
        data.resize(n, val);
    }

    /// Reserves memory for at least n elements.
    void reserve(const size_t n)
    {
        data.reserve(n);
    }

    /// Returns the number of elements in the vector.
    size_t size() const
    {
        return data.size();
    }

    /// Returns true if the vector is empty.
    bool empty() const noexcept
    {
        return data.empty();
    }

    /// Clears all elements in the vector.
    void clear()
    {
        data.clear();
    }

    /**
     * @brief Access element by index (non-const).
     * 
     * Provides unchecked access to the element at the given index. Throws
     * std::out_of_range if the index is out of bounds.
     * 
     * @param index Index of the element.
     * @return Reference to the element at the specified index.
     * @throws std::out_of_range if index >= size().
     */
    std::vector<bool>::reference operator[](const IndexType& index)
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data[static_cast<size_t>(index)];
    }

    /**
     * @brief Access element by index (const version).
     * 
     * Provides unchecked access to the element at the given index for
     * const objects. Throws std::out_of_range if the index is out of bounds.
     * 
     * @param index Index of the element.
     * @return Const reference to the element at the specified index.
     * @throws std::out_of_range if index >= size().
     */
    std::vector<bool>::const_reference operator[](const IndexType& index) const
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data[static_cast<size_t>(index)];
    }

    /**
     * @brief Access element by index with bounds checking (non-const).
     * 
     * Checks if the index is within bounds and returns a reference to the
     * element. Throws std::out_of_range if the index is invalid.
     * 
     * @param index Index of the element.
     * @return Reference to the element.
     * @throws std::out_of_range if index >= size().
     */
    std::vector<bool>::reference at(const IndexType& index)
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(static_cast<size_t>(index));
    }

    /**
     * @brief Access element by index with bounds checking (const version).
     * 
     * Checks if the index is within bounds and returns a const reference
     * to the element. Throws std::out_of_range if the index is invalid.
     * 
     * @param index Index of the element.
     * @return Const reference to the element.
     * @throws std::out_of_range if index >= size().
     */
    std::vector<bool>::const_reference at(const IndexType& index) const
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(static_cast<size_t>(index));
    }

    /**
     * @brief Access element by numeric index (non-const).
     * 
     * Provides bounds-checked access using a size_t index.
     * 
     * @param index Numeric index of the element.
     * @return Reference to the element at the specified index.
     * @throws std::out_of_range if index >= size().
     */
    std::vector<bool>::reference getValue(const size_t& index)
    {
        if(index >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(index);
    }

    /**
     * @brief Access element by numeric index (const version).
     * 
     * Provides bounds-checked access using a size_t index for const objects.
     * 
     * @param index Numeric index of the element.
     * @return Const reference to the element at the specified index.
     * @throws std::out_of_range if index >= size().
     */
    std::vector<bool>::const_reference getValue(const size_t& index) const
    {
        if(index >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(index);
    }

    /**
     * @brief Accesses the last element in the container (non-const).
     * 
     * @return Reference to the last element.
     * @throws std::out_of_range if the container is empty.
     */
    typename std::vector<bool>::reference back() {
        return data.back();
    }

    /**
     * @brief Accesses the last element in the container (const version).
     * 
     * @return Const reference to the last element.
     * @throws std::out_of_range if the container is empty.
     */
    typename std::vector<bool>::const_reference back() const {
        return data.back();
    }

    /**
     * @brief Returns an iterator to the beginning of the container (non-const).
     * 
     * Allows range-based for loops and other iterator-based access.
     * 
     * @return Iterator pointing to the first element.
     */
    typename std::vector<bool>::iterator begin() noexcept {
        return data.begin();
    }

    /**
     * @brief Returns a const iterator to the beginning of the container.
     * 
     * @return Const iterator pointing to the first element.
     */
    typename std::vector<bool>::const_iterator begin() const noexcept {
        return data.cbegin();
    }

    /**
     * @brief Returns a const iterator to the beginning of the container.
     * 
     * Equivalent to `begin() const`.
     * 
     * @return Const iterator pointing to the first element.
     */
    typename std::vector<bool>::const_iterator cbegin() const noexcept {
        return data.cbegin();
    }

    /**
     * @brief Returns an iterator to the end of the container (non-const).
     * 
     * The iterator points past the last element.
     * 
     * @return Iterator pointing past the last element.
     */
    typename std::vector<bool>::iterator end() noexcept {
        return data.end();
    }

    /**
     * @brief Returns a const iterator to the end of the container.
     * 
     * @return Const iterator pointing past the last element.
     */
    typename std::vector<bool>::const_iterator end() const noexcept {
        return data.cend();
    }

    /**
     * @brief Returns a const iterator to the end of the container.
     * 
     * Equivalent to `end() const`.
     * 
     * @return Const iterator pointing past the last element.
     */
    typename std::vector<bool>::const_iterator cend() const noexcept {
        return data.cend();
    }

    /**
     * @brief Serializes the bool vector using Boost.Serialization.
     * @tparam Archive Archive type.
     * @param ar Archive to serialize to or from.
     * @param version Version of serialization format (unused).
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        (void) version;
        
        ar & data;
    }

private:
    std::vector<bool> data; /**< Internal storage of bool elements */
};

#endif // CUSTOM_INDEXED_VECTOR_H_
