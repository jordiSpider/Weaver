/*
 * types.h
 *
 *  Created on: Aug 6, 2012
 *      Author: jrbcast
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <filesystem>
#include "Coordinate3D.h"
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <ostream>
#include <cfloat>

// Geometric
typedef Coordinate3D<float> fVector3D;	// 'f' stands for float
typedef Coordinate3D<float> fPoint3D;

typedef Coordinate3D<int> iVector3D;	// 'i' stands for int
typedef Coordinate3D<int> iPoint3D;

typedef Coordinate3D<unsigned int> uiVector3D;	// 'ui' stands for unsigned int
typedef Coordinate3D<unsigned int> uiPoint3D;

typedef Coordinate3D<double> dVector3D;	// 'd' stands for double
typedef Coordinate3D<double> dPoint3D;




namespace fs = std::filesystem;


typedef unsigned int date_type;

// Sumo más uno porque la función devuelve el número de dígitos restando el bit del signo
#define MAX_NUM_DIGITS_DAY (std::numeric_limits<date_type>::digits10 + 1)


typedef unsigned int id_type;

// Sumo más uno porque la función devuelve el número de dígitos restando el bit del signo
#define MAX_NUM_DIGITS_ID (std::numeric_limits<id_type>::digits10 + 1)


#define NEG_DBL_MAX (-1 * DBL_MAX)


class Instar {
private:
    unsigned int value;

public:
    Instar();
    explicit Instar(unsigned int value);
    inline const unsigned int& getValue() const { return value; }
    inline void moveOnPreviousInstar() { value--; }
    inline void moveOnNextInstar() { value++; }
    friend Instar operator+(const Instar& lhs, const int& rhs);
    friend Instar operator+(const int& lhs, const Instar& rhs);
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
};


#endif /* TYPES_H_ */
