#ifndef POINT_MAP_H_
#define POINT_MAP_H_

#include <string>
#include <magic_enum.hpp>
#include <vector>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/Map/Points/Axis.h"
#include "Exceptions/LineInfoException.h"


class MapInterface;


class PointMap {
protected:
	std::vector<unsigned int> axisValues;

public:
	static std::unique_ptr<PointMap> createInstance(const MapInterface& mapInterface);

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

	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, PointMap* &pointMapPtr, const unsigned int version, const MapInterface& mapInterface);
    }
}

#endif /* POINT_MAP_H_ */