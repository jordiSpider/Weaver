#include "Resource.h"

using namespace std;


id_type Resource::resourceCounter = 0;


Resource::Resource(ResourceSpecies* const mySpecies, float biomass, double resourceMaximumCapacity, 
	bool competitionAmongResourceSpecies, double massRatio, bool patchSpread) 
	: Edible(mySpecies), competitionAmongResourceSpecies(competitionAmongResourceSpecies), massRatio(massRatio),
	  patchSpread(patchSpread)
{
	this->biomass = biomass;
	newBiomass = 0;
	newlyAdded = false;
	setResourceMaximumCapacity(resourceMaximumCapacity);

	instar = Instar(mySpecies->getNumberOfInstars());

	Resource::resourceCounter++;
}

Resource::~Resource()
{

}

ResourceSpecies* const Resource::getSpecies() const { return static_cast<ResourceSpecies* const>(mySpecies); }

const double Resource::getVoracity() const 
{ 
	return 0.0; 
}

const double Resource::getCurrentBodySize() const 
{ 
	return 0.0; 
}

const double Resource::getSpeed() const 
{ 
	return 0.0; 
}

const bool Resource::isHunting() const 
{ 
	return false; 
}

void Resource::incrementEncountersWithPredator(const int &predatorId) 
{

}

bool Resource::canEatEdible(Edible* edible, const list<Edible*> &ediblesHasTriedToPredate) 
{ 
	return false; 
}

int Resource::getPredatedByID() const 
{ 
	return -1; 
}

void Resource::setNewLifeStage(const LifeStage newLifeStage) 
{

}

void Resource::setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath) 
{

}

void Resource::setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath, int predatorId) 
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

const double Resource::calculateDryMass() const 
{ 
	return biomass / getSpecies()->getConversionToWetMass(); 
}

const double Resource::getInterpolatedDryMass() const
{
	return Math_Functions::linearInterpolate01(getSpecies()->convertToDryMass(biomass), getSpecies()->getK_Value());
}
																								
const double Resource::turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const 
{ 
	return min(predatorVoracity / profitability, leftovers); 
}

bool Resource::predateEdible(Edible* edibleToBePredated, int day, bool retaliation, std::list<Edible*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea) 
{ 
	return false; 
}

void Resource::substractBiomass(double dryMassToBeSubstracted) 
{ 
	biomass -= getSpecies()->getConversionToWetMass() * dryMassToBeSubstracted;	
}

bool Resource::isExtinct() 
{ 
	return biomass <= 0.0; 
}

void Resource::setBiomass(double biomass) 
{ 
	this->biomass = biomass; 
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

double Resource::getNewBiomass(const double &rateOfIncrease, const double &timeStepsPerDay, SimulType simulType) const
{
	double newBiomass;

	switch (simulType) {
		case SimulType::dinosaurs: {
			double cellNumber = biomass/mySpecies->getCellMass();
			double newCellNumber = rateOfIncrease * cellNumber;
			newBiomass = newCellNumber*mySpecies->getCellMass();
			break;
		}
		case SimulType::arthropods: {
			newBiomass = rateOfIncrease*biomass;
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
	//We add this biomass below, after checking the excess
	/*double exBiomass = newBiomass;*/
	newBiomass = newBiomass / timeStepsPerDay;

	return newBiomass;
}

// Grows the resource within this cell and returns the new population
double Resource::grow(int timeStep, double totalMaximumResourceCapacity, double moisture, double temperature, double totalBiomass, SimulType simulType)  //Dinosaurs debug  , original variable is int timeStepsPerDay and comes from terrainCell->grow(timeStep) which comes from World's currentTerrainCell->growResource(timeStep) 
{
	double excess = 0;

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
	double maximumResourcecapacity = totalMaximumResourceCapacity;


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
	
	if(biomass <= 0 || biomass < getSpecies()->getMinimumEdibleBiomass())
		{
			
			biomass = getSpecies()->getMinimumEdibleBiomass();
			
		}   

	if(biomass < getSpecies()->getCellMass()) //less than one cell is not possible
		{
			
			biomass = 0;
			
		}  
	
	if(!competitionAmongResourceSpecies){
	
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

	if ((biomass > 0) && ( moisture >= getSpecies()->getMinHR()) && (moisture <= getSpecies()->getMaxHR()))
	{
		previousTotalBiomass = totalBiomass;

		if(getSpecies()->getVariableIntrinsicRateOfIncrease()) {
			rateOfIncrease = getSpecies()->getRateOfGrowth(temperature, moisture, massRatio);
		}
		else {
			rateOfIncrease = getSpecies()->getMaxRScale();
		}

		//cout << "rateOfIncrease: " << rateOfIncrease << endl;

		if(rateOfIncrease<0){ rateOfIncrease = 0; }

		if(competitionAmongResourceSpecies && rateOfIncrease > 0){
			newBiomass = getNewBiomass(rateOfIncrease, timeStepsPerDay, simulType);

			if( newBiomass < 0 )
			{
				Output::cout("BIOMASA NEGATIVA!!\n");
				Output::cout("{}\n", rateOfIncrease);
				Output::cout("{}\n", biomass);
				Output::cout("{}\n", totalMaximumResourceCapacity);
			}

			if( (previousTotalBiomass + newBiomass - biomass) > totalMaximumResourceCapacity )
			{
				excess = (previousTotalBiomass + newBiomass - biomass) - totalMaximumResourceCapacity;
				biomass = newBiomass-excess;
				double biomass2 = totalMaximumResourceCapacity-previousTotalBiomass;
				//TODO Jordi: check if biomass and biomass2 are identical
			}
			else
			{
				biomass = newBiomass;
			}
		}
		else {
			if(rateOfIncrease > 0){
				newBiomass = getNewBiomass(rateOfIncrease, timeStepsPerDay, simulType);

				//cout << "newBiomass: " << newBiomass << endl;

				if( newBiomass < 0 )
				{
					Output::cout("BIOMASA NEGATIVA!!\n");
					Output::cout("{}\n", rateOfIncrease);
					Output::cout("{}\n", biomass);
					Output::cout("{}\n", maximumResourcecapacity);
				}

				//cout << maximumResourcecapacity << endl;  

				if(newBiomass > maximumResourcecapacity)
				{
					excess = newBiomass - maximumResourcecapacity;
					biomass = maximumResourcecapacity;
				}
				else
				{
					biomass = newBiomass;
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
	
	if(biomass < getSpecies()->getCellMass()) //less than one cell is not possible
	{
			
	biomass = 0;
			
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

	return excess;
}

const double Resource::calculateWetMass() {
	if(biomass < getSpecies()->getCellMass()){
		biomass = 0;  //this is because no less than one biological cell can possibly grow
	}
	return biomass;	
};

//this below is only useful when resource get extinct from cells and to process extinctions
void Resource::processExternalContributions(double totalMaximumResourceCapacity, double totalBiomass)
{

	double maximumResourcecapacity = totalMaximumResourceCapacity;

	if(!competitionAmongResourceSpecies){
	
		maximumResourcecapacity = getResourceMaximumCapacity();
	}

	if (newlyAdded || newBiomass > 0)
	{
		newlyAdded = false;
		biomass = newBiomass;

		if(!competitionAmongResourceSpecies){
			if(biomass > maximumResourcecapacity)
			{
				double excess = biomass - maximumResourcecapacity;
				biomass -= excess;
			}
		}else{
			if(totalBiomass > totalMaximumResourceCapacity)
			{
				double excess = totalBiomass - totalMaximumResourceCapacity;
				biomass -= excess;
			}

		}
		
		//	cout << "TerrainCell: (" << position->getX() << "," << position->getY() << "," << position->getZ() << ")" << " biomass + newBiomass " << getSpecies()->getScientificName() <<  ": " <<
	//biomass << " + " << newBiomass << endl;

	
		
		newBiomass = 0;
	}  
}

bool Resource::isDepleted(double foodDemand) 
{ 
	//if(this->position->getX() == 1 && this->position->getY() == 1){
	//if((foodDemand > 25) && (biomass/getSpecies()->getConversionToWetMass() - foodDemand < 1)){
	/* cout << "here:  " << endl;
	cout << biomass/getSpecies()->getConversionToWetMass() << endl;
	cout << foodDemand << endl;
	cout << ((biomass / getSpecies()->getConversionToWetMass()) - foodDemand) << endl;
	cout << getSpecies()->getMinimumEdibleBiomass()/getSpecies()->getConversionToWetMass() << endl;  */
	//exit(-1);
	//}
	//  
	bool out = false;
	if((round((biomass / getSpecies()->getConversionToWetMass())*pow(10,7)) - round(foodDemand*pow(10,7))) < round((getSpecies()->getMinimumEdibleBiomass()/getSpecies()->getConversionToWetMass())*pow(10,7))){
		out = true;
		//biomass = getSpecies()->getMinimumEdibleBiomass(); this makes resource to stop growing¿¿??
	}

	return out;  
}

double Resource::anyLeft() 
{
	double leftovers = 0;
	
	if(biomass / getSpecies()->getConversionToWetMass() - getSpecies()->getMinimumEdibleBiomass()/getSpecies()->getConversionToWetMass() > 0)
	{
		leftovers = biomass / getSpecies()->getConversionToWetMass() - getSpecies()->getMinimumEdibleBiomass()/getSpecies()->getConversionToWetMass();
	}

	return leftovers;	
}

void Resource::addBiomass(double addedMass, bool competition, double maxCapacity, int size, double totBiomass) 
{ 
	
	// does not work if(this->biomass > 0){ ///only if it did have previous biomass - so empy cells do not get artificially filled from error in getCellsAtDistance

	if(competition){
	
		if( (totBiomass + addedMass) < maxCapacity )
		{
			this->biomass += addedMass; 
		}else{
			this->biomass = (maxCapacity - totBiomass) + this->biomass;
		}
	
	}else{

	
	//the commented code below serves to set a target species of resource to have
	//many times lower carrying capacity than others - see also Resource.cpp
	//maxCapacity/size	
	//if(this->mySpecies->getScientificName() != "Resource_sp3"){
	
	//}else{
	//maximumResourcecapacity=maxCapacity/20.0;
	//}
		/* cout << "BEFORE " << endl;
		cout << this->biomass << endl;
		cout << addedMass << endl; */

		if((addedMass + this->biomass) < getResourceMaximumCapacity())
		{

			this->biomass += addedMass;

		/* cout << "AFTER " << endl;
		cout << this->biomass << endl; */
		
		}else{

		this->biomass = getResourceMaximumCapacity();	

		/* cout << "AFTER " << endl;
		cout << this->biomass << endl; */

		}
	}
};
