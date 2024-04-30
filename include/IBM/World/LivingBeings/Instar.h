#ifndef INSTAR_H_
#define INSTAR_H_


#include <ostream>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>


class Instar {
private:
    unsigned int value;

    friend class boost::serialization::access;

public:
    Instar();
    explicit Instar(unsigned int value);
    Instar(const Instar &other);
    ~Instar();
    inline const unsigned int& getValue() const { return value; }
    inline void moveOnPreviousInstar() { value--; }
    inline void moveOnNextInstar() { value++; }
    friend Instar operator+(const Instar& lhs, const int& rhs);
    friend Instar operator+(const int& lhs, const Instar& rhs);
    friend Instar operator-(const Instar& lhs, const int& rhs);
    friend Instar operator-(const int& lhs, const Instar& rhs);
    Instar& operator++();
    Instar operator++(int);
    friend bool operator<(const Instar& lhs, const Instar& rhs);
    friend bool operator<(const Instar& lhs, const int& rhs);
    friend bool operator<(const int& lhs, const Instar& rhs);
    friend bool operator<=(const Instar& lhs, const Instar& rhs);
    friend bool operator<=(const Instar& lhs, const int& rhs);
    friend bool operator<=(const int& lhs, const Instar& rhs);
    friend bool operator>(const Instar& lhs, const Instar& rhs);
    friend bool operator>(const Instar& lhs, const int& rhs);
    friend bool operator>(const int& lhs, const Instar& rhs);
    friend bool operator>=(const Instar& lhs, const Instar& rhs);
    friend bool operator>=(const Instar& lhs, const int& rhs);
    friend bool operator>=(const int& lhs, const Instar& rhs);
    friend bool operator==(const Instar& lhs, const Instar& rhs);
    friend bool operator==(const Instar& lhs, const int& rhs);
    friend bool operator==(const int& lhs, const Instar& rhs);
    friend bool operator!=(const Instar& lhs, const Instar& rhs);
    friend bool operator!=(const Instar& lhs, const int& rhs);
    friend bool operator!=(const int& lhs, const Instar& rhs);
    friend std::ostream& operator<<(std::ostream& os, const Instar& instar);
    explicit operator size_t() const;

    /**
     * @brief Serialize the Instar object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) ;
};

template <>
struct std::hash<Instar> {
    std::size_t operator()(const Instar& instar) const;
};

template <typename T>
class InstarVector {
private:
    std::vector<T> data;

    friend class boost::serialization::access;

public:
    InstarVector()
        : data()
    {

    }

    InstarVector(const unsigned int n)
        : data(n)
    {

    }

    InstarVector(const unsigned int n, const T& val)
        : data(n, val)
    {

    }

    InstarVector(const std::vector<T> &values)
        : data(values)
    {
        
    }

    void resize(const unsigned int n)
    {
        data.resize(n);
    }

    void resize(const unsigned int n, const T& val)
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

    T& operator[](const Instar& index)
    {
        #ifdef DEBUG
        if(index.getValue() >= data.size())
        {
            throw std::out_of_range("The value of the instar is greater than the size of the vector");
        }
        #endif

        return data[index.getValue()];
    }
    const T& operator[](const Instar& index) const
    {
        #ifdef DEBUG
        if(index.getValue() >= data.size())
        {
            throw std::out_of_range("The value of the instar is greater than the size of the vector");
        }
        #endif

        return data[index.getValue()];
    }

    T& at(const Instar& index)
    {
        if(index.getValue() >= data.size())
        {
            throw std::out_of_range("The value of the instar is greater than the size of the vector");
        }

        return data.at(index.getValue());
    }

    const T& at(const Instar& index) const
    {
        if(index.getValue() >= data.size())
        {
            throw std::out_of_range("The value of the instar is greater than the size of the vector");
        }

        return data.at(index.getValue());
    }

    void push_back(const T &val)
    {
        data.push_back(val);
    }

    typename std::vector<T>::iterator begin() noexcept {
        return data.begin();
    }

    typename std::vector<T>::const_iterator begin() const noexcept {
        return data.cbegin();
    }

    typename std::vector<T>::const_iterator cbegin() const noexcept {
        return data.cbegin();
    }

    // Sobrecarga del operador end() para permitir el bucle for-each
    typename std::vector<T>::iterator end() noexcept {
        return data.end();
    }

    typename std::vector<T>::const_iterator end() const noexcept {
        return data.cend();
    }

    typename std::vector<T>::const_iterator cend() const noexcept {
        return data.cend();
    }

    /**
     * @brief Serialize the InstarVector object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & data;
    }
};

// Especialización para bool
// En esta especialización se implementa tal y como lo hace std::vector<bool>
// devolviendo un objeto de tipo std::vector<bool>::reference en el operador []
// en lugar de bool&, ya que esto daría error, además esta forma de devolver los
// elementos hace que los bools se interpreten como bits, ahorrando memoria.
template <>
class InstarVector<bool> {
private:
    std::vector<bool> data;

    friend class boost::serialization::access;

public:
    InstarVector()
        : data()
    {

    }

    InstarVector(const unsigned int n)
        : data(n)
    {

    }

    InstarVector(const unsigned int n, const bool& val)
        : data(n, val)
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

    std::vector<bool>::reference operator[](const Instar& index)
    {
        #ifdef DEBUG
        if(index.getValue() >= data.size())
        {
            throw std::out_of_range("The value of the instar is greater than the size of the vector");
        }
        #endif

        return data[index.getValue()];
    }
    std::vector<bool>::const_reference operator[](const Instar& index) const
    {
        #ifdef DEBUG
        if(index.getValue() >= data.size())
        {
            throw std::out_of_range("The value of the instar is greater than the size of the vector");
        }
        #endif

        return data[index.getValue()];
    }

    std::vector<bool>::reference at(const Instar& index)
    {
        if(index.getValue() >= data.size())
        {
            throw std::out_of_range("The value of the instar is greater than the size of the vector");
        }

        return data.at(index.getValue());
    }

    std::vector<bool>::const_reference at(const Instar& index) const
    {
        if(index.getValue() >= data.size())
        {
            throw std::out_of_range("The value of the instar is greater than the size of the vector");
        }

        return data.at(index.getValue());
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
     * @brief Serialize the InstarVector<bool> object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & data;
    }
};

#endif /* INSTAR_H_ */
