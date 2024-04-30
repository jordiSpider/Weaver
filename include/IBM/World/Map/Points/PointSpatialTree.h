#ifndef POINT_SPATIAL_TREE_H_
#define POINT_SPATIAL_TREE_H_

#include <vector>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/Map/Points/PointMap.h"
#include "IBM/World/Map/Points/Axis.h"



class PointSpatialTree : public PointMap {
protected:
	unsigned int depth;

public:
	PointSpatialTree();
	PointSpatialTree(const std::vector<unsigned int> &axisValues, const unsigned int &depth);
	PointSpatialTree(const PointSpatialTree &other);
	virtual ~PointSpatialTree();

	const unsigned int getDepth() const;

	bool operator==(const PointSpatialTree& other) const;
	bool operator!=(const PointSpatialTree& other) const;
	friend std::ostream& operator<<(std::ostream& os, const PointSpatialTree& point);

	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


#endif /* POINT_SPATIAL_TREE_H_ */