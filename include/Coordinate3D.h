/*
 * point3D.h
 *
 *  Created on: Aug 2, 2012
 *      Author: jrbcast
 */

#ifndef COORDINATE3D_H_
#define COORDINATE3D_H_

template< class T>
class Coordinate3D {
private:
	T x;
	T y;
	T z;
public:
	Coordinate3D();
	Coordinate3D(T newX, T newY, T newZ);
	virtual ~Coordinate3D();

	// Getters
	T X();
	T Y();
	T Z();

	// Setters
	void setX(T newValue);
	void setY(T newValue);
	void setZ(T newValue);

};

template<class T>
Coordinate3D<T>::Coordinate3D()
{
}

template<class T>
Coordinate3D<T>::Coordinate3D(T newX, T newY, T newZ)
{
	x=newX;
	y=newY;
	z=newZ;
}

template<class T>
Coordinate3D<T>::~Coordinate3D()
{
}

// Getters
template<class T>
T Coordinate3D<T>::X()
{
	return x;
}

template<class T>
T Coordinate3D<T>::Y()
{
	return y;
}

template<class T>
T Coordinate3D<T>::Z()
{
	return z;
}

// Setters
template<class T>
void Coordinate3D<T>::setX(T newValue)
{
	x=newValue;
}

template<class T>
void Coordinate3D<T>::setY(T newValue)
{
	y=newValue;
}

template<class T>
void Coordinate3D<T>::setZ(T newValue)
{
	z=newValue;
}



#endif /* COORDINATE3D_H_ */
