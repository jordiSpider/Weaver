/*
 * Edible.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/LifeStage.h"

using namespace std;


ostream& operator<<(ostream& os, const LifeStage lifeStage)
{
	os << magic_enum::enum_name(lifeStage);
	
	return os;
}
