/*
 * TerrainCellFungus.cpp
 *
 *  Created on: May 21, 2012
 *      Author: jrbcast
 */

#include "Fungus.h"
#include "TerrainCell.h"

Fungus::Fungus(Species* mySpecies, float biomass, TerrainCell* position) : Edible(mySpecies)
{
	this->position = position;
	this->biomass = biomass;
	newBiomass = 0;
	newlyAdded = false;
}

Fungus::~Fungus()
{

}

// Set this fungus population to 0
void Fungus::erradicate()
{
	biomass = 0;
}


//arthro and for dinos, to debug negative fungi
/* bool Fungus::isDepleted(double foodDemand) { 
	if(this->position->getX()==1 && this->position->getY()==1){
	cout << "here:  " << endl;
	cout << biomass << endl;
	cout << foodDemand << endl;
	cout << ((biomass / mySpecies->getConversionToWetMass()) - foodDemand) << endl;
	cout << mySpecies->getMinimumEdibleBiomass() << endl; 
	}
return ((biomass / mySpecies->getConversionToWetMass()) - foodDemand) < mySpecies->getMinimumEdibleBiomass(); 
}; */

// Grows the fungus within this cell and returns the new population
double Fungus::grow(int timeStep)  //Dinosaurs debug  , original variable is int timeStepsPerDay and comes from terrainCell->grow(timeStep) which comes from World's currentTerrainCell->growFungi(timeStep) 
{
	double excess = 0;

	double timeStepsPerDay = 1;  //Dinosaur debuging
	//cout << "venga dame el dato ¿no?" << timeStep << endl;
	/* if(timeStep >= 0){ //Dinosaur debuging
		//exit(-1);
		position->setMaximumCapacities((position->maximumFungiCapacity)*2); //= position->getMaximumFungiCapacity()*2;
	} */
	//cout << "biomass: " << biomass << endl;

	double previousTotalBiomass;
	newBiomass = 0;
	double rateOfIncrease = 0;
	double thisSpeciesMaximumFungicapacity=position->maximumFungiCapacity;


	/* if (biomass > thisSpeciesMaximumFungicapacity){
	biomass = thisSpeciesMaximumFungicapacity;  
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
	
	 if(biomass <= 0 || biomass < mySpecies->getMinimumEdibleBiomass())
		{
			
			biomass = mySpecies->getMinimumEdibleBiomass();
			
		}   

	 if(biomass < mySpecies->getCellMass()) //less than one cell is not possible
		{
			
			biomass = 0;
			
		}  
    
	if(!position->getTheWorld()->getCompetitionAmongFungiSpecies()){
	
		//the commented code below serves to set a target species of fungus to have
		//many times lower carrying capacity than others - see also Fungus.h
		//if(mySpecies->getScientificName() != "Fungus_sp3"){
		thisSpeciesMaximumFungicapacity=position->maximumFungiCapacity/(position->getTheWorld()->existingFungiSpecies.size()); //-1
		//}else{
		//thisSpeciesMaximumFungicapacity=position->maximumFungiCapacity/20.0;
		//}
	}

	//cout << "TerrainCell: (" << position->getX() << "," << position->getY() << "," << position->getZ() << ")" << " water condition " << mySpecies->getScientificName() <<  ": " <<
     // mySpecies->getMinHR() << " <= " << position->getWater() << " <= " << mySpecies->getMaxHR() << endl;
	
//cout << position->getWater() << endl;
//cout << mySpecies->getMinHR() << endl;
//cout << mySpecies->getMaxHR() << endl;

	if ((biomass > 0) && ( position->getWater() >= mySpecies->getMinHR()) && (position->getWater() <= mySpecies->getMaxHR()))
	{
		previousTotalBiomass = position->getTotalFungusBiomass();

		rateOfIncrease = mySpecies->getRateOfGrowth(position->getTemperature(), position->getWater());

		//cout << "rateOfIncrease: " << rateOfIncrease << endl;

	  if(rateOfIncrease<0){ rateOfIncrease = 0; }

	  if(position->getTheWorld()->getCompetitionAmongFungiSpecies() && rateOfIncrease > 0){

		/* if(biomass > position->maximumFungiCapacity){
		biomass=position->maximumFungiCapacity;
		} */
		


		newBiomass = rateOfIncrease * biomass;
		//We add this biomass below, after checking the excess
		/*double exBiomass = newBiomass;*/
		newBiomass = newBiomass / timeStepsPerDay;

		

		if( newBiomass < 0 )
		{
			std::cout << "BIOMASA NEGATIVA!!" << std::endl;
			std::cout << rateOfIncrease << std::endl;
			std::cout << biomass << std::endl;
			std::cout << position->maximumFungiCapacity << std::endl;
		}

		if( (previousTotalBiomass + newBiomass - biomass) > position->maximumFungiCapacity )
		{
			excess = (previousTotalBiomass + newBiomass - biomass) - position->maximumFungiCapacity;
			biomass = newBiomass-excess;
			double biomass2 = position->maximumFungiCapacity-previousTotalBiomass;
			//TODO Jordi: check if biomass and biomass2 are identical
		}
		else
		{
			biomass = newBiomass;
		}


      }else{

  		if(rateOfIncrease > 0){
			/* if(biomass>thisSpeciesMaximumFungicapacity){
				biomass=thisSpeciesMaximumFungicapacity;
			}  */

			/* cout << "before exp:   " << endl;
			cout << biomass <<  endl;    */
			newBiomass = rateOfIncrease * biomass;
			/* cout << "after exp:   " << endl;
			cout << newBiomass <<  endl;   */ 
			//We add this biomass below, after checking the excess
			/*double exBiomass = newBiomass;*/
			newBiomass = newBiomass / timeStepsPerDay;

			//cout << "newBiomass: " << newBiomass << endl;

			if( newBiomass < 0 )
			{
				cout << "BIOMASA NEGATIVA!!" << endl;
				cout << rateOfIncrease << endl;
				cout << biomass << endl;
				cout << thisSpeciesMaximumFungicapacity << endl;
			}

			//cout << thisSpeciesMaximumFungicapacity << endl;  

			if(newBiomass > thisSpeciesMaximumFungicapacity)
			{
				excess = newBiomass - thisSpeciesMaximumFungicapacity;
				biomass = thisSpeciesMaximumFungicapacity;
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
	
	if(biomass < mySpecies->getCellMass()) //less than one cell is not possible
	{
			
	  biomass = 0;
			
	}  
	/* else{//arthros but for dinos.... because marginal cells are really bad!! 

		//cout << "biomass before   " << biomass << endl;
		biomass = thisSpeciesMaximumFungicapacity;
		//cout << "biomass after   " << biomass << endl;
	
	} */

/* 	if(biomass <= 0 || biomass < mySpecies->getMinimumEdibleBiomass())
	{
		biomass = mySpecies->getMinimumEdibleBiomass();
	} */

	/*if(biomass < 0)
	{
		biomass = 0.0;
	}*/


	 /*  cout << "after excess 1:   " << endl;
	  cout << biomass <<  endl;
	  cout << newBiomass << endl;
	  cout << thisSpeciesMaximumFungicapacity << endl;
	  cout << excess <<  endl;  */

   /* if(biomass>2.5){
	  cout << "after excess 1:   " << endl;
	  cout << biomass <<  endl;
	  cout << newBiomass << endl;
	  cout << thisSpeciesMaximumFungicapacity << endl;
	  cout << excess <<  endl; 
	  }    */ 

	  /* if(excess==0){
	  cout << "after excess 1:   " << endl;
	  cout << biomass <<  endl;
	  cout << newBiomass << endl;
	  cout << thisSpeciesMaximumFungicapacity << endl;
	  cout << excess <<  endl; 
	  cout << position->getWater() << endl;
	  }  */
	/* if (biomass > thisSpeciesMaximumFungicapacity){
	biomass = thisSpeciesMaximumFungicapacity;  
	} */

	//if( timeStep == 0 && position->getX()==)

	return excess;
}


//this below is only useful when fungi get extinct from cells and to process extinctions
void Fungus::processExternalContributions()
{

 	double thisSpeciesMaximumFungicapacity = position->maximumFungiCapacity;

	if(!position->getTheWorld()->getCompetitionAmongFungiSpecies()){
		thisSpeciesMaximumFungicapacity=position->maximumFungiCapacity/(position->getTheWorld()->existingFungiSpecies.size());
	}

	if (newlyAdded || newBiomass > 0)
	{
		newlyAdded = false;
		biomass = newBiomass;

		if(!position->getTheWorld()->getCompetitionAmongFungiSpecies()){
			if(biomass > thisSpeciesMaximumFungicapacity)
			{
				double excess = biomass - thisSpeciesMaximumFungicapacity;
				biomass -= excess;
			}
		}else{
			if(position->getTotalFungusBiomass() > position->maximumFungiCapacity)
			{
				double excess = position->getTotalFungusBiomass() - position->maximumFungiCapacity;
				biomass -= excess;
			}

		}
		
		//	cout << "TerrainCell: (" << position->getX() << "," << position->getY() << "," << position->getZ() << ")" << " biomass + newBiomass " << mySpecies->getScientificName() <<  ": " <<
      //biomass << " + " << newBiomass << endl;

    
		
		newBiomass = 0;
	}  
 }
