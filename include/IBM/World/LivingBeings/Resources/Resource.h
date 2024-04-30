#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <list>
#include <vector>
#include <cfloat>
#include <iostream>
#include <cmath>

#include "IBM/World/LivingBeings/Edible.h"
#include "Misc/SimulationConstants.h"
#include "IBM/Maths/Constants.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "Misc/Types.h"
#include "IBM/Maths/MathFunctions.h"
#include "IBM/World/LivingBeings/Resources/ResourceInterface.h"
#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"
#include "IBM/Maths/OptimisedOperators.h"



class Resource : public ResourceInterface
{
public:
	Resource(TerrainCellInterface* terrainCellInterface);
	Resource(ResourceSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface, double biomass, double resourceMaximumCapacity, double massRatio, bool patchSpread);
	Resource(const Resource &other, const double &biomass);
	virtual ~Resource();

	void setResourceMaximumCapacity(const double& resourceMaximumCapacity);
	double getResourceMaximumCapacity() const;

	virtual bool isExtinct();


	// Grows the resource within this cell and returns the new population
	void update();
	
	//arthro and for dinos
	double addBiomass(double addedMass);
	double addBiomass(double addedMass, const TerrainCellInterface* const source);
	double addBiomass(double addedMass, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions);

	void setFullCapacity(const bool newFullCapacityValue, const TerrainCellInterface* const source);
	void setFullCapacity(const bool newFullCapacityValue, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions);

	// Set this resource population to 0
	void erradicate();

	bool canSpread() const;

	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
	// This is to avoid reprocessing the same resource on the same day
	bool newlyAdded;
	double newBiomass;

	double resourceMaximumCapacity;

	double massRatio;

	bool patchSpread;

	double getNewBiomass(const double &rateOfIncrease) const;
};

#endif /* RESOURCE_H_ */
