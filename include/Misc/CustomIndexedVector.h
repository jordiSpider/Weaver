#ifndef CUSTOM_INDEXED_VECTOR_H_
#define CUSTOM_INDEXED_VECTOR_H_


#include <vector>
#include <boost/serialization/vector.hpp>


template <typename IndexType, typename ContentType>
class CustomIndexedVector {
public:
    CustomIndexedVector()
        : data()
    {

    }

    CustomIndexedVector(const unsigned int n)
        : data(n)
    {

    }

    CustomIndexedVector(const unsigned int n, const ContentType& val)
        : data(n, val)
    {

    }

    CustomIndexedVector(const std::vector<ContentType> &values)
        : data(values)
    {
        
    }

    void resize(const unsigned int n)
    {
        data.resize(n);
    }

    void resize(const unsigned int n, const ContentType& val)
    {
        data.resize(n, val);
    }

    void reserve(const unsigned int n)
    {
        data.reserve(n);
    }

    const unsigned int size() const
    {
        return data.size();
    }

    bool empty() const noexcept
    {
        return data.empty();
    }

    ContentType& operator[](const IndexType& index)
    {
        #ifdef DEBUG
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }
        #endif

        return data[static_cast<size_t>(index)];
    }
    const ContentType& operator[](const IndexType& index) const
    {
        #ifdef DEBUG
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }
        #endif

        return data[static_cast<size_t>(index)];
    }

    ContentType& at(const IndexType& index)
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(static_cast<size_t>(index));
    }

    const ContentType& at(const IndexType& index) const
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(static_cast<size_t>(index));
    }

    void push_back(const ContentType &val)
    {
        data.push_back(val);
    }

    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        data.emplace_back(std::forward<Args>(args)...);
    }

    typename std::vector<ContentType>::reference back() {
        return data.back();
    }

    typename std::vector<ContentType>::const_reference back() const {
        return data.back();
    }

    typename std::vector<ContentType>::iterator begin() noexcept {
        return data.begin();
    }

    typename std::vector<ContentType>::const_iterator begin() const noexcept {
        return data.cbegin();
    }

    typename std::vector<ContentType>::const_iterator cbegin() const noexcept {
        return data.cbegin();
    }

    // Sobrecarga del operador end() para permitir el bucle for-each
    typename std::vector<ContentType>::iterator end() noexcept {
        return data.end();
    }

    typename std::vector<ContentType>::const_iterator end() const noexcept {
        return data.cend();
    }

    typename std::vector<ContentType>::const_iterator cend() const noexcept {
        return data.cend();
    }

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & data;
    }

private:
    std::vector<ContentType> data;

    friend class boost::serialization::access;
};



// Especialización para bool
// En esta especialización se implementa tal y como lo hace std::vector<bool>
// devolviendo un objeto de tipo std::vector<bool>::reference en el operador []
// en lugar de bool&, ya que esto daría error, además esta forma de devolver los
// elementos hace que los bools se interpreten como bits, ahorrando memoria.
template <typename IndexType>
class CustomIndexedVector<IndexType, bool> {
public:
    CustomIndexedVector()
        : data()
    {

    }

    CustomIndexedVector(const unsigned int n)
        : data(n)
    {

    }

    CustomIndexedVector(const unsigned int n, const bool& val)
        : data(n, val)
    {

    }

    CustomIndexedVector(const std::vector<bool> &values)
        : data(values)
    {
        
    }

    void resize(const unsigned int n)
    {
        data.resize(n);
    }

    void resize(const unsigned int n, const bool& val)
    {
        data.resize(n, val);
    }

    void reserve(const unsigned int n)
    {
        data.reserve(n);
    }

    const unsigned int size() const
    {
        return data.size();
    }

    bool empty() const noexcept
    {
        return data.empty();
    }

    std::vector<bool>::reference operator[](const IndexType& index)
    {
        #ifdef DEBUG
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }
        #endif

        return data[static_cast<size_t>(index)];
    }
    std::vector<bool>::const_reference operator[](const IndexType& index) const
    {
        #ifdef DEBUG
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }
        #endif

        return data[static_cast<size_t>(index)];
    }

    std::vector<bool>::reference at(const IndexType& index)
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(static_cast<size_t>(index));
    }

    std::vector<bool>::const_reference at(const IndexType& index) const
    {
        if(static_cast<size_t>(index) >= data.size())
        {
            throw std::out_of_range("The value of the index is greater than the size of the vector");
        }

        return data.at(static_cast<size_t>(index));
    }

    typename std::vector<bool>::reference back() {
        return data.back();
    }

    typename std::vector<bool>::const_reference back() const {
        return data.back();
    }

    typename std::vector<bool>::iterator begin() noexcept {
        return data.begin();
    }

    typename std::vector<bool>::const_iterator begin() const noexcept {
        return data.cbegin();
    }

    typename std::vector<bool>::const_iterator cbegin() const noexcept {
        return data.cbegin();
    }

    // Sobrecarga del operador end() para permitir el bucle for-each
    typename std::vector<bool>::iterator end() noexcept {
        return data.end();
    }

    typename std::vector<bool>::const_iterator end() const noexcept {
        return data.cend();
    }

    typename std::vector<bool>::const_iterator cend() const noexcept {
        return data.cend();
    }

    /**
     * @brief Serialize the CustomIndexedVector<bool> object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & data;
    }

private:
    std::vector<bool> data;

    friend class boost::serialization::access;
};

#endif // CUSTOM_INDEXED_VECTOR_H_
