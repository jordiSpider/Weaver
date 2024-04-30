/*
 * point3D.h
 *
 *  Created on: Aug 2, 2012
 *      Author: jrbcast
 */

#ifndef COORDINATE3D_H_
#define COORDINATE3D_H_

template<class T>
class Coordinate3D {
private:
	const T x;
	const T y;
	const T z;
public:
	Coordinate3D(const T &x, const T &y, const T &z) : x(x), y(y), z(z) {}

	// Getters
	const T getX() const { return x; }
	const T getY() const { return y; }
	const T getZ() const { return z; }
};

#endif /* COORDINATE3D_H_ */


