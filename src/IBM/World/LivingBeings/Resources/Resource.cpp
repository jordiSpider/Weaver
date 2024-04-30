#include "IBM/World/LivingBeings/Resources/Resource.h"
#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"
#include "IBM/World/Map/MapInterface.h"
#include "IBM/World/WorldInterface.h"

using namespace std;



id_type Resource::resourceCounter = 0;



Resource::Resource(ResourceSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface, double biomass, double resourceMaximumCapacity, 
	double massRatio, bool patchSpread) 
	: ResourceInterface(mySpecies, terrainCellInterface, Instar(mySpecies->getNumberOfInstars()), biomass),
	  resourceMaximumCapacity(resourceMaximumCapacity), 
	  massRatio(massRatio),
	  patchSpread(patchSpread)
{
	newBiomass = 0;
	newlyAdded = false;

	resourceCounter++;

	if(getTerrainCellInterface()->getMapInterface().getWorldInterface()->getCompetitionAmongResourceSpecies())
	{
		double totalResourceBiomass = 0.0;

		for(auto &resource : getMutableTerrainCellInterface()->getMutableResources())
		{
			if(resource != nullptr && resource != this)
			{
				totalResourceBiomass += resource->getBiomass();
			}
		}

		totalResourceBiomass += biomass;

		if(fabs(getTerrainCellInterface()->getTotalMaximumResourceCapacity()-totalResourceBiomass) <= numeric_limits<double>::epsilon())
		{
			ResourceInterface::setFullCapacity(true);
		}
	}
	else
	{
		if(fabs(resourceMaximumCapacity-biomass) <= numeric_limits<double>::epsilon())
		{
			ResourceInterface::setFullCapacity(true);
		}
	}
}

Resource::Resource(const Resource &other, const double &biomass)
	: Resource(other.getSpecies(), other.getMutableTerrainCellInterface(), biomass, other.getResourceMaximumCapacity(), 
		other.getTerrainCellInterface()->getMapInterface().getWorldInterface()->getMassRatio(), other.canSpread()
	  )
{

}

Resource::~Resource()
{

}


bool Resource::canSpread() const {
	return patchSpread;
}


void Resource::setResourceMaximumCapacity(const double& resourceMaximumCapacity) 
{ 
	this->resourceMaximumCapacity = resourceMaximumCapacity; 
}


double Resource::getResourceMaximumCapacity() const 
{ 
	return resourceMaximumCapacity; 
}


bool Resource::isExtinct() 
{ 
	return biomass <= 0.0; 
}

// Set this resource population to 0

void Resource::erradicate()
{
	biomass = 0;
}


//arthro and for dinos, to debug negative resource
/* bool Resource::isDepleted(double foodDemand) { 
	if(this->position->getX()==1 && this->position->getY()==1){
	cout << "here:  " << endl;
	cout << biomass << endl;
	cout << foodDemand << endl;
	cout << ((biomass / getSpecies()->getConversionToWetMass()) - foodDemand) << endl;
	cout << getSpecies()->getMinimumEdibleBiomass() << endl; 
	}
return ((biomass / getSpecies()->getConversionToWetMass()) - foodDemand) < getSpecies()->getMinimumEdibleBiomass(); 
}; */


double Resource::getNewBiomass(const double &rateOfIncrease) const
{
	const auto worldInterface = this->getTerrainCellInterface()->getMapInterface().getWorldInterface();

	double newBiomass = worldInterface->calculateNewBiomass(biomass, rateOfIncrease, getSpecies());

	//We add this biomass below, after checking the excess
	/*double exBiomass = newBiomass;*/
	newBiomass = newBiomass / worldInterface->getTimeStepsPerDay();

	return newBiomass;
}

// Grows the resource within this cell and returns the new population

void Resource::update()
{
	double excess = 0.0;

	double timeStepsPerDay = 1;  //Dinosaur debuging
	//cout << "venga dame el dato ¿no?" << timeStep << endl;
	/* if(timeStep >= 0){ //Dinosaur debuging
		//exit(-1);
		position->setMaximumCapacities((position->getTotalMaximumResourceCapacity())*2); //= position->getMaximumResourceCapacity()*2;
	} */
	//cout << "biomass: " << biomass << endl;

	double previousTotalBiomass;
	newBiomass = 0;
	double rateOfIncrease = 0;
	double maximumResourcecapacity = getTerrainCellInterface()->getTotalMaximumResourceCapacity();


	/* if (biomass > maximumResourcecapacity){
	biomass = maximumResourcecapacity;  
	}*/

	/* cout << "sa biomassa    " << endl;
	cout << biomass << endl; */

	/* if(biomass < 0)
		{
			cout << timeStep << endl;
			cout << biomass << endl;
			cout << this->position->getX() << endl;
			cout << this->position->getY() << endl;
			exit(-1);
			
		}   */
	
	if(getBiomass() <= 0 || getBiomass() < getSpecies()->getMinimumEdibleBiomass())
	{
		setBiomass(getSpecies()->getMinimumEdibleBiomass());
	}   

	if(getBiomass() < getSpecies()->getCellMass()) //less than one cell is not possible
	{
		setBiomass(0.0);
	}  
	
	if(!getTerrainCellInterface()->getMapInterface().getWorldInterface()->getCompetitionAmongResourceSpecies())
	{
	
		//the commented code below serves to set a target species of resource to have
		//many times lower carrying capacity than others - see also Resource.h
		//if(mySpecies->getScientificName() != "Resource_sp3"){
		
		maximumResourcecapacity = getResourceMaximumCapacity();
	}

	//cout << "TerrainCell: (" << position->getX() << "," << position->getY() << "," << position->getZ() << ")" << " water condition " << getSpecies()->getScientificName() <<  ": " <<
	// getSpecies()->getMinHR() << " <= " << position->getWater() << " <= " << getSpecies()->getMaxHR() << endl;
	
//cout << position->getWater() << endl;
//cout << getSpecies()->getMinHR() << endl;
//cout << getSpecies()->getMaxHR() << endl;

	if ((getBiomass() > 0) && ( getTerrainCellInterface()->getMoistureInfo()->getMoisture() >= getSpecies()->getMinHR()) && (getTerrainCellInterface()->getMoistureInfo()->getMoisture() <= getSpecies()->getMaxHR()))
	{
		previousTotalBiomass = getTerrainCellInterface()->getTotalResourceBiomass();

		if(getSpecies()->getVariableIntrinsicRateOfIncrease())
		{
			rateOfIncrease = getSpecies()->getRateOfGrowth(getTerrainCellInterface()->getMoistureInfo()->getTemperature(), getTerrainCellInterface()->getMoistureInfo()->getMoisture(), massRatio);
		}
		else
		{
			rateOfIncrease = getSpecies()->getMaxRScale();
		}

		//cout << "rateOfIncrease: " << rateOfIncrease << endl;

		if(rateOfIncrease<0)
		{ 
			rateOfIncrease = 0; 
		}

		if(getTerrainCellInterface()->getMapInterface().getWorldInterface()->getCompetitionAmongResourceSpecies() && rateOfIncrease > 0)
		{
			newBiomass = getNewBiomass(rateOfIncrease);

			if( newBiomass < 0 )
			{
				cout << "BIOMASA NEGATIVA!!" << endl;
				cout << rateOfIncrease << endl;
				cout << getBiomass() << endl;
				cout << getTerrainCellInterface()->getTotalMaximumResourceCapacity() << endl;
			}

			if( (previousTotalBiomass + newBiomass - getBiomass()) > getTerrainCellInterface()->getTotalMaximumResourceCapacity() )
			{
				excess = (previousTotalBiomass + newBiomass - getBiomass()) - getTerrainCellInterface()->getTotalMaximumResourceCapacity();
				setBiomass(newBiomass-excess);
				double biomass2 = getTerrainCellInterface()->getTotalMaximumResourceCapacity() - previousTotalBiomass;

				//TODO Jordi: check if biomass and biomass2 are identical
			}
			else
			{
				setBiomass(newBiomass);
			}
		}
		else
		{
			if(rateOfIncrease > 0)
			{
				newBiomass = getNewBiomass(rateOfIncrease);

				//cout << "newBiomass: " << newBiomass << endl;

				if(newBiomass < 0)
				{
					cout << "BIOMASA NEGATIVA!!" << endl;
					cout << rateOfIncrease << endl;
					cout << getBiomass() << endl;
					cout << maximumResourcecapacity << endl;
				}

				//cout << maximumResourcecapacity << endl;  

				if(newBiomass >= (maximumResourcecapacity-numeric_limits<double>::epsilon()))
				{
					excess = newBiomass - maximumResourcecapacity;
					setBiomass(maximumResourcecapacity);
				}
				else
				{
					setBiomass(newBiomass);
				}

			}

		}

	//for debugging excess algorithm
	/* if(position->getX()==2 && position->getY()==2){
		cout << "aquiiii...        si                 xx" << endl;
		cout << biomass << endl;
		biomass=0.001; //biomass-0.5*biomass;
	}  */

	


//this goes with configuration: "food_bottle_neck.zip"
	/* if(biomass >= 7.87E+06){
		biomass=biomass/1000000;
	} */

	}
	
	if(getBiomass() < getSpecies()->getCellMass()) //less than one cell is not possible
	{
		setBiomass(0.0);
	}  
	/* else{//arthros but for dinos.... because marginal cells are really bad!! 

		//cout << "biomass before   " << biomass << endl;
		biomass = maximumResourcecapacity;
		//cout << "biomass after   " << biomass << endl;
	
	} */

/* 	if(biomass <= 0 || biomass < getSpecies()->getMinimumEdibleBiomass())
	{
		biomass = getSpecies()->getMinimumEdibleBiomass();
	} */

	/*if(biomass < 0)
	{
		biomass = 0.0;
	}*/


	/*  cout << "after excess 1:   " << endl;
	cout << biomass <<  endl;
	cout << newBiomass << endl;
	cout << maximumResourcecapacity << endl;
	cout << excess <<  endl;  */

/* if(biomass>2.5){
	cout << "after excess 1:   " << endl;
	cout << biomass <<  endl;
	cout << newBiomass << endl;
	cout << maximumResourcecapacity << endl;
	cout << excess <<  endl; 
	}    */ 

	/* if(excess==0){
	cout << "after excess 1:   " << endl;
	cout << biomass <<  endl;
	cout << newBiomass << endl;
	cout << maximumResourcecapacity << endl;
	cout << excess <<  endl; 
	cout << position->getWater() << endl;
	}  */
	/* if (biomass > maximumResourcecapacity){
	biomass = maximumResourcecapacity;  
	} */

	//if( timeStep == 0 && position->getX()==)

	if(canSpread() && excess > 0.0) //spread only if excess > cell size - otherwise is not biologically possible
	{                                 
		double spreadExcess = getMutableTerrainCellInterface()->spreadResource(excess, getSpecies()->getResourceSpeciesId());

		if(fabs(excess-spreadExcess) <= numeric_limits<double>::epsilon())
		{
			ResourceInterface::setFullCapacity(true);
		}
	}
}


double Resource::addBiomass(double addedMass) 
{ 
	double availableCapacity;

	if(getTerrainCellInterface()->getMapInterface().getWorldInterface()->getCompetitionAmongResourceSpecies())
	{
		availableCapacity = getTerrainCellInterface()->getTotalMaximumResourceCapacity() - getTerrainCellInterface()->getTotalResourceBiomass();
	}
	else
	{
		availableCapacity = getResourceMaximumCapacity() - this->biomass;
	}

	setBiomass(getBiomass() + availableCapacity);

	double excess = 0.0;

	if(addedMass - availableCapacity > 0.0)
	{
		excess = addedMass - availableCapacity;
	}

	return excess;
};

double Resource::addBiomass(double addedMass, const TerrainCellInterface* const source) 
{ 
	return addBiomass(addedMass);
};

double Resource::addBiomass(double addedMass, const unique_ptr<vector<unsigned int>> &childrenPositions)
{
    return addBiomass(addedMass);
}

void Resource::setFullCapacity(const bool newFullCapacityValue, const TerrainCellInterface* const source)
{
	ResourceInterface::setFullCapacity(newFullCapacityValue);
}

void Resource::setFullCapacity(const bool newFullCapacityValue, const unique_ptr<vector<unsigned int>> &childrenPositions)
{
	ResourceInterface::setFullCapacity(newFullCapacityValue);
}
