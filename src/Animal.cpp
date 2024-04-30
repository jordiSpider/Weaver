	/*
 * Animal.cpp
 *
 *  Created on: 20 Nov 2012
 *      Author: gabi
 */

#include "Animal.h"
#include "TerrainCell.h"
#include "LineInfoException.h"
#include "Maths/Random.h"
#include "Maths/Math_Functions.h"


Animal::Animal(Genome* genome, double factorEggMassFromMom, TerrainCell* position, double timeStep, double timeStepsPerDay, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, Species* mySpecies, unsigned int gender,
		unsigned int huntingMode, bool temporary) : Edible(mySpecies, temporary)
{
	this->genome = genome;
	this->eggDryMassAtBirth = -1;
	this->factorEggMassFromMom = factorEggMassFromMom;
	mated = false;
	genomeFromMatedMale = NULL;
	idFromMatedMale = -1;
	this->position = position;
	this->targetNeighborToTravelTo = NULL;
	this->gender = gender;
	this->huntingMode = huntingMode;
	this->predatedByID = -1;
	this->setMassAtBirth(mySpecies->getEggDryMass());

	for(vector<Species*>::iterator it = mySpecies->getEdibleFungusSpecies()->begin(); it != mySpecies->getEdibleFungusSpecies()->end(); it++)
	{
		abundancesExperiencedPerSpecies[(*it)] = mySpecies->getEdiblePreference((*it));
		meanExperiencesPerSpecies[(*it)] = mySpecies->getEdiblePreference((*it));
	}
	for(vector<Species*>::iterator it = mySpecies->getEdibleAnimalSpecies()->begin(); it != mySpecies->getEdibleAnimalSpecies()->end(); it++)
	{
		abundancesExperiencedPerSpecies[(*it)] = mySpecies->getEdiblePreference((*it));
		meanExperiencesPerSpecies[(*it)] = mySpecies->getEdiblePreference((*it));
	}

	fixedTraits = mySpecies->getFixedTraits();

	steps = 0;
	stepsAttempted = 0;
	oldSearchArea = 0; //this was created to keep the original search area before helping out of the cell
	nextDinoMassPredicted = 0;
	foodMass = 0;
	finalJMaxVB = 0;
	sated = false;
	edibleToBePredatedProfitability = 1.0;
	foodMassLeftForNextTimeStep = 0.0;
	lastHuntedAnimalDryMass = 1.0; //To avoid divisions by 0
	lifeStage = UNBORN;
	//encounters_prey = 0;
	todayEncountersWithPredators = 0;
	daysWithoutFood = 0;
	//total_prey_encs = 0;
	//totalEncountersWithPredators = 0;
	mature = false;
	currentAge = 0; //Dinosaurs
    daysExperienced = 0;

	actualMoltingTimeVector.reserve(mySpecies->getNumberOfInstars()-1);
	actualMoltingMassVector.reserve(mySpecies->getNumberOfInstars()-1);
	finalDevTimeVector.reserve(mySpecies->getNumberOfInstars()-1);
	for (int i = 0; i < mySpecies->getNumberOfInstars()-1; ++i)
	{
		finalDevTimeVector.push_back(-1);
	}
	lengthsVector.reserve(mySpecies->getNumberOfInstars()-1); //Dinosaurs ini
	for (int i = 0; i < mySpecies->getNumberOfInstars()-1; ++i)
	{
		lengthsVector.push_back(-1);
	}
	massesVector.reserve(mySpecies->getNumberOfInstars()-1);
	for (int i = 0; i < mySpecies->getNumberOfInstars()-1; ++i)
	{
		massesVector.push_back(-1);
	}  //Dinosaurs end



	pupaTimer = 0;
	handlingTimer = 0.0;
	timeStepMaximumHandlingTimer = 1.0;
	diapauseTimeSteps = 0;
	ageOfMaturation=-1;
	ageOfFirstMaturation = 100000;//to prevent senescence at initialization
	ageOfLastMoultOrReproduction = -1;
	dateOfDeath = -1;
	pheno_ini = 0;
	date_egg = timeStep;
	lastDayMoved = -1;
	days_digest = 0;
	eatenToday = 0;
	wetMassAtTheBeginningOfTheTimeStep = eggDryMassAtBirth*mySpecies->getConversionToWetMass();
	tankAtGrowth = 0.0;
	slopeTarget = 0.0;
	interceptTarget = 0.0;
	
	this->generationNumberFromFemaleParent = g_numb_prt1;
	this->generationNumberFromMaleParent = g_numb_prt2;
	this->ID_prt1 = ID_prt1; //TODO puntero al padre mejor? o no? esto se usa?
	this->ID_prt2 = ID_prt2;
	rep_count = 0;
	fecundity = 0;
	age_first_rep = -1;

	initTraits();
	pseudoGrowth = getTrait(Trait::growth);
}

Animal::~Animal()
{
	delete genome;
	if(genomeFromMatedMale != NULL)	delete genomeFromMatedMale;
}

void Animal::initTraits()
{
	genome->createHomologousCorrelosomes();

	//TODO Here each group can have 2 or 3 traits. This MUST be implemented.
	//Correlations of EACH group can be positive or negative. This is already implemented.
	//TODO Maybe, move this method to the class Genome so it will be all well organized (see variables involved).
	int distanceFromDominant;
	int moduleNumber;

	for(Trait trait : mySpecies->getVariableTraits())
	{
		variableTraits[trait] = 0.0;

		unsigned int trait_order = mySpecies->getTraitOrder(trait);
		moduleNumber = trait_order / mySpecies->getTraitsPerModule();
		//The division is made using RHO. For every trait, the left side alleles of their own chromosomes must be added.
		for (int j = 0; j < mySpecies->getRhoRangePerModule(moduleNumber); ++j) {
			if(genome->getHomologousCorrelosomes()->at(trait_order).first->getAlleles()->at(j)->getAlphabeticOrder() >= genome->getHomologousCorrelosomes()->at(trait_order).second->getAlleles()->at(j)->getAlphabeticOrder())
			{
				variableTraits[trait] += genome->getHomologousCorrelosomes()->at(trait_order).first->getAlleles()->at(j)->getValue();
			}
			else
			{
				variableTraits[trait] += genome->getHomologousCorrelosomes()->at(trait_order).second->getAlleles()->at(j)->getValue();
			}
		}

		//The right side depends on two factors: the sign for RHO for the current module and the dominance of the chromosome.
		distanceFromDominant = trait_order % mySpecies->getTraitsPerModule();

		//If RHO is positive, for every trait the right side alleles of the dominant chromosome must be added.
		if(mySpecies->getRhoPerModule(moduleNumber) >= 0)
		{
			for (unsigned int j = mySpecies->getRhoRangePerModule(moduleNumber); j < mySpecies->getNumberOfLociPerTrait(); ++j)
			{
				if(genome->getHomologousCorrelosomes()->at(trait_order-distanceFromDominant).first->getAlleles()->at(j)->getAlphabeticOrder() >= genome->getHomologousCorrelosomes()->at(trait_order-distanceFromDominant).second->getAlleles()->at(j)->getAlphabeticOrder())
				{
					variableTraits[trait] += genome->getHomologousCorrelosomes()->at(trait_order-distanceFromDominant).first->getAlleles()->at(j)->getValue();
				}
				else
				{
					variableTraits[trait] += genome->getHomologousCorrelosomes()->at(trait_order-distanceFromDominant).second->getAlleles()->at(j)->getValue();
				}

			}
		}
		//If RHO is negative.
		else {
			//If the trait IS dominant, the right side alleles of its chromosome must be added.
			if(distanceFromDominant == 0)
			{
				for (unsigned int j = mySpecies->getRhoRangePerModule(moduleNumber); j < mySpecies->getNumberOfLociPerTrait(); ++j)
				{
					if(genome->getHomologousCorrelosomes()->at(trait_order).first->getAlleles()->at(j)->getAlphabeticOrder() >= genome->getHomologousCorrelosomes()->at(trait_order).second->getAlleles()->at(j)->getAlphabeticOrder())
					{
						variableTraits[trait] += genome->getHomologousCorrelosomes()->at(trait_order).first->getAlleles()->at(j)->getValue();
					}
					else
					{
						variableTraits[trait] += genome->getHomologousCorrelosomes()->at(trait_order).second->getAlleles()->at(j)->getValue();
					}
				}
			}
			//If the trait is NOT dominant, 1 - the right side alleles of the dominant chromosome must be added.
			else
			{
				for (unsigned int j = mySpecies->getRhoRangePerModule(moduleNumber); j < mySpecies->getNumberOfLociPerTrait(); ++j)
				{
					if(genome->getHomologousCorrelosomes()->at(trait_order-distanceFromDominant).first->getAlleles()->at(j)->getAlphabeticOrder() >= genome->getHomologousCorrelosomes()->at(trait_order-distanceFromDominant).second->getAlleles()->at(j)->getAlphabeticOrder())
					{
						variableTraits[trait] += (1.0 - genome->getHomologousCorrelosomes()->at(trait_order-distanceFromDominant).first->getAlleles()->at(j)->getValue());
					}
					else
					{
						variableTraits[trait] += (1.0 - genome->getHomologousCorrelosomes()->at(trait_order-distanceFromDominant).second->getAlleles()->at(j)->getValue());
					}
				}
			}
		}
	}

	genome->deleteHomologousCorrelosomes();

	//TODO See if we need to restrict the values to something "plausible"
	//and to avoid negative traits HERE. Maybe we don't need to do it
	//if trait<low_limit then low limit, if trait>up_limit then up_limit

	//TODO Should I save ALL the initial TRAITS for using them later?
}

void Animal::adjustTraits()
{
	//pseudoGrowthMean and pseudoGrowthSd have been calculated only using the first generation
	double normalizedPseudoGrowth = (pseudoGrowth - mySpecies->getPseudoGrowthMean()) / mySpecies->getPseudoGrowthSd();
	double valueFromNormalDistributionForCholesky = mySpecies->getValueFromNormalDistributionY();

	//double Knorm = normalizedPseudoGrowth * mySpecies->cholMat[0][0];
	Linf = normalizedPseudoGrowth * mySpecies->getValueFromCholMat(1,0) + valueFromNormalDistributionForCholesky * mySpecies->getValueFromCholMat(1,1);
	//ASÍ ERA ANTES:
	//Linf = (mySpecies->vonBertLinf/mySpecies->vonBertKini)*traits[Trait::growth]+mySpecies->LinfKcorr*randomUniform();

	//Controlling the range of Linfs for adults
	//Here we use OriginLimits because pseudoGrowth was NOT interpolated
	double minNormalizedPseudoGrowth = (mySpecies->getMinObservedPseudoValue(Trait::growth) - mySpecies->getPseudoGrowthMean()) / mySpecies->getPseudoGrowthSd();
	double maxNormalizedPseudoGrowth = (mySpecies->getMaxObservedPseudoValue(Trait::growth) - mySpecies->getPseudoGrowthMean()) / mySpecies->getPseudoGrowthSd();
	//TODO -3.5 y 3.5 porque son los valores mínimos y máximos fijados artificialmente por nosotros para la distribución normal Y
	double minLinf = minNormalizedPseudoGrowth * mySpecies->getValueFromCholMat(1,0) + (-3.5) * mySpecies->getValueFromCholMat(1,1);
	double maxLinf = maxNormalizedPseudoGrowth * mySpecies->getValueFromCholMat(1,0) + (3.5) * mySpecies->getValueFromCholMat(1,1);

	Linf = Math_Functions::linearInterpolate(Linf, minLinf, maxLinf, mySpecies->getVonBertLdistanceMin(), mySpecies->getVonBertLdistanceMax());


	pheno_ini = getTrait(Trait::pheno);

	//DONE time is now a double
	//traits[Trait::pheno] = round(traits[Trait::pheno]);
	setTrait(Trait::pheno, date_egg + getTrait(Trait::pheno));

	//We work directly with the traits vector, and store these ini values for later use.
	//OLD size_ini = traits[Trait::factorEggMass];
	voracity_ini = getTrait(Trait::voracity);
	search_area_ini = getTrait(Trait::search_area);
	speed_ini = getTrait(Trait::speed);



	
	tank_ini = getTrait(Trait::energy_tank);
	//OLD assim_ini = traits[Trait::assim];

    //Dinosaurs fixed part ini - dummy initialization, it changes in calculateGrowthCurves() or in forceMolting() and forceMolting2()
	setTrait(Trait::energy_tank, tank_ini*pow(mySpecies->getEggDryMass(),mySpecies->getBetaScaleTank()));
	
	currentBodySize = mySpecies->getEggDryMass()-getTrait(Trait::energy_tank);
	//cout << traits[Trait::energy_tank] << endl;
	//Dinosaurs fixed part end
	
	eggDryMassForClutch=mySpecies->getEggDryMass(); //this is just a dummy initialization


}

void Animal::increaseAge(int increase)
{
	currentAge += increase;
}

const double& Animal::getTrait(const Trait& trait) const
{
	try
	{
		return fixedTraits.at(trait);
	}
	catch(const std::out_of_range& e)
	{
		try
		{
			return variableTraits.at(trait);
		}
		catch(const std::out_of_range& e)
		{
			throwLineInfoException("Trait " + to_string((unsigned int)trait) + " not found");
		}
	}
}

void Animal::setTrait(const Trait& trait, const double& newValue)
{
	try
	{
		fixedTraits.at(trait) = newValue;
	}
	catch(const std::out_of_range& e)
	{
		try
		{
			variableTraits.at(trait) = newValue;
		}
		catch(const std::out_of_range& e)
		{
			throwLineInfoException("Trait " + to_string((unsigned int)trait) + " not found");
		}
	}
}

void Animal::tuneTraits(int timeStep, int timeStepsPerDay, float temperature, float relativeHumidity, ostream& tuneTraitsFile, bool printGrowthData, bool fromForceMolting1)
{
	#ifdef _DEBUG
	searchedAnimalsToday.clear();
	encounteredAnimalsToday.clear();
	huntedAnimalsToday.clear();
	predatedAnimalsToday.clear();
	#endif

    // This increase has been extracted and converted to the function increaseAge
    //currentAge++; //arthros and dinos, update at the begining ot the time Step so feeding and reproi hav the same age conditions

	//I thought the curves where not beeing calculated everyday/timeStep... but yes check TerrainCell.cpp
/* 	ofstream noStream; //found in Dinosaurs but matter for others really this error???
	calculateGrowthCurves(temperature, noStream, false); //found in Dinosaurs but matter for others really this error???
 */
	//Diapo2
	//2___
	//Using Change_K (and Dells) for tuning Growth according to temperature
	double newT = 273 + temperature;
	//Optimo para las curvas de crecimiento=35. Para Vor Speed Search será 25
	double ToptVoracity = 273 + mySpecies->getTempOptVoracity();
	double ToptSearch = 273 + mySpecies->getTempOptSearch();
	double ToptSpeed = 273 + mySpecies->getTempOptSpeed();

	//the two values below are now going to be used again
	double preTsearch = search_area_ini*pow(calculateWetMass(), mySpecies->getScaleForSearchArea());
	
	
	double preTspeed = (speed_ini*pow(calculateWetMass(), mySpecies->getScaleForSpeed()))*(1-exp(-22*pow(calculateWetMass(),-0.6)));  //25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017

	/*
	double dellsToptVor = dells(ToptVoracity, ToptVoracity, traits[Trait::actE_vor]);
	double dellsNewTVor = dells(newT, ToptVoracity, traits[Trait::actE_vor]);
	double dellsToptSearch = dells(ToptSearch, ToptSearch, traits[Trait::actE_search]);
	double dellsNewTSearch = dells(newT, ToptSearch, traits[Trait::actE_search]);
	double dellsToptSpeed = dells(ToptSpeed, ToptSpeed, traits[Trait::actE_speed]);
	double dellsNewTSpeed = dells(newT, ToptSpeed, traits[Trait::actE_speed]);

	double postTvor = linearInterpolate(dellsNewTVor, 0, dellsToptVor, 0, preTvor);
	double postTsearch = linearInterpolate(dellsNewTSearch, 0, dellsToptSearch, 0, preTsearch);
	double postTspeed = linearInterpolate(dellsNewTSpeed, 0, dellsToptSpeed, 0, preTspeed);

	//3___
	maxPostTvor = postTvor + postTvor * mySpecies->maxPlasticityDueToConditionVor;
	minPostTvor = postTvor - postTvor * mySpecies->minPlasticityDueToConditionVor;
	maxPostTsearch = postTsearch + postTsearch * mySpecies->maxPlasticityDueToConditionSearch;
	minPostTsearch = postTsearch - postTsearch * mySpecies->minPlasticityDueToConditionSearch;
	maxPostTspeed = postTspeed + postTspeed * mySpecies->maxPlasticityDueToConditionSpeed;
	minPostTspeed = postTspeed - postTspeed * mySpecies->minPlasticityDueToConditionSpeed;
	*/
	//cout << "id... " << getId() << endl;
	//cout << "time 1... " << maxPostTsearch << endl;
	//cout << "time 1 wet mass " << getWetMass() << endl;


    wetMassAtTheBeginningOfTheTimeStep = calculateWetMass();

   //Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;

	

	//Jordi starts here::::
    //forgot to declare this: Important to do it outside the if else..
    double iniCurrentInstarMass = 0.0;
    double targetNextInstarMass = 0.0;
    double minTotalMetabolicDryMassLoss = 0.0;
    double finalJMinVB = 0.0;
    double finalVoracity = 0.0;
    double postTsearch = 0.0;
	maxPostTsearch = 0.0;					  
    double postTspeed = 0.0;
    double finalSearch = 0.0;
    double finalSpeed = 0.0;
	maxNextInstarMassFromVBPlasticity = 0.0;		
	double lengthAtBirth = 0;
	double xmid = 0;						 

    //double finalJMaxVB = 0.0;
    // I have made this Public in Animal class as to use it in other functions

    //double minMassAtCurrentAge = currentBodySize + currentBodySize * mySpecies->getMinPlasticityKVonBertalanffy();

	//Forcing continuous growth in Dinosaurs
   /*	double dinoTankPredicted = tank_ini*pow(calculateDryMass(),mySpecies->getBetaScaleTank());
	currentBodySize = mySpecies->getAssignedForMolt()*(calculateDryMass() - dinoTankPredicted);
	traits[Trait::energy_tank] = mySpecies->getAssignedForMolt()*dinoTankPredicted;*/

       
	//Forcing continuous growth in Dinosaurs - warning this involves heavy investment in growth - reproduction?
   	double dinoLengthPredicted = 0.0;
	//if(currentAge > 5){ //to surpass the critical young stages
	if(mySpecies->getGrowthType() == 0){
		dinoLengthPredicted = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
		//exit(-1);
	}
	if(mySpecies->getGrowthType() == 1){
		
		lengthAtBirth = pow((getMassAtBirth()/mySpecies->getCoefficientForMassAforMature()),1/mySpecies->getScaleForMassBforMature()); 
		
		//fix_xmid_K =  log((Asym-trex[1,"FC_Km"])/trex[1,"FC_Km"])*(1/target_K)
		
		xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
		
		dinoLengthPredicted = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
	}
	/*}else{   //this is to avoid animals to be born with size of zero age but being much older-thus need to be larger
	  dinoLengthPredicted = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));	
	}*/

	double dinoMassPredicted = mySpecies->getCoefficientForMassAforMature()*pow(dinoLengthPredicted,mySpecies->getScaleForMassBforMature());

    //dinos2023 - moulting only if realized mass is > predicted mass
	 if(position->getTheWorld()->getSimulType() == 1){
		if(calculateDryMass() > dinoMassPredicted){	
		double dinoTankPredicted = tank_ini*pow(calculateDryMass(),mySpecies->getBetaScaleTank());
		currentBodySize = mySpecies->getAssignedForMolt()*(calculateDryMass() - dinoTankPredicted);
		setTrait(Trait::energy_tank, mySpecies->getAssignedForMolt()*dinoTankPredicted);
	}
	 }
	//end dinos2023

//not for arthropods only for debugging 	
/* if(calculateDryMass() > dinoMassPredicted){ //(currentAge>5) && (
    double dinoTankPredicted = tank_ini*pow(dinoMassPredicted,mySpecies->getBetaScaleTank());
	double dinoBodyPredicted = dinoMassPredicted - dinoTankPredicted;
    currentBodySize = dinoBodyPredicted;
	traits[Trait::energy_tank] = mySpecies->getAssignedForMolt()*(calculateDryMass()-currentBodySize);
	//cout << "holar" << endl;
	}   */  
//END - not for arthropods only for debugging 

	//death criterion fixed to the true plasticity bands - Dinosaurs
	double minMassAtCurrentAge;
	double longevity = mySpecies->getLongevitySinceMaturation()*ageOfFirstMaturation;

/* 	cout << ageOfFirstMaturation << endl;
	exit(-1); */

	if (currentAge>(0.15*longevity)){ //arthropod to prevent the tiny animals from dying too soon...
	minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * mySpecies->getMinPlasticityKVonBertalanffy();
	}else{
    minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * 3 * mySpecies->getMinPlasticityKVonBertalanffy();
	}
//Dinosaurs
/* 	double minMassAtCurrentAge = Linf*(1-exp(-traits[Trait::growth]*(currentAge-thisAnimalVonBertTime0)));
	minMassAtCurrentAge = 0.1*mySpecies->getCoefficientForMassAforMature()*pow(minMassAtCurrentAge,mySpecies->getScaleForMassBforMature()); //+ mySpecies->getMinPlasticityKVonBertalanffy()*mySpecies->getCoefficientForMassAforMature()*pow(minMassAtCurrentAge,mySpecies->getScaleForMassBforMature());
 */


//Dinosaurs (really only?) should by daily target - attempt because current voracities are ridiculous
	
	//Calculating body mass plasticity bands for the next timeStep
 	
	/* if(getId() == 17){
		cout << getId() << endl;
		cout << massForNextReproduction << endl;
		cout << calculateDryMass() << endl;
		//exit(-1);
		} */

	

	

	double nextDinoLengthPredicted = 0.0; 
	//double nextDinoMassPredicted;
	if(!isMature()){ 
		
		if(mySpecies->getGrowthType() == 0){
			if(currentAge==0 || currentAge==1){
				nextDinoLengthPredicted = Linf*(1-exp(-getTrait(Trait::growth)*(1-thisAnimalVonBertTime0)));
			}else{
				nextDinoLengthPredicted = Linf*(1-exp(-getTrait(Trait::growth)*((currentAge+1)-thisAnimalVonBertTime0)));
			}
		}

		if(mySpecies->getGrowthType() == 1){
			if(currentAge==0 || currentAge==1){
				nextDinoLengthPredicted = Linf/(1+exp((xmid-1)*getTrait(Trait::growth)));
			}else{
				nextDinoLengthPredicted = Linf/(1+exp((xmid-(currentAge+1))*getTrait(Trait::growth)));
			}
		}

		nextDinoMassPredicted = mySpecies->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,mySpecies->getScaleForMassBforMature());


	
	}else{

		/* if(fromForceMolting1 == false){ 
		cout << "Id            xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << getId() << endl;
		cout << gender << endl;
		cout << massForNextReproduction << endl;
		cout << calculateDryMass() << endl;
		cout << ageForNextReproduction << endl; 
		cout << currentAge << endl;
		} */
		//here we assume an instantaneous linear relationship between current and future mass
		//as it is changed at every time step, the final relationship must be a decelerating curve

		/* if(massForNextReproduction < calculateDryMass() || ageForNextReproduction < currentAge){
		cout << "Id            xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << getId() << endl;
		 */
		
		
		if(massForNextReproduction > calculateDryMass() && ageForNextReproduction >= currentAge){
		
			if(ageForNextReproduction > currentAge){
				slopeTarget = (massForNextReproduction - calculateDryMass())/(ageForNextReproduction-currentAge);
				interceptTarget = calculateDryMass()-slopeTarget*currentAge;
				nextDinoMassPredicted = interceptTarget + slopeTarget*(currentAge+1);

				//nextDinoMassPredicted = mySpecies->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,mySpecies->getScaleForMassBforMature());	
				slopeTarget = 0.0;
				interceptTarget = 0.0;
			}else{//force currentAge-1 for calculations as to avoid nans
				slopeTarget = (massForNextReproduction - calculateDryMass())/(ageForNextReproduction-(currentAge-1));
				interceptTarget = calculateDryMass()-slopeTarget*(currentAge-1);
				nextDinoMassPredicted = interceptTarget + slopeTarget*currentAge;
			    //nextDinoMassPredicted = mySpecies->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,mySpecies->getScaleForMassBforMature());	
				
				
				slopeTarget = 0.0;
				interceptTarget = 0.0;
			}
				
		}else{
		
			if(massForNextReproduction < calculateDryMass() && ageForNextReproduction > currentAge){
				nextDinoMassPredicted = interceptTarget + slopeTarget*(currentAge+1); 
			}
			
			//here we force a positive slope to allow animals to keep feeding until the target is met 
			if(massForNextReproduction > calculateDryMass() && ageForNextReproduction < currentAge){
				slopeTarget = (massForNextReproduction-calculateDryMass())/(ageForNextReproduction-currentAge);
				interceptTarget = massForNextReproduction-slopeTarget*ageForNextReproduction;
				nextDinoMassPredicted = interceptTarget + slopeTarget*ageForNextReproduction;
				//nextDinoMassPredicted = mySpecies->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,mySpecies->getScaleForMassBforMature());	
				/* cout << "comorl?.........+++++++++++++++++++++++++++++++++++++++" << endl;
				cout << getId() << endl;
		        cout << gender << endl;
		        cout << nextDinoMassPredicted << endl; */

				
				
				slopeTarget = 0.0;
				interceptTarget = 0.0;
			}

				  /*if(getId() == 0){
	                cout << getId() << endl;
					cout << nextDinoMassPredicted << endl;
					cout << slopeTarget << endl;
					cout << interceptTarget << endl;
					cout << massForNextReproduction << endl;
					cout << calculateDryMass() << endl;
					cout << ageForNextReproduction << endl; 
					cout << currentAge << endl;
					//exit(-1); 
					} */ 
		
		//ScientificName() == "Microbisium_sp" && instar+2 == 4

		//for whatever reason this happens at initialization, so we do this just to straightening the targets
		/* if(massForNextReproduction < calculateDryMass() && ageForNextReproduction > currentAge){

	    slopeTarget = (calculateDryMass() - massForNextReproduction)/(ageForNextReproduction-currentAge);
		interceptTarget = massForNextReproduction-slopeTarget*currentAge;
		nextDinoMassPredicted = interceptTarget + slopeTarget*(currentAge+1);
		} */

		
		}

		
				/* if(getId() == 3 && timeStep==15){
					cout << slopeTarget << endl;
					cout << interceptTarget << endl;
				} */


		/* if (relativeHumidity<100) {
		cout << massForNextReproduction << endl;

		cout << ageForNextReproduction << endl;
		
		cout << currentAge << endl;

		cout << slopeTarget << endl;
		
		cout << interceptTarget << endl;

		//exit(-1);
		} */
	}

	
   double targetMass;
   double minNextDinoMassPredicted;
   double maxNextDinoMassPredicted;
    /*if(getId() == 2){

    	cout << "instar: " << instar+2 << endl;
    	cout << "mature? " << isMature() << endl;
    	cout << "calculateDryMass: " << calculateDryMass() << endl;
    	cout << "massesVector[instar+1]: " << massesVector[instar+1] << endl;
    	cout << "currentAge: " << currentAge << endl;
    	cout << "finalDevTimeVector[instar+1]: " << finalDevTimeVector[instar+1] << endl;
    	cout << "lifeStage: " << lifeStage << endl;

    	if(timeStep>30){exit(-1);}
    	}*/

	
 
	

    if(!isMature())
	{

    	/*if(getId() == 2 && instar+2==10 &&timeStep>94){
    					cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    					cout << instar+2 << endl;
    					cout << calculateDryMass() << endl;
    					cout << massesVector[instar+1] << endl;
    					cout << currentAge << endl;
    					cout << finalDevTimeVector[instar+1] << endl;
    					cout << lifeStage << endl;

    				exit(-1);
    				}*/



		

    minTotalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, temperature, 0, timeStepsPerDay);

    minNextDinoMassPredicted = nextDinoMassPredicted - nextDinoMassPredicted * mySpecies->getMinPlasticityKVonBertalanffy();
	
	maxNextDinoMassPredicted = nextDinoMassPredicted + nextDinoMassPredicted * mySpecies->getMaxPlasticityKVonBertalanffy();

    targetMass = maxNextDinoMassPredicted - calculateDryMass();

	if(targetMass<0){
		targetMass = 0;		
	}

	targetMass = targetMass + minTotalMetabolicDryMassLoss;	
	
	}
	else //isMature == true
    {

					/* double lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
					eggDryMassForClutch = mySpecies->getCoefficientForMassA()*pow(lengthAtBirth,mySpecies->getScaleForMassB())+mySpecies->getCoefficientForMassA()*pow(lengthAtBirth,mySpecies->getScaleForMassB()) * traits[Trait::factorEggMass];

					if(mySpecies->hasEggClutchFromEquation())
					{
						clutchDryMass = (mySpecies->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, mySpecies->getForClutchMassScale())) / mySpecies->getConversionToWetMass();
					}
					else //If it does not come from equation,
					{
						clutchDryMass = eggDryMassForClutch*mySpecies->getEggsPerBatch(); //clutch dry mass is identical to the mass of an egg - done for mites or others that only lay one egg at a time.
					} */




	minTotalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, temperature, 0, timeStepsPerDay);

    minNextDinoMassPredicted = nextDinoMassPredicted - nextDinoMassPredicted * mySpecies->getMinPlasticityKVonBertalanffy();
	
	maxNextDinoMassPredicted = nextDinoMassPredicted + nextDinoMassPredicted * (mySpecies->getMaxPlasticityKVonBertalanffy()*1.0);

    
	targetMass = maxNextDinoMassPredicted - calculateDryMass();

	if(targetMass<0){
			
		if(currentAge > ageForNextReproduction){
			targetMass = abs(maxNextDinoMassPredicted - calculateDryMass());		
			/* cout << "whatrl?.........OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" << endl;
					cout << getId() << endl;
					cout << gender << endl;
					cout << targetMass << endl;
					exit(-1);  */
		}else{
			targetMass=0;
		}

	}



		targetMass = targetMass + minTotalMetabolicDryMassLoss; //Arthropods - increasing voracity to one egg per day is sufficient to allow Onychiurus to go on;	

	}
  
    
	/*cout << getId() << endl;
	cout << calculateDryMass() << endl;
	cout << nextDinoMassPredicted << endl;
	cout << maxNextDinoMassPredicted << endl;
	cout << targetMass << endl;*/

   /* if(mySpecies->getId() == 1){
			cout << massForNextReproduction << endl;
			cout << calculateDryMass() << endl;
			cout << ageForNextReproduction << endl;
			cout << currentAge << endl;
			cout << targetMass << endl;
			exit(-1);
		}	 */

  /* if (isMature() && relativeHumidity<100){
		
		cout << massForNextReproduction << endl;

		cout << ageForNextReproduction << endl;
		
		cout << calculateDryMass() << endl;

		cout << maxNextDinoMassPredicted << endl;
		
		cout << targetMass << endl;

		//exit(-1);
		} */

	//BELOW WE INCLUDE TEMP EFFECTS FOR VORACITY
    double dellsNewT = Math_Functions::dells(
    		newT,
			ToptVoracity,
			mySpecies->getEdVoracity(),
			getTrait(Trait::actE_vor)
			);

    double dellsMinT = Math_Functions::dells(
    		0+273,
			ToptVoracity,
			mySpecies->getEdVoracity(),
			getTrait(Trait::actE_vor)
			);

    double dellsMaxT = Math_Functions::dells(
    		ToptVoracity,
			ToptVoracity,
			mySpecies->getEdVoracity(),
			getTrait(Trait::actE_vor)
    		);

    double postTvor = Math_Functions::linearInterpolate(
    		dellsNewT,
			dellsMinT,
			dellsMaxT,
			mySpecies->getMinVorExtremeT(),
			voracity_ini
			);

    /*cout << "newT: " << newT << endl;
    cout << "dellsNewT: " << dellsNewT << endl;
    cout << "dellsMinT: " << dellsMinT << endl;
    cout << "dellsMaxT: " << dellsMaxT << endl;

    cout << "mySpecies->getMinVorExtremeT(): " << mySpecies->getMinVorExtremeT() << endl;
    cout << "voracity_ini: " << voracity_ini << endl;*/

    //cout << "postTvor: " << postTvor << endl;

    //postTvor = 1;

	finalJMinVB=0;
	finalJMaxVB=0;

    double preFinalVoracity = postTvor*targetMass; // Dinosaurs



    //cout << "here: " << preFinalVoracity/calculateDryMass() << endl;
    //exit(-1);

 /*  if(getId() == 1 && timeStep == 40){

            	cout << "preFinalVoracity: "<< preFinalVoracity << endl;
            	cout << "finalJMinVB: "<< finalJMinVB << endl;
            	cout << "finalJMaxVB: "<< finalJMaxVB << endl;
            	cout << "postTvor: "<< postTvor << endl;

            exit(-1);
            }*/

	setTrait(Trait::voracity, preFinalVoracity);
	//below we ensure that voracity does not get into negative values, if for instance voracity_ini is negative


    preFinalVoracity = max(getTrait(Trait::voracity),0.0);

    //preFinalVoracity = min(preFinalVoracity+calculateDryMass(),maxNextInstarMassFromVBPlasticity)-calculateDryMass();

	if(preFinalVoracity > 0 && nextDinoMassPredicted > 0.00000001) //when slope and interceptTarget = 0
	{
	//BELOW WE INCLUDE PLASTICITY DUE TO CONDITION EFFECTS FOR VORACITY
	//double massToTarget = maxNextInstarMassFromVBPlasticity - calculateDryMass();

    //0 is the mass to achieve when the animal is actually on the target
    double fullTrajectoryMass = maxNextDinoMassPredicted - minNextDinoMassPredicted;

	double finalVoracity = Math_Functions::linearInterpolate(
    		targetMass,
			0,
			fullTrajectoryMass,
			preFinalVoracity - preFinalVoracity * mySpecies->getMinPlasticityDueToConditionVor(),
			preFinalVoracity + preFinalVoracity * mySpecies->getMaxPlasticityDueToConditionVor()
			);

				
    //below we ensure that voracity does not get into negative values
	setTrait(Trait::voracity, max(finalVoracity, 0.0));

	//cout << "here: " << traits[Trait::voracity]/calculateDryMass() << endl;

    //BELOW WE INCLUDE TEMP TUNING FOR SEARCH AREA AND SPEED
    double dellsNewTsearch = Math_Functions::dells(newT, ToptSearch, mySpecies->getEdSearch(), getTrait(Trait::actE_search));
    double dellsMinTsearch = Math_Functions::dells(0+273, ToptSearch, mySpecies->getEdSearch(), getTrait(Trait::actE_search));
    double dellsMaxTsearch = Math_Functions::dells(ToptSearch, ToptSearch, mySpecies->getEdSearch(), getTrait(Trait::actE_search));
    postTsearch = Math_Functions::linearInterpolate(
    		dellsNewTsearch,
			dellsMinTsearch,
			dellsMaxTsearch,
			preTsearch - preTsearch * mySpecies->getFractSearchExtremeT(),
			preTsearch);
	
	postTsearch = preTsearch; //this is to have more predictive mobilities

    double dellsNewTspeed = Math_Functions::dells(newT, ToptSpeed, mySpecies->getEdSpeed(), getTrait(Trait::actE_speed));
    double dellsMinTspeed = Math_Functions::dells(0+273, ToptSpeed, mySpecies->getEdSpeed(), getTrait(Trait::actE_speed));
    double dellsMaxTspeed = Math_Functions::dells(ToptSpeed, ToptSpeed, mySpecies->getEdSpeed(), getTrait(Trait::actE_speed));
    postTspeed = Math_Functions::linearInterpolate(
    		dellsNewTspeed,
    		dellsMinTspeed,
			dellsMaxTspeed,
			preTspeed - preTspeed * mySpecies->getFractSpeedExtremeT(),
			preTspeed);

    //BELOW WE INCLUDE CONDITION-DEPENDENT PLASTICITY FOR SEARCH AREA AND SPEED
    finalSearch = Math_Functions::linearInterpolate(
    		targetMass,
    		0,
			fullTrajectoryMass,
			postTsearch - postTsearch*mySpecies->getMinPlasticityDueToConditionSearch(),
			postTsearch + postTsearch*mySpecies->getMaxPlasticityDueToConditionSearch());

    finalSearch = postTsearch; //this is to have more predictive mobilities
   
    maxPostTsearch = postTsearch + postTsearch * mySpecies->getMaxPlasticityDueToConditionSearch();
	setTrait(Trait::search_area, finalSearch);

    //traits[Trait::search_area] = preTsearch;

    finalSpeed = Math_Functions::linearInterpolate(
    		targetMass,
			0,
			fullTrajectoryMass,
			postTspeed - postTspeed * mySpecies->getMinPlasticityDueToConditionSpeed(),
			postTspeed + postTspeed * mySpecies->getMaxPlasticityDueToConditionSpeed()
			);

	setTrait(Trait::speed, finalSpeed);

/* if(getId() == 3 && timeStep==15){
		cout << nextDinoMassPredicted << endl;
		//exit(-1);
		}  */

	if(::isnan(finalSpeed) || ::isnan(finalSearch) || ::isnan(finalVoracity) || ::isinf(finalSpeed) || ::isinf(finalSearch) || ::isinf(finalVoracity))
	{
		cout << "Animal id: " << this->getId() << " (" << this->getSpecies()->getScientificName() << ") - The growing curve resulted in a NaN value." << endl;
		cout << finalVoracity << endl;
		cout << finalSpeed << endl;
		cout << finalSearch << endl;
		exit(-1);
	}

	

	//below we ensure that search area does not get into negative values
	setTrait(Trait::search_area, max(finalSearch, 0.0));
	//below we ensure that speed does not get into negative values
	setTrait(Trait::speed, max(finalSpeed, 0.0));
	//Jordi ends here::::

	 //below we establish a decrease in activity due to previous encounters with predators
    //the punishment on the traits depends on the parameter decreaseOnTraitsDueToEncounters
	double vorAfterEncounters = getTrait(Trait::voracity) / (mySpecies->getDecreaseOnTraitsDueToEncounters()*todayEncountersWithPredators+1);
	double searchAfterEncounters = getTrait(Trait::search_area) / (mySpecies->getDecreaseOnTraitsDueToEncounters()*todayEncountersWithPredators+1);

	//vorAfterEncounters=min(vorAfterEncounters,maxNextInstarMassFromVBPlasticity);

   	if(vorAfterEncounters+calculateDryMass() > maxNextDinoMassPredicted){
		if(calculateDryMass() < maxNextDinoMassPredicted){
		vorAfterEncounters = maxNextDinoMassPredicted - calculateDryMass(); //min(vorAfterEncounters+calculateDryMass(),maxNextDinoMassPredicted)-calculateDryMass();
		}else{
		/* if(mySpecies->getScientificName() == "Microbisium_sp" && instar+2 == 4){
			cout << calculateDryMass() << endl;
			cout << maxNextDinoMassPredicted << endl;
			cout << vorAfterEncounters << endl;
		} */
		vorAfterEncounters = 0;	
		}
	}

	setTrait(Trait::voracity, vorAfterEncounters);

	setTrait(Trait::search_area, searchAfterEncounters);

	}else{

		setTrait(Trait::search_area, 0.0);

		setTrait(Trait::speed, 0.0);

		setTrait(Trait::voracity, preFinalVoracity);
      

	}
	timeStepMaximumHandlingTimer = 1.0;
	timeStepMaximumSearchArea = getTrait(Trait::search_area);



	
	
	//Dinos2023 - this should occur only for Dinos or large vertebrates but needs to be tested for invertebrates
	//We truncate voracity using the allometric equation of Garland 1983 for terrestrial mammals 
    if(position->getTheWorld()->getSimulType() == 0){  //is a simulation on arthropods
		double AmNat = (0.1*pow((calculateWetMass()),0.75))/mySpecies->getConversionToWetMass();
		
		if(AmNat < getTrait(Trait::voracity)){
			setTrait(Trait::voracity, AmNat);
		}  
	}
    if(position->getTheWorld()->getSimulType() == 1){  //is a simulation on dinosaurs
		double Garland1983 = ((152*pow((calculateWetMass()),0.738))/1000)/mySpecies->getConversionToWetMass();
		
	/* 	if(traits[Trait::voracity] > 60){
			cout << Garland1983 << endl;
			cout << traits[Trait::voracity] << endl; 
			cout << calculateDryMass() << endl;
			cout << currentAge << endl;
			cout << timeStep << endl;
			exit(-1);
		} */
		
		
		if(Garland1983 < getTrait(Trait::voracity)){
			setTrait(Trait::voracity, Garland1983);
		}  
	}
	//end Dinos2023


	//We assume that encounters DO NOT AFFECT the animal speed - although in reality stamina should be affected
	//However this affects also to the predators so we do not consider this for now
	//traits[Trait::speed] = traits[Trait::speed] / (mySpecies->decreaseOnTraitsDueToEncounters*todayEncountersWithPredators+1);

	//traits[Trait::energy_tank] < 0
	if(getTrait(Trait::energy_tank) < 0) //Dinosaurs calculateDryMass() < minMassAtCurrentAge
	{
		setNewLifeStage(LIFE_STAGES::STARVED, timeStep);
	}
	/*else if(minMassAtCurrentAge >= finalJMaxVB+calculateDryMass())//Jordi note: I do not follow why this sentence //Gabi: prevents from crashing when the limits are reverted (due to disadjudments in the curve values)
	{
		//cout << "For species " << mySpecies->getScientificName() << ": maxKplasticityForVonBertalanfy and minKplasticityForVonBertalanfy are not set correctly. Modify these values and re-run..." << endl;
		lifeStage = STARVED;
		setDateOfDeath(timeStep);
	}
	else if (calculateDryMass() > finalJMaxVB+massesVector[instar+1])
	{
		//Reduce animal performance to the minimum. It will molt when the time comes
		if(!isMature() || (isMature() && isMated()))
		{
			lifeStage = ACTIVE;
		}
	}
	else
	{
		lifeStage = ACTIVE;
	}
*/
	/*
	if(timeStep==200){
	if (0.99 > Random::randomUniform()) //to induce a bottleneck 
	{
		setNewLifeStage(LIFE_STAGES::BACKGROUND, timeStep);
	}
	}else{  */
	//arthros and dinos to create patches of enemy free space - kill all predators in designated patches for initialization

	//create patches with predator (enemy) free space at initialization
	if(position->getTheWorld()->getCell2(position->getZ(), position->getY(), position->getX())->isInEnemyFreeSpace() && timeStep == 0 && this->getHuntingMode()!=DOES_NOT_HUNT)
	{	
		setNewLifeStage(LIFE_STAGES::BACKGROUND, timeStep);
	}
	
	//create patches with consumer competitor free space at initialization
	if(position->getTheWorld()->getCell2(position->getZ(), position->getY(), position->getX())->isInCompetitorFreeSpace() && timeStep == 0 && this->getHuntingMode()==DOES_NOT_HUNT)
	{	
		setNewLifeStage(LIFE_STAGES::BACKGROUND, timeStep);
	}
	
	//background mortality
	if(getId() != 0){
    if (mySpecies->getProbabilityDeathFromBackground() > Random::randomUniform())
	{
		setNewLifeStage(LIFE_STAGES::BACKGROUND, timeStep);
	}	
    }
	//}
	

	if(isMature() && gender == AnimalSpecies::GENDERS::MALE)
	{
		setTrait(Trait::search_area, getTrait(Trait::search_area) * mySpecies->getMaleMobility());
	}

	//TODO PARA LOS CRECIMIENTO INDETERMINADO, SE DEJAN POR AHORA QUE SIGAN MOVIENDOSE INFINITO
	//TODO EN UN FUTURO SE HARÁ PARA LOS INDETERMINADO, DEJANDO QUE SIGAN CRECIENDO SI ALCANZAN EL TIEMPO DE LONGEVITY
	if(currentAge >= ageOfFirstMaturation * mySpecies->getLongevitySinceMaturation())
	{
		setNewLifeStage(LIFE_STAGES::SENESCED, timeStep);
	}


	//below it means that metabolic downregulation only exists for predators, such as spiders
	if(huntingMode != Animal::HUNTING_MODES::DOES_NOT_HUNT && daysWithoutFood >= mySpecies->getDaysWithoutFoodForMetabolicDownregulation())
	{
		setTrait(Trait::voracity, getTrait(Trait::voracity) - mySpecies->getPercentageCostForMetabolicDownregulationVoracity() * getTrait(Trait::voracity));
		setTrait(Trait::search_area, getTrait(Trait::search_area) - mySpecies->getPercentageCostForMetabolicDownregulationSearchArea() * getTrait(Trait::search_area));
		setTrait(Trait::speed, getTrait(Trait::speed) - mySpecies->getPercentageCostForMetabolicDownregulationSpeed() * getTrait(Trait::speed));
	}

	//In this version the dummy1 trait involves Maximum Critical Temperature (CTmax)
	if(getTrait(Trait::dummy1)<(temperature+273))
	{
		setNewLifeStage(LIFE_STAGES::SHOCKED, timeStep);
	}

	if(relativeHumidity < mySpecies->getMinRelativeHumidityThreshold())
	{
		setNewLifeStage(LIFE_STAGES::DIAPAUSE);
	}



	if(printGrowthData)
	{
		tuneTraitsFile << getIdStr() << "\t"
		<< mySpecies->getScientificName() << "\t"
		<< lifeStage << "\t"
		<< currentAge << "\t"
		<< instar << "\t"
		<< currentBodySize << "\t"
		<< getTrait(Trait::energy_tank) << "\t"
		<< tankAtGrowth << "\t" //done to document excessInvestment behavior
		<< calculateDryMass() << "\t"
		<< isMature() << "\t"
		<< minMassAtCurrentAge << "\t"
		<< finalJMinVB << "\t"
		<< finalJMaxVB << "\t"
		<< voracity_ini << "\t"
		<< calculateDryMass()+finalJMinVB << "\t"
		<< calculateDryMass()+getTrait(Trait::voracity) << "\t"
		<< maxNextInstarMassFromVBPlasticity << "\t"
		<< calculateWetMass() << "\t"
		<< preTsearch << "\t"
		<< preTspeed << "\t"
		<< postTsearch << "\t"
		<< postTspeed << "\t"
		<< iniCurrentInstarMass << "\t"
		<< targetNextInstarMass << "\t"
		<< minTotalMetabolicDryMassLoss << "\t"
		<< finalSearch << "\t"
		<< finalSpeed << "\t"
		<< preFinalVoracity << "\t"
		<< finalVoracity << "\t"
		<< getTrait(Trait::voracity) << "\t"
		<< getTrait(Trait::search_area) << "\t"
		<< getTrait(Trait::speed) << "\t"
		<< getDateOfDeath() << "\t";
		tuneTraitsFile << endl;
	}

/*  	if(isMature() == 1 && timeStep > 449 && getId() == 0){
		cout << nextDinoMassPredicted << endl;
		cout << calculateDryMass() << endl;
		cout << isMature() << endl;
		cout << ageForNextReproduction << endl; 
		cout << currentAge << endl;
		cout << timeStep << endl;
		exit(-1);
	} 
 */
	todayEncountersWithPredators = 0;

	//currentAge += 1; //Dinosaurs - move to the beginning of tuneTraits as to meet with age for reproduction

	//TODO-SEMI FALTA ASOCIAR WeightPerLocus tiene que ser un código el cual indique qué locus de qué rasgo tiene un valor específico.
	//Está sin utilizar ahora mismo. Será una lista de entradas de la forma: mirar linked_aca

	//Output: "id\tspecies\tstate\tcurrent_age\ttarget_instar_devtime\tinstar\ttarget_instar\tbody_size\tenergy_tank\tbody_mass\ttarget_mass\tmin_expected_body_mass\texpected_body_mass\tmax_expected_body_mass\tcondition_dependence\tmax_condition_dependence\tpreT_voracity\tpreT_search_area\tpreT_speed\tmin_postT_voracity\tmin_postT_search_area\tmin_postT_speed\tpostT_voracity\tpostT_search_area\tpostT_speed\tmax_postT_voracity\tmax_postT_search_area\tmax_postT_speed\tconditioned_voracity\tconditioned_search_area\tconditioned_speed" << endl;

	//TODO Diapausa cuando se pone el huevo. diapauseTimer = pheno. Solamente se disminuye diapauseTimer si las condiciones (temperatura y/o humedad) lo permiten, por debajo de un umbral.
	//TODO Los numeros de individuos por instar ahora van a ser DENSIDADES por instar. Que pasaran a ser numero de individuos dependiendo del área habitable.

	//TODO Eliminar el hongo cuando sea <= 0, y borrar minimumfungicapacity y getZeroFungi.
	//TODO Añadir un parametro que decida si el animal crece por mudas o continuo (dinosaurios)






}

double Animal::getTotalMetabolicDryMassLoss(double wetMass, double temperature, double proportionOfTimeTheAnimalWasMoving, int timeStepsPerDay)
{
	
double totalMetabolicDryMassLoss = 0;

	if(position->getTheWorld()->getSimulType() == 0){
		double basalMetabolicTax = exp(-7.2945+43.966*getTrait(Trait::actE_met)+getTrait(Trait::met_rate)*log(wetMass)-getTrait(Trait::actE_met)*(1/((temperature+273.15)*BOLZMANN)));

		double fraction_with_stress = (Math_Functions::linearInterpolate(todayEncountersWithPredators, 0, mySpecies->getMaxEncountersT(), 0, 0.407)/timeStepsPerDay)*basalMetabolicTax;

		//TODO multiplicar por timeStep
		//24 because the metab rates are given in days.
		basalMetabolicTax = basalMetabolicTax + fraction_with_stress;
		double loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24;
		//TODO This 3 is a raw value from bibl.
		double field_met_tax = 3*basalMetabolicTax;
		double loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24;

		//7 is NOT referred to weeks. Conversion from Jules.
		loss_from_bmr=loss_from_bmr/7;
		loss_from_fmr=loss_from_fmr/7;
		totalMetabolicDryMassLoss = (loss_from_bmr + loss_from_fmr) / mySpecies->getConversionToWetMass();
	}


	if(position->getTheWorld()->getSimulType() == 1){
		//here Grady et al. 2014 provide results in Watts (j/s) and M in g
		double basalMetabolicTax = 0.002*pow(wetMass*1000,getTrait(Trait::met_rate));

		double fraction_with_stress = (Math_Functions::linearInterpolate(todayEncountersWithPredators, 0, mySpecies->getMaxEncountersT(), 0, 0.407)/timeStepsPerDay)*basalMetabolicTax;

		//TODO multiplicar por timeStep
		//24*3600 because the metab rates are given in J/s.
		basalMetabolicTax = basalMetabolicTax + fraction_with_stress;
		double loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24*3600;
		//TODO This 3 is a raw value from bibl.
		double field_met_tax = 3*basalMetabolicTax;
		double loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24*3600;

		//7 is NOT referred to weeks. Conversion from Jules.
		loss_from_bmr=loss_from_bmr/7;//7 joule = 1mg 
		loss_from_fmr=loss_from_fmr/7;
		//here we transform from mg to Kg to dinoWeaver
		totalMetabolicDryMassLoss = ((loss_from_bmr + loss_from_fmr)*0.000001) / mySpecies->getConversionToWetMass();
	}


totalMetabolicDryMassLoss = totalMetabolicDryMassLoss / timeStepsPerDay;


return totalMetabolicDryMassLoss;
}

//TODO parametro para que se ejecuten cada X timesteps
void Animal::metabolize(int timeStep, int timeStepsPerDay)
{
	#ifdef _DEBUG
		double lastEnergyTank = traits[Trait::energy_tank];
		double lastBodySize = traits[Trait::factorEggMass];
	#endif

	//double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;

	double proportionOfTimeTheAnimalWasMoving;
	if(lifeStage == SATIATED || lifeStage == HANDLING)
	{
		proportionOfTimeTheAnimalWasMoving = 0;
	}
	else
	{
		if(maxPostTsearch>0){
		    proportionOfTimeTheAnimalWasMoving = steps/maxPostTsearch;
		}else{
			proportionOfTimeTheAnimalWasMoving = 0;
		}
		
	
	}
	steps = 0;
	stepsAttempted = 0;

	double totalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, position->temperature, proportionOfTimeTheAnimalWasMoving, timeStepsPerDay);

	//Downregulation only here, do not change this into getMetabolicDryMassLoss because it would alter the expected loss in tuneTraits
	if(huntingMode != Animal::HUNTING_MODES::DOES_NOT_HUNT && daysWithoutFood >= mySpecies->getDaysWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLoss -= totalMetabolicDryMassLoss * mySpecies->getPercentageMetabolicDownregulation();
	}

	setTrait(Trait::energy_tank, getTrait(Trait::energy_tank) - totalMetabolicDryMassLoss);

	#ifdef _DEBUG
		double afterLossEnergyTank = traits[Trait::energy_tank];
		if(afterLossEnergyTank >= lastEnergyTank)
		{
			cerr << "The metabolic loss was 0 or positive:" << endl;
			cerr << " - Animal: " << id << "(" << mySpecies->getScientificName() << ")" << endl;
			cerr << " - Last energy tank: " << lastEnergyTank << endl;
			cerr << " - After loss energy tank: " << afterLossEnergyTank << endl;
		}
	#endif
}

//TODO parametro para que se ejecuten cada X timesteps
void Animal::grow(int timeStep, int timeStepsPerDay)
{
	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;

		double lengthAtBirth = 0;
		double xmid = 0;					
		double propAdultMass = 0;
		double currentEggs = 0;
	 

		if(mySpecies->getGrowthType() == 1){
			lengthAtBirth = pow((getMassAtBirth()/mySpecies->getCoefficientForMassAforMature()),1/mySpecies->getScaleForMassBforMature()); 
			xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
		}
		
	if(!isMature())
	{


	//to force initialization in juveniles - arthro - the values are dummy and won't be used unitl updated 
		double daysForPseudoTargetReproduction;
		daysForPseudoTargetReproduction = 1.1*currentAge;//1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = currentAge + daysForPseudoTargetReproduction;
		double currentLength = 0;
		double nextReproLength = 0;
		
		if(mySpecies->getGrowthType() == 0){

			currentLength = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
			nextReproLength = Linf*(1-exp(-getTrait(Trait::growth)*(ageForNextReproduction-thisAnimalVonBertTime0)));
		
		}

		if(mySpecies->getGrowthType() == 1){

			currentLength = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
			nextReproLength = Linf/(1+exp((xmid-ageForNextReproduction)*getTrait(Trait::growth)));

		}


		massOfMaturationOrLastReproduction = mySpecies->getCoefficientForMassAforMature()*pow(currentLength,mySpecies->getScaleForMassBforMature());
		massForNextReproduction = mySpecies->getCoefficientForMassAforMature()*pow(nextReproLength,mySpecies->getScaleForMassBforMature());
		//end arthro

		//arthro - assignedForMolt is considered for molting or investment could be lower than the actual target
		if(calculateDryMass() >= massesVector[instar.getValue()] && currentAge >= finalDevTimeVector[instar.getValue()])
		{

			/* if(currentAge > 1){
			cout << currentAge << endl;
			exit(-1);
			} */

			//massesVector[instar+1]

			double investment = calculateDryMass()*mySpecies->getAssignedForMolt();
			double next_tank = tank_ini * pow(massesVector[instar.getValue()], mySpecies->getBetaScaleTank());
			double next_size = massesVector[instar.getValue()] - next_tank;
			double excessInvestment = investment-massesVector[instar.getValue()];
			lastMassBeforeMaturationOrOviposition = calculateDryMass();

			/* cout << excessInvestment << endl;
			cout << next_tank << endl;  */
			//exit(-1);
		

	 		currentBodySize = next_size + excessInvestment*mySpecies->getExcessInvestInSize();
			setTrait(Trait::energy_tank, next_tank + excessInvestment*(1-mySpecies->getExcessInvestInSize()));
			
			tankAtGrowth = getTrait(Trait::energy_tank);

			#ifdef _DEBUG
				double afterMoultEnergyTank = traits[Trait::energy_tank];
				double afterMoultBodySize = traits[Trait::factorEggMass];
				if(afterMoultEnergyTank >= lastEnergyTank)
				{
					cerr << "The energy tank after moult was higher than before:" << endl;
					cerr << " - Animal: " << id << "(" << mySpecies->getScientificName() << ")" << endl;
					cerr << " - Last energy tank: " << lastEnergyTank << endl;
					cerr << " - After moult energy tank: " << afterMoultEnergyTank << endl;
				}
				if(afterMoultBodySize <= lastBodySize)
				{
					cerr << "The body size after moult was lower than before: " << endl;
					cerr << " - Animal: " << id << "(" << mySpecies->getScientificName() << ")" << endl;
					cerr << " - Last body size: " << lastBodySize << endl;
					cerr << " - After moult body size: " << afterMoultBodySize << endl;
				}
			#endif
			
			instar.moveOnNextInstar();

			/* if(mySpecies->hasIndeterminateGrowth() && (instar+2 > mySpecies->getInstarFirstReproduction())){

			 if((mySpecies->getInstarFirstReproduction() % 2 == 0) &&
			  ((instar+2 - mySpecies->getInstarFirstReproduction()) % 2 == 0)){ //both even

				mature = true;

			 }else{ //at least one is odd

			  if((mySpecies->getInstarFirstReproduction() % 2 > 0) &&
			   ((instar+2 - mySpecies->getInstarFirstReproduction()) % 2 == 0)){ //second even

				mature = true;



			  }

			 }


			}else{ */

				//(instar+2 == mySpecies->getInstarFirstReproduction()) ||
				if((instar >= mySpecies->getInstarFirstReproduction()) || (instar == mySpecies->getNumberOfInstars()))
				{

					mature = true;


				}

		   //}
		//}//this is to mute the molting algorithm, wich is done in the if(10000)

		//arthro but really for dinos when molting algorithm above is muted - ends loop
		//exactly in the same place

		/*cout << finalDevTimeVector[(mySpecies->getInstarFirstReproduction()-2)] << endl;
		exit(-1);*/

		//warning, to use this below need to uncomment the } above
		/* if(currentAge >= finalDevTimeVector[(mySpecies->getInstarFirstReproduction()-2)]){
				
				//arthros and dinos especially this line below is necessary
				//when growth by moulting is muted
				lastMassBeforeMaturationOrOviposition = calculateDryMass();
				/////////////
				mature = true; */
			    //end arthro
				
				/* cout << mySpecies->getId() << endl;
				exit(-1);  */

			
			actualMoltingTimeVector.push_back(currentAge+1);
			//arthro really for dinos actualMoltingMassVector.push_back(investment-excessInvestment);
			ageOfLastMoultOrReproduction = currentAge;



			if(isMature())
			{


				//TODO TIMESTEP
				//This gets decremented in activateAnimals at the beginning of each day.
				massOfMaturationOrLastReproduction = calculateDryMass();

				//arthros but very important for repro in Dinos!!!! ageOfFirstMaturation by age not by instar
				 /* cout << currentAge << endl;
				cout << finalDevTimeVector[(mySpecies->getInstarFirstReproduction()-2)] << endl;
				exit(-1);  */

				if(currentAge == getAgeFirstReproduction()){ //instar+2 == mySpecies->getInstarFirstReproduction()){
				 	
					//if(mySpecies->getGrowthType() == 0){

						ageOfMaturation = currentAge;
						
						ageOfFirstMaturation = currentAge;

					//}

					/*if(mySpecies->getGrowthType() == 1){

						ageOfMaturation = xmid;
						
						ageOfFirstMaturation = xmid;
					
					}*/

					/* cout << ageOfFirstMaturation << endl;
					exit(-1);  
 */
			    }
				
				/* else{ arthros
				     ageOfMaturation = currentAge;
				} */
				if(currentAge==round(ageOfFirstMaturation)) // rep_count==0
				{
					pupaTimer = mySpecies->getPupaPeriodLength()*timeStepsPerDay;
				}
				//double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;
				//DONE RepFactor for different genders
				double daysForTargetReproduction;
				if(gender == AnimalSpecies::GENDERS::FEMALE)
				{

					//Dinosaurs
					//and arthros we remove eggDryMass from script because it may not work - early death from starvation
					//double lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
					eggDryMassForClutch = mySpecies->getEggDryMass()+mySpecies->getEggDryMass()*getTrait(Trait::factorEggMass);
				
					double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*mySpecies->getMaxPlasticityKVonBertalanffy();
					//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
					double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * mySpecies->getMinPlasticityKVonBertalanffy();//Dinosaurs

					//below final eggDryMass is set after the female eggMass trait, if it surpasses the
					//growing curve band limits, it is set to the limit  //Dinosaurs
					eggDryMassForClutch = min(eggDryMassForClutch, maxMassAtCurrentAge);
					eggDryMassForClutch = max(eggDryMassForClutch, minMassAtCurrentAge);
				
				/* double lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
				eggDryMassForClutch = mySpecies->getCoefficientForMassA()*pow(lengthAtBirth,mySpecies->getScaleForMassB())+mySpecies->getCoefficientForMassA()*pow(lengthAtBirth,mySpecies->getScaleForMassB()) * traits[Trait::factorEggMass];
 */

					if(eggDryMassForClutch < 0){
						//cout << "lengthAtBirth: " << lengthAtBirth << endl;
						cout << "eggDryMassForClutch: " << eggDryMassForClutch << endl;
						cout << "Linf: " << Linf << endl;
						cout << "Growth: " << getTrait(Trait::growth) << endl;
						cout << "thisAnimalVonBertTime0: " << thisAnimalVonBertTime0 << endl;
						exit(-1);
					}

					if(mySpecies->hasEggClutchFromEquation())
					{
						clutchDryMass = (mySpecies->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, mySpecies->getForClutchMassScale())) / mySpecies->getConversionToWetMass();
					}
					else //If it does not come from equation,
					{
					
						
						
						//arthro
						if(mySpecies->hasIndeterminateGrowth()){

							double lengthAtThisAge = 0;
							
							if(mySpecies->getGrowthType() == 0){
							  lengthAtThisAge = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
							}

							if(mySpecies->getGrowthType() == 1){
							  lengthAtThisAge = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
							}


							double massAtThisAge = mySpecies->getCoefficientForMassAforMature()*pow(lengthAtThisAge,mySpecies->getScaleForMassBforMature());
							
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(mySpecies->getEggsPerBatch()*propAdultMass), mySpecies->getEggsPerBatch()); 
							
							clutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
						}else{
							clutchDryMass = eggDryMassForClutch*mySpecies->getEggsPerBatch();	
						}
						//end arthro
					
					}

					/* cout << "instar  " << instar+2 << endl;
					cout << "clutchDryMass  " << clutchDryMass << endl; */
					

					massForNextReproduction = lastMassBeforeMaturationOrOviposition + clutchDryMass;
					//daysForTargetReproduction = ceil( (clutchDryMass*ageOfMaturation) / lastMassBeforeMaturationOrOviposition);
					
					/* cout << lastMassBeforeMaturationOrOviposition << endl;
					cout << clutchDryMass << endl;
					exit(-1); */

					//arthros
					double longevity = mySpecies->getLongevitySinceMaturation()*ageOfFirstMaturation;
					double reproTime = longevity - ageOfFirstMaturation + mySpecies->getPupaPeriodLength();
				
					daysForTargetReproduction = floor(reproTime/mySpecies->getFemaleMaxReproductionEvents());
					
					
					//end athros


				}
				else //is male
				{
					
					//arthro - to make targets more continuous also for dinos
					double currentLength = 0;
					double nextReproLength = 0;
							
						if(mySpecies->getGrowthType() == 0){
		  				    currentLength = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
    		   			    nextReproLength = Linf*(1-exp(-getTrait(Trait::growth)*((currentAge+1)-thisAnimalVonBertTime0)));
						}

						if(mySpecies->getGrowthType() == 1){
		  				    currentLength = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
    		   			    nextReproLength = Linf/(1+exp((xmid-(currentAge+1))*getTrait(Trait::growth)));
						}
					
					
					double nextReproMass = mySpecies->getCoefficientForMassAforMature()*pow(nextReproLength,mySpecies->getScaleForMassBforMature());	
					double currentMass =mySpecies->getCoefficientForMassAforMature()*pow(currentLength,mySpecies->getScaleForMassBforMature());

					if(mySpecies->hasIndeterminateGrowth()){
						if(instar < mySpecies->getNumberOfInstars()){
							massForNextReproduction = nextReproMass;
						}else{
							massForNextReproduction = currentMass;
						}
						//daysForTargetReproduction = ceil( ((mySpecies->maleReproductionFactor * (massesVector[mySpecies->numberOfInstars-1] - massesVector[mySpecies->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
						daysForTargetReproduction = ceil(0.01);
					}else{
					
					massForNextReproduction = lastMassBeforeMaturationOrOviposition + (nextReproMass-currentMass);
					//daysForTargetReproduction = ceil( ((mySpecies->maleReproductionFactor * (massesVector[mySpecies->numberOfInstars-1] - massesVector[mySpecies->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
					daysForTargetReproduction = ceil(0.01);
					}
					//end arthro
				}

				if(currentAge==round(ageOfFirstMaturation)){//currentAge==ageOfFirstMaturation rep_count==0
				//arthro
				/* ageOfLastMoultOrReproduction = currentAge; //Dinosaurs removed this + ageOfLastMoultOrReproduction
					ageForNextReproduction = currentAge + mySpecies->getPupaPeriodLength();  *///Dinosaurs removed this + ageOfLastMoultOrReproduction + daysForTargetReproduction;
					
/* 					cout << ageForNextReproduction << endl;
					cout << mySpecies->getId() << endl;
					exit(-1); 
 */	
					setNewLifeStage(LIFE_STAGES::PUPA);
					/*cout << "pupaTimer: " << pupaTimer << endl;
					exit(-1);*/		/* + daysForTargetReproduction */
                    ageForNextReproduction = currentAge + mySpecies->getPupaPeriodLength();


				}else{
				//arthro	
				/* ageOfLastMoultOrReproduction = currentAge;
					ageForNextReproduction = currentAge + daysForTargetReproduction; */
					ageForNextReproduction = currentAge + daysForTargetReproduction;
				}

			//arthro
			ageOfLastMoultOrReproduction = currentAge;
			//dinos2023 - to ensure that ageForNextReproduction does not surpass the next instar (year)
			if(instar < mySpecies->getNumberOfInstars()){
				ageForNextReproduction = min(ageForNextReproduction,finalDevTimeVector[instar.getValue()-1]+round(0.9*(finalDevTimeVector[instar.getValue()]-finalDevTimeVector[instar.getValue()-1])));
			}

			//end dinos2023
	

			}
		}
	}
	else //isMature() == true
	{
		//Jordi: the following is to force males to molt whether they mate or not - otherwise they stop growing if the do not mate
		if(gender == AnimalSpecies::GENDERS::MALE){
			if(mySpecies->hasIndeterminateGrowth() && (instar < mySpecies->getNumberOfInstars())){
				if(currentAge > finalDevTimeVector[instar.getValue()-1]+1){//plus one day enough to mate
			    //cout << "rep_count: " << rep_count << endl;
				mature = false;
				}
			}
		}else{
		
		//Dinos2023 Recalculate this here because females that are initialized as adults
		//should use their factorEggMass trait too
		eggDryMassForClutch = mySpecies->getEggDryMass()+mySpecies->getEggDryMass()*getTrait(Trait::factorEggMass);

		double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*mySpecies->getMaxPlasticityKVonBertalanffy();
		//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
		double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * mySpecies->getMinPlasticityKVonBertalanffy();//Dinosaurs

		//below final eggDryMass is set after the female eggMass trait, if it surpasses the
		//growing curve band limits, it is set to the limit  //Dinosaurs
		eggDryMassForClutch = min(eggDryMassForClutch, maxMassAtCurrentAge);
		eggDryMassForClutch = max(eggDryMassForClutch, minMassAtCurrentAge);
		//end Dinos2023

		//Only FEMALES are coupled
		double investmentForReproduction = calculateDryMass();
		////Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;

	/*	if(getId()==2){ // && timeStep == 30
						cout << "timeStep: " << timeStep << endl;
						cout << "investmentForReproduction: " << investmentForReproduction << endl;
						cout << "massForNextReproduction: " << massForNextReproduction << endl;
						cout << "currentAge: " << currentAge << endl;
						cout << "ageForNextReproduction: " << ageForNextReproduction << endl;
		exit(-1);
		}*/

		
		
		/* if(currentAge == ageForNextReproduction){
			
			cout << mySpecies->getId() << endl;
			cout << ageForNextReproduction << endl;

			exit(-1);
		}   */

		   /*  cout << mySpecies->getId() << endl;
			cout << ageForNextReproduction << endl;
			cout << timeStep << endl;
			cout << currentAge << endl;
			cout << investmentForReproduction << endl;
			cout << massForNextReproduction << endl;
			cout << clutchDryMass << endl;
			
			exit(-1);      */
			

		/* cout << currentAge << endl;
		cout << ageForNextReproduction << endl; */
		//exit(-1); */

/* 		if(currentAge == 6387){
		cout << "investmentForReproduction: " << investmentForReproduction << endl;
		cout << "massForNextReproduction: " << massForNextReproduction << endl;
		cout << "currentAge: " << currentAge << endl;
		cout << "ageForNextReproduction: " << ageForNextReproduction << endl;
		exit(-1); 
		}
 */

		
		if(investmentForReproduction >= massForNextReproduction &&  currentAge >= ageForNextReproduction  && mySpecies->getEggsPerBatch()>0)  //Arthropods post-dinos July 2021  && mySpecies->getEggsPerBatch()>0
		{

			
			
			


			/* cout << mySpecies->getId() << endl;
			cout << ageForNextReproduction << endl;
			cout << currentAge << endl;
			exit(-1); */
 
			/*if(getId()==2 && timeStep == 27){
				cout << "investmentForReproduction: " << investmentForReproduction << endl;
				cout << "massForNextReproduction: " << massForNextReproduction << endl;
				cout << "currentAge: " << currentAge << endl;
				cout << "ageForNextReproduction: " << ageForNextReproduction << endl;
			}*/


			double daysForTargetReproduction;

			if(mySpecies->getSexualType() != AnimalSpecies::SEXUAL_TYPES::DIPLOID || (mySpecies->getSexualType() == AnimalSpecies::SEXUAL_TYPES::DIPLOID && isMated()))
			{
				
/* 			if(getId() == 0){ // && timeStep == 0
		    cout << getId() << endl;
			cout << isMature() << endl;
			cout << ageForNextReproduction << endl;
			cout << timeStep << endl;
			cout << currentAge << endl;
			cout << investmentForReproduction << endl;
			cout << massForNextReproduction << endl;
			cout << clutchDryMass << endl;
			cout << lifeStage << endl;
			
			//exit(-1);      
		    }  */
				
				setNewLifeStage(LIFE_STAGES::REPRODUCING);

			/*if(currentAge >= ageForNextReproduction){
			cout << "investmentForReproduction: " << investmentForReproduction << endl;
			cout << "massForNextReproduction: " << massForNextReproduction << endl;
			exit(-1);
		    }*/

				
			
				//double expectedLengthAtNextAge = currentPostTempLinf*(1-exp(-currentPostTempGrowth*(ageOfMaturation+(1.0/timeStepsPerDay)-thisAnimalVonBertTime0)));
				//double expectedMassAtNextAge = mySpecies->coefficientForMassA*pow(expectedLengthAtNextAge,mySpecies->scaleForMassB);
				//double neededEnergyFromCurrentMassToNextAge = expectedMassAtNextAge - getDryMass();
				//double proportionOfTimeTheAnimalWasMoving = 0;
				//double expectedMinimumMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(getDryMass(), proportionOfTimeTheAnimalWasMoving);
				//double minimumPossibleVoracity = expectedMinimumMetabolicDryMassLoss + neededEnergyFromCurrentMassToNextAge;

//arthro and for dinos, why was egg size calcultaed again, is enough with the calculation at maturation
/* 				clutchDryMass = 0;
				double lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
				eggDryMassForClutch = mySpecies->getCoefficientForMassA()*pow(lengthAtBirth,mySpecies->getScaleForMassB())+mySpecies->getCoefficientForMassA()*pow(lengthAtBirth,mySpecies->getScaleForMassB()) * traits[Trait::factorEggMass];

 				if(eggDryMassForClutch < 0){
					cout << "lengthAtBirth2: " << lengthAtBirth << endl;
					cout << "eggDryMassForClutch2: " << eggDryMassForClutch << endl;
					cout << "Linf:" << Linf << endl;
					cout << "Growth:" << traits[Trait::growth] << endl;
					cout << "thisAnimalVonBertTime02: " << thisAnimalVonBertTime0 << endl;
					exit(-1);
				}

				if(lengthAtBirth < 0)
				{
					cout << "Animal id: " << this->getId() << " (" << this->getSpecies()->getScientificName() << ") - The length at birth has been calculated as a negative value. Check growth related variables." << endl;
				}*/


				if(mySpecies->hasEggClutchFromEquation())
				{
					clutchDryMass = (mySpecies->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, mySpecies->getForClutchMassScale())) / mySpecies->getConversionToWetMass();
				}
				else //If it does not come from equation,
				{
						//arthro
						if(mySpecies->hasIndeterminateGrowth()){
		
							double lengthAtThisAge = 0;
							
							if(mySpecies->getGrowthType() == 0){
							  lengthAtThisAge = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
							}

							if(mySpecies->getGrowthType() == 1){
							  lengthAtThisAge = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
							}
						
							double massAtThisAge = mySpecies->getCoefficientForMassAforMature()*pow(lengthAtThisAge,mySpecies->getScaleForMassBforMature());
												
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(mySpecies->getEggsPerBatch()*propAdultMass), mySpecies->getEggsPerBatch()); 
							
							clutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
						}else{
							clutchDryMass = eggDryMassForClutch*mySpecies->getEggsPerBatch();	
						}
						//end arthro
				}

				/*cout << "instar  " << instar+2 << endl;*/
				//if(currentAge == 6387){
 			/* 	cout << "investmentForReproduction: " << investmentForReproduction << endl;
				cout << "massForNextReproduction: " << massForNextReproduction << endl;
				cout << "currentAge: " << currentAge << endl;
				cout << "ageForNextReproduction: " << ageForNextReproduction << endl;

				cout << "currentEggs  " << currentEggs << endl;
				cout << "propAdultMass  " << propAdultMass << endl;
				//exit(-1); 
				//}


				cout << "clutchDryMass  " << clutchDryMass << endl;
				exit(-1); */
 
			
			    
				
				//arthros
				//daysForTargetReproduction = ceil( (clutchDryMass*ageOfMaturation) / lastMassBeforeMaturationOrOviposition);
				double longevity = mySpecies->getLongevitySinceMaturation()*ageOfFirstMaturation;
				double reproTime = longevity - ageOfFirstMaturation;
				daysForTargetReproduction = floor(reproTime/mySpecies->getFemaleMaxReproductionEvents());
				//end arthros

				ageForNextReproduction = currentAge + daysForTargetReproduction;
				ageOfLastMoultOrReproduction = currentAge;

			//dinos2023
			//ageForNextReproduction = ageForNextReproduction //,finalDevTimeVector[instar]+round((finalDevTimeVector[instar+1]-finalDevTimeVector[instar])/2));
			//end dinos2023

				//arthro
				

				double nextClutchDryMass;
				if(mySpecies->hasEggClutchFromEquation())
				{
					nextClutchDryMass = (mySpecies->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, mySpecies->getForClutchMassScale())) / mySpecies->getConversionToWetMass();
				}
				else //If it does not come from equation,
				{
						//arthro
						if(mySpecies->hasIndeterminateGrowth()){
							double lengthAtThisAge = 0;
							
							if(mySpecies->getGrowthType() == 0){
							  lengthAtThisAge = Linf*(1-exp(-getTrait(Trait::growth)*(ageForNextReproduction-thisAnimalVonBertTime0)));
							}

							if(mySpecies->getGrowthType() == 1){
							  lengthAtThisAge = Linf/(1+exp((xmid-ageForNextReproduction)*getTrait(Trait::growth)));
							}

							double massAtThisAge = mySpecies->getCoefficientForMassAforMature()*pow(lengthAtThisAge,mySpecies->getScaleForMassBforMature());
											
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(mySpecies->getEggsPerBatch()*propAdultMass), mySpecies->getEggsPerBatch()); 
							
							nextClutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
							massForNextReproduction = (calculateDryMass()-clutchDryMass) + nextClutchDryMass;	

						}else{
							nextClutchDryMass = eggDryMassForClutch*mySpecies->getEggsPerBatch();	
						    massForNextReproduction = massOfMaturationOrLastReproduction + nextClutchDryMass;	
				
						}
						
				}

				
				
				//end arthro

			}
			else if(currentAge >= ageForNextReproduction)
			{
				//massForNextReproduction = lastMassBeforeMaturationOrOviposition + (mySpecies->maleReproductionFactor * (massesVector[mySpecies->numberOfInstars-1] - massesVector[mySpecies->numberOfInstars-2]));
				//daysForTargetReproduction = ceil( ((mySpecies->maleReproductionFactor * (massesVector[mySpecies->numberOfInstars-1] - massesVector[mySpecies->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
				//daysForTargetReproduction = ceil(0.056*ageOfMaturation);
				//ageForNextReproduction = currentAge + daysForTargetReproduction;
				//ageOfLastMoultOrReproduction = currentAge;
			}
		}
	}
	}
}

bool Animal::isActive()
{
	return lifeStage == ACTIVE;
}

/**
 * Activates the animal if it is ready. In order to activate, an animal needs
 * to be unborn and reach its day of phenology.
 * @param day the current day in the simulation
 */
void Animal::isReadyToBeBorn(int timeStep, int timeStepsPerDay)
{
	//Dinosaurs ini
	double thisCurrentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - getTrait(Trait::pheno) + 1.0/timeStepsPerDay;
	if (thisCurrentAge > 0)
	{
		setNewLifeStage(LIFE_STAGES::ACTIVE);
		currentAge = round(thisCurrentAge);
		ageOfLastMoultOrReproduction = thisCurrentAge;//Dinosaurs end
	}
}

void Animal::isReadyToResumeFromPupaOrDecreasePupaTimer()
{

/*	cout << "pupaTimer:" << pupaTimer << endl;

	exit(-1);*/

	if (pupaTimer == 0)
	{
		setNewLifeStage(LIFE_STAGES::ACTIVE);
	}
	else
	{
		pupaTimer--;
	}
}

void Animal::isReadyToResumeFromHandlingOrDecreaseHandlingTimer()
{
	if (handlingTimer == 0)
	{
		setNewLifeStage(LIFE_STAGES::ACTIVE);
	}
	else
	{
		handlingTimer--;
	}
}

void Animal::isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(float relativeHumidity)
{
	if (relativeHumidity >= mySpecies->getMinRelativeHumidityThreshold())
	{
		if(pupaTimer > 0)
		{
			setNewLifeStage(LIFE_STAGES::PUPA);
		}
		/*
		else if(handlingTimer > 0)
		{
			lifeStage = HANDLING;
		}
		*/
		else
		{
			setNewLifeStage(LIFE_STAGES::ACTIVE);
		}
	}
	else
	{
		diapauseTimeSteps++;
	}
}

double Animal::getNormalizedHuntedVoracity(double huntedVoracity)
{
	return Math_Functions::linearInterpolate(huntedVoracity,mySpecies->getMinVorHunted(),mySpecies->getMaxVorHunted(),0,1); 
	//return ((huntedVoracity - mySpecies->getMeanVorHunted()) / mySpecies->getSdVoracityHunted());
}

double Animal::getNormalizedHunterVoracity()
{
	return Math_Functions::linearInterpolate(getTrait(Trait::voracity),mySpecies->getMinVorHunter(),mySpecies->getMaxVorHunter(),0,1); 
	//return ((traits[Trait::voracity] - mySpecies->getMeanVoracityHunter()) / mySpecies->getSdVoracityHunter());
}

double Animal::getNormalizedVoracityProduct(double huntedVoracity)
{
	
	return Math_Functions::linearInterpolate(getTrait(Trait::voracity)* huntedVoracity,mySpecies->getMinVorXVor(),mySpecies->getMaxVorXVor(),0,1); 
	/*return ((traits[Trait::voracity] * huntedVoracity - mySpecies->getMeanVoracityXVoracity())
			/ mySpecies->getSdVoracityXVoracity());*/
}

double Animal::getNormalizedHuntedBodySize(double huntedBodySize)
{
	return Math_Functions::linearInterpolate(huntedBodySize,mySpecies->getMinSizeHunted(),mySpecies->getMaxSizeHunted(),0,1); 
	//return ((huntedBodySize - mySpecies->getMeanSizeHunted()) / mySpecies->getSdSizeHunted());
}

double Animal::getNormalizedHunterBodySize()
{
	return Math_Functions::linearInterpolate(currentBodySize,mySpecies->getMinSizeHunter(),mySpecies->getMaxSizeHunter(),0,1); 
	//return ((currentBodySize - mySpecies->getMeanSizeHunter()) / mySpecies->getSdSizeHunter());
}

double Animal::getNormalizedPDF(double probabilityDensityFunction)
{
	return Math_Functions::linearInterpolate(probabilityDensityFunction,mySpecies->getMinProbabilityDensityFunction(),mySpecies->getMaxProbabilityDensityFunction(),0,1);
	/*return ((probabilityDensityFunction - mySpecies->getMeanProbabilityDensityFunction())
			/ mySpecies->getSdProbabilityDensityFunction());*/
}

double Animal::getNormalizedSpeedRatio(double huntedSpeed)
{

	return Math_Functions::linearInterpolate(getTrait(Trait::speed)/huntedSpeed,mySpecies->getMinSpeedRatio(),mySpecies->getMaxSpeedRatio(),0,1);
	/*return (((traits[Trait::speed] / huntedSpeed) - mySpecies->getMeanSpeedRatio())
			/ mySpecies->getSdSpeedRatio());*/

}

double calculateLogMassRatio(double hunterAnimalDryMass, double huntedAnimalDryMass)
{
	return log(hunterAnimalDryMass/huntedAnimalDryMass);
}

double Animal::getSearchAreaHunter()
{
	return ((getTrait(Trait::search_area) - mySpecies->getMeanSearchAreaHunter())
			/ mySpecies->getSdSearchAreaHunter());
}

double Animal::getSpeedHunter()
{
	return ((getTrait(Trait::speed) - mySpecies->getMeanSpeedHunter()) / mySpecies->getSdSpeedHunter());
}

void Animal::incrementEncountersWithPredator(int predatorId)
{
	encounterEvents.push_back(predatorId);//the ID of the predator that has encounter gets recorded in a vector
	todayEncountersWithPredators++;
	//totalEncountersWithPredators++;
}

/*void Animal::incrementEncounters_prey()
{
	encounters_prey++;
	total_prey_encs++;
}*/

ostream& Animal::print(ostream& os)
{
	os << getIdStr() << "\t"
	<< mySpecies->getScientificName() << "\t"
	<< gender << "\t"
	<< position->getX() << "\t"
	<< position->getY() << "\t"
	<< position->getZ() << "\t"
	<< lifeStage << "\t"
	<< instar << "\t"
	<< pheno_ini << "\t"
	<< date_egg << "\t"
	<< age_first_rep << "\t"
	<< rep_count << "\t"
	<< fecundity << "\t"
	<< getDateOfDeath() << "\t"
	<< generationNumberFromFemaleParent << "\t"
	<< generationNumberFromMaleParent << "\t"
	<< ID_prt1 << "\t"
	<< ID_prt2 << "\t"
	<< todayEncountersWithPredators << "\t"
	<< encounterEvents.size() << "\t"
	<< days_digest << "\t"
	<< voracity_ini << "\t"
	<< search_area_ini << "\t"
	<< speed_ini << "\t"
	<< tank_ini << "\t"
	<< pheno_ini << "\t"
	<< getCurrentBodySize() << "\t"
	<< calculateDryMass() << "\t";



	for (size_t i = 0; i < TraitConverter::size(); ++i) {
		/*os << "CR" << i << "_1: " << *(chromosomes->at(i).first) << endl;
		os << "CR" << i << "_2: " << *(chromosomes->at(i).second) << endl;
		os << "TRT_" << i << ": " << traits.at(i) << endl;*/
		try
		{
			os << getTrait((Trait)i) << "\t";
		}
		catch(const LineInfoException& e)
		{
			os << -1 << "\t";
		}
	}

	/*
	for (unsigned int i = 0; i < finalDevTimeVector.size(); ++i) {
			os << finalDevTimeVector.at(i) << "\t";
	}
	for (unsigned int i = 0; i < finalDevTimeVector.size(); ++i) {
		os << massesVector[i] << "\t";
	}
	for (unsigned int i = 0; i < actualMoltingTimeVector.size(); ++i) {
		os << actualMoltingTimeVector[i] << "\t";
	}
	for (unsigned int i = actualMoltingTimeVector.size(); i < finalDevTimeVector.size(); ++i) {
			os << "NA" << "\t";
	}
	for (unsigned int i = 0; i < actualMoltingMassVector.size(); ++i) {
		os << actualMoltingMassVector[i] << "\t";
	}
	for (unsigned int i = actualMoltingMassVector.size(); i < finalDevTimeVector.size(); ++i) {
		os << "NA" << "\t";
	}
	*/
	return os;
}

void Animal::printVoracities(int timeStep, int timeStepsPerDay, ostream& voracitiesFile)
{
	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;
	double dryMassAfterAssim = calculateDryMass() + foodMass;
	double wetMassAfterAssim = dryMassAfterAssim * mySpecies->getConversionToWetMass();
	double prop_time_mov;

	double lengthAtBirth = 0;
	double xmid = 0;						 

	if(mySpecies->getGrowthType() == 1){
		lengthAtBirth = pow((getMassAtBirth()/mySpecies->getCoefficientForMassAforMature()),1/mySpecies->getScaleForMassBforMature()); 
		xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
	}

	if(lifeStage == SATIATED || lifeStage == HANDLING)
	{
		prop_time_mov = 0;
	}
	else
	{
		if(maxPostTsearch>0){
		  prop_time_mov = steps/maxPostTsearch;
		}else{
		  prop_time_mov = 0;	
		}
	}
	double totalMetabolicDryMassLossAfterAssim = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, position->temperature, prop_time_mov, timeStepsPerDay);
	if(totalMetabolicDryMassLossAfterAssim)										
	if(huntingMode != Animal::HUNTING_MODES::DOES_NOT_HUNT && daysWithoutFood >= mySpecies->getDaysWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLossAfterAssim -= totalMetabolicDryMassLossAfterAssim * mySpecies->getPercentageMetabolicDownregulation();
	}
	double dryMassAfterMetabLoss = (dryMassAfterAssim - totalMetabolicDryMassLossAfterAssim);

	double expectedLengthAtNextTimeStep = 0;
							
			if(mySpecies->getGrowthType() == 0){
			  expectedLengthAtNextTimeStep = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge+(round((1.0/(double)timeStepsPerDay)))-thisAnimalVonBertTime0)));
			}

			if(mySpecies->getGrowthType() == 1){
			  expectedLengthAtNextTimeStep = Linf/(1+exp((xmid-(currentAge+(round((1.0/(double)timeStepsPerDay)))))*getTrait(Trait::growth)));
			}



	double expectedMassAtNextTimeStep;


    if(instar == mySpecies->getInstarFirstReproduction()){
    	expectedMassAtNextTimeStep = mySpecies->getCoefficientForMassAforMature()*pow(expectedLengthAtNextTimeStep,mySpecies->getScaleForMassBforMature());
    }else{
    	expectedMassAtNextTimeStep = mySpecies->getCoefficientForMassA()*pow(expectedLengthAtNextTimeStep,mySpecies->getScaleForMassB());
	}




	//Dinosaurs
	double diffFromExpectedMassAtNextTimeStep = expectedMassAtNextTimeStep - dryMassAfterMetabLoss;

  	double dinoLengthPredicted = 0;

	if(mySpecies->getGrowthType() == 0){
		if(instar > 1){
			dinoLengthPredicted = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
		}else{  //this is to avoid animals to be born with size of zero age but being much older- yhus need to be larger
			dinoLengthPredicted = Linf*(1-exp(-getTrait(Trait::growth)*(0-thisAnimalVonBertTime0)));	
		}
	}

	if(mySpecies->getGrowthType() == 1){
		if(instar > 1){
			dinoLengthPredicted = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
		}else{  //this is to avoid animals to be born with size of zero age but being much older- yhus need to be larger
			dinoLengthPredicted = Linf/(1+exp((xmid-0)*getTrait(Trait::growth)));
		}
	}

	double dinoMassPredicted = mySpecies->getCoefficientForMassAforMature()*pow(dinoLengthPredicted,mySpecies->getScaleForMassBforMature());

    double minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * mySpecies->getMinPlasticityKVonBertalanffy();

    double nextDinoLengthPredicted = 0;
		
	if(mySpecies->getGrowthType() == 0){
		nextDinoLengthPredicted = Linf*(1-exp(-getTrait(Trait::growth)*((currentAge+1)-thisAnimalVonBertTime0)));
	}

	if(mySpecies->getGrowthType() == 1){
		nextDinoLengthPredicted = Linf/(1+exp((xmid-(currentAge+1))*getTrait(Trait::growth)));
	}

	
	
	//double nextDinoMassPredicted = mySpecies->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,mySpecies->getScaleForMassBforMature());

	
	/*if(getId() == 2 && timeStep == 51){
    			cout << calculateDryMass() << endl;
    			exit(-1);
    		}*/

	voracitiesFile << getIdStr() << "\t"
	<< mySpecies->getScientificName() << "\t"
	<< lifeStage << "\t"
	<< currentAge-1 << "\t" //Dinosaurs - it has been updated in tuneTraits
	<< instar << "\t"
	<< currentBodySize << "\t"
	<< getTrait(Trait::energy_tank) << "\t"
	<< calculateDryMass() << "\t"
	<< nextDinoMassPredicted << "\t" //Dinosaures
	<< minMassAtCurrentAge << "\t"
	<< getTrait(Trait::voracity) << "\t"
	<< foodMass << "\t"
	<< calculateDryMass() + foodMass << "\t"
	<< totalMetabolicDryMassLossAfterAssim << "\t"
	<< eatenToday << "\t"
	<< steps << "\t"
	<< stepsAttempted << "\t"
	<< oldSearchArea << "\t"
	<< sated << "\t"
	<< ((timeStepMaximumSearchArea > 0) ? steps / timeStepMaximumSearchArea : 0) << "\t"
	<< handlingTimer << "\t"
	<< getTrait(Trait::voracity) / calculateDryMass() << "\t"
	<< gender << "\t"
	<< mated << "\t"
	<< eggDryMassForClutch << "\t"
	<< getTrait(Trait::growth) << "\t"
	<< Linf << "\t"
	<< pseudoGrowth << "\t"
	<< Linf << "\t"
	<< getTrait(Trait::factorEggMass) << "\t"
	<< eggDryMassAtBirth << "\t"
	<< getDateOfDeath() << "\t"
	<< ageOfFirstMaturation << "\t"
	<< rep_count << "\t";

	voracitiesFile << endl;

}

void Animal::printGenetics(ostream& geneticsFile)
{
	geneticsFile << getIdStr() << "\t"
	<< mySpecies->getScientificName() << "\t"
	<< generationNumberFromFemaleParent << "\t"
	<< generationNumberFromMaleParent << "\t"
	<< ID_prt1 << "\t"
	<< ID_prt2 << "\t";

	for (unsigned int i = 0; i < mySpecies->getNumberOfTraits(); ++i)
	{
		geneticsFile << *(genome->getHomologousChromosomes()->at(i).first) << *(genome->getHomologousChromosomes()->at(i).second);
	}
	geneticsFile << endl;
}

void Animal::printTraits(ostream& traitsFile)
{
	traitsFile << getIdStr() << "\t"
	<< mySpecies->getScientificName() << "\t"
	<< generationNumberFromFemaleParent << "\t"
	<< generationNumberFromMaleParent << "\t"
	<< ID_prt1 << "\t"
	<< ID_prt2 << "\t";

	for (size_t i = 0; i < TraitConverter::size(); ++i)
	{
		try
		{
			traitsFile << getTrait((Trait)i) << "\t";
		}
		catch(const LineInfoException& e)
		{
			traitsFile << -1 << "\t";
		}
	}
	traitsFile << endl;
}

void Animal::updateLimits()
{
	for(const auto& [key, value] : variableTraits)
	{
		mySpecies->setMinObservedPseudoValue(key, min(mySpecies->getMinObservedPseudoValue(key), getTrait(key)));
		mySpecies->setMaxObservedPseudoValue(key, max(mySpecies->getMaxObservedPseudoValue(key), getTrait(key)));
	}
}

void Animal::sumPseudoGrowthMean()
{
	mySpecies->setPseudoGrowthMean(mySpecies->getPseudoGrowthMean() + pseudoGrowth);
}

void Animal::sumPseudoGrowthSd()
{
	mySpecies->setPseudoGrowthSd(mySpecies->getPseudoGrowthSd() + pow(pseudoGrowth - mySpecies->getPseudoGrowthMean(), 2));
}
/*
void Animal::sumPseudoLengthMean()
{


	//growth cambia según dells a Topt = 35 (usando changedKmin changedKmax etc..)
	double vonBertLinf = linearInterpolate(traits[Trait::growth], mySpecies->minRestrictedLimits[Trait::growth], mySpecies->maxRestrictedLimits[Trait::growth], mySpecies->vonBertLdistanceMin, mySpecies->vonBertLdistanceMax);
	//vonBertLinf disminuye según tempSizeRuleConstant (usando Topt = 35)
	//finalDevTimeVector cambia usando dells (usamos la misma Topt = 35 otra vez)
	double pseudoVonBertMaxLength = vonBertLinf*(1-exp(-traits[Trait::growth]*(finalDevTimeVector[CADAINSTAR]-mySpecies->vonBertTime0)));
	//calculamos mean y sd para pseudoVonBertMaxLength

	double normalizedPseudoVonBertMaxLength = (pseudoVonBertMaxLength - pseudoVonBertMaxLengthMean) / pseudoVonBertMaxLengthSd;
	vonBertMaxLength<-normalizedPseudoGrowth*cholMat[1,2]+normalizedPseudoVonBertMaxLength*cholMat[2,2]

	pseudoGrowth = traits[Trait::growth];
	mySpecies->pseudoGrowthMean += pseudoGrowth;
}

void Animal::sumPseudoLengthSd()
{
	mySpecies->pseudoGrowthSd += pow(pseudoGrowth - mySpecies->pseudoGrowthMean, 2);
}
*/

void Animal::setNewLifeStage(unsigned int newLifeStage)
{
	position->eraseAnimal(this, lifeStage);

	lifeStage = newLifeStage;
	position->addAnimal(this);
}

void Animal::setNewLifeStage(unsigned int newLifeStage, double dayOfDeath)
{
	setNewLifeStage(newLifeStage);

	switch (newLifeStage)
	{
	case LIFE_STAGES::STARVED:
		setDateOfDeath(dayOfDeath);
		break;
	case LIFE_STAGES::PREDATED:
		setDateOfDeath(dayOfDeath);
		break;
	case LIFE_STAGES::BACKGROUND:
		setDateOfDeath(dayOfDeath);
		break;
	case LIFE_STAGES::SENESCED:
		setDateOfDeath(dayOfDeath);
		break;
	case LIFE_STAGES::SHOCKED:
		setDateOfDeath(dayOfDeath);
		break;
	default:
		throwLineInfoException("In the new state the animal does not die");
		break;
	}
}

void Animal::setNewLifeStage(unsigned int newLifeStage, double dayOfDeath, int predatorId)
{
	setNewLifeStage(newLifeStage, dayOfDeath);

	switch (newLifeStage)
	{
	case LIFE_STAGES::PREDATED:
		setPredatedByID(predatorId);
		break;
	default:
		throwLineInfoException("In the new state the animal is not depredated");
		break;
	}
}

pair<bool, bool> Animal::interpolateTraits()
{
	bool isInsideRestrictedRanges = true;
	bool isViableAnimal = true;
	for(const auto& [key, value] : variableTraits)
	{
		variableTraits[key] = Math_Functions::linearInterpolate(variableTraits[key], mySpecies->getMinObservedPseudoValue(key), mySpecies->getMaxObservedPseudoValue(key),
				mySpecies->getMinRestrictedRange(key), mySpecies->getMaxRestrictedRange(key));
		if(variableTraits[key] < mySpecies->getMinRestrictedRange(key) || variableTraits[key] > mySpecies->getMaxRestrictedRange(key))
		{
			//cout << "WARNING: individual " << id << " on species " << mySpecies->getScientificName() << ": trait number " << i << ": " << traits[i] << " out of RESTRICTED RANGES: " << mySpecies->minRestrictedRanges[i] << " - " << mySpecies->maxRestrictedRanges[i] << endl << flush;
			isInsideRestrictedRanges = false;
		}
		if(variableTraits[key] < mySpecies->getMinTraitLimit(key) || variableTraits[key] > mySpecies->getMaxTraitLimit(key))
		{
			cout << "WARNING: individual " << getId() << " on species " << mySpecies->getScientificName() << ": trait number " << key << ": " << variableTraits[key] << " out of LIMITS: " << mySpecies->getMinTraitLimit(key) << " - " << mySpecies->getMaxTraitLimit(key) << endl << flush;
			isViableAnimal = false;
		}
	}
	return make_pair(isInsideRestrictedRanges, isViableAnimal);
}

void Animal::calculateGrowthCurves(float temperature, ostream& tuneTraitsFile, bool printGrowthData, double ageAtInitialization) //Dinosaurs
{
	
	double lengthAtBirth = 0;
	double xmid = 0;						 

	if(mySpecies->getGrowthType() == 1){
		lengthAtBirth = pow((getMassAtBirth()/mySpecies->getCoefficientForMassAforMature()),1/mySpecies->getScaleForMassBforMature()); 
		xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
	}

	//vonBertK is now the actual growth trait

	//Calculating Linf and changing growth curves according to LinfKcorr
	//Cholesky has been called AFTER the creation of each species and the matrix has been saved as cholMat
	//The creation of the cholMat is performed inside the setLinfKcorr function

	//Using Change_K (and Dells) for tuning Growth according to temperature
	//double tmin = 270;
	double tmax = 273 + mySpecies->getTempFromLab();
	/*double newT = 273 + temperature;
	//Optimo para las curvas de crecimiento=35. Para Vor Speed Search será 25
	double Topt = 273 + mySpecies->getTempOptGrowth();
	double activationEnergyGrowth = 0.65;//changed from 0.66 now following Dell, perhaps take it out as an entry parameter, but since is growth assume MTE

	double dellsNewT = dells(newT, Topt, mySpecies->getEdGrowth(), activationEnergyGrowth);
	double changeKmin = mySpecies->getMinTraitRange(Trait::growth)-0.000001;
	double changedKmin = linearInterpolate(dellsNewT, dells(xmin, Topt, mySpecies->getEdGrowth(), activationEnergyGrowth), dells(xmax, Topt, mySpecies->getEdGrowth(), activationEnergyGrowth), changeKmin, mySpecies->getMinTraitRange(Trait::growth));
	double changedKmax = linearInterpolate(dellsNewT, dells(xmin, Topt, mySpecies->getEdGrowth(), activationEnergyGrowth), dells(xmax, Topt, mySpecies->getEdGrowth(), activationEnergyGrowth), changeKmin, mySpecies->getMaxTraitRange(Trait::growth));
*/
	//This equals K<-interpol(Knorm, Kmin, Kmax)
	//currentPostTempGrowth = linearInterpolate(traits[Trait::growth], mySpecies->getMinTraitRange(Trait::growth), mySpecies->getMaxTraitRange(Trait::growth), changedKmin, changedKmax);

	//Decrease in Linf according to TempSizeRuleConstant
	double thisAnimalTempSizeRuleConstant = mySpecies->getTempSizeRuleConstant();
	double degreesDifference = abs(temperature - mySpecies->getTempFromLab());
	if(temperature > mySpecies->getTempFromLab())
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant);
	}

	//Here the TSR rule is improved by directly affecting asymptotic body mass, not length.
	/*double preTsrLinfMass = mySpecies->getCoefficientForMassAforMature()*pow(Linf,mySpecies->getScaleForMassBforMature());
	double postTsrLinfMass = preTsrLinfMass + thisAnimalTempSizeRuleConstant*degreesDifference*preTsrLinfMass;

	currentPostTempLinf = pow(postTsrLinfMass/mySpecies->getCoefficientForMassAforMature(),1/mySpecies->getScaleForMassBforMature());*/

	if(printGrowthData)
	{
		tuneTraitsFile
		<< getTrait(Trait::growth) << "\t"
		<< Linf << "\t"; //<< currentPostTempLinf << "\t";
	}

    //Here to calculate the new dev time we need to calculate the mass of the adult after TSR has been applied
    double ageLastInstar = getAgeLastInstar();

	
    double maxLfromSpeciesLastInstar = 0;

	if(mySpecies->getGrowthType() == 0){
		maxLfromSpeciesLastInstar = Linf*(1-exp(-getTrait(Trait::growth)*(ageLastInstar-thisAnimalVonBertTime0)));
	}

	if(mySpecies->getGrowthType() == 1){
		maxLfromSpeciesLastInstar = Linf/(1+exp((xmid-ageLastInstar)*getTrait(Trait::growth)));
	}

    double massLastInstarForDevT = mySpecies->getCoefficientForMassAforMature()*pow(maxLfromSpeciesLastInstar,mySpecies->getScaleForMassBforMature());

    double postTSRMassLastInstarForDevT = massLastInstarForDevT + thisAnimalTempSizeRuleConstant*degreesDifference*massLastInstarForDevT;

    //transform to g in wetMass to adjust to the equation by Gillooly et al. 2012
    double massLastInstarForDevTinG = mySpecies->getConversionToWetMass() * postTSRMassLastInstarForDevT * 1000;

    double devTime = exp(-0.11 * (temperature / (1 + temperature / 273)) + mySpecies->getDevInter()) * pow(massLastInstarForDevTinG, 0.25);

	//double finalDevTimeProportion = devTime / ageLastInstar;

	double finalDevTimeProportion = 1; //Dinosaurs

	//Fixing t0 in BV
	//Original length without
	//Jordi: Are the three sentences below necessary anymore?
//	double maxLfromSpeciesMatureAge = Linf*(1-exp(-traits[Trait::growth]*(matureAge-mySpecies->getVonBertTime0())));
//	double thisAnimalMaxLfromSpeciesMatureAge = maxLfromSpeciesMatureAge + thisAnimalTempSizeRuleConstant*degreesDifference*maxLfromSpeciesMatureAge;
//	double thisAnimalMatureAge = matureAge*finalDevTimeProportion;

	//below we anchor VBt0 regardless of temperature - after a detailed study across Taxa by Jordi
	thisAnimalVonBertTime0 = mySpecies->getVonBertTime0();

	if(eggDryMassAtBirth == -1)//for the new borns
	{
		
		//double lengthAtBirth = 0;
		//Only for initialization (value -1). The new offsprings will have factorEggMassFromMom correctly set as factorEggMass(FROM MOM).
		if(factorEggMassFromMom == -1)
		{
			factorEggMassFromMom = getTrait(Trait::factorEggMass);
		}

	/* 		if(ageAtInitialization < 1){ //Dinosaurs
			lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
			}else{
			lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(ageAtInitialization-thisAnimalVonBertTime0)));	
			}

		}else{ //newborn during simulation */

		//lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
		
		//}


		//eggDryMassAtBirth = mySpecies->getCoefficientForMassA()*pow(lengthAtBirth,mySpecies->getScaleForMassB());
		eggDryMassAtBirth = mySpecies->getEggDryMass() + mySpecies->getEggDryMass() * factorEggMassFromMom;
		//traits[Trait::energy_tank] =  tank_ini * pow(eggDryMassAtBirth, mySpecies->getBetaScaleTank());
		//currentBodySize = eggDryMassAtBirth - traits[Trait::energy_tank];

		
		double maxMassAtCurrentAge = eggDryMassAtBirth + eggDryMassAtBirth*mySpecies->getMaxPlasticityKVonBertalanffy();
		//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
		double minMassAtCurrentAge = eggDryMassAtBirth - eggDryMassAtBirth * mySpecies->getMinPlasticityKVonBertalanffy();//Dinosaurs

		//below final eggDryMass is set after the female eggMass trait, if it surpasses the
		//growing curve band limits, it is set to the limit  //Dinosaurs
		eggDryMassAtBirth = min(eggDryMassAtBirth, maxMassAtCurrentAge);
		eggDryMassAtBirth = max(eggDryMassAtBirth, minMassAtCurrentAge);

		setTrait(Trait::energy_tank, tank_ini * pow(eggDryMassAtBirth, mySpecies->getBetaScaleTank()));
		currentBodySize = eggDryMassAtBirth - getTrait(Trait::energy_tank);
	}


	//Now we calculate the vector of Ls using Linf and von Bertalanfy function on each age
	//And we use each L as the next_L in the growth equations to calculate the next_M


/* 	lengthsVector = new double[mySpecies->getNumberOfInstars()]; //Dinosaurs take out of the heap... better memory?
	massesVector = new double[mySpecies->getNumberOfInstars()];
 */
	for (int i = 0; i < finalDevTimeVector.size(); ++i)
	{
		Instar instar(i+1);
		finalDevTimeVector[i] = mySpecies->getDevTime(instar);
		
		if(mySpecies->getGrowthType() == 0){
			lengthsVector[i] = Linf*(1-exp(-getTrait(Trait::growth)*(finalDevTimeVector[i]-thisAnimalVonBertTime0)));
		}

		if(mySpecies->getGrowthType() == 1){
			lengthsVector[i] = Linf/(1+exp((xmid-finalDevTimeVector[i])*getTrait(Trait::growth)));
		}


		if(mySpecies->getNumberOfInstars() == mySpecies->getInstarFirstReproduction()){
			massesVector[i] = mySpecies->getCoefficientForMassAforMature()*pow(lengthsVector[i],mySpecies->getScaleForMassBforMature());
		}else{
			massesVector[i] = mySpecies->getCoefficientForMassA()*pow(lengthsVector[i],mySpecies->getScaleForMassB());
		}

		//TSR is applied directly here so the k value is effectively changed at the individual level
		massesVector[i] = massesVector[i] + massesVector[i]*thisAnimalTempSizeRuleConstant*degreesDifference;

	}

	for (int i = 0; i < finalDevTimeVector.size(); ++i)
	{
		Instar instar(i+1);
		finalDevTimeVector[i] = mySpecies->getDevTime(instar)*finalDevTimeProportion;

		// cout << "finalDevTimeVector[i] :"<< finalDevTimeVector[i] << endl;
		// cout << "lengthsVector[i] :" << lengthsVector[i] << endl;
		// cout << "massesVector[i] :" << massesVector[i] << endl;

	}

		//arthro
		//ageOfFirstMaturation = finalDevTimeVector[mySpecies->getInstarFirstReproduction()-2];

					//if(mySpecies->getGrowthType() == 0){

						ageOfFirstMaturation = getAgeFirstReproduction();

					//}

					/*if(mySpecies->getGrowthType() == 1){

						ageOfFirstMaturation = xmid;
					
					}*/


		//end arthro

//exit(-1);
}

//The function below is to grow the animals that need to be at a certain instar at the beginning of the simulation
float Animal::forceMolting(int timeStepsPerDay, double ageAtInitialization, Instar instarAtInitialization)
{
	const vector<float> temperatureCycle = position->getTemperatureCycle();
	int temperatureCycleDay = Random::randomIndex(temperatureCycle.size());
	float temperatureCycleTemperature = temperatureCycle.at(temperatureCycleDay);
	ofstream noStream;
	calculateGrowthCurves(temperatureCycleTemperature, noStream, false, -1);//Dinosaurs

	double lengthAtBirth = 0;
	double xmid = 0;				
	double propAdultMass = 0;
	double currentEggs = 0;		 

	if(mySpecies->getGrowthType() == 1){
		lengthAtBirth = pow((getMassAtBirth()/mySpecies->getCoefficientForMassAforMature()),1/mySpecies->getScaleForMassBforMature()); 
		xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
	}


	if(ageAtInitialization == -1)
	{
		ageAtInitialization = 0;
		if(instarAtInitialization > 1)
		{
			ageAtInitialization = getAgeInInstar(instarAtInitialization);
		}
	}

	eggDryMassForClutch = 0.0;

	if(ageAtInitialization < finalDevTimeVector[0])
	{
		instar = Instar();
	}
	else
	{
		instar = Instar(mySpecies->getNumberOfInstars());

		actualMoltingTimeVector.push_back(getAgeInInstar(instar));
		actualMoltingMassVector.push_back(getMassesInInstar(instar));

		while(ageAtInitialization < getAgeInInstar(instar))
		{
			instar.moveOnPreviousInstar();

			actualMoltingTimeVector.push_back(getAgeInInstar(instar));
			actualMoltingMassVector.push_back(getMassesInInstar(instar));
		}
	}

	//int maturationInstar = mySpecies->getInstarFirstReproduction();
	//ageOfMaturation = finalDevTimeVector[maturationInstar];
	//cout << maturationInstar << endl;

	//La edad del animal en negativo para que los calculos de tuneTraits sean correctos
	date_egg = -ageAtInitialization - getTrait(Trait::pheno);
	setTrait(Trait::pheno, -ageAtInitialization);

	ageOfLastMoultOrReproduction = 0;
	if(instar > 1)
	{
		ageOfLastMoultOrReproduction = getAgeInInstar(instar);
	}

/* cout << instar+2 << endl;
cout << mySpecies->getInstarFirstReproduction() << endl;
exit(-1);
 */


	/* if(mySpecies->hasIndeterminateGrowth() && (instar+2 > mySpecies->getInstarFirstReproduction())){

	 if((mySpecies->getInstarFirstReproduction() % 2 == 0) &&
	  ((instar+2 - mySpecies->getInstarFirstReproduction()) % 2 == 0)){ //both even

		mature = true;

	 }else{ //at least one is odd

	  if((mySpecies->getInstarFirstReproduction() % 2 > 0) &&
	   ((instar+2 - mySpecies->getInstarFirstReproduction()) % 2 == 0)){ //second even

		mature = true;

	  }

	 }


	}else{ */

		//(instar+2 == mySpecies->getInstarFirstReproduction()) ||
		if((instar >= mySpecies->getInstarFirstReproduction()) || (instar == mySpecies->getNumberOfInstars())){
			mature = true;
		}

	//}

	if(isMature()){
	
	
	  //if(mySpecies->getGrowthType() == 0){
		
		if(instar == mySpecies->getInstarFirstReproduction()){
			ageOfMaturation = ageAtInitialization;
			ageOfFirstMaturation = getAgeFirstReproduction(); //Dinos2023 - to prevent dates to close to target next maturation
		}else{
			ageOfMaturation = ageAtInitialization;
			ageOfFirstMaturation = getAgeFirstReproduction();
		}
	 // }

	  /*if(mySpecies->getGrowthType() == 1){
		
		if(ageAtInitialization >= xmid){
							ageOfMaturation = xmid;
							ageOfFirstMaturation = xmid;
		}else{
							ageOfMaturation = 0;
							ageOfFirstMaturation = 0;
		}

	  }*/

	
	
	
	}

	//Decrease in Linf according to TempSizeRuleConstant
	double thisAnimalTempSizeRuleConstant = mySpecies->getTempSizeRuleConstant();
	double degreesDifference = abs(temperatureCycleTemperature - mySpecies->getTempFromLab());
	if(temperatureCycleTemperature > mySpecies->getTempFromLab())
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant);
	}


	if(!isMature()){



		double expectedLengthAtCurrentAge = 0;
		
		if(mySpecies->getGrowthType() == 0){
			expectedLengthAtCurrentAge = Linf*(1-exp(-getTrait(Trait::growth)*(ageAtInitialization-thisAnimalVonBertTime0)));
	    }

	    if(mySpecies->getGrowthType() == 1){
			expectedLengthAtCurrentAge = Linf/(1+exp((xmid-ageAtInitialization)*getTrait(Trait::growth)));
	    }

		
		double expectedMassAtCurrentAge = mySpecies->getCoefficientForMassA()*pow(expectedLengthAtCurrentAge,mySpecies->getScaleForMassB());

		expectedMassAtCurrentAge = expectedMassAtCurrentAge + expectedMassAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;

		setTrait(Trait::energy_tank, tank_ini * pow(expectedMassAtCurrentAge, mySpecies->getBetaScaleTank()));
		currentBodySize = expectedMassAtCurrentAge - getTrait(Trait::energy_tank);

		//to force initialization in juveniles 
		//to force initialization in juveniles - arthro - the values are dummy and won't be used unitl updated 
		double daysForPseudoTargetReproduction;
		daysForPseudoTargetReproduction = 1.1*currentAge; //1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = currentAge + daysForPseudoTargetReproduction;
		
		double currentLength = 0;
		double nextReproLength = 0;
		
		if(mySpecies->getGrowthType() == 0){
			currentLength = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
			nextReproLength = Linf*(1-exp(-getTrait(Trait::growth)*(ageForNextReproduction-thisAnimalVonBertTime0)));
	    }

	    if(mySpecies->getGrowthType() == 1){
			currentLength = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
			nextReproLength = Linf/(1+exp((xmid-ageForNextReproduction)*getTrait(Trait::growth)));
	    }
		
		massOfMaturationOrLastReproduction = mySpecies->getCoefficientForMassAforMature()*pow(currentLength,mySpecies->getScaleForMassBforMature());
		massForNextReproduction = mySpecies->getCoefficientForMassAforMature()*pow(nextReproLength,mySpecies->getScaleForMassBforMature());
		//end arthro

	}else{//is mature


		double expectedLengthAtCurrentAge = 0;
		
		if(mySpecies->getGrowthType() == 0){
			expectedLengthAtCurrentAge = Linf*(1-exp(-getTrait(Trait::growth)*(ageAtInitialization-thisAnimalVonBertTime0)));
	    }

	    if(mySpecies->getGrowthType() == 1){
			expectedLengthAtCurrentAge = Linf/(1+exp((xmid-ageAtInitialization)*getTrait(Trait::growth)));
	    }

		double expectedMassAtCurrentAge = mySpecies->getCoefficientForMassAforMature()*pow(expectedLengthAtCurrentAge,mySpecies->getScaleForMassBforMature());

		expectedMassAtCurrentAge = expectedMassAtCurrentAge + expectedMassAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;

		setTrait(Trait::energy_tank, tank_ini * pow(expectedMassAtCurrentAge, mySpecies->getBetaScaleTank()));
		currentBodySize = expectedMassAtCurrentAge - getTrait(Trait::energy_tank);


		//TODO ADD HERE ADJUSTMENTS FOR ENERGY TANK WHEN FORCING ADULTS!!
		//Dinosaurs double currentAge = -traits[Trait::pheno] + 1.0/timeStepsPerDay;
		massOfMaturationOrLastReproduction = getMassesInInstar(instar);
		ageOfMaturation = getAgeInInstar(instar);
		lastMassBeforeMaturationOrOviposition = getMassesInInstar(instar);
		double daysForTargetReproduction;
		if(gender == AnimalSpecies::GENDERS::FEMALE)
		{
			clutchDryMass=0;//Dinosaurs
			//double lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
			eggDryMassForClutch = mySpecies->getEggDryMass() + mySpecies->getEggDryMass()* getTrait(Trait::factorEggMass);

			double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*mySpecies->getMaxPlasticityKVonBertalanffy();
					//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
			double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * mySpecies->getMinPlasticityKVonBertalanffy();//Dinosaurs

					//below final eggDryMass is set after the female eggMass trait, if it surpasses the
					//growing curve band limits, it is set to the limit  //Dinosaurs
			eggDryMassForClutch = min(eggDryMassForClutch, maxMassAtCurrentAge);
			eggDryMassForClutch = max(eggDryMassForClutch, minMassAtCurrentAge);
			
			
			if(mySpecies->hasEggClutchFromEquation())
			{
				clutchDryMass = (mySpecies->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, mySpecies->getForClutchMassScale())) / mySpecies->getConversionToWetMass();
			}
			else //If it does not come from equation,
			{
						//arthro
						if(mySpecies->hasIndeterminateGrowth()){
							
							double lengthAtThisAge = 0;
							
							if(mySpecies->getGrowthType() == 0){
								lengthAtThisAge = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
							}

							if(mySpecies->getGrowthType() == 1){
								lengthAtThisAge = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
							}

							
							double massAtThisAge = mySpecies->getCoefficientForMassAforMature()*pow(lengthAtThisAge,mySpecies->getScaleForMassBforMature());
						
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(mySpecies->getEggsPerBatch()*propAdultMass), mySpecies->getEggsPerBatch()); 
							
							clutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
						}else{
							clutchDryMass = eggDryMassForClutch*mySpecies->getEggsPerBatch();	
						}
						//end arthro
			}
			massForNextReproduction = lastMassBeforeMaturationOrOviposition + clutchDryMass;
			//arthros
			//daysForTargetReproduction = ceil( (clutchDryMass*ageOfMaturation) / lastMassBeforeMaturationOrOviposition);
			double longevity = mySpecies->getLongevitySinceMaturation()*ageOfFirstMaturation;
			double reproTime = longevity - ageOfFirstMaturation;
				
			daysForTargetReproduction = floor(reproTime/mySpecies->getFemaleMaxReproductionEvents());
			//end arthros
			
			

			//daysForTargetReproduction = ceil(mySpecies->getReproTimeFactor()*ageOfFirstMaturation);
		}
		else
		{
		  	//arthro - to make targets more continuous also for dinos
			double currentLength = 0;
			double nextReproLength = 0;
			
			if(mySpecies->getGrowthType() == 0){
				currentLength = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
				nextReproLength = Linf*(1-exp(-getTrait(Trait::growth)*(ageForNextReproduction-thisAnimalVonBertTime0)));
			}

			if(mySpecies->getGrowthType() == 1){
				currentLength = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
				nextReproLength = Linf/(1+exp((xmid-ageForNextReproduction)*getTrait(Trait::growth)));
			}
    		
			double nextReproMass = mySpecies->getCoefficientForMassAforMature()*pow(nextReproLength,mySpecies->getScaleForMassBforMature());	
			double currentMass = mySpecies->getCoefficientForMassAforMature()*pow(currentLength,mySpecies->getScaleForMassBforMature());

			if(mySpecies->hasIndeterminateGrowth()){
				if(instar < mySpecies->getNumberOfInstars()){
					massForNextReproduction = nextReproMass;
				}else{
					massForNextReproduction = currentMass;
				}
				//daysForTargetReproduction = ceil( ((mySpecies->maleReproductionFactor * (massesVector[mySpecies->numberOfInstars-1] - massesVector[mySpecies->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
				daysForTargetReproduction = ceil(0.01);
			}else{
				massForNextReproduction = lastMassBeforeMaturationOrOviposition + (nextReproMass-currentMass);
				//daysForTargetReproduction = ceil( ((mySpecies->maleReproductionFactor * (massesVector[mySpecies->numberOfInstars-1] - massesVector[mySpecies->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
				daysForTargetReproduction = ceil(0.01);
			}
			//end arthro
		}

		
		ageOfLastMoultOrReproduction = getAgeInInstar(instar);
		//arthro
		ageForNextReproduction = ageAtInitialization + daysForTargetReproduction;
		//end arthro
		double daysActiveSinceMaturation = round(Random::randomUniform()*daysForTargetReproduction);
		//expectedMassAtCurrentAge = massOfMaturationOrLastReproduction; // + daysActiveSinceMaturation * ((massForNextReproduction - massOfMaturationOrLastReproduction) / (ageForNextReproduction - ageOfLastMoultOrReproduction));
		//traits[Trait::energy_tank] = expectedMassAtCurrentAge - currentBodySize;

/*		if(getId() == 23){
			cout << "ageOfLastMoultOrReproduction: " << ageOfLastMoultOrReproduction << endl;
			cout << "ageAtInitialization: " << ageAtInitialization << endl;
			cout << "daysForTargetReproduction: " << daysForTargetReproduction << endl;
	    	cout << "hello: " << traits[Trait::energy_tank] << endl;
	    }*/


		ofstream noStream2;
		tuneTraits(-1, 1, temperatureCycleTemperature, 100, noStream2, false, true);
	}


	return temperatureCycleTemperature;
}



//The function below is to grow the animals that need to be at a certain instar at the beginning of the simulation
float Animal::forceMolting2(int timeStepsPerDay, double ageAtInitialization, Instar instarAtInitialization)
{
	const vector<float> temperatureCycle = position->getTemperatureCycle();
	int temperatureCycleDay = Random::randomIndex(temperatureCycle.size());
	float temperatureCycleTemperature = temperatureCycle.at(temperatureCycleDay);
	ofstream noStream;

	double lengthAtBirth = 0;
	double xmid = 0;					
	double propAdultMass = 0;
	double currentEggs = 0;		 	 

	if(mySpecies->getGrowthType() == 1){
		lengthAtBirth = pow((getMassAtBirth()/mySpecies->getCoefficientForMassAforMature()),1/mySpecies->getScaleForMassBforMature());  
		xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
	}

	for (int i = 0; i < mySpecies->getDevTimeVector().size(); ++i)  //Dinosaurs for calculteGrowthCurves with ageAtInitialization != 0
	{
		Instar instar(i+1);
		finalDevTimeVector[i] = mySpecies->getDevTime(instar);
	}

	if(ageAtInitialization == -1)
	{
		ageAtInitialization = 0;
		if(instarAtInitialization > 1)
		{
			ageAtInitialization = getAgeInInstar(instarAtInitialization);
		}
	}

	eggDryMassForClutch = 0.0;

	if(ageAtInitialization < finalDevTimeVector[0])
	{
		instar = Instar();
	}
	else
	{
		instar = Instar(mySpecies->getNumberOfInstars());

		actualMoltingTimeVector.push_back(getAgeInInstar(instar));
		actualMoltingMassVector.push_back(getMassesInInstar(instar));

		while(ageAtInitialization < getAgeInInstar(instar))
		{
			instar.moveOnPreviousInstar();

			actualMoltingTimeVector.push_back(getAgeInInstar(instar));
			actualMoltingMassVector.push_back(getMassesInInstar(instar));
		}
	}

	
	if(instar == 1){ //Dinosaurs and arthro - fixed for short-lifecycle mites - initialization is now done at random ages within the instar to generate reproduction variability
		
		ageAtInitialization = Random::randomIndex(finalDevTimeVector[0]);
	
	}else if((instar > 1) && (instar < mySpecies->getNumberOfInstars())){
	
		ageAtInitialization = ageAtInitialization + Random::randomIndex(finalDevTimeVector[instar.getValue()]-getAgeInInstar(instar));	
	    
	} else { //is the last instar
		//age at initialization of animals in the last instar depends on longevity
		
		ageAtInitialization = ageAtInitialization+Random::randomIndex((getAgeFirstReproduction()*mySpecies->getLongevitySinceMaturation())-ageAtInitialization);
	} 
	
	calculateGrowthCurves(temperatureCycleTemperature, noStream, false, ageAtInitialization); //Dinosaurs
	
	currentAge = ageAtInitialization; //Dinosaurs
	//int maturationInstar = mySpecies->getInstarFirstReproduction();
	//ageOfMaturation = finalDevTimeVector[maturationInstar];
	//cout << maturationInstar << endl;

	//La edad del animal en negativo para que los calculos de tuneTraits sean correctos
	date_egg = -ageAtInitialization - getTrait(Trait::pheno);
	setTrait(Trait::pheno, -ageAtInitialization);

	ageOfLastMoultOrReproduction = 0;
	if(instar > 1)
	{
		ageOfLastMoultOrReproduction = getAgeInInstar(instar);
	}

/* cout << instar+2 << endl;
cout << mySpecies->getInstarFirstReproduction() << endl;
exit(-1);
 */
	//if(mySpecies->hasIndeterminateGrowth() && (instar+2 >= mySpecies->getInstarFirstReproduction())){

	 //if((mySpecies->getInstarFirstReproduction() % 2 == 0) &&
	 // ((instar+2 - mySpecies->getInstarFirstReproduction()) % 2 == 0)){ //both even

		//mature = true;

	 //}else{ //at least one is odd

	  //if((mySpecies->getInstarFirstReproduction() % 2 > 0) &&
	   //((instar+2 - mySpecies->getInstarFirstReproduction()) % 2 == 0)){ //second even

		//mature = true;

	  //}/////*else{

		 ////if((mySpecies->getInstarFirstReproduction() % 2 > 0) &&
		 ////(instar+2 == mySpecies->getNumberOfInstars()+1)){ //first is odd

			 ////mature = true;

		 ////}
	  /////}*/
	  //mature = true;

	 //}


	//}else{

		//(instar+2 == mySpecies->getInstarFirstReproduction()) ||
		if((instar >= mySpecies->getInstarFirstReproduction()) || (instar == mySpecies->getNumberOfInstars())){
			mature = true;
		}

	//}

	if(isMature()){
	 // if(mySpecies->getGrowthType() == 0){
		
		if(instar == mySpecies->getInstarFirstReproduction()){
			ageOfMaturation = ageAtInitialization;
			ageOfFirstMaturation = getAgeFirstReproduction(); //Dinos2023 - to prevent dates to close to target next maturation
		}else{
			ageOfMaturation = ageAtInitialization;
			ageOfFirstMaturation = getAgeFirstReproduction();
		}
	  //}

	/* if(mySpecies->getGrowthType() == 1){
		
		if(ageAtInitialization >= xmid){
							ageOfMaturation = xmid;
							ageOfFirstMaturation = xmid;
		}else{
							ageOfMaturation = 0;
							ageOfFirstMaturation = 0;
		}

	  }*/
	}

	//Decrease in Linf according to TempSizeRuleConstant
	double thisAnimalTempSizeRuleConstant = mySpecies->getTempSizeRuleConstant();
	double degreesDifference = abs(temperatureCycleTemperature - mySpecies->getTempFromLab());
	if(temperatureCycleTemperature > mySpecies->getTempFromLab())
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant);
	}


	if(!isMature()){



		double expectedLengthAtCurrentAge = 0;
		
		if(mySpecies->getGrowthType() == 0){
			expectedLengthAtCurrentAge = Linf*(1-exp(-getTrait(Trait::growth)*(ageAtInitialization-thisAnimalVonBertTime0)));
	    }

	    if(mySpecies->getGrowthType() == 1){
			expectedLengthAtCurrentAge = Linf/(1+exp((xmid-ageAtInitialization)*getTrait(Trait::growth)));
	    }

		double expectedMassAtCurrentAge = mySpecies->getCoefficientForMassA()*pow(expectedLengthAtCurrentAge,mySpecies->getScaleForMassB());

		expectedMassAtCurrentAge = expectedMassAtCurrentAge + expectedMassAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;

		setTrait(Trait::energy_tank, tank_ini * pow(expectedMassAtCurrentAge, mySpecies->getBetaScaleTank()));
		currentBodySize = expectedMassAtCurrentAge - getTrait(Trait::energy_tank);

		//to force initialization in juveniles - arthro - the values are dummy and won't be used unitl updated 
		double daysForPseudoTargetReproduction;
		daysForPseudoTargetReproduction = 1.1*currentAge; //1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = currentAge + daysForPseudoTargetReproduction;
		double currentLength = 0;
		double nextReproLength = 0;
		
		if(mySpecies->getGrowthType() == 0){
			currentLength = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
			nextReproLength = Linf*(1-exp(-getTrait(Trait::growth)*(ageForNextReproduction-thisAnimalVonBertTime0)));
	    }

	    if(mySpecies->getGrowthType() == 1){
			currentLength = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
			nextReproLength = Linf/(1+exp((xmid-ageForNextReproduction)*getTrait(Trait::growth)));
	    }

		massOfMaturationOrLastReproduction = mySpecies->getCoefficientForMassAforMature()*pow(currentLength,mySpecies->getScaleForMassBforMature());
		massForNextReproduction = mySpecies->getCoefficientForMassAforMature()*pow(nextReproLength,mySpecies->getScaleForMassBforMature());
		//end arthro

	}else{//is mature


		double expectedLengthAtCurrentAge = 0;
		
		if(mySpecies->getGrowthType() == 0){
			expectedLengthAtCurrentAge = Linf*(1-exp(-getTrait(Trait::growth)*(ageAtInitialization-thisAnimalVonBertTime0)));
	    }

	    if(mySpecies->getGrowthType() == 1){
			expectedLengthAtCurrentAge = Linf/(1+exp((xmid-ageAtInitialization)*getTrait(Trait::growth)));
	    }

		double expectedMassAtCurrentAge = mySpecies->getCoefficientForMassAforMature()*pow(expectedLengthAtCurrentAge,mySpecies->getScaleForMassBforMature());

		expectedMassAtCurrentAge = expectedMassAtCurrentAge + expectedMassAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;

		setTrait(Trait::energy_tank, tank_ini * pow(expectedMassAtCurrentAge, mySpecies->getBetaScaleTank()));
		currentBodySize = expectedMassAtCurrentAge - getTrait(Trait::energy_tank);


		//TODO ADD HERE ADJUSTMENTS FOR ENERGY TANK WHEN FORCING ADULTS!!
		//Dinosaurs double currentAge = -traits[Trait::pheno] + 1.0/timeStepsPerDay;
		
		
		double expectedLengthAtFormerAge = 0;

		if(ageAtInitialization > 0){
			if(mySpecies->getGrowthType() == 0){
				expectedLengthAtFormerAge = Linf*(1-exp(-getTrait(Trait::growth)*((ageAtInitialization-1)-thisAnimalVonBertTime0)));
			}

			if(mySpecies->getGrowthType() == 1){
				expectedLengthAtFormerAge = Linf/(1+exp((xmid-(ageAtInitialization-1))*getTrait(Trait::growth)));
			}
	    }else{
			if(mySpecies->getGrowthType() == 0){
				expectedLengthAtFormerAge = Linf*(1-exp(-getTrait(Trait::growth)*(0-thisAnimalVonBertTime0)));
			}

			if(mySpecies->getGrowthType() == 1){
				expectedLengthAtFormerAge = Linf/(1+exp((xmid-0)*getTrait(Trait::growth)));
			}
		}



		double expectedMassAtFomerAge = mySpecies->getCoefficientForMassAforMature()*pow(expectedLengthAtFormerAge,mySpecies->getScaleForMassBforMature());
		
		massOfMaturationOrLastReproduction = getMassesInInstar(instar);
		ageOfMaturation = getAgeInInstar(instar);
		lastMassBeforeMaturationOrOviposition = getMassesInInstar(instar);
		double daysForTargetReproduction;
		if(gender == AnimalSpecies::GENDERS::FEMALE)
		{
			clutchDryMass=0;//Dinosaurs
			//double lengthAtBirth = Linf*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
			eggDryMassForClutch = mySpecies->getEggDryMass() + mySpecies->getEggDryMass()* getTrait(Trait::factorEggMass);

			double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*mySpecies->getMaxPlasticityKVonBertalanffy();
			//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
			double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * mySpecies->getMinPlasticityKVonBertalanffy();//Dinosaurs

			//below final eggDryMass is set after the female eggMass trait, if it surpasses the
			//growing curve band limits, it is set to the limit  //Dinosaurs
			eggDryMassForClutch = min(eggDryMassForClutch, maxMassAtCurrentAge);
			eggDryMassForClutch = max(eggDryMassForClutch, minMassAtCurrentAge);

			if(mySpecies->hasEggClutchFromEquation())
			{
				clutchDryMass = (mySpecies->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, mySpecies->getForClutchMassScale())) / mySpecies->getConversionToWetMass();
			}
			else //If it does not come from equation,
			{
						//arthro
						if(mySpecies->hasIndeterminateGrowth()){

							double lengthAtThisAge = 0;
							
							if(mySpecies->getGrowthType() == 0){
								lengthAtThisAge = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
							}

							if(mySpecies->getGrowthType() == 1){
								lengthAtThisAge = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
							}

							double massAtThisAge = mySpecies->getCoefficientForMassAforMature()*pow(lengthAtThisAge,mySpecies->getScaleForMassBforMature());
							
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(mySpecies->getEggsPerBatch()*propAdultMass), mySpecies->getEggsPerBatch()); 
							
							clutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
						}else{
							clutchDryMass = eggDryMassForClutch*mySpecies->getEggsPerBatch();	
						}
						//end arthro
			}
			massForNextReproduction = calculateDryMass() + clutchDryMass;


	   
			
		/* if(calculateDryMass() > massForNextReproduction){
		cout << "Id            xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << getId() << endl;
		cout << massForNextReproduction << endl;
		cout << calculateDryMass() << endl;
		cout << lastMassBeforeMaturationOrOviposition << endl;
		cout << clutchDryMass << endl;
		cout << ageForNextReproduction << endl; 
		cout << currentAge << endl;
		exit(-1);
		} */
			
			//arthros
			//daysForTargetReproduction = ceil( (clutchDryMass*ageOfMaturation) / lastMassBeforeMaturationOrOviposition);
			double longevity = mySpecies->getLongevitySinceMaturation()*ageOfFirstMaturation;
			double reproTime = longevity - ageOfFirstMaturation;
				
			daysForTargetReproduction = floor(reproTime/mySpecies->getFemaleMaxReproductionEvents());
			//end arthros
			
 			
			//daysForTargetReproduction = ceil(mySpecies->getReproTimeFactor()*ageOfFirstMaturation);
		}
		else
		{
				//arthro - to make targets more continuous also for dinos
				double currentLength = 0;
				double nextReproLength = 0;
				
				if(mySpecies->getGrowthType() == 0){
					currentLength = Linf*(1-exp(-getTrait(Trait::growth)*(currentAge-thisAnimalVonBertTime0)));
					nextReproLength = Linf*(1-exp(-getTrait(Trait::growth)*(ageForNextReproduction-thisAnimalVonBertTime0)));
				}

				if(mySpecies->getGrowthType() == 1){
					currentLength = Linf/(1+exp((xmid-currentAge)*getTrait(Trait::growth)));
					nextReproLength = Linf/(1+exp((xmid-ageForNextReproduction)*getTrait(Trait::growth)));
				}
    			double nextReproMass = mySpecies->getCoefficientForMassAforMature()*pow(nextReproLength,mySpecies->getScaleForMassBforMature());	
				double currentMass =mySpecies->getCoefficientForMassAforMature()*pow(currentLength,mySpecies->getScaleForMassBforMature());

				if(mySpecies->hasIndeterminateGrowth()){
					if(instar < mySpecies->getNumberOfInstars()){
					  	massForNextReproduction = nextReproMass;
					}else{
					  	massForNextReproduction = currentMass;
					}
					//daysForTargetReproduction = ceil( ((mySpecies->maleReproductionFactor * (massesVector[mySpecies->numberOfInstars-1] - massesVector[mySpecies->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
					daysForTargetReproduction = ceil(0.01);
				}else{
					
					  massForNextReproduction = calculateDryMass() + (nextReproMass-currentMass);
					//daysForTargetReproduction = ceil( ((mySpecies->maleReproductionFactor * (massesVector[mySpecies->numberOfInstars-1] - massesVector[mySpecies->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
					  daysForTargetReproduction = ceil(0.01);
				}
				//end arthro
		}

		//Dinosaurs - this allows dinosaurs to start reproducing readily 
		//at the beginning of the simulation
		ageOfLastMoultOrReproduction = ageAtInitialization;
		//if(mySpecies->hasIndeterminateGrowth()){
		ageForNextReproduction = ageAtInitialization + Random::randomIndex(getAgeInInstar(instar)) + 2; //add two days at least


        /* cout << "Id            xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << getId() << endl;
		cout << gender << endl;
		cout << massForNextReproduction << endl;
		cout << calculateDryMass() << endl;
		cout << ageForNextReproduction << endl; 
		cout << currentAge << endl; */


	//}else{
			
		//}	

		double daysActiveSinceMaturation = round(Random::randomUniform()*daysForTargetReproduction);
		//expectedMassAtCurrentAge = massOfMaturationOrLastReproduction; // + daysActiveSinceMaturation * ((massForNextReproduction - massOfMaturationOrLastReproduction) / (ageForNextReproduction - ageOfLastMoultOrReproduction));
		//traits[Trait::energy_tank] = expectedMassAtCurrentAge - currentBodySize;

/*		if(getId() == 23){
			cout << "ageOfLastMoultOrReproduction: " << ageOfLastMoultOrReproduction << endl;
			cout << "ageAtInitialization: " << ageAtInitialization << endl;
			cout << "daysForTargetReproduction: " << daysForTargetReproduction << endl;
	    	cout << "hello: " << traits[Trait::energy_tank] << endl;
	    }*/


		ofstream noStream2;
		//tuneTraits(-1, 1, temperatureCycleTemperature, 100, noStream2, false);
	
	}


	return temperatureCycleTemperature;
}


TerrainCell* Animal::move(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile)
{


	#ifdef _DEBUG
	cout << "moving animal... " << endl << flush;
	#endif

	if (lastDayMoved < timeStep)
	{
		updateAbundancesExperiencedPerSpecies(timeStepsPerDay);
		handlingTimer = 0.0;
		//cout << sated << endl;
		/* cout << "foodMass.. voracity... food left" << endl;
 		cout << sated << endl;
		cout << traits[Trait::voracity] << endl;
		cout << foodMassLeftForNextTimeStep << endl; */
		foodMassLeftForNextTimeStep = 0; //uncomment for Dinosaurs and but not for arthropods after Dinos
		sated = false; //Dinosaurs and arthropods after Dinos - do not know why¿? 
//      cout << foodMassLeftForNextTimeStep  << endl;
		assimilateLastHuntedAnimalAndComputeHandlingTime();

		if(foodMassLeftForNextTimeStep > -0.00000001 && foodMassLeftForNextTimeStep < 0.00000001) 
		{
			oldSearchArea = getTrait(Trait::search_area);
			biomassTrackedToday = 0;
			cellsTrackedToday.clear();
			hasEvaluatedTheWholeWorld = false;
			searchAnimalsAndFungiToEat(timeStep, timeStepsPerDay, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile);

			//Mature MALES will move now even when they are sated
			if(calculateDryMass() > mySpecies->getQuitCellThreshold()){
				while (handlingTimer < timeStepMaximumHandlingTimer &&
				steps < getTrait(Trait::search_area) - (0.5 * position->getSize()) &&
				lifeStage == ACTIVE && days_digest == 0 &&
				(!sated || (sated && mature && gender == AnimalSpecies::GENDERS::MALE)))
				{
				moveOneStep();
				searchAnimalsAndFungiToEat(timeStep, timeStepsPerDay, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile);
				}
			}else{
				if(stepsAttempted<1){
				double addSearchArea = Random::randomUniform(0.5, 1.0); //arthro - revise for dinos - very small animals can double their search area so they quite the 10 mm cells
				setTrait(Trait::search_area, getTrait(Trait::search_area) + addSearchArea*getTrait(Trait::search_area));
				//cout << "whe are here" << endl;
				while (handlingTimer < timeStepMaximumHandlingTimer &&
				steps < getTrait(Trait::search_area) - (0.5 * position->getSize()) &&
				lifeStage == ACTIVE && days_digest == 0 &&
				(!sated || (sated && mature && gender == AnimalSpecies::GENDERS::MALE)))
				{
				moveOneStep();
				searchAnimalsAndFungiToEat(timeStep, timeStepsPerDay, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile);
				}
				}
			}
			// IMPORTANT! steps might be higher than search_area just when the animal travels the LAST STEP in a diagonal direction.
			if(steps > getTrait(Trait::search_area))
			{
				steps = getTrait(Trait::search_area);
			}

			//cout << targetNeighborToTravelTo->getX() << "-" << targetNeighborToTravelTo->getY() << "-" << targetNeighborToTravelTo->getZ() << endl;

			//For having into account metabolic downregulation on Spiders
			if(huntingMode != Animal::HUNTING_MODES::DOES_NOT_HUNT)
			{
				if(foodMass > 0)
				{
					daysWithoutFood = 0;
				}
				else
				{
					daysWithoutFood += round(((double)timeStep/(double)timeStepsPerDay));
				}
			}
			lastDayMoved = timeStep;
			targetNeighborToTravelTo = NULL;

		}

		ediblesHasTriedToPredate.clear();
	}
	return position;
}

bool edibleCompare(const std::pair<double, pair<Edible*, Edible*>>& firstElem, const std::pair<double, pair<Edible*, Edible*>>& secondElem) {
  return firstElem.first > secondElem.first;
}

bool Animal::searchAnimalsAndFungiToEat(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile)
{
	cellsTrackedToday.push_back(position);

	if(isMature() && !isMated())
	{
		vector<Edible*> * activeAnimals = position->getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, mySpecies);
		vector<Edible*> * satiatedAnimals = position->getAnimalsBySpecies(Animal::LIFE_STAGES::SATIATED, mySpecies);
		vector<Edible*> * handlingAnimals = position->getAnimalsBySpecies(Animal::LIFE_STAGES::HANDLING, mySpecies);
		vector<Edible*> * diapauseAnimals = position->getAnimalsBySpecies(Animal::LIFE_STAGES::DIAPAUSE, mySpecies);
		vector<Edible*> searchableAnimals = vector<Edible*>();
		searchableAnimals.insert(searchableAnimals.end(), activeAnimals->begin(), activeAnimals->end());
		searchableAnimals.insert(searchableAnimals.end(), satiatedAnimals->begin(), satiatedAnimals->end());
		searchableAnimals.insert(searchableAnimals.end(), handlingAnimals->begin(), handlingAnimals->end());
		searchableAnimals.insert(searchableAnimals.end(), diapauseAnimals->begin(), diapauseAnimals->end());

		//std::random_shuffle(searchableAnimals.begin(), searchableAnimals.end());
		for(auto animalsIt = searchableAnimals.begin(); animalsIt != searchableAnimals.end(); animalsIt++)
		{
			Edible* currentAnimal = *animalsIt;
			if(currentAnimal->isMature() && gender != currentAnimal->getGender() && !currentAnimal->isMated())
			{
				if(gender == AnimalSpecies::GENDERS::MALE)
				{
					currentAnimal->setGenomeFromMatedMale(this);
					//mated = true;
					if(this->postBreeding(timeStep, timeStepsPerDay))
					{
						//Erase from vectors and add to senesced vector
						position->changeAnimalToSenesced(this, timeStep);
						return true;
					}
				}
				else
				{

					setGenomeFromMatedMale(currentAnimal);
					//mated = true;
					if(currentAnimal->postBreeding(timeStep, timeStepsPerDay))
					{


						//Erase from vectors and add to senesced vector
						position->changeAnimalToSenesced(currentAnimal, timeStep);
						//return true;
					}
				}
			}
		}

		searchableAnimals.clear();
	}

	vector<pair<double, pair<Edible*, Edible*>>> ediblesByEdibility;

	double pdfThreshold;

	if(position->getTheWorld()->getSimulType() == 0){
		 pdfThreshold = 0.08;
	}

	if(position->getTheWorld()->getSimulType() == 1){
		 pdfThreshold = 0.0003;
	}


	for(vector<Species*>::iterator speciesIt = position->getTheWorld()->existingAnimalSpecies.begin(); speciesIt != position->getTheWorld()->existingAnimalSpecies.end(); speciesIt++)
	{
		Species* currentAnimalSpecies = *speciesIt;
		vector<Edible*> * activeAnimals = position->getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
		vector<Edible*> * satiatedAnimals = position->getAnimalsBySpecies(Animal::LIFE_STAGES::SATIATED, currentAnimalSpecies);
		vector<Edible*> * handlingAnimals = position->getAnimalsBySpecies(Animal::LIFE_STAGES::HANDLING, currentAnimalSpecies);
		vector<Edible*> * diapauseAnimals = position->getAnimalsBySpecies(Animal::LIFE_STAGES::DIAPAUSE, currentAnimalSpecies);

		if(currentAnimalSpecies->canEatAnimalSpecies(mySpecies) && mySpecies->canEatAnimalSpecies(currentAnimalSpecies))
		{
			for(vector<Edible*>::iterator animalsIt = activeAnimals->begin(); animalsIt != activeAnimals->end(); animalsIt++)
			{
				Edible* currentAnimal = *animalsIt;
				
				//Dinosaurs but elsewhere - to prevent unreasonable encounters and ingestion of very large prey
				double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/currentAnimal->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
				


				if(probabilityDensityFunction >= pdfThreshold){ //Dinosaurs but elsewhere
				
				if (currentAnimal->canEatEdible(this) && canEatEdible(currentAnimal) && currentAnimal != this)
				{
					
					if(currentAnimal->calculatePredationProbability(this, false) > calculatePredationProbability(currentAnimal, false))
					{
						ediblesByEdibility.push_back(make_pair(currentAnimal->calculateEdibilityValue(this), make_pair(currentAnimal, this)));
					}
					else
					{
						ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentAnimal), make_pair(this, currentAnimal)));
						biomassTrackedToday += currentAnimal->calculateDryMass();
					}
				}
				else if (currentAnimal->canEatEdible(this) && currentAnimal != this)
				{
					ediblesByEdibility.push_back(make_pair(currentAnimal->calculateEdibilityValue(this), make_pair(currentAnimal, this)));
				}
				else if (canEatEdible(currentAnimal) && currentAnimal != this)
				{
					ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentAnimal), make_pair(this, currentAnimal)));
					biomassTrackedToday += currentAnimal->calculateDryMass();
				}
				}
			}
		}
		else if(currentAnimalSpecies->canEatAnimalSpecies(mySpecies))
		{
			for(vector<Edible*>::iterator animalsIt = activeAnimals->begin(); animalsIt != activeAnimals->end(); animalsIt++)
			{
				Edible* currentAnimal = *animalsIt;

				double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/currentAnimal->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
				
				if(probabilityDensityFunction >= pdfThreshold){ //Dinosaurs but elsewhere

				if (currentAnimal->canEatEdible(this) && currentAnimal != this)
				{
					ediblesByEdibility.push_back(make_pair(currentAnimal->calculateEdibilityValue(this), make_pair(currentAnimal, this)));
				}

				}
			}
		}
		else if(mySpecies->canEatAnimalSpecies(currentAnimalSpecies))
		{
			//HANDLING animals can't predate, but can be predated
			for(vector<Edible*>::iterator animalsIt = activeAnimals->begin(); animalsIt != activeAnimals->end(); animalsIt++)
			{
				Edible* currentAnimal = *animalsIt;

				double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/currentAnimal->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
				
				if(probabilityDensityFunction >= pdfThreshold){ //Dinosaurs but elsewhere

				if (canEatEdible(currentAnimal) && currentAnimal != this)
				{
					ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentAnimal), make_pair(this, currentAnimal)));
					biomassTrackedToday += currentAnimal->calculateDryMass();
				}

				}
			}
			for(vector<Edible*>::iterator animalsIt = satiatedAnimals->begin(); animalsIt != satiatedAnimals->end(); animalsIt++)
			{
				Edible* currentAnimal = *animalsIt;

				double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/currentAnimal->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
				
				if(probabilityDensityFunction >= pdfThreshold){ //Dinosaurs but elsewhere

				if (canEatEdible(currentAnimal) && currentAnimal != this)
				{
					ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentAnimal), make_pair(this, currentAnimal)));
					biomassTrackedToday += currentAnimal->calculateDryMass();
				}

				}
			}
			for(vector<Edible*>::iterator animalsIt = handlingAnimals->begin(); animalsIt != handlingAnimals->end(); animalsIt++)
			{
				Edible* currentAnimal = *animalsIt;

				double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/currentAnimal->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
				
				if(probabilityDensityFunction >= pdfThreshold){ //Dinosaurs but elsewhere

				if (canEatEdible(currentAnimal) && currentAnimal != this)
				{
					ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentAnimal), make_pair(this, currentAnimal)));
					biomassTrackedToday += currentAnimal->calculateDryMass();
				}

				}
			}
			for(vector<Edible*>::iterator animalsIt = diapauseAnimals->begin(); animalsIt != diapauseAnimals->end(); animalsIt++)
			{
				Edible* currentAnimal = *animalsIt;

				double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/currentAnimal->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
				
				if(probabilityDensityFunction >= pdfThreshold){ //Dinosaurs but elsewhere

				if (canEatEdible(currentAnimal) && currentAnimal != this)
				{
					ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentAnimal), make_pair(this, currentAnimal)));
					biomassTrackedToday += currentAnimal->calculateDryMass();
				}

				}
			}
		}
	}

    for(vector<Edible*>::iterator fungusIt = position->getFungi()->begin(); fungusIt != position->getFungi()->end(); fungusIt++)
	{
		Edible* currentFungus = *fungusIt;
		if(mySpecies->canEatFungusSpecies(currentFungus->getSpecies()))
		{
			if(canEatEdible(currentFungus))
			{
				ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentFungus), make_pair(this, currentFungus)));
				biomassTrackedToday += currentFungus->calculateDryMass();
			
			}
		}
	}

	//Sorting the elements by edibility
	std::sort(ediblesByEdibility.begin(), ediblesByEdibility.end(), edibleCompare);

	for(auto ediblesIt = ediblesByEdibility.begin(); ediblesIt != ediblesByEdibility.end(); ediblesIt++)
	{
		edibilitiesFile
		<< timeStep << "\t"
		<< getId() << "\t"
		<< getSpecies()->getScientificName() << "\t"
		<< foodMass << "\t"
		<< (*ediblesIt).second.first->getId() << "\t"
		<< (*ediblesIt).second.first->getSpecies()->getScientificName() << "\t"
		<< (*ediblesIt).second.first->calculateDryMass() << "\t"
		<< (*ediblesIt).second.second->getId() << "\t"
		<< (*ediblesIt).second.second->getSpecies()->getScientificName() << "\t"
		<< (*ediblesIt).second.second->calculateDryMass() << "\t"
		<< (*ediblesIt).second.first->calculateEncounterProbability((*ediblesIt).second.second) << "\t"
		<< (*ediblesIt).second.first->calculatePredationProbability((*ediblesIt).second.second, false) << "\t"
		<< (*ediblesIt).second.first->getSpecies()->getEdiblePreference((*ediblesIt).second.second->getSpecies()) << "\t"
		<< (*ediblesIt).second.first->getMeanExperience((*ediblesIt).second.second->getSpecies()) << "\t"
		<< (*ediblesIt).first << endl;
	}

	bool hasEatenOnce = false;
	for(vector<pair<double, pair<Edible*, Edible*>>>::iterator ediblesIt = ediblesByEdibility.begin(); !hasEatenOnce && ediblesIt != ediblesByEdibility.end(); ediblesIt++)
	{
		Edible* huntingAnimal = (*ediblesIt).second.first;
		Edible* targetAnimal = (*ediblesIt).second.second;


		double probabilityDensityFunction = exp(-0.5 * pow((log(huntingAnimal->calculateWetMass()/targetAnimal->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
				
		//Checking again if the hunting animal is already sated or has traveled its total search area.
		//This could happen when the hunting animal is actually the current moving animal and it is getting its step increased hunt after hunt.
		//Dinosaurs - add probabilityDensityFunction >= 0.0003 - check this for arthropods and dinosaurs - so they do not pursue animals that are too large or conversely too small
		if((!huntingAnimal->isSated() && !huntingAnimal->isExhausted() && (probabilityDensityFunction >= pdfThreshold)) || (!huntingAnimal->isSated() && !huntingAnimal->isExhausted() && !targetAnimal->getSpecies()->isMobile()))
		{
			float attackOrExposedAttackProbability = huntingAnimal->getSpecies()->getAttackProbability();
			if(huntingAnimal != this)
			{
				attackOrExposedAttackProbability = targetAnimal->getSpecies()->getExposedAttackProbability();
			}
			bool searchWasSuccessful = huntingAnimal->encounterEdible(targetAnimal, attackOrExposedAttackProbability, timeStep, encounterProbabilitiesFile, predationProbabilitiesFile);
			if(searchWasSuccessful)
			{
				//Steps only goes up when at least the animal HAS TRIED TO HUNT the prey. It goes up regardless of the success
				//The steps penalization is assumed depending on the search area and the longest diagonal
				bool huntWasSuccessful = huntingAnimal->predateEdible(targetAnimal, timeStep, false, encounterProbabilitiesFile, predationProbabilitiesFile);
				if(huntWasSuccessful)
				{
					hasEatenOnce = true;
					if(targetAnimal == this)
					{
						//The searcher was cleaned up from the animals to move vector in its origin cell.
						//Therefore we do not need to clean the searcher up again here (it does not exist in this position's animals to move vector)
						//Searcher died, so return true
						return true;
					}
				}
				else // Hunt was NOT successful
				{
					//Counter attack only takes place if the target animal was not the current moving animal.
					if(targetAnimal->canEatEdible(huntingAnimal) && targetAnimal != this)
					{
						bool counterHuntWasSuccessful = targetAnimal->predateEdible(huntingAnimal, timeStep, true, encounterProbabilitiesFile, predationProbabilitiesFile);
						if(counterHuntWasSuccessful)
						{
							hasEatenOnce = true;
							if(huntingAnimal == this)
							{
								//The searcher was cleaned up from the animals to move vector in its origin cell.
								//Therefore we do not need to clean the searcher up again here (it does not exist in this position's animals to move vector)
								//Searcher died, so return true
								return true;
							}
						}
					}
				}
			}
		}
	}

	#ifdef _DEBUG
		if(foodMass > traits[Trait::voracity])
		{
			cerr << "The food mass eaten was higher than the voracity value:" << endl;
			cerr << " - Animal: " << id << "(" << mySpecies->getScientificName() << ")" << endl;
			cerr << " - Food mass eaten: " << foodMass << endl;
			cerr << " - Voracity value: " << traits[Trait::voracity] << endl;
		}
	#endif

	/*if(round(foodMass*pow10(7)) >= round(traits[Trait::voracity]*pow10(7)))
	{*/
	/*if(getId() == 2 && timeStep == 51){
		cout << "foodMass: " << foodMass << endl;
	}*/

	if(round(foodMass*pow(10,7)) >= round(getTrait(Trait::voracity)*pow(10,7)))
		{
		
		foodMass = getTrait(Trait::voracity);
		sated = true;

		/*cout << foodMass << endl;
		exit(-1);*/
	
	}




	//Searcher did not die, so return false
	return false;
}

void Animal::setGenomeFromMatedMale(const Edible* matedMale)
{
	mated = true;
	if(genomeFromMatedMale != NULL)	delete genomeFromMatedMale;
	genomeFromMatedMale = matedMale->getGenome()->clone();										   
	//genomeFromMatedMale = matedGenome->clone();
	idFromMatedMale = matedMale->getId();
	generationNumberFromMatedMale = matedMale->getGenerationNumberFromMaleParent();
}

void Animal::clearGenomeFromMatedMale()
{
	mated = false;
	delete genomeFromMatedMale;
	genomeFromMatedMale = NULL;
	idFromMatedMale = -1;
	generationNumberFromMatedMale = -1;
}

bool Animal::postBreeding(int timeStep, int timeStepsPerDay)
{
	rep_count++;

	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;



	ageOfLastMoultOrReproduction = currentAge;

	if (rep_count == 1)
	{
		age_first_rep = currentAge;
	}
	if (rep_count >= getMaxReproductionEvents())
	{

		//cout << "rep_count: " << rep_count << endl;
		//cout << "max rep: "<< getMaxReproductionEvents() << endl;
		//cout << mySpecies->getScientificName() << endl;

		return true;

	}
	else
	{
		//this below is allow females with indeterminate growth to keep growing while reproducing
		//Dinos2023 - animals can now follow mature patterns, be mature while growing, does it work?
		//if we comment below indeterminate growth stops - thus, we need another formula to avoid excessive food targets
	 	if(instar < mySpecies->getNumberOfInstars())
		{
			//Mature will be false only if is NOT the last instar, otherwise the animal will reproduce again
			mature = false;
		}  
		//end Dinos2023
		return false;
	}
}

int Animal::computeEggBatchNumber()
{
	// Simplification of floor(clutchMassForReproduction /
	// (mySpecies->getEggDryMass() + mySpecies->getEffDryMass() * traits[Trait::factorEggMass]));

	return floor(clutchDryMass / (mySpecies->getEggDryMass() * (1 + getTrait(Trait::factorEggMass))));
}

list<Edible*> * Animal::breedAsexually( int objectiveEggsNumber,int timeStep, int timeStepsPerDay, float temperature )
{
	double totalOffspringDryMass = 0.0;

	vector<Gamete*>* femaleGametes = new vector<Gamete *>();
	vector<Gamete*>* maleGametes = new vector<Gamete *>();

	list<Edible*> * offspring = new list<Edible *>();

	//Asexual animals are always females (or treated as females) and they DO NOT perform meiosis.
	int offSpringGender = AnimalSpecies::GENDERS::FEMALE;
	int createdEggsNumber = 0;
	int generationDam;
	int generationSire;

	// Generate as much female gametes as needed depending on the eggNumber
	while( femaleGametes->size() < objectiveEggsNumber )
	{
		// Create four copies of female gametes and male gametes
		femaleGametes->push_back( genome->cloneFirstGameteFromHaploid() );
		maleGametes->push_back( genome->cloneFirstGameteFromHaploid() );
	}

	/*if(nextOffspringToBeBorn != NULL)
	{
		// Update both position and date of egg, to allocate the new animal in the correct space and time
		nextOffspringToBeBorn->setPosition(position);
		nextOffspringToBeBorn->setDateEgg(round(((double)timeStep/(double)timeStepsPerDay)));
		offspring->push_back(nextOffspringToBeBorn);
		totalOffspringDryMass += nextOffspringToBeBorn->getEggDryMassAtBirth();
		nextOffspringToBeBorn = NULL;
	}*/

	while( createdEggsNumber < objectiveEggsNumber )
	{
		Genome* offspringGenome = NULL;
		createdEggsNumber ++;
		generationDam = generationNumberFromFemaleParent + 1;
		generationSire = generationNumberFromFemaleParent + 1;

		Gamete* femaleGameteSelected = femaleGametes->back();
		Gamete* maleGameteSelected = maleGametes->back();

		vector<pair<Chromosome*, Chromosome*> >* inheritedChromosomes = new vector<pair<Chromosome*, Chromosome*> >(femaleGameteSelected->size());

		for (unsigned int i = 0; i < femaleGameteSelected->size(); ++i)
		{
			inheritedChromosomes->at(i) = make_pair(femaleGameteSelected->getChromosome(i)->clone(), maleGameteSelected->getChromosome(i)->clone());
		}

		offspringGenome = new Genome(inheritedChromosomes, mySpecies);

		// Delete gametes as they are no longer needed but don't delete its chromosomes as they have passed to the offspring
		femaleGametes->pop_back();
		maleGametes->pop_back();

		delete femaleGameteSelected;
		delete maleGameteSelected;

		Animal* newOffspring = new Animal(offspringGenome, getTrait(Trait::factorEggMass), position, round(((double)timeStep/(double)timeStepsPerDay)), timeStepsPerDay, generationNumberFromFemaleParent + 1, generationNumberFromMatedMale + 1, getId(), idFromMatedMale, mySpecies, offSpringGender, mySpecies->getDefaultHuntingMode());

		pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newOffspring->interpolateTraits();

		//TODO ALWAYS print the traits after interpolating and before adjusting
		//(*offSpringsIt)->printTraits(constitutiveTraitsFile);

		//REMEMBER to adjustTraits here after printing the traits!
		newOffspring->adjustTraits();
		ofstream noStream;
		newOffspring->calculateGrowthCurves(temperature, noStream, false, 0); //Dinosaurs
		totalOffspringDryMass += newOffspring->getEggDryMassAtBirth();

		newOffspring->setMassAtBirth(newOffspring->getEggDryMassAtBirth());

		//There will be a number of falses in isViableOffspring when generating new offsprings
		//PLEASE NEVER CONSIDER AGAIN creating a new offSpring for each offspring we erased.
		//This erasing behaviour is good for evolution because it stops inviable phenotypes from growing in number
		if(isInsideRestrictedRangesAndIsViableOffSpring.second)
		{
			offspring->push_back(newOffspring);
		}
		else // non-viable offspring -> kill it
		{
			std::cout << "Deleting non-viable Animal with ID: "<< newOffspring->getId() << std::endl;
			delete newOffspring;
		}
	}

	
		
	fecundity += offspring->size();

	setTrait(Trait::energy_tank, getTrait(Trait::energy_tank) - totalOffspringDryMass);

	//lastMassBeforeMaturationOrOviposition = calculateDryMass() + traits[Trait::energy_tank];
	//We recalculate the massForNextReproduction here? Not now tho, check this...
	//massForNextReproduction = massOfMaturationOrLastReproduction + clutchDryMass;
	//massOfMaturationOrLastReproduction = getDryMass();

	if(postBreeding(timeStep, timeStepsPerDay))
	{
		position->changeAnimalToSenesced(this, timeStep);
	}
	else
	{
		unsigned int oldStage = lifeStage;
		lifeStage = ACTIVE;
		position->addAnimal(this);
		position->eraseAnimal(this, oldStage);
		//clearGenomeFromMatedMale();
		//CARE delete only when in TerrainCell
		//position->addAnimal(this);
		//position->eraseAnimal(this, REPRODUCING);
	}


	for(auto elem : *femaleGametes) {
		delete elem;
	}
	femaleGametes->clear();
	delete femaleGametes;

	for(auto elem : *maleGametes) {
		delete elem;
	}
	maleGametes->clear();
	delete maleGametes;

	

	return offspring;
}


list<Edible*> * Animal::breedSexually( int objectiveEggsNumber,int timeStep, int timeStepsPerDay, float temperature )
{
	double totalOffspringDryMass = 0.0;

	vector<Gamete*>* femaleGametes = new vector<Gamete *>();
	
	list<Edible*> * offspring = new list<Edible *>();

	int offSpringGender;
	int createdEggsNumber = 0;
	int generationDam;
	int generationSire;

	// Generate as much female gametes as needed depending on the eggNumber
	while( femaleGametes->size() < objectiveEggsNumber )
	{
		// Create four copies of female gametes and male gametes
		femaleGametes->push_back( genome->cloneFirstGameteFromHaploid() );
	}

	while( createdEggsNumber < objectiveEggsNumber )
	{
		Genome* offspringGenome = NULL;
		createdEggsNumber ++;  // DUDA: Debe hacerse antes o sólo si se crea el offspring?
		
		Gamete* femaleGameteSelected = femaleGametes->back();
		Gamete* maleGameteSelected = NULL;
		
		if(mySpecies->getSexualType() == AnimalSpecies::SEXUAL_TYPES::DIPLOID)
		{
			maleGameteSelected = genomeFromMatedMale->getRandomGameteFromMeiosis();
			//gender here depends on the species sexRatio
			offSpringGender = mySpecies->getRandomGender();
		}
		else if(mySpecies->getSexualType() == AnimalSpecies::SEXUAL_TYPES::HAPLODIPLOID)
		{
			if(isMated())
			{
				//Gender here depends on the species sexRatio
				offSpringGender = mySpecies->getRandomGender();
				if(offSpringGender == AnimalSpecies::GENDERS::MALE)
				{
					//Males are haploid so they will use only the female gamete (duplicated for simplicity, in reality they have only one)
					maleGameteSelected = new Gamete();
					for(unsigned int i = 0; i < femaleGameteSelected->size(); ++i)
					{
						maleGameteSelected->pushChromosome(femaleGameteSelected->getChromosome(i)->clone());
					}
				}
				else
				{
					//Females are still diploid, but males contribute with their genes as they are (no meiosis involved here)
					maleGameteSelected = genomeFromMatedMale->cloneFirstGameteFromHaploid();
				}
			}
			else //!isMated()
			{
				//Males are haploid so they will use only the female gamete (duplicated for simplicity, in reality they have only one)
				maleGameteSelected = new Gamete();
				for(unsigned int i = 0; i < femaleGameteSelected->size(); ++i)
				{
					maleGameteSelected->pushChromosome(femaleGameteSelected->getChromosome(i)->clone());
				}
				//Gender here is always MALE
				offSpringGender = AnimalSpecies::GENDERS::MALE;
			}
		}

		generationDam = generationNumberFromFemaleParent + 1; 
		generationSire = generationNumberFromMatedMale + 1;

		vector<pair<Chromosome*, Chromosome*> >* inheritedChromosomes = new vector<pair<Chromosome*, Chromosome*> >(femaleGameteSelected->size());

		for (unsigned int i = 0; i < femaleGameteSelected->size(); ++i)
		{
			inheritedChromosomes->at(i) = make_pair(femaleGameteSelected->getChromosome(i)->clone(), maleGameteSelected->getChromosome(i)->clone());
		}

		offspringGenome = new Genome(inheritedChromosomes, mySpecies);

		// Delete gametes as they are no longer needed but don't delete its chromosomes as they have passed to the offspring
		femaleGametes->pop_back();

		delete femaleGameteSelected;
		delete maleGameteSelected;

		Animal* newOffspring = new Animal(offspringGenome, getTrait(Trait::factorEggMass), position, round(((double)timeStep/(double)timeStepsPerDay)), timeStepsPerDay, generationNumberFromFemaleParent + 1, generationNumberFromMatedMale + 1, getId(), idFromMatedMale, mySpecies, offSpringGender, mySpecies->getDefaultHuntingMode());

		pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newOffspring->interpolateTraits();

		//TODO ALWAYS print the traits after interpolating and before adjusting
		//(*offSpringsIt)->printTraits(constitutiveTraitsFile);

		//REMEMBER to adjustTraits here after printing the traits!
		newOffspring->adjustTraits();
		ofstream noStream;
		newOffspring->calculateGrowthCurves(temperature, noStream, false, 0); //Dinosaurs
		totalOffspringDryMass += newOffspring->getEggDryMassAtBirth();

		newOffspring->setMassAtBirth(newOffspring->getEggDryMassAtBirth());

		//There will be a number of falses in isViableOffspring when generating new offsprings
		//PLEASE NEVER CONSIDER AGAIN creating a new offSpring for each offspring we erased.
		//This erasing behaviour is good for evolution because it stops inviable phenotypes from growing in number
		if(isInsideRestrictedRangesAndIsViableOffSpring.second)
		{
			offspring->push_back(newOffspring);
		}
		else // non-viable offspring -> kill it
		{
			std::cout << "Deleting non-viable Animal with ID: "<< newOffspring->getId() << std::endl;
			delete newOffspring;
		}
	}

	fecundity += offspring->size();

	setTrait(Trait::energy_tank, getTrait(Trait::energy_tank) - totalOffspringDryMass);

	//lastMassBeforeMaturationOrOviposition = calculateDryMass() + traits[Trait::energy_tank];
	//We recalculate the massForNextReproduction here? Not now tho, check this...
	//massForNextReproduction = massOfMaturationOrLastReproduction + clutchDryMass;
	//massOfMaturationOrLastReproduction = getDryMass();

	if(postBreeding(timeStep, timeStepsPerDay))
	{
		position->changeAnimalToSenesced(this, timeStep);
	}
	else
	{
		unsigned int oldStage = lifeStage;
		lifeStage = ACTIVE;
		position->addAnimal(this);
		position->eraseAnimal(this, oldStage);
		//clearGenomeFromMatedMale();
		//CARE delete only when in TerrainCell
		//position->addAnimal(this);
		//position->eraseAnimal(this, REPRODUCING);
	}


	for(auto elem : *femaleGametes) {
		delete elem;
	}
	femaleGametes->clear();
	delete femaleGametes;
	

	return offspring;
}

list<Edible*> * Animal::breed(int timeStep, int timeStepsPerDay, float temperature)
{
	int currentEggBatchNumber = computeEggBatchNumber();

	// Stores animals to be born
	list<Edible*> * offspring = NULL;

	switch (mySpecies->getSexualType())
	{
		case AnimalSpecies::SEXUAL_TYPES::ASEXUAL:
			offspring = breedAsexually(currentEggBatchNumber, timeStep, timeStepsPerDay, temperature);
			break;
		case AnimalSpecies::SEXUAL_TYPES::DIPLOID:
			offspring = breedSexually(currentEggBatchNumber, timeStep, timeStepsPerDay, temperature);
			break;
		case AnimalSpecies::SEXUAL_TYPES::HAPLODIPLOID:
			offspring = breedSexually(currentEggBatchNumber, timeStep, timeStepsPerDay, temperature);
			break;
		default:
			std::cerr << "Check out animal sexuality, this is not a valid SEXUAL_TYPE value: " << mySpecies->getSexualType() << std::endl;
			exit(-1);
	}

	return offspring;
}

bool cellCompare(const std::pair<double, TerrainCell*>& firstElem, const std::pair<double, TerrainCell*>& secondElem) {
  return firstElem.first > secondElem.first;
}

bool cellEqual(const std::pair<double, TerrainCell*>& firstElem, const std::pair<double, TerrainCell*>& secondElem) {
  return firstElem.first > secondElem.first;
}

void Animal::moveOneStep()
{
	stepsAttempted++;
	//bool obstacleFound = false;
	if(targetNeighborToTravelTo != NULL && targetNeighborToTravelTo != position)
	{
		//cout << "TOWARDS TARGET NEIGHBOR... " << id << " ## D.remaining: " << position->getDistanceToCell(targetNeighborToTravelTo) << " d: " << ((int) round((traits[Trait::search_area] - steps) / position->getSize())) << " s: " << (traits[Trait::search_area] - steps) << endl << flush;
		//TODO OBSTACULOS en getCellByBearing!!
		TerrainCell* cellToMoveTo = position->getTheWorld()->getCellByBearing(position, targetNeighborToTravelTo);
		//if(cellToMoveTo != NULL)
		//{
			double distanceToAdd = position->getDistanceToCell(cellToMoveTo);
			steps += distanceToAdd;
			if(timeStepMaximumSearchArea > 0.0)
			{
				timeStepMaximumHandlingTimer -= distanceToAdd / timeStepMaximumSearchArea;
			} else {
				timeStepMaximumHandlingTimer = 0.0;
			}
			timeStepMaximumHandlingTimer = max(timeStepMaximumHandlingTimer, 0.0);
			position->migrateAnimalTo(this, cellToMoveTo);
			position = cellToMoveTo;
		//}
		//else
		//{
		//	obstacleFound = true;
		//}
	}
	else
	{
		//cout << mySpecies->getScientificName() << "#" << id << ": SEARCH TARGET START... " << " stepsLeft: " << ((int) round((traits[Trait::search_area] - steps) / position->getSize())) << " searchAreaLeft: " << (traits[Trait::search_area] - steps) << endl << flush;
		int maxAnimalDistance = (int) round((getTrait(Trait::search_area) - steps)/position->getSize());
		int maxWorldX = max(((position->getTheWorld()->getWidth()-1)-position->getX()), position->getX());
		int maxWorldY = max((position->getTheWorld()->getLength()-1)-position->getY(), position->getY());
		int maxWorldZ = max((position->getTheWorld()->getDepth()-1)-position->getZ(), position->getZ());
		int maxWorldDistance = max(max(maxWorldX, maxWorldY), maxWorldZ);
		int maxDistance = min(maxWorldDistance, maxAnimalDistance);
		//cout << mySpecies->getScientificName() << "#" << id << ": position = " << position->getX() << "-" << position->getY() << "-" << position->getZ() << "; maxDistance = " << maxDistance << endl << flush;

		if(!hasEvaluatedTheWholeWorld)
		{
			for(int distance = 1; distance <= maxDistance && (targetNeighborToTravelTo == NULL || targetNeighborToTravelTo == position); distance++)
			{
				//cout << mySpecies->getScientificName() << "#" << id << ": looking for resources at distance = " << distance << " of " << maxDistance << endl << flush;
				vector<TerrainCell*> * neighbors = position->getCellsAtDistance(distance, getTrait(Trait::search_area) - steps + position->getSize(),this->getSpecies()->isMobile());
				list<TerrainCell*>::iterator cellsTrackedTodayIt;
				//TODO what will happen if they have tracked all already?

				for (cellsTrackedTodayIt = cellsTrackedToday.begin(); cellsTrackedTodayIt != cellsTrackedToday.end(); cellsTrackedTodayIt++)
				{
					neighbors->erase(std::remove(neighbors->begin(), neighbors->end(), (*cellsTrackedTodayIt)), neighbors->end());
				}
				///////////////////////////////////////

				double closestNeighborDistance = DBL_MAX;

				//neighbors here DOES NOT include the animal's current position
				vector<TerrainCell*>* possibleTargetNeighbors = new vector<TerrainCell*>();
				for (vector<TerrainCell*>::iterator neighborsIt = neighbors->begin(); neighborsIt != neighbors->end(); neighborsIt++)
				{
					TerrainCell* currentTargetNeighbor = (*neighborsIt);
					bool resourceAvailableOnCurrentTargetNeighbor = false;
					if (isMature() && gender == AnimalSpecies::GENDERS::MALE)
					{
						if(currentTargetNeighbor->getNumberOfMatureFemales(this) > 0)
						{
							resourceAvailableOnCurrentTargetNeighbor = true;
						}
					}
					else
					{
						double edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass;
						tie(edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass) = currentTargetNeighbor->getCellEvaluation(this);
						if((edibilityValue + conspecificBiomass) > 0.0)
						{
							resourceAvailableOnCurrentTargetNeighbor = true;
						}
					}

					if(resourceAvailableOnCurrentTargetNeighbor)
					{
						possibleTargetNeighbors->push_back(currentTargetNeighbor);
						double distanceToCurrentNeighbor = position->getDistanceToCell(currentTargetNeighbor);
						if (distanceToCurrentNeighbor < closestNeighborDistance && distanceToCurrentNeighbor > 0)
						{
							closestNeighborDistance = distanceToCurrentNeighbor;
							//targetNeighborToTravelTo = (*neighborsIt);
						}
					}
				}

				//cout << mySpecies->getScientificName() << "#" << id << ": position = " << position->getX() << "-" << position->getY() << "-" << position->getZ() << "; neighbors size = " << neighbors->size() << "; possibleTargetNeighbors size = " << possibleTargetNeighbors->size() << endl << flush;

				double bestCellEvaluation = DBL_MIN;
				//neighbors here DOES include the animal's current position
				vector<pair<double, TerrainCell*>> neighborsByEdibilityEvaluation;
				vector<pair<double, TerrainCell*>> neighborsByPredatoryRiskEdibilityEvaluation;
				vector<pair<double, TerrainCell*>> neighborsByConspecificEvaluation;
				for (vector<TerrainCell*>::iterator neighborsIt = possibleTargetNeighbors->begin(); neighborsIt != possibleTargetNeighbors->end(); neighborsIt++)
				{
					TerrainCell* currentTargetNeighbor = (*neighborsIt);
					double distanceToCurrentNeighbor = position->getDistanceToCell(currentTargetNeighbor);
					if (distanceToCurrentNeighbor == closestNeighborDistance || distanceToCurrentNeighbor == 0)
					{
						double edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass;
						tie(edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass) = currentTargetNeighbor->getCellEvaluation(this);
						neighborsByEdibilityEvaluation.push_back(make_pair(edibilityValue, currentTargetNeighbor));
						neighborsByPredatoryRiskEdibilityEvaluation.push_back(make_pair(predatoryRiskEdibilityValue, currentTargetNeighbor));
						neighborsByConspecificEvaluation.push_back(make_pair(conspecificBiomass, currentTargetNeighbor));
					}
				}


				// Sorting the elements by edibility and conspecific evaluations
				std::sort(neighborsByEdibilityEvaluation.begin(), neighborsByEdibilityEvaluation.end(), cellCompare);
				std::sort(neighborsByPredatoryRiskEdibilityEvaluation.begin(), neighborsByPredatoryRiskEdibilityEvaluation.end(), cellCompare);
				std::sort(neighborsByConspecificEvaluation.begin(), neighborsByConspecificEvaluation.end(), cellCompare);

				vector<pair<double, TerrainCell*>> neighborsByWeighedEvaluation;
				for (vector<pair<double, TerrainCell*>>::iterator edibilityIt = neighborsByEdibilityEvaluation.begin(); edibilityIt != neighborsByEdibilityEvaluation.end(); edibilityIt++)
				{
					TerrainCell* currentTargetNeighbor = (*edibilityIt).second;
					// Normalize values to compare in the same unit. We use the min and max of all the considered terrain cells
					double cellEdibilityEvaluation = Math_Functions::linearInterpolate((*edibilityIt).first,
																		neighborsByEdibilityEvaluation.back().first,
																		neighborsByEdibilityEvaluation.front().first,
																		0.0,
																		1.0);
					double cellPredatoryRiskEdibilityEvaluation = 0.0;
					for (vector<pair<double, TerrainCell*>>::iterator predatoryRiskEdibilityIt = neighborsByPredatoryRiskEdibilityEvaluation.begin(); predatoryRiskEdibilityIt != neighborsByPredatoryRiskEdibilityEvaluation.end(); predatoryRiskEdibilityIt++)
					{
						if(currentTargetNeighbor == (*predatoryRiskEdibilityIt).second)
						{
							// Normalize values to compare in the same unit. We use the min and max of all the considered terrain cells
							cellPredatoryRiskEdibilityEvaluation = Math_Functions::linearInterpolate((*predatoryRiskEdibilityIt).first,
																					neighborsByPredatoryRiskEdibilityEvaluation.back().first,
																					neighborsByPredatoryRiskEdibilityEvaluation.front().first,
																					0.0,
																					1.0);
						}
					}

					double cellConspecificEvaluation = 0.0;
					for (vector<pair<double, TerrainCell*>>::iterator conspecificIt = neighborsByConspecificEvaluation.begin(); conspecificIt != neighborsByConspecificEvaluation.end(); conspecificIt++)
					{
						if(currentTargetNeighbor == (*conspecificIt).second)
						{
							// Normalize values to compare in the same unit. We use the min and max of all the considered terrain cells
							cellConspecificEvaluation = Math_Functions::linearInterpolate((*conspecificIt).first,
																			neighborsByConspecificEvaluation.back().first,
																			neighborsByConspecificEvaluation.front().first,
																			0.0,
																			1.0);
						}
					}

					double cellEdibilityAndRiskEvaluation = mySpecies->getCellEvaluationBiomass() * cellEdibilityEvaluation
											- mySpecies->getCellEvaluationRisk() * cellPredatoryRiskEdibilityEvaluation;

					double cellConspecificAndAntiEvaluation = mySpecies->getCellEvaluationProConspecific() * cellConspecificEvaluation
											- mySpecies->getCellEvaluationAntiConspecific() * pow(cellConspecificEvaluation,2);

					double weighedEdibility = cellEdibilityAndRiskEvaluation * (1.0 - mySpecies->getConspecificWeighing());
					double weighedConspecific = cellConspecificAndAntiEvaluation * mySpecies->getConspecificWeighing();
					double weighedEvaluation = (weighedEdibility + weighedConspecific); 
					neighborsByWeighedEvaluation.push_back(make_pair(weighedEvaluation, currentTargetNeighbor));
				}
				neighborsByEdibilityEvaluation.clear();
				neighborsByConspecificEvaluation.clear();

				int size = neighborsByWeighedEvaluation.size();
				std::sort(neighborsByWeighedEvaluation.begin(), neighborsByWeighedEvaluation.end(), cellCompare);
				if(!neighborsByWeighedEvaluation.empty())
				{
					targetNeighborToTravelTo = neighborsByWeighedEvaluation.front().second;
				}
				neighborsByWeighedEvaluation.clear();

				possibleTargetNeighbors->clear();
				delete possibleTargetNeighbors;
				neighbors->clear();
				delete neighbors;
			}
			
			
			//Select a random direction if there are no animals around
			
			if (targetNeighborToTravelTo == NULL || targetNeighborToTravelTo == position)
			{
				if(maxAnimalDistance >= maxWorldDistance)
				{
					hasEvaluatedTheWholeWorld = true;
				}
				vector<TerrainCell*> * neighbors = position->getCellsAtDistance(maxDistance, DBL_MAX, this->getSpecies()->isMobile()); //arthros and for dinos
				if(neighbors->size() > 0){
				targetNeighborToTravelTo = neighbors->at(Random::randomIndex(neighbors->size()));
				}
				neighbors->clear();
				delete neighbors;
			} 
			//targetNeighborToTravelTo = position;
		}
		else //hasEvaluatedTheWholeWorld==true
		{
			vector<TerrainCell*> * neighbors = position->getCellsAtDistance(maxDistance, DBL_MAX, this->getSpecies()->isMobile());
			if(neighbors->size() > 0){
			targetNeighborToTravelTo = neighbors->at(Random::randomIndex(neighbors->size()));
			}
			
			//targetNeighborToTravelTo = position;
			neighbors->clear();
			delete neighbors;
		}

		//TODO OBSTACULOS en getCellByBearing!!
		TerrainCell* cellToMoveTo = position->getTheWorld()->getCellByBearing(position, targetNeighborToTravelTo);
		double distanceToAdd = position->getDistanceToCell(cellToMoveTo);
		
		steps += distanceToAdd;
		if(timeStepMaximumSearchArea > 0.0)
		{
			timeStepMaximumHandlingTimer -= distanceToAdd / timeStepMaximumSearchArea;
		} else {
			timeStepMaximumHandlingTimer = 0.0;
		}
		timeStepMaximumHandlingTimer = max(timeStepMaximumHandlingTimer, 0.0);
		position->migrateAnimalTo(this, cellToMoveTo);
		position = cellToMoveTo;

		//cout << mySpecies->getScientificName() << "#" << id << ": SEARCH TARGET OVER... " << " stepsLeft: " << ((int) round((traits[Trait::search_area] - steps) / position->getSize())) << " searchAreaLeft: " << (traits[Trait::search_area] - steps) << endl << flush;
	}

}

void Animal::dieFromBackground(int timeStep)
{
	if (mySpecies->getProbabilityDeathFromBackground() > Random::randomUniform())
	{
		setNewLifeStage(LIFE_STAGES::BACKGROUND, timeStep);
	}
}

void Animal::assimilateFoodMass(int timeStep)
{
	
	
	//cout << foodMass << endl;
	
	if (foodMass > 0)
	{
		#ifdef _DEBUG
		if(foodMass < traits[Trait::voracity])
		{
			cerr << "The animal has only eaten " << foodMass << "(" << eatenToday << " preys)" << " out of " << traits[Trait::voracity] << "(" << steps << " steps)" << endl;
		}
		#endif

		/*
		if(food_mass / traits[Trait::energy_tank] > 4) {
			std::cout << "ME HE COMIDO UN ANIMAL ENORME" << endl;
		}
		 */

		/*
		double foodMassToAssimilatePerDay = foodMass / handlingTimer;
		foodMassToAssimilatePerDay = min(foodMassToAssimilatePerDay, foodMass);
		foodMass -= foodMassToAssimilatePerDay;
		handlingTimer -= min(1.0, handlingTimer);

		*/

        /*double maxNextInstarLengthFromVBPlasticity = lengthsVector[instar+1] + mySpecies->getMaxPlasticityKVonBertalanffy()*lengthsVector[instar+1];
        double maxNextInstarMassFromVBPlasticity = mySpecies->getCoefficientForMassA()*pow(maxNextInstarLengthFromVBPlasticity,mySpecies->getScaleForMassB());

        if(calculateDryMass() > maxNextInstarMassFromVBPlasticity){
        	foodMass=0;
        }else{
		if(calculateDryMass()+foodMass > maxNextInstarMassFromVBPlasticity){
		            cout << "dryMass + foodMass" << calculateDryMass()+foodMass << endl;
		            foodMass=maxNextInstarMassFromVBPlasticity-calculateDryMass();
		            cout << "dryMass + foodMass" << calculateDryMass()+foodMass << endl;
		}
        }*/
		/* if(mySpecies->getId() == 1 && timeStep >30 && timeStep <40){
			cout << timeStep << endl;
			cout << foodMass << endl;
			cout << traits[Trait::energy_tank] << endl;
		}	 */
		
		//double demand = traits[Trait::voracity] + traits[Trait::energy_tank] 
		
		setTrait(Trait::energy_tank, getTrait(Trait::energy_tank) + foodMass);//*0.2; YA ESTÁ RESUELTO CUANDO COMEN!!

		
		//cout << "hola: " << endl;



		/*
		double daysDigestWithoutTemp;
		if (eating)
		{
			//TODO 2 RAW VALUES HERE 0.05 AND 0.15
			daysDigestWithoutTemp = linearInterpolate(food_mass/traits[Trait::voracity], 0, 1, 1, 3);

			days_digest = ceil(daysDigestWithoutTemp * linearInterpolate(position->temperature, MIN_T, MAX_T, 1,
					mySpecies->Q10DIGEST));
			eating = false;
		}
		 */
        if(round(foodMass*pow(10,7)) < round(getTrait(Trait::voracity)*pow(10,7)))
		{
		sated = false;
		}else{
		sated = true;	
		}

		foodMass = 0;
		eatenToday = 0;
	}
	
/*	 (days_digest > 0)
	{
		days_digest--;
	}
*/
}

void Animal::becomePredated(int timeStep)
{
	setNewLifeStage(PREDATED, timeStep, getId());
}


int Animal::getDaysDigest()
{
	return days_digest;
}

int Animal::getMaxReproductionEvents()
{
	if(gender == AnimalSpecies::GENDERS::FEMALE)
	{
		return mySpecies->getFemaleMaxReproductionEvents();
	}
	else
	{
		return mySpecies->getMaleMaxReproductionEvents();
	}
}

list<int> * Animal::getEncounterEvents()
{
	return &encounterEvents;
}

double Animal::calculatePredationProbability(Edible* edibleAnimal, bool retaliation)
{
	double predationProbability = 1;
	if(edibleAnimal->getSpecies()->isMobile())
	{
		double normalizedHunterVoracity = getNormalizedHunterVoracity();

		double normalizedSpeedRatio;

		if(edibleAnimal->getSpeed()>0.0){
		  normalizedSpeedRatio = getNormalizedSpeedRatio(edibleAnimal->getSpeed());
	    }else{//this is to circunvent NANs - the ratio is made 1
	      normalizedSpeedRatio = getNormalizedSpeedRatio(getTrait(Trait::speed));
	    }

		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/edibleAnimal->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
		double normalizedPDF = getNormalizedPDF(probabilityDensityFunction);

/* 		if(getSpecies()->getScientificName()=="Tyrannosaurus_sp"){
			cout << probabilityDensityFunction << endl;
			cout << normalizedPDF << endl;
		}
 */


		if (huntingMode == ACTIVE_HUNTING || retaliation)
		{
			predationProbability = 
					(position->getTheWorld()->getPredationSpeedRatioAh() * normalizedSpeedRatio +
					position->getTheWorld()->getPredationHunterVoracityAH() * normalizedHunterVoracity +
					position->getTheWorld()->getPredationProbabilityDensityFunctionAH() * normalizedPDF)/3;

		}
		else if (huntingMode == SIT_AND_WAIT)
		{
			predationProbability = 
					(position->getTheWorld()->getPredationSpeedRatioSaw() * normalizedSpeedRatio + //##spd_ratio does not matter anymore
					position->getTheWorld()->getPredationHunterVoracitySAW() * normalizedHunterVoracity +
					position->getTheWorld()->getPredationProbabilityDensityFunctionSAW() * normalizedPDF)/3;
		}
	}

/* 		if(getSpecies()->getScientificName()=="Tyrannosaurus_sp"){
			cout << predationProbability << endl;
		} */



	return predationProbability;
}

bool Animal::predateEdible(Edible* edibleToBePredated, int timeStep, bool retaliation, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile)
{
	if(edibleToBePredated->getSpecies()->isMobile())
	{
		ediblesHasTriedToPredate.push_back(edibleToBePredated);
	}
	bool huntWasSuccessful = false;
	//Steps only goes up when at least the animal HAS TRIED TO HUNT the prey. It goes up regardless of the success
	//The steps penalization is assumed depending on the search area and the longest diagonal

	if(!retaliation && edibleToBePredated->getSpecies()->isMobile())
	{
		double halfLongestDiagonalInsideCurrentCell = sqrt(3 * pow(position->size, 2));
		double distanceToAdd = halfLongestDiagonalInsideCurrentCell*getTrait(Trait::search_area)/position->getTheWorld()->getMaxSearchArea();
		steps += distanceToAdd;
		if(timeStepMaximumSearchArea > 0.0)
		{
			timeStepMaximumHandlingTimer -= distanceToAdd / timeStepMaximumSearchArea;
		} else {
			timeStepMaximumHandlingTimer = 0.0;
		}
		timeStepMaximumHandlingTimer = max(timeStepMaximumHandlingTimer, 0.0);
		steps = min(steps, getTrait(Trait::search_area));
	}

	//arthros and for dinos!!!!
	double randomPredationProbability = Random::randomUniform();

	#ifdef _DEBUG
	if(find(huntedAnimalsToday.begin(), huntedAnimalsToday.end(), edibleToBePredated->getId()) != huntedAnimalsToday.end())
	{
		position->increaseMultipleSameHuntedAnimalToday();
		//cerr << "The animal hunted off the same animal at least twice the same day: " << endl;
		//cerr << " - Animal: " << id << "(" << mySpecies->getScientificName() << ")" << endl;
		//cerr << " - Animal hunted off: " << huntedAnimal->getId() << "(" << huntedAnimal->getSpecies()->getScientificName() << ")" << endl;
	}
	huntedAnimalsToday.push_back(edibleToBePredated->getId());
	#endif
	//if (predationProbability > randomPredationProbability)
	//TODO attackProb y killProb van EN CADA ESPECIE

//getSpecies()->getKillProbability() > randomPredationProbability

    double prob = calculatePredationProbability(edibleToBePredated, retaliation);//Dinosaur
//prob*
	if (((getSpecies()->getKillProbability() > randomPredationProbability) && edibleToBePredated->getSpecies()->isMobile()) || !edibleToBePredated->getSpecies()->isMobile()) ///arthropods + Dinosaur //
	{
		#ifdef _DEBUG
		if(find(predatedAnimalsToday.begin(), predatedAnimalsToday.end(), edibleToBePredated->getId()) != predatedAnimalsToday.end())
		{
			position->increaseMultipleSamePredatedAnimalToday();
			//cerr << "The animal predated the same animal at least twice the same day: " << endl;
			//cerr << " - Animal: " << id << "(" << mySpecies->getScientificName() << ")" << endl;
			//cerr << " - Animal predated: " << huntedAnimal->getId() << "(" << huntedAnimal->getSpecies()->getScientificName() << ")" << endl;
		}
		predatedAnimalsToday.push_back(edibleToBePredated->getId());
		#endif

		mySpecies->addPredationEventOnOtherSpecies(edibleToBePredated->getSpecies()->getId());
		eatenToday++;
		huntWasSuccessful = true;

		edibleToBePredatedProfitability = mySpecies->getEdibleProfitability(edibleToBePredated->getSpecies());

		//arthro and for dinos
	    double forNotToDepleteFungi = getTrait(Trait::voracity)/(edibleToBePredatedProfitability+getTrait(Trait::assim));
		double leftovers;

        if(edibleToBePredated->isDepleted(forNotToDepleteFungi)){ //fungi leftover to partially fulfill voracity
	      leftovers = edibleToBePredated->anyLeft()*(edibleToBePredatedProfitability+getTrait(Trait::assim));
        }else{ //the entire voracity is available
		  leftovers = forNotToDepleteFungi;
		}  

		//The target animal is cleaned up from its source lifestage vector. Or the fungus is substracted.
		double fullDryMassToBeEaten = position->turnEdibleIntoDryMassToBeEaten(edibleToBePredated, timeStep, this, leftovers);
		//end arthro
		
	/*	cout << fullDryMassToBeEaten << endl;

		if(timeStep==10){
			exit(-1);
		}*/

		//arthro and for dinos commented to prevent segmentation fault if zero biomass - use it when minspores = 0 but need to check several parts
		/* if (edibleToBePredated->isExtinct())
		{
			position->deleteFungus(edibleToBePredated);
			delete edibleToBePredated;
		} */

		//double profitableDryMassToBeEaten = fullDryMassToBeEaten * edibleToBePredatedProfitability;
		abundancesExperiencedPerSpecies[edibleToBePredated->getSpecies()] += fullDryMassToBeEaten;
		
		//Assim with handling time will be calculated according to the FULL dry mass
		//It will be reduced to only profitableMass after handling calculations
		foodMassLeftForNextTimeStep = fullDryMassToBeEaten;
		lastHuntedAnimalDryMass = fullDryMassToBeEaten;
		assimilateLastHuntedAnimalAndComputeHandlingTime();
		
		/*cout << foodMassLeftForNextTimeStep << endl;
		cout << timeStep << endl;
			
		if(timeStep==10){	
		exit(-1);
		}*/
	
	}

	predationProbabilitiesFile << getIdStr() << "\t"
	<< edibleToBePredated->getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< edibleToBePredated->getSpecies()->getScientificName() << "\t"
	<< ((edibleToBePredated->getHuntingMode()!=DOES_NOT_HUNT)?1:0) << "\t"
	<< calculateDryMass() << "\t"
	<< edibleToBePredated->calculateDryMass() << "\t"
	<< randomPredationProbability << "\t"
	<< prob << "\t"
	<< ((prob*getSpecies()->getKillProbability() > randomPredationProbability)?1:0) << "\t"
	<< endl;


	return huntWasSuccessful;
}

void Animal::updateAbundancesExperiencedPerSpecies(int timeStepsPerDay)
{
	float totalOfAbundances = 0;
	daysExperienced++;
	for(map<Species*, double>::iterator it = abundancesExperiencedPerSpecies.begin(); it != abundancesExperiencedPerSpecies.end(); it++)
	{
		totalOfAbundances += (*it).second;
	}
	if(totalOfAbundances > 0.0)
	{
		for(map<Species*, double>::iterator it = abundancesExperiencedPerSpecies.begin(); it != abundancesExperiencedPerSpecies.end(); it++)
		{
			(*it).second = (*it).second / totalOfAbundances;
		}
	}

	double experienceInfluencePerTimeStep = mySpecies->getExperienceInfluencePerDay() / timeStepsPerDay;
	for(map<Species*, double>::iterator it = meanExperiencesPerSpecies.begin(); it != meanExperiencesPerSpecies.end(); it++)
	{
		//1.0 will be the base value, meaning it has not come across any animal yet
		(*it).second = abundancesExperiencedPerSpecies[(*it).first] * experienceInfluencePerTimeStep + (*it).second * (1.0 - experienceInfluencePerTimeStep);
		//cout << (*it).second << endl;
	}

	//Jordi - 23/03/2022 - this "abundancesExperiencedPerSpecies" needs to turn it back to the original biomass scale, otherwise only the last food item matters added to a 0-1 value
	if(totalOfAbundances > 0.0)
	{
		for(map<Species*, double>::iterator it = abundancesExperiencedPerSpecies.begin(); it != abundancesExperiencedPerSpecies.end(); it++)
		{
			if(daysExperienced <= 5){ //this 5 needs to be taken out to the Json file - was already declared somwhere bu Gabi!!!
				(*it).second = (*it).second * totalOfAbundances; //however, to do this well, it really needs to take account of all food items in the last 5 days
			}else{
				(*it).second = (*it).second * totalOfAbundances; 
				(*it).second = (*it).second/5; ///experience averaged over the last 5 days and continue experiencing from 0 days
				daysExperienced = 0;	
			}
		}
	}
}

void Animal::assimilateLastHuntedAnimalAndComputeHandlingTime()
{

	//does not work foodMassLeftForNextTimeStep=0; //Dinosaurs only - 1 day handling time maximum TODO include the remains as carcass containers
					
	if(foodMassLeftForNextTimeStep > 0.0 && timeStepMaximumHandlingTimer > 0.0)
	{
		double kelvinTemperature = position->temperature + 273;
		double log_ratio = calculateLogMassRatio(calculateDryMass(), lastHuntedAnimalDryMass);
		double lnHandlingTime = -1814 + 0.7261*log_ratio + 12.04*kelvinTemperature + (-0.02006)*pow(kelvinTemperature, 2);
		double handlingTime = (1.0/exp(lnHandlingTime)) / (60 * 60 * 24); //Converted here to DAYS

		//TODO generalizar a timestep
		double handlingTimeToAdd = handlingTime * (foodMassLeftForNextTimeStep / lastHuntedAnimalDryMass);

		double addedWouldBeHandlingTimer = handlingTimer + handlingTimeToAdd;
		double foodMassToAdd;
		if(addedWouldBeHandlingTimer <= timeStepMaximumHandlingTimer)
		{
			foodMassToAdd = foodMassLeftForNextTimeStep;
			//cout << "first" << endl;
		}
		else //addedWouldBeHandlingTimer > timeStepMaximumHandlingTimer
		{
			if((timeStepMaximumHandlingTimer - handlingTimer) > 0.0)
			{
				foodMassToAdd = (timeStepMaximumHandlingTimer - handlingTimer) * (foodMassLeftForNextTimeStep / handlingTimeToAdd);
			} else {
				foodMassToAdd = 0.0;
			}

			//cout << "second" << endl;

		}

		//Converting the full dry mass to only profitable mass; that is applying assimilation efficiency and profitability
		double profitableFoodMassToAdd = foodMassToAdd * (edibleToBePredatedProfitability + getTrait(Trait::assim));

		double foodMassToAssimilate;

		double addedWouldBeFoodMass = foodMass + profitableFoodMassToAdd;
		if(round(addedWouldBeFoodMass*pow(10,7)) >= round(getTrait(Trait::voracity)*pow(10,7)))
		{

			foodMassToAssimilate = getTrait(Trait::voracity) - foodMass;
			foodMassLeftForNextTimeStep -= (foodMassToAssimilate / (edibleToBePredatedProfitability + getTrait(Trait::assim)));
			foodMass = getTrait(Trait::voracity);
			sated = true;

			//cout << "SECOND   " << endl;

			//cout << sated << endl; 
		}
		else //0 mass left on the ground OR do not eat anything at all
		{
			foodMassToAssimilate = profitableFoodMassToAdd;
			foodMassLeftForNextTimeStep -= foodMassToAdd;
			foodMass += foodMassToAssimilate;
			if(getRemainingVoracity() < 0.0000000001)
			{
				foodMass = getTrait(Trait::voracity);
				sated = true;

			/* cout << "THIRD   " << endl;
			cout << sated << endl; */
			}
			/*cout << "THIRD   " << endl;
			cout << sated << endl; */

		}

 		if(foodMassLeftForNextTimeStep < 0.00001) 
		{                                        
			foodMassLeftForNextTimeStep = 0.0;
		} 

		double handlingTimeToAssimilate = handlingTime * ((foodMassToAssimilate / (edibleToBePredatedProfitability + getTrait(Trait::assim))) / lastHuntedAnimalDryMass);

		//traits[Trait::energy_tank] += traits[Trait::assim] * foodMassToAssimilate;
		handlingTimer += handlingTimeToAssimilate;
		if(timeStepMaximumHandlingTimer > 0.0)
		{
			setTrait(Trait::search_area, getTrait(Trait::search_area) - (handlingTimeToAssimilate / timeStepMaximumHandlingTimer) * timeStepMaximumSearchArea);
		}

		if(::isnan(foodMass) || ::isnan(handlingTimer))
		{
			cout << "Animal id: " << this->getId() << " (" << this->getSpecies()->getScientificName() << ") - The assimilation resulted in a NaN value for foodMass or handlingTimer." << endl;
		}
	}
}

double Animal::calculatePredatoryRiskEdibilityValue(Edible* edibleToBeEvaluated)
{
	double edibilityValue = (calculateEncounterProbability(edibleToBeEvaluated)
			+ calculatePredationProbability(edibleToBeEvaluated, false)
			+ mySpecies->getEdiblePreference(edibleToBeEvaluated->getSpecies())
			+ getMeanExperience(edibleToBeEvaluated->getSpecies()))/4;

	return edibilityValue;
}

double Animal::calculateEdibilityValue(Edible* edibleToBeEvaluated)
{
	double edibilityValue = (calculateEncounterProbability(edibleToBeEvaluated)
			+ calculatePredationProbability(edibleToBeEvaluated, false)
			+ mySpecies->getEdiblePreference(edibleToBeEvaluated->getSpecies())
			+ getMeanExperience(edibleToBeEvaluated->getSpecies()))/4;

	return edibilityValue;
}

double Animal::calculateEdibilityValueWithMass(Edible* edibleToBeEvaluated)
{
	double edibilityValue = edibleToBeEvaluated->calculateDryMass()
			* ((calculateEncounterProbability(edibleToBeEvaluated)
			+ calculatePredationProbability(edibleToBeEvaluated, false)
			+ mySpecies->getEdiblePreference(edibleToBeEvaluated->getSpecies())
			+ getMeanExperience(edibleToBeEvaluated->getSpecies()))/4);

	return edibilityValue;
}

double Animal::calculateEncounterProbability(Edible* edibleToBeEncountered)
{
	double encounterProbability = 1.0;
	if(edibleToBeEncountered->getSpecies()->isMobile())
	{
		double normalizedHuntedVoracity = getNormalizedHuntedVoracity(edibleToBeEncountered->getVoracity());
		double normalizedHunterVoracity = getNormalizedHunterVoracity();
		double normalizedVoracityProduct = getNormalizedVoracityProduct(edibleToBeEncountered->getVoracity());

		double normalizedHuntedBodySize = getNormalizedHuntedBodySize(edibleToBeEncountered->getCurrentBodySize());
		double normalizedHunterBodySize = getNormalizedHunterBodySize();

		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/edibleToBeEncountered->calculateWetMass()) - position->getTheWorld()->getMuForPDF()) / position->getTheWorld()->getSigmaForPDF(), 2)) / (position->getTheWorld()->getSigmaForPDF() * sqrt(2*PI));
		double normalizedPDF = getNormalizedPDF(probabilityDensityFunction);

		if (huntingMode == SIT_AND_WAIT)// || isAnExposedAnimal)
		{
			encounterProbability = 
					(position->getTheWorld()->getEncounterHuntedVoracitySAW() * normalizedHuntedVoracity +
					position->getTheWorld()->getEncounterHunterVoracitySAW() * normalizedHunterVoracity +
					position->getTheWorld()->getEncounterVoracitiesProductSAW() * normalizedVoracityProduct +
					position->getTheWorld()->getEncounterHunterSizeSAW() * normalizedHunterBodySize +
					position->getTheWorld()->getEncounterHuntedSizeSAW() * normalizedHuntedBodySize +
					position->getTheWorld()->getEncounterProbabilityDensityFunctionSAW() * normalizedPDF)/6;
		}
		else if (huntingMode == ACTIVE_HUNTING)
		{
			encounterProbability = 
					(position->getTheWorld()->getEncounterHuntedVoracityAH() * normalizedHuntedVoracity +
					position->getTheWorld()->getEncounterHunterVoracityAH() * normalizedHunterVoracity +
					position->getTheWorld()->getEncounterVoracitiesProductAH() * normalizedVoracityProduct +
					position->getTheWorld()->getEncounterHunterSizeAH() * normalizedHunterBodySize +
					position->getTheWorld()->getEncounterHuntedSizeAH() * normalizedHuntedBodySize +
					position->getTheWorld()->getEncounterProbabilityDensityFunctionAH() * normalizedPDF)/6;

			/*encounterProbability = (1 / (1 + exp(-1 * (1 +
					position->getTheWorld()->getEncounterHuntedVoracityAH() * normalizedHuntedVoracity +
					position->getTheWorld()->getEncounterHunterVoracityAH() * normalizedHunterVoracity +
					position->getTheWorld()->getEncounterVoracitiesProductAH() * normalizedVoracityProduct +
					position->getTheWorld()->getEncounterHunterSizeAH() * normalizedHunterBodySize +
					position->getTheWorld()->getEncounterHuntedSizeAH() * normalizedHuntedBodySize +
					position->getTheWorld()->getEncounterProbabilityDensityFunctionAH() * normalizedPDF))));*/

		}
	}

	return encounterProbability;
}

bool Animal::encounterEdible(Edible* edibleToBeEncountered, float attackOrExposedAttackProbability, int day, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile)
{
	double randomEncounterProbability = Random::randomUniform();

	encounterProbabilitiesFile << getIdStr() << "\t"
	<< edibleToBeEncountered->getIdStr() << "\t"
	<< mySpecies->getScientificName() << "\t"
	<< edibleToBeEncountered->getSpecies()->getScientificName() << "\t"
	<< ((edibleToBeEncountered->getHuntingMode()!=DOES_NOT_HUNT)?1:0) << "\t"
	<< calculateDryMass() << "\t"
	<< edibleToBeEncountered->calculateDryMass() << "\t"
	<< randomEncounterProbability << "\t"
	<< calculateEncounterProbability(edibleToBeEncountered) << "\t"
	<< attackOrExposedAttackProbability << "\t"											 
	<< ((attackOrExposedAttackProbability*calculateEncounterProbability(edibleToBeEncountered) > randomEncounterProbability)?1:0) << "\t"
	<< endl;

	#ifdef _DEBUG
	if(find(searchedAnimalsToday.begin(), searchedAnimalsToday.end(), edibleToBeEncountered->getId()) != searchedAnimalsToday.end())
	{
		position->increaseMultipleSameSearchedAnimalToday();
		//cerr << "The animal searched for the same animal at least twice the same day: " << endl;
		//cerr << " - Animal: " << id << "(" << mySpecies->getScientificName() << ")" << endl;
		//cerr << " - Animal searched for: " << searchedAnimal->getId() << "(" << searchedAnimal->getSpecies()->getScientificName() << ")" << endl;
	}
	searchedAnimalsToday.push_back(edibleToBeEncountered->getId());
	#endif
	//if (encounterProbability > randomEncounterProbability)
	//*calculateEncounterProbability(edibleToBeEncountered)
	if ((attackOrExposedAttackProbability > randomEncounterProbability) || !edibleToBeEncountered->getSpecies()->isMobile())
	{
		#ifdef _DEBUG
		if(find(encounteredAnimalsToday.begin(), encounteredAnimalsToday.end(), edibleToBeEncountered->getId()) != encounteredAnimalsToday.end())
		{
			position->increaseMultipleSameEncounteredAnimalToday();
			//cerr << "The animal encountered the same animal at least twice the same day: " << endl;
			//cerr << " - Animal: " << id << "(" << mySpecies->getScientificName() << ")" << endl;
			//cerr << " - Animal encountered: " << searchedAnimal->getId() << "(" << searchedAnimal->getSpecies()->getScientificName() << ")" << endl;
		}
		encounteredAnimalsToday.push_back(edibleToBeEncountered->getId());
		#endif
		// if encounter occurs
		if(edibleToBeEncountered->getSpecies()->isMobile()){
		edibleToBeEncountered->incrementEncountersWithPredator(getId());
		}
		//incrementEncounters_prey();
		return true;
	}


	return false;
}

bool Animal::canEatEdible(Edible* edibleToCheck)
{
	//double log_mass_ratio = getLog_mass_ratio(this, edibleToCheck);
	//cout << "LMR -> " << log_mass_ratio << endl;
	
	//cout << edibleToCheck->isDepleted(traits[Trait::voracity]) << endl;
	
	//if(traits[Trait::voracity] < 0){
	//cout << timeStep << endl;	
	
	//cout << traits[Trait::voracity] << endl;
	//cout << edibleToCheck->getId() << endl;
	//exit(-1);
	//}

   //this was incorrect in Gabi's version, he used just fungus biomass vs min spore parameter
   //now considering everything the animal takes what is left by if available by decresing its voracity value
   double forNotToDepleteFungi = getTrait(Trait::voracity)/(mySpecies->getEdibleProfitability(edibleToCheck->getSpecies())+getTrait(Trait::assim));

  //this to to feed on what is left if he entire voracity cannot be satisfied
  //Warning: this prevents this animal to fulfill entirely its original voracity (voracity shrinks) with and alternative
 //fungus species if it were available - need to test this behavior
 
   
  //arthros and for dinos - this allows feeding on the leftovers without surpassing
  //the limit imposed by the minspores... so fungi can grow back
  bool newVor = false;
  if(edibleToCheck->isDepleted(forNotToDepleteFungi))
  {
	  if(edibleToCheck->anyLeft()*(mySpecies->getEdibleProfitability(edibleToCheck->getSpecies())+getTrait(Trait::assim))>0){
	  newVor = true;
      }
  }  
	
	return !sated &&
			!isExhausted() &&  
			(mySpecies->canEatAnimalSpecies(edibleToCheck->getSpecies()) || mySpecies->canEatFungusSpecies(edibleToCheck->getSpecies())) &&
			(!edibleToCheck->isDepleted(forNotToDepleteFungi) || newVor == true) &&
			edibleToCheck->getPredatedByID() == -1 &&
			predatedByID == -1 &&
			!hasTriedToHunt(edibleToCheck);
	// && log_mass_ratio >= position->theWorld->getMinLogMassRatio() && log_mass_ratio <= position->theWorld->getMaxLogMassRatio()/*6.678*/;
}

bool Animal::hasTriedToHunt(Edible* edibleToCheck)
{
	return find(ediblesHasTriedToPredate.begin(), ediblesHasTriedToPredate.end(), edibleToCheck) != ediblesHasTriedToPredate.end();
}
