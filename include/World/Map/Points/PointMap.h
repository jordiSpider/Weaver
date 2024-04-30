#ifndef POINT_MAP_H_
#define POINT_MAP_H_

#include <string>
#include <magic_enum.hpp>
#include <vector>
#include <iostream>

#include "World/Map/Points/Axis.h"
#include "Exceptions/LineInfoException.h"




class PointMap {
protected:
	std::vector<unsigned int> axisValues;

public:
	PointMap();
	PointMap(const std::vector<unsigned int> &axisValues);
	PointMap(const PointMap &other);
	virtual ~PointMap();

	const unsigned int get(const Axis &axis) const;
	void set(const Axis & axis, const unsigned int newValue);
	const std::vector<unsigned int>& getAxisValues() const;

	bool operator==(const PointMap& other) const;
	bool operator!=(const PointMap& other) const;
	friend std::ostream& operator<<(std::ostream& os, const PointMap& point);
};


#endif /* POINT_MAP_H_ */