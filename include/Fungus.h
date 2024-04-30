/*
 * terrainCellFungus.hpp
 *
 *  Created on: May 21, 2012
 *      Author: jrbcast
 */

#ifndef TERRAINCELLFUNGUS_HPP_
#define TERRAINCELLFUNGUS_HPP_

#include <list>
#include <vector>
#include <math.h>
#include "FungusSpecies.h"
#include "SimulationConstants.h"
#include "Maths/Constants.h"

#include <float.h>
#include <iostream>
#include <cmath>

// Forward declaration
class TerrainCell;

using namespace std;

class Fungus: public Edible
{
protected:

	TerrainCell* position;
	double biomass;
	// This is to avoid reprocessing the same fungus on the same day
	bool newlyAdded;
	double newBiomass;


public:
	Fungus(Species* mySpecies, float biomass, TerrainCell* position);
	virtual ~Fungus();

	virtual double calculateDryMass() {	return biomass / mySpecies->getConversionToWetMass(); };   
																									
	virtual double turnIntoDryMassToBeEaten(double predatorVoracity, float profitability, double leftovers) {	return min(predatorVoracity / profitability, leftovers); };
	virtual double calculateWetMass() {	
		if(biomass < mySpecies->getCellMass()){
			biomass = 0;  //this is because no less than one biological cell can possibly grow
		}
		return biomass;	
	};

	virtual void substractBiomass(double dryMassToBeSubstracted) { biomass -= mySpecies->getConversionToWetMass() * dryMassToBeSubstracted;	};
	virtual bool isExtinct() { return biomass <= 0.0; };
	
	virtual inline TerrainCell* getPosition(){ return position; };
	void setPosition(TerrainCell* position) { this->position = position; };
	
	virtual bool isDepleted(double foodDemand) { 
	//if(this->position->getX() == 1 && this->position->getY() == 1){
	//if((foodDemand > 25) && (biomass/mySpecies->getConversionToWetMass() - foodDemand < 1)){
	/* cout << "here:  " << endl;
	cout << biomass/mySpecies->getConversionToWetMass() << endl;
	cout << foodDemand << endl;
	cout << ((biomass / mySpecies->getConversionToWetMass()) - foodDemand) << endl;
	cout << mySpecies->getMinimumEdibleBiomass()/mySpecies->getConversionToWetMass() << endl;  */
	//exit(-1);
	//}
	//  
	bool out = false;
	if((round((biomass / mySpecies->getConversionToWetMass())*pow(10,7)) - round(foodDemand*pow(10,7))) < round((mySpecies->getMinimumEdibleBiomass()/mySpecies->getConversionToWetMass())*pow(10,7))){
		out = true;
		//biomass = mySpecies->getMinimumEdibleBiomass(); this makes fungus to stop growing¿¿??
	}

	return out;  
	};

	virtual double anyLeft() {
		double leftovers = 0;
			if(biomass / mySpecies->getConversionToWetMass() - mySpecies->getMinimumEdibleBiomass()/mySpecies->getConversionToWetMass() > 0)
			{
				leftovers = biomass / mySpecies->getConversionToWetMass() - mySpecies->getMinimumEdibleBiomass()/mySpecies->getConversionToWetMass();
			}
	return leftovers;	
	}


	// In order to effectively increase biomass from fungus coming from other cells, this must be called
	virtual void processExternalContributions();
	// Grows the fungus within this cell and returns the new population
	virtual double grow(int timeStep); //Dinosaurs debug   , int timeStepsPerDay
	virtual void setBiomass(double biomass) { this->biomass = biomass; };
	
	//arthro and for dinos
	virtual void addBiomass(double addedMass, bool competition, double maxCapacity, int size, double totBiomass) 
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

		
		//the commented code below serves to set a target species of fungus to have
		//many times lower carrying capacity than others - see also Fungus.cpp
		double thisSpeciesMaximumFungicapacity=0; //maxCapacity/size	
		//if(this->mySpecies->getScientificName() != "Fungus_sp3"){
		thisSpeciesMaximumFungicapacity=maxCapacity/(size); //-1
		//}else{
		//thisSpeciesMaximumFungicapacity=maxCapacity/20.0;
		//}
			
			

			/* cout << "BEFORE " << endl;
			cout << this->biomass << endl;
			cout << addedMass << endl; */

			if((addedMass + this->biomass) < thisSpeciesMaximumFungicapacity)
			{

				this->biomass += addedMass;

			/* cout << "AFTER " << endl;
			cout << this->biomass << endl; */
			
			}else{

			this->biomass = thisSpeciesMaximumFungicapacity;	

			/* cout << "AFTER " << endl;
			cout << this->biomass << endl; */

			}
		
		  

		}

        
		



	};

	// Set this fungus population to 0
	void erradicate();

	

	//FungusSpecies* mySpecies;

};

#endif /* TERRAINCELLFUNGUS_HPP_ */
