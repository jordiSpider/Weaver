/*
 * Edible.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#include "World/LivingBeings/LifeStage.h"

using namespace std;


const std::vector<LifeStage::LifeStageValue> LifeStage::createEnumValues()
{
	std::vector<LifeStageValue> values;

	for(const auto &value : magic_enum::enum_values<LifeStageValue>())
	{
		values.push_back(value);
	}

	return values;
}

const vector<LifeStage::LifeStageValue> LifeStage::enumValues = LifeStage::createEnumValues();

LifeStage::LifeStage(const LifeStageValue value)
	 : value(value)
{

}

LifeStage::LifeStage(const LifeStage &other)
	 : value(other.value)
{

}


ostream& operator<<(ostream& os, const LifeStage lifeStage)
{
	os << magic_enum::enum_name(lifeStage.getValue());
	
	return os;
}
