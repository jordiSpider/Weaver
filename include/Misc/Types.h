/*
 * types.h
 *
 *  Created on: Aug 6, 2012
 *      Author: jrbcast
 */

#ifndef TYPES_H_
#define TYPES_H_

#include "Coordinate3D.h"
#include <unordered_map>
#include <string>
#include <ostream>
#include <cfloat>
#include <vector>

#include "Exceptions/LineInfoException.h"


class Animal;

class ResourceInterface;



// Geometric
typedef Coordinate3D<float> fVector3D;	// 'f' stands for float
typedef Coordinate3D<float> fPoint3D;

typedef Coordinate3D<int> iVector3D;	// 'i' stands for int
typedef Coordinate3D<int> iPoint3D;

typedef Coordinate3D<unsigned int> uiVector3D;	// 'ui' stands for unsigned int
typedef Coordinate3D<unsigned int> uiPoint3D;

typedef Coordinate3D<double> dVector3D;	// 'd' stands for double
typedef Coordinate3D<double> dPoint3D;






// Sumo más uno porque la función devuelve el número de dígitos restando el bit del signo
#define MAX_NUM_DIGITS_DAY (std::numeric_limits<unsigned int>::digits10 + 1)


typedef unsigned int id_type;

// Sumo más uno porque la función devuelve el número de dígitos restando el bit del signo
#define MAX_NUM_DIGITS_ID (std::numeric_limits<id_type>::digits10 + 1)


#define NEG_DBL_MAX (-1 * DBL_MAX)


#endif /* TYPES_H_ */
