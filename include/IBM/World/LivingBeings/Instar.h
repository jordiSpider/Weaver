#ifndef INSTAR_H_
#define INSTAR_H_


#include <ostream>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
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
    friend Instar operator+(const Instar& lhs, const unsigned int& rhs);
    friend Instar operator+(const unsigned int& lhs, const Instar& rhs);
    friend bool operator<(const Instar& lhs, const Instar& rhs);
    friend bool operator<=(const Instar& lhs, const Instar& rhs);
    friend bool operator>(const Instar& lhs, const Instar& rhs);
    friend bool operator>=(const Instar& lhs, const Instar& rhs);
    friend bool operator==(const Instar& lhs, const Instar& rhs);
    friend bool operator!=(const Instar& lhs, const Instar& rhs);
    friend std::ostream& operator<<(std::ostream& os, const Instar& instar);
    explicit operator size_t() const;

    /**
     * @brief Serialize the Instar object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

template <>
struct std::hash<Instar> {
    std::size_t operator()(const Instar& instar) const;
};

#endif /* INSTAR_H_ */
