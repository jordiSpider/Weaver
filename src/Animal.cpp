	/*
 * Animal.cpp
 *
 *  Created on: 20 Nov 2012
 *      Author: gabi
 */

#include "Animal.h"
#include "TerrainCellInterface.h"
#include "World.h"
#include "Resource.h"
#include "LineInfoException.h"
#include "Maths/Random.h"
#include "AnimalSpecies.h"
#include <cfloat>


id_type Animal::animalCounter = 0;

Animal::Animal(double factorEggMassFromMom, TerrainCellInterface* position, double timeStep, double timeStepsPerDay, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, unsigned int gender,
		WorldInterface* worldInterface, bool temporary) 
	: Edible(mySpecies, temporary), genome(getSpecies()->getLoci(), getSpecies()->getRandomlyCreatedPositionsForChromosomes(), getSpecies()->getNumberOfChromosomes(), 
	  getSpecies()->getNumberOfLociPerChromosome(), getSpecies()->getNumberOfChiasmasPerChromosome()), huntingMode(getSpecies()->getDefaultHuntingMode()),
	  lifeStage(LifeStage::ACTIVE), worldInterface(worldInterface), variableTraits(getSpecies()->getTypeVariableTraits())
{
	if(!temporary) {
		Animal::animalCounter++;
	}

	setOtherAttributes(factorEggMassFromMom, position, timeStep, timeStepsPerDay, g_numb_prt1,
					   g_numb_prt2, ID_prt1, ID_prt2, gender);
}

Animal::Animal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, double factorEggMassFromMom, TerrainCellInterface* position, double timeStep, double timeStepsPerDay, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, unsigned int gender,
		WorldInterface* worldInterface, bool temporary) 
	: Edible(mySpecies, temporary), genome(firstParentGamete, secondParentGamete, getSpecies()->getRandomlyCreatedPositionsForChromosomes(), 
	  getSpecies()->getNumberOfLociPerChromosome(), getSpecies()->getNumberOfChiasmasPerChromosome()), huntingMode(getSpecies()->getDefaultHuntingMode()),
	  lifeStage(LifeStage::UNBORN), worldInterface(worldInterface), variableTraits(getSpecies()->getTypeVariableTraits())
{
	if(!temporary) {
		Animal::animalCounter++;
	}

	setOtherAttributes(factorEggMassFromMom, position, timeStep, timeStepsPerDay, g_numb_prt1,
					   g_numb_prt2, ID_prt1, ID_prt2, gender);
}

void Animal::setOtherAttributes(double eggMassAtBirth, TerrainCellInterface* position, double timeStep, double timeStepsPerDay, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, unsigned int gender)
{
	this->eggDryMassAtBirth = -1;
	this->factorEggMassFromMom = factorEggMassFromMom;
	mated = false;
	genomeFromMatedMale = NULL;
	idFromMatedMale = -1;
	this->position = position;
	this->gender = gender;
	this->predatedByID = -1;
	this->setMassAtBirth(getSpecies()->getEggDryMass());

	getSpecies()->initializeFixedTraits(traits);

	steps = 0;
	stepsAttempted = 0;
	oldSearchArea = 0; //this was created to keep the original search area before helping out of the cell
	nextDinoMassPredicted = 0;
	foodMass = 0;
	finalJMaxVB = 0;
	sated = false;
	edibleToBePredatedProfitability = 1.0;
	setFoodMassLeftForNextTimeStep(0.0);
	lastHuntedAnimalDryMass = 1.0; //To avoid divisions by 0
	//encounters_prey = 0;
	todayEncountersWithPredators = 0;
	daysWithoutFood = 0;
	//total_prey_encs = 0;
	//totalEncountersWithPredators = 0;
	mature = false;
	currentAge = 0; //Dinosaurs
    daysExperienced = 0;

	actualMoltingTimeVector.reserve(getSpecies()->getNumberOfInstars()-1);
	actualMoltingMassVector.reserve(getSpecies()->getNumberOfInstars()-1);
	finalDevTimeVector.reserve(getSpecies()->getNumberOfInstars()-1);
	for (int i = 0; i < getSpecies()->getNumberOfInstars()-1; ++i)
	{
		finalDevTimeVector.push_back(-1);
	}
	lengthsVector.reserve(getSpecies()->getNumberOfInstars()-1); //Dinosaurs ini
	for (int i = 0; i < getSpecies()->getNumberOfInstars()-1; ++i)
	{
		lengthsVector.push_back(-1);
	}
	massesVector.reserve(getSpecies()->getNumberOfInstars()-1);
	for (int i = 0; i < getSpecies()->getNumberOfInstars()-1; ++i)
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
	wetMassAtTheBeginningOfTheTimeStep = eggDryMassAtBirth*getSpecies()->getConversionToWetMass();
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

	switch(getSpecies()->getGrowthCurve()->getType()) {
		case CurveType::VonBertalanffy: {
			growthCurveParams = new VonBertalanffyCurveParams();
			break;
		}
		case CurveType::Logistic: {
			growthCurveParams = new LogisticCurveParams();
			break;
		}
		case CurveType::Logistic3P: {
			growthCurveParams = new Logistic3PCurveParams();
			break;
		}
		case CurveType::Logistic4P: {
			growthCurveParams = new Logistic4PCurveParams();
			break;
		}
		case CurveType::Gompertz: {
			growthCurveParams = new GompertzCurveParams();
			break;
		}
		case CurveType::Exponential: {
			growthCurveParams = new ExponentialCurveParams();
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

Animal::~Animal()
{
	if(genomeFromMatedMale != NULL)	delete genomeFromMatedMale;

	switch(getSpecies()->getGrowthCurve()->getType()) {
		case CurveType::VonBertalanffy: {
			delete reinterpret_cast<VonBertalanffyCurveParams*>(growthCurveParams);
			break;
		}
		case CurveType::Logistic: {
			delete reinterpret_cast<LogisticCurveParams*>(growthCurveParams);
			break;
		}
		case CurveType::Logistic3P: {
			delete reinterpret_cast<Logistic3PCurveParams*>(growthCurveParams);
			break;
		}
		case CurveType::Logistic4P: {
			delete reinterpret_cast<Logistic4PCurveParams*>(growthCurveParams);
			break;
		}
		case CurveType::Gompertz: {
			delete reinterpret_cast<GompertzCurveParams*>(growthCurveParams);
			break;
		}
		case CurveType::Exponential: {
			delete reinterpret_cast<ExponentialCurveParams*>(growthCurveParams);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}

	MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(getPosition());

	if(macroCell != nullptr) {
		delete reinterpret_cast<MacroTerrainCell*>(getPosition());
	}
}

void Animal::initTraits()
{
	//TODO Here each group can have 2 or 3 traits. This MUST be implemented.
	//Correlations of EACH group can be positive or negative. This is already implemented.
	//TODO Maybe, move this method to the class Genome so it will be all well organized (see variables involved).
	int distanceFromDominant;
	int moduleNumber;

	for(size_t i = 0; i < getSpecies()->getNumberOfVariableTraits(); i++)
	{
		unsigned int trait_order = getSpecies()->getTraitOrder(variableTraits[i]);
		moduleNumber = trait_order / getSpecies()->getTraitsPerModule();
		//The division is made using RHO. For every trait, the left side alleles of their own chromosomes must be added.
		for (int j = 0; j < getSpecies()->getRhoRangePerModule(moduleNumber); ++j) {
			if(genome.getHomologousCorrelosomes().at(trait_order).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(trait_order).second->getAllele(j)->getAlphabeticOrder())
			{
				traits[variableTraits[i]] += genome.getHomologousCorrelosomes().at(trait_order).first->getAllele(j)->getValue();
			}
			else
			{
				traits[variableTraits[i]] += genome.getHomologousCorrelosomes().at(trait_order).second->getAllele(j)->getValue();
			}
		}

		//The right side depends on two factors: the sign for RHO for the current module and the dominance of the chromosome.
		distanceFromDominant = trait_order % getSpecies()->getTraitsPerModule();

		//If RHO is positive, for every trait the right side alleles of the dominant chromosome must be added.
		if(getSpecies()->getRhoPerModule(moduleNumber) >= 0)
		{
			for (unsigned int j = getSpecies()->getRhoRangePerModule(moduleNumber); j < getSpecies()->getNumberOfLociPerTrait(); ++j)
			{
				if(genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).second->getAllele(j)->getAlphabeticOrder())
				{
					traits[variableTraits[i]] += genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).first->getAllele(j)->getValue();
				}
				else
				{
					traits[variableTraits[i]] += genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).second->getAllele(j)->getValue();
				}

			}
		}
		//If RHO is negative.
		else {
			//If the trait IS dominant, the right side alleles of its chromosome must be added.
			if(distanceFromDominant == 0)
			{
				for (unsigned int j = getSpecies()->getRhoRangePerModule(moduleNumber); j < getSpecies()->getNumberOfLociPerTrait(); ++j)
				{
					if(genome.getHomologousCorrelosomes().at(trait_order).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(trait_order).second->getAllele(j)->getAlphabeticOrder())
					{
						traits[variableTraits[i]] += genome.getHomologousCorrelosomes().at(trait_order).first->getAllele(j)->getValue();
					}
					else
					{
						traits[variableTraits[i]] += genome.getHomologousCorrelosomes().at(trait_order).second->getAllele(j)->getValue();
					}
				}
			}
			//If the trait is NOT dominant, 1 - the right side alleles of the dominant chromosome must be added.
			else
			{
				for (unsigned int j = getSpecies()->getRhoRangePerModule(moduleNumber); j < getSpecies()->getNumberOfLociPerTrait(); ++j)
				{
					if(genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).second->getAllele(j)->getAlphabeticOrder())
					{
						traits[variableTraits[i]] += (1.0 - genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).first->getAllele(j)->getValue());
					}
					else
					{
						traits[variableTraits[i]] += (1.0 - genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).second->getAllele(j)->getValue());
					}
				}
			}
		}
	}

	deleteHomologousCorrelosomes();

	//TODO See if we need to restrict the values to something "plausible"
	//and to avoid negative traits HERE. Maybe we don't need to do it
	//if trait<low_limit then low limit, if trait>up_limit then up_limit

	//TODO Should I save ALL the initial TRAITS for using them later?
}

void Animal::adjustTraits()
{
	if(getSpecies()->getGrowthCurve()->getType() == CurveType::Logistic || getSpecies()->getGrowthCurve()->getType() == CurveType::VonBertalanffy)
	{
		//pseudoGrowthMean and pseudoGrowthSd have been calculated only using the first generation
		double normalizedPseudoGrowth = (pseudoGrowth - getSpecies()->getPseudoGrowthMean()) / getSpecies()->getPseudoGrowthSd();
		double valueFromNormalDistributionForCholesky = getSpecies()->getValueFromNormalDistributionY();

		//double Knorm = normalizedPseudoGrowth * getSpecies()->cholMat[0][0];
		double Linf = normalizedPseudoGrowth * getSpecies()->getValueFromCholMat(1,0) + valueFromNormalDistributionForCholesky * getSpecies()->getValueFromCholMat(1,1);
		//ASÍ ERA ANTES:
		//Linf = (getSpecies()->vonBertLinf/getSpecies()->vonBertKini)*traits[Trait::growth]+getSpecies()->LinfKcorr*randomUniform();

		//Controlling the range of Linfs for adults
		//Here we use OriginLimits because pseudoGrowth was NOT interpolated
		double minNormalizedPseudoGrowth = (getSpecies()->getMinObservedPseudoValue(Trait::growth) - getSpecies()->getPseudoGrowthMean()) / getSpecies()->getPseudoGrowthSd();
		double maxNormalizedPseudoGrowth = (getSpecies()->getMaxObservedPseudoValue(Trait::growth) - getSpecies()->getPseudoGrowthMean()) / getSpecies()->getPseudoGrowthSd();
		//TODO -3.5 y 3.5 porque son los valores mínimos y máximos fijados artificialmente por nosotros para la distribución normal Y
		double minLinf = minNormalizedPseudoGrowth * getSpecies()->getValueFromCholMat(1,0) + (-3.5) * getSpecies()->getValueFromCholMat(1,1);
		double maxLinf = maxNormalizedPseudoGrowth * getSpecies()->getValueFromCholMat(1,0) + (3.5) * getSpecies()->getValueFromCholMat(1,1);

		switch(getSpecies()->getGrowthCurve()->getType()) {
			case CurveType::VonBertalanffy: {
				const VonBertalanffyCurve* const castGrowthCurve = static_cast<const VonBertalanffyCurve* const>(getSpecies()->getGrowthCurve());

				Linf = Math_Functions::linearInterpolate(Linf, minLinf, maxLinf, castGrowthCurve->getLdistanceMin(), castGrowthCurve->getLdistanceMax());

				static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->setAsymptoticSize(Linf);
				break;
			}
			case CurveType::Logistic: {
				const LogisticCurve* const castGrowthCurve = static_cast<const LogisticCurve* const>(getSpecies()->getGrowthCurve());

				Linf = Math_Functions::linearInterpolate(Linf, minLinf, maxLinf, castGrowthCurve->getLdistanceMin(), castGrowthCurve->getLdistanceMax());

				static_cast<LogisticCurveParams* const>(growthCurveParams)->setAsymptoticSize(Linf);
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}
	}

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
	setDryMass(getSpecies()->getEggDryMass());

	//cout << traits[Trait::energy_tank] << endl;
	//Dinosaurs fixed part end
	
	eggDryMassForClutch=getSpecies()->getEggDryMass(); //this is just a dummy initialization


}

void Animal::increaseAge(int increase)
{
	currentAge += increase;
}

const double& Animal::getTrait(const TraitType& type) const
{
	return traits[type];
}

void Animal::setTrait(const TraitType& type, const double& newValue)
{
	traits[type] = newValue;
}

double Animal::getValueGrowthCurve(const double &age, const double &midpointValue)
{
	growthCurveParams->setGrowthCoefficient(getTrait(Trait::growth));
	growthCurveParams->setTime(age);
	switch(getSpecies()->getGrowthCurve()->getType()) {
		case CurveType::VonBertalanffy: {
			break;
		}
		case CurveType::Logistic: {
			static_cast<LogisticCurveParams* const>(growthCurveParams)->setMidpointValue(midpointValue);
			break;
		}
		case CurveType::Logistic3P: {
			break;
		}
		case CurveType::Logistic4P: {
			break;
		}
		case CurveType::Gompertz: {
			break;
		}
		case CurveType::Exponential: {
			static_cast<ExponentialCurveParams* const>(growthCurveParams)->setValueTime0(lengthAtBirth);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}

	return getSpecies()->getGrowthCurve()->getValue(*growthCurveParams);
}

void Animal::tuneTraits(int timeStep, int timeStepsPerDay, float temperature, float relativeHumidity, ostream& tuneTraitsFile, bool printGrowthData, bool fromForceMolting1, SimulType simulType)
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
	double ToptVoracity = 273 + getSpecies()->getTempOptVoracity();
	double ToptSearch = 273 + getSpecies()->getTempOptSearch();
	double ToptSpeed = 273 + getSpecies()->getTempOptSpeed();

	//the two values below are now going to be used again
	double preTsearch = search_area_ini*pow(calculateWetMass(), getSpecies()->getScaleForSearchArea());
	
	
	double preTspeed = (speed_ini*pow(calculateWetMass(), getSpecies()->getScaleForSpeed()))*(1-exp(-22*pow(calculateWetMass(),-0.6)));  //25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017

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
	maxPostTvor = postTvor + postTvor * getSpecies()->maxPlasticityDueToConditionVor;
	minPostTvor = postTvor - postTvor * getSpecies()->minPlasticityDueToConditionVor;
	maxPostTsearch = postTsearch + postTsearch * getSpecies()->maxPlasticityDueToConditionSearch;
	minPostTsearch = postTsearch - postTsearch * getSpecies()->minPlasticityDueToConditionSearch;
	maxPostTspeed = postTspeed + postTspeed * getSpecies()->maxPlasticityDueToConditionSpeed;
	minPostTspeed = postTspeed - postTspeed * getSpecies()->minPlasticityDueToConditionSpeed;
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

    //double minMassAtCurrentAge = currentBodySize + currentBodySize * getSpecies()->getMinPlasticityKVonBertalanffy();

	//Forcing continuous growth in Dinosaurs
   /*	double dinoTankPredicted = tank_ini*pow(calculateDryMass(),getSpecies()->getBetaScaleTank());
	currentBodySize = getSpecies()->getAssignedForMolt()*(calculateDryMass() - dinoTankPredicted);
	traits[Trait::energy_tank] = getSpecies()->getAssignedForMolt()*dinoTankPredicted;*/

	lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		
	//fix_xmid_K =  log((Asym-trex[1,"FC_Km"])/trex[1,"FC_Km"])*(1/target_K)
	
	double asymptoticSize = 0.0;
	switch(getSpecies()->getGrowthCurve()->getType()) {
		case CurveType::VonBertalanffy: {
			asymptoticSize = static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->getAsymptoticSize();
			break;
		}
		case CurveType::Logistic: {
			asymptoticSize = static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize();
			break;
		}
		default: {
			break;
		}
	}

	xmid = log((asymptoticSize-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
       
	//Forcing continuous growth in Dinosaurs - warning this involves heavy investment in growth - reproduction?
   	double dinoLengthPredicted = getValueGrowthCurve(currentAge, xmid);
	//if(currentAge > 5){ //to surpass the critical young stages

	double dinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(dinoLengthPredicted,getSpecies()->getScaleForMassBforMature());

    //dinos2023 - moulting only if realized mass is > predicted mass
 	if(simulType == SimulType::dinosaurs && instar < getSpecies()->getNumberOfInstars()){
		if(calculateDryMass() > dinoMassPredicted){	
			double dinoTankPredicted = tank_ini*pow(calculateDryMass(),getSpecies()->getBetaScaleTank());
			
			setDryMass(
				getSpecies()->getAssignedForMolt()*dinoTankPredicted,
				getSpecies()->getAssignedForMolt()*(calculateDryMass() - dinoTankPredicted)
			);
		}
	 }
	//end dinos2023

//not for arthropods only for debugging 	
/* if(calculateDryMass() > dinoMassPredicted){ //(currentAge>5) && (
    double dinoTankPredicted = tank_ini*pow(dinoMassPredicted,getSpecies()->getBetaScaleTank());
	double dinoBodyPredicted = dinoMassPredicted - dinoTankPredicted;
    currentBodySize = dinoBodyPredicted;
	traits[Trait::energy_tank] = getSpecies()->getAssignedForMolt()*(calculateDryMass()-currentBodySize);
	//cout << "holar" << endl;
	}   */  
//END - not for arthropods only for debugging 

	//death criterion fixed to the true plasticity bands - Dinosaurs
	double minMassAtCurrentAge;
	double longevity = getSpecies()->getLongevitySinceMaturation()*ageOfFirstMaturation;

/* 	cout << ageOfFirstMaturation << endl;
	exit(-1); */

	if (currentAge>(0.15*longevity)){ //arthropod to prevent the tiny animals from dying too soon...
	minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * getSpecies()->getMinPlasticityKVonBertalanffy();
	}else{
    minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * 3 * getSpecies()->getMinPlasticityKVonBertalanffy();
	}
//Dinosaurs
/* 	double minMassAtCurrentAge = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(currentAge-thisAnimalVonBertTime0)));
	minMassAtCurrentAge = 0.1*getSpecies()->getCoefficientForMassAforMature()*pow(minMassAtCurrentAge,getSpecies()->getScaleForMassBforMature()); //+ getSpecies()->getMinPlasticityKVonBertalanffy()*getSpecies()->getCoefficientForMassAforMature()*pow(minMassAtCurrentAge,getSpecies()->getScaleForMassBforMature());
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
		if(currentAge == 0 || currentAge == 1){
			nextDinoLengthPredicted = getValueGrowthCurve(1, xmid);
		}
		else {
			nextDinoLengthPredicted = getValueGrowthCurve(currentAge+1, xmid);
		}

		nextDinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());


	
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

				//nextDinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	
				slopeTarget = 0.0;
				interceptTarget = 0.0;
			}else{//force currentAge-1 for calculations as to avoid nans
				slopeTarget = (massForNextReproduction - calculateDryMass())/(ageForNextReproduction-(currentAge-1));
				interceptTarget = calculateDryMass()-slopeTarget*(currentAge-1);
				nextDinoMassPredicted = interceptTarget + slopeTarget*currentAge;
			    //nextDinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	
				
				
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
				//nextDinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	
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



		

    minTotalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, temperature, 0, timeStepsPerDay, simulType);

    minNextDinoMassPredicted = nextDinoMassPredicted - nextDinoMassPredicted * getSpecies()->getMinPlasticityKVonBertalanffy();
	
	maxNextDinoMassPredicted = nextDinoMassPredicted + nextDinoMassPredicted * getSpecies()->getMaxPlasticityKVonBertalanffy();

    targetMass = maxNextDinoMassPredicted - calculateDryMass();

	if(targetMass<0){
		targetMass = 0;		
	}

	targetMass = targetMass + minTotalMetabolicDryMassLoss;	
	
	}
	else //isMature == true
    {

					/* double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
					eggDryMassForClutch = getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB())+getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB()) * traits[Trait::factorEggMass];

					if(getSpecies()->hasEggClutchFromEquation())
					{
						clutchDryMass = (getSpecies()->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, getSpecies()->getForClutchMassScale())) / getSpecies()->getConversionToWetMass();
					}
					else //If it does not come from equation,
					{
						clutchDryMass = eggDryMassForClutch*getSpecies()->getEggsPerBatch(); //clutch dry mass is identical to the mass of an egg - done for mites or others that only lay one egg at a time.
					} */




	minTotalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, temperature, 0, timeStepsPerDay, simulType);

    minNextDinoMassPredicted = nextDinoMassPredicted - nextDinoMassPredicted * getSpecies()->getMinPlasticityKVonBertalanffy();
	
	maxNextDinoMassPredicted = nextDinoMassPredicted + nextDinoMassPredicted * (getSpecies()->getMaxPlasticityKVonBertalanffy()*1.0);

    
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

   /* if(getSpecies()->getId() == 1){
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
			getSpecies()->getEdVoracity(),
			getTrait(Trait::actE_vor)
			);

    double dellsMinT = Math_Functions::dells(
    		0+273,
			ToptVoracity,
			getSpecies()->getEdVoracity(),
			getTrait(Trait::actE_vor)
			);

    double dellsMaxT = Math_Functions::dells(
    		ToptVoracity,
			ToptVoracity,
			getSpecies()->getEdVoracity(),
			getTrait(Trait::actE_vor)
    		);

    double postTvor = Math_Functions::linearInterpolate(
    		dellsNewT,
			dellsMinT,
			dellsMaxT,
			getSpecies()->getMinVorExtremeT(),
			voracity_ini
			);

    /*cout << "newT: " << newT << endl;
    cout << "dellsNewT: " << dellsNewT << endl;
    cout << "dellsMinT: " << dellsMinT << endl;
    cout << "dellsMaxT: " << dellsMaxT << endl;

    cout << "getSpecies()->getMinVorExtremeT(): " << getSpecies()->getMinVorExtremeT() << endl;
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
			preFinalVoracity - preFinalVoracity * getSpecies()->getMinPlasticityDueToConditionVor(),
			preFinalVoracity + preFinalVoracity * getSpecies()->getMaxPlasticityDueToConditionVor()
			);

				
    //below we ensure that voracity does not get into negative values
	setTrait(Trait::voracity, max(finalVoracity, 0.0));

	//cout << "here: " << traits[Trait::voracity]/calculateDryMass() << endl;

    //BELOW WE INCLUDE TEMP TUNING FOR SEARCH AREA AND SPEED
    double dellsNewTsearch = Math_Functions::dells(newT, ToptSearch, getSpecies()->getEdSearch(), getTrait(Trait::actE_search));
    double dellsMinTsearch = Math_Functions::dells(0+273, ToptSearch, getSpecies()->getEdSearch(), getTrait(Trait::actE_search));
    double dellsMaxTsearch = Math_Functions::dells(ToptSearch, ToptSearch, getSpecies()->getEdSearch(), getTrait(Trait::actE_search));
    postTsearch = Math_Functions::linearInterpolate(
    		dellsNewTsearch,
			dellsMinTsearch,
			dellsMaxTsearch,
			preTsearch - preTsearch * getSpecies()->getFractSearchExtremeT(),
			preTsearch);
	
	postTsearch = preTsearch; //this is to have more predictive mobilities

    double dellsNewTspeed = Math_Functions::dells(newT, ToptSpeed, getSpecies()->getEdSpeed(), getTrait(Trait::actE_speed));
    double dellsMinTspeed = Math_Functions::dells(0+273, ToptSpeed, getSpecies()->getEdSpeed(), getTrait(Trait::actE_speed));
    double dellsMaxTspeed = Math_Functions::dells(ToptSpeed, ToptSpeed, getSpecies()->getEdSpeed(), getTrait(Trait::actE_speed));
    postTspeed = Math_Functions::linearInterpolate(
    		dellsNewTspeed,
    		dellsMinTspeed,
			dellsMaxTspeed,
			preTspeed - preTspeed * getSpecies()->getFractSpeedExtremeT(),
			preTspeed);

    //BELOW WE INCLUDE CONDITION-DEPENDENT PLASTICITY FOR SEARCH AREA AND SPEED
    finalSearch = Math_Functions::linearInterpolate(
    		targetMass,
    		0,
			fullTrajectoryMass,
			postTsearch - postTsearch*getSpecies()->getMinPlasticityDueToConditionSearch(),
			postTsearch + postTsearch*getSpecies()->getMaxPlasticityDueToConditionSearch());

    finalSearch = postTsearch; //this is to have more predictive mobilities
   
    maxPostTsearch = postTsearch + postTsearch * getSpecies()->getMaxPlasticityDueToConditionSearch();
	setTrait(Trait::search_area, finalSearch);

    //traits[Trait::search_area] = preTsearch;

    finalSpeed = Math_Functions::linearInterpolate(
    		targetMass,
			0,
			fullTrajectoryMass,
			postTspeed - postTspeed * getSpecies()->getMinPlasticityDueToConditionSpeed(),
			postTspeed + postTspeed * getSpecies()->getMaxPlasticityDueToConditionSpeed()
			);

	setTrait(Trait::speed, finalSpeed);

/* if(getId() == 3 && timeStep==15){
		cout << nextDinoMassPredicted << endl;
		//exit(-1);
		}  */

	if(::isnan(finalSpeed) || ::isnan(finalSearch) || ::isnan(finalVoracity) || ::isinf(finalSpeed) || ::isinf(finalSearch) || ::isinf(finalVoracity))
	{
		setNewLifeStage(LifeStage::SHOCKED, timeStep);
		return;
		/*
		cout << "Animal id: " << this->getId() << " (" << this->getSpecies()->getScientificName() << ") - The growing curve resulted in a NaN value." << endl;
		cout << finalVoracity << endl;
		cout << finalSpeed << endl;
		cout << finalSearch << endl;
		exit(-1);
		*/
	}

	

	//below we ensure that search area does not get into negative values
	setTrait(Trait::search_area, max(finalSearch, 0.0));
	//below we ensure that speed does not get into negative values
	setTrait(Trait::speed, max(finalSpeed, 0.0));
	//Jordi ends here::::

	 //below we establish a decrease in activity due to previous encounters with predators
    //the punishment on the traits depends on the parameter decreaseOnTraitsDueToEncounters
	double vorAfterEncounters = getTrait(Trait::voracity) / (getSpecies()->getDecreaseOnTraitsDueToEncounters()*todayEncountersWithPredators+1);
	double searchAfterEncounters = getTrait(Trait::search_area) / (getSpecies()->getDecreaseOnTraitsDueToEncounters()*todayEncountersWithPredators+1);

	//vorAfterEncounters=min(vorAfterEncounters,maxNextInstarMassFromVBPlasticity);

   	if(vorAfterEncounters+calculateDryMass() > maxNextDinoMassPredicted){
		if(calculateDryMass() < maxNextDinoMassPredicted){
		vorAfterEncounters = maxNextDinoMassPredicted - calculateDryMass(); //min(vorAfterEncounters+calculateDryMass(),maxNextDinoMassPredicted)-calculateDryMass();
		}else{
		/* if(getSpecies()->getScientificName() == "Microbisium_sp" && instar+2 == 4){
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
	switch (simulType) {
		case SimulType::dinosaurs: {
			double Garland1983 = ((152*pow((calculateWetMass()),0.738))/1000)/getSpecies()->getConversionToWetMass();
		
			/* if(traits[Trait::voracity] > 60){
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
			break;
		}
		case SimulType::arthropods: {
			double AmNat = (0.1*pow((calculateWetMass()),0.75))/getSpecies()->getConversionToWetMass();
		
			if(AmNat < getTrait(Trait::voracity)){
				setTrait(Trait::voracity, AmNat);
			}  
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
	//end Dinos2023


	//We assume that encounters DO NOT AFFECT the animal speed - although in reality stamina should be affected
	//However this affects also to the predators so we do not consider this for now
	//traits[AnimalSpecies::TraitOrderConverter::speed] = traits[AnimalSpecies::TraitOrderConverter::speed] / (getSpecies()->decreaseOnTraitsDueToEncounters*todayEncountersWithPredators+1);

	//traits[Trait::energy_tank] < 0
	if(getTrait(Trait::energy_tank) < 0) //Dinosaurs calculateDryMass() < minMassAtCurrentAge
	{
		if(getSpecies()->isSurviveWithoutFood()) {
			setTrait(Trait::energy_tank,0.1);
		}
		else {
			setNewLifeStage(LifeStage::STARVED, timeStep);
		}
	}
	
	/*else if(minMassAtCurrentAge >= finalJMaxVB+calculateDryMass())//Jordi note: I do not follow why this sentence //Gabi: prevents from crashing when the limits are reverted (due to disadjudments in the curve values)
	{
		//cout << "For species " << getSpecies()->getScientificName() << ": maxKplasticityForVonBertalanfy and minKplasticityForVonBertalanfy are not set correctly. Modify these values and re-run..." << endl;
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
		setNewLifeStage(LifeStage::BACKGROUND, timeStep);
	}
	}else{  */
	//arthros and dinos to create patches of enemy free space - kill all predators in designated patches for initialization

	//create patches with predator (enemy) free space at initialization
	
	//background mortality
	//if(getId() != 0){
/*	if(position->getTheWorld()->getSimulType() == 1 && getSpecies()->getScientificName() == "Tyrannosaurus_sp"){  //for Trex
			double GompertzA = 0.002;
			double GompertzG = 0.2214;
			double currentAgeInYears = currentAge*371;
			double prop_alive = exp((GompertzA/GompertzG)*(1 - exp(GompertzG*currentAgeInYears)));
			if (((1-prop_alive)/371) > Random::randomUniform()) //prob death per day
			{
				setNewLifeStage(LifeStage::BACKGROUND, timeStep);
			}	
		
	}else{ *///arthropods & other dinosaurs
    
	//}
	//}
	

	if(isMature() && gender == AnimalSpecies::GENDERS::MALE)
	{
		setTrait(Trait::search_area, getTrait(Trait::search_area) * getSpecies()->getMaleMobility());
	}

	//TODO PARA LOS CRECIMIENTO INDETERMINADO, SE DEJAN POR AHORA QUE SIGAN MOVIENDOSE INFINITO
	//TODO EN UN FUTURO SE HARÁ PARA LOS INDETERMINADO, DEJANDO QUE SIGAN CRECIENDO SI ALCANZAN EL TIEMPO DE LONGEVITY
	if(currentAge >= ageOfFirstMaturation * getSpecies()->getLongevitySinceMaturation())
	{
		setNewLifeStage(LifeStage::SENESCED, timeStep);
	}


	//below it means that metabolic downregulation only exists for predators, such as spiders
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		setTrait(Trait::voracity, getTrait(Trait::voracity) - getSpecies()->getPercentageCostForMetabolicDownregulationVoracity() * getTrait(Trait::voracity));
		setTrait(Trait::search_area, getTrait(Trait::search_area) - getSpecies()->getPercentageCostForMetabolicDownregulationSearchArea() * getTrait(Trait::search_area));
		setTrait(Trait::speed, getTrait(Trait::speed) - getSpecies()->getPercentageCostForMetabolicDownregulationSpeed() * getTrait(Trait::speed));
	}

	//In this version the shock_resistance trait involves Maximum Critical Temperature (CTmax)
	if(getTrait(Trait::shock_resistance)<(temperature+273))
	{
		setNewLifeStage(LifeStage::SHOCKED, timeStep);
	}

	if(relativeHumidity < getSpecies()->getMinRelativeHumidityThreshold())
	{
		setNewLifeStage(LifeStage::DIAPAUSE);
	}



	if(printGrowthData)
	{
		tuneTraitsFile << getIdStr() << "\t"
		<< getSpecies()->getScientificName() << "\t"
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

	//TODO Eliminar el hongo cuando sea <= 0, y borrar minimumresourcecapacity y getZeroResource.
	//TODO Añadir un parametro que decida si el animal crece por mudas o continuo (dinosaurios)






}

double Animal::getTotalMetabolicDryMassLoss(double wetMass, double temperature, double proportionOfTimeTheAnimalWasMoving, int timeStepsPerDay, SimulType simulType)
{
	double totalMetabolicDryMassLoss = 0;

	switch (simulType) {
		case SimulType::dinosaurs: {
			//here Grady et al. 2014 provide results in Watts (j/s) and M in g
			double basalMetabolicTax = 0.002*pow(wetMass*1000,getTrait(Trait::met_rate));

			double fraction_with_stress = (Math_Functions::linearInterpolate(todayEncountersWithPredators, 0, getSpecies()->getMaxEncountersT(), 0, 0.407)/timeStepsPerDay)*basalMetabolicTax;

			//TODO multiplicar por timeStep
			//24*3600 because the metab rates are given in J/s.
			basalMetabolicTax = basalMetabolicTax + fraction_with_stress;
			
			double distanceMoved = proportionOfTimeTheAnimalWasMoving*getTrait(Trait::search_area);
			double field_met_tax = distanceMoved*(10.7*pow(calculateWetMass(),0.68)); //Calder 2016 for mammals - Jouls
			//field_met_tax = 0;
			//Remove this and use cost of transport in Calders
			//double loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24*3600;
			//TODO This 3 is a raw value from bibl.
			//double field_met_tax = 3*basalMetabolicTax;
			//double loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24*3600;

			double loss_from_bmr = basalMetabolicTax*24*3600;
			double loss_from_fmr = field_met_tax;

			//7 is NOT referred to weeks. Conversion from Jules.
			loss_from_bmr=loss_from_bmr/7;//7 joule = 1mg 
			loss_from_fmr=loss_from_fmr/7;
			//here we transform from mg to Kg to dinoWeaver
			totalMetabolicDryMassLoss = ((loss_from_bmr + loss_from_fmr)*0.000001) / getSpecies()->getConversionToWetMass();
			break;
		}
		case SimulType::arthropods: {
			double basalMetabolicTax = exp(-7.2945+43.966*getTrait(Trait::actE_met)+getTrait(Trait::met_rate)*log(wetMass)-getTrait(Trait::actE_met)*(1/((temperature+273.15)*BOLZMANN)));

			double fraction_with_stress = (Math_Functions::linearInterpolate(todayEncountersWithPredators, 0, getSpecies()->getMaxEncountersT(), 0, 0.407)/timeStepsPerDay)*basalMetabolicTax;

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
			totalMetabolicDryMassLoss = (loss_from_bmr + loss_from_fmr) / getSpecies()->getConversionToWetMass();
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}

	totalMetabolicDryMassLoss = totalMetabolicDryMassLoss / timeStepsPerDay;

	return totalMetabolicDryMassLoss;
}

//TODO parametro para que se ejecuten cada X timesteps
void Animal::metabolize(int timeStep, int timeStepsPerDay, SimulType simulType)
{
	#ifdef _DEBUG
		double lastEnergyTank = traits[Trait::energy_tank];
		double lastBodySize = traits[Trait::factorEggMass];
	#endif

	//double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;

	double proportionOfTimeTheAnimalWasMoving;
	if(lifeStage == LifeStage::SATIATED || lifeStage == LifeStage::HANDLING)
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

	double totalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, position->getTemperature(), proportionOfTimeTheAnimalWasMoving, timeStepsPerDay, simulType);

	//Downregulation only here, do not change this into getMetabolicDryMassLoss because it would alter the expected loss in tuneTraits
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLoss -= totalMetabolicDryMassLoss * getSpecies()->getPercentageMetabolicDownregulation();
	}

	setTrait(Trait::energy_tank, getTrait(Trait::energy_tank) - totalMetabolicDryMassLoss);

	#ifdef _DEBUG
		double afterLossEnergyTank = traits[Trait::energy_tank];
		if(afterLossEnergyTank >= lastEnergyTank)
		{
			cerr << "The metabolic loss was 0 or positive:" << endl;
			cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
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
	

	if(getSpecies()->getGrowthCurve()->getType() == CurveType::Logistic){
		lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
	}
		
	if(!isMature())
	{


	//to force initialization in juveniles - arthro - the values are dummy and won't be used unitl updated 
		double daysForPseudoTargetReproduction;
		daysForPseudoTargetReproduction = 1.1*currentAge;//1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = currentAge + daysForPseudoTargetReproduction;
		double currentLength = getValueGrowthCurve(currentAge, xmid);
		double nextReproLength = getValueGrowthCurve(ageForNextReproduction, xmid);


		massOfMaturationOrLastReproduction = getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());
		massForNextReproduction = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());
		//end arthro

		//arthro - assignedForMolt is considered for molting or investment could be lower than the actual target
		if(calculateDryMass() >= massesVector[instar.getValue()] && currentAge >= finalDevTimeVector[instar.getValue()])
		{

			/* if(currentAge > 1){
			cout << currentAge << endl;
			exit(-1);
			} */

			//massesVector[instar+1]

			double investment = calculateDryMass()*getSpecies()->getAssignedForMolt();
			double next_tank = tank_ini * pow(massesVector[instar.getValue()], getSpecies()->getBetaScaleTank());
			double next_size = massesVector[instar.getValue()] - next_tank;
			double excessInvestment = investment-massesVector[instar.getValue()];
			lastMassBeforeMaturationOrOviposition = calculateDryMass();

			/* cout << excessInvestment << endl;
			cout << next_tank << endl;  */
			//exit(-1);
		
			setDryMass(
				next_tank + excessInvestment*(1-getSpecies()->getExcessInvestInSize()),
				next_size + excessInvestment*getSpecies()->getExcessInvestInSize()
			);
			
			tankAtGrowth = getTrait(Trait::energy_tank);

			#ifdef _DEBUG
				double afterMoultEnergyTank = traits[Trait::energy_tank];
				double afterMoultBodySize = traits[Trait::factorEggMass];
				if(afterMoultEnergyTank >= lastEnergyTank)
				{
					cerr << "The energy tank after moult was higher than before:" << endl;
					cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
					cerr << " - Last energy tank: " << lastEnergyTank << endl;
					cerr << " - After moult energy tank: " << afterMoultEnergyTank << endl;
				}
				if(afterMoultBodySize <= lastBodySize)
				{
					cerr << "The body size after moult was lower than before: " << endl;
					cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
					cerr << " - Last body size: " << lastBodySize << endl;
					cerr << " - After moult body size: " << afterMoultBodySize << endl;
				}
			#endif
			
			instar.moveOnNextInstar();

			/* if(getSpecies()->hasIndeterminateGrowth() && (instar+2 > getSpecies()->getInstarFirstReproduction())){

			 if((getSpecies()->getInstarFirstReproduction() % 2 == 0) &&
			  ((instar+2 - getSpecies()->getInstarFirstReproduction()) % 2 == 0)){ //both even

				mature = true;

			 }else{ //at least one is odd

			  if((getSpecies()->getInstarFirstReproduction() % 2 > 0) &&
			   ((instar+2 - getSpecies()->getInstarFirstReproduction()) % 2 == 0)){ //second even

				mature = true;



			  }

			 }


			}else{ */

				//(instar+2 == getSpecies()->getInstarFirstReproduction()) ||
				if((instar >= getSpecies()->getInstarFirstReproduction()) || (instar == getSpecies()->getNumberOfInstars()))
				{

					mature = true;


				}

		   //}
		//}//this is to mute the molting algorithm, wich is done in the if(10000)

		//arthro but really for dinos when molting algorithm above is muted - ends loop
		//exactly in the same place

		/*cout << finalDevTimeVector[(getSpecies()->getInstarFirstReproduction()-2)] << endl;
		exit(-1);*/

		//warning, to use this below need to uncomment the } above
		/* if(currentAge >= finalDevTimeVector[(getSpecies()->getInstarFirstReproduction()-2)]){
				
				//arthros and dinos especially this line below is necessary
				//when growth by moulting is muted
				lastMassBeforeMaturationOrOviposition = calculateDryMass();
				/////////////
				mature = true; */
			    //end arthro
				
				/* cout << getSpecies()->getId() << endl;
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
				cout << finalDevTimeVector[(getSpecies()->getInstarFirstReproduction()-2)] << endl;
				exit(-1);  */

				if(currentAge == getAgeFirstReproduction()){ //instar+2 == getSpecies()->getInstarFirstReproduction()){
				 	
					//if(getSpecies()->getGrowthType() == 0){

						ageOfMaturation = currentAge;
						
						ageOfFirstMaturation = currentAge;

					//}

					/*if(getSpecies()->getGrowthType() == 1){

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
					pupaTimer = getSpecies()->getPupaPeriodLength()*timeStepsPerDay;
				}
				//double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;
				//DONE RepFactor for different genders
				double daysForTargetReproduction;
				if(gender == AnimalSpecies::GENDERS::FEMALE)
				{

					//Dinosaurs
					//and arthros we remove eggDryMass from script because it may not work - early death from starvation
					//double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
					eggDryMassForClutch = getSpecies()->getEggDryMass()+getSpecies()->getEggDryMass()*getTrait(Trait::factorEggMass);
				
					double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*getSpecies()->getMaxPlasticityKVonBertalanffy();
					//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
					double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * getSpecies()->getMinPlasticityKVonBertalanffy();//Dinosaurs

					//below final eggDryMass is set after the female eggMass trait, if it surpasses the
					//growing curve band limits, it is set to the limit  //Dinosaurs
					eggDryMassForClutch = min(eggDryMassForClutch, maxMassAtCurrentAge);
					eggDryMassForClutch = max(eggDryMassForClutch, minMassAtCurrentAge);
				
				/* double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
				eggDryMassForClutch = getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB())+getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB()) * traits[Trait::factorEggMass];
 */

					if(eggDryMassForClutch < 0){
						//cout << "lengthAtBirth: " << lengthAtBirth << endl;
						Output::cout("eggDryMassForClutch: {}\n", eggDryMassForClutch);
						double asymptoticSize;
						switch(getSpecies()->getGrowthCurve()->getType()) {
							case CurveType::VonBertalanffy: {
								asymptoticSize = static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->getAsymptoticSize();
								break;
							}
							case CurveType::Logistic: {
								asymptoticSize = static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize();
								break;
							}
							case CurveType::Logistic3P: {
								asymptoticSize = static_cast<const Logistic3PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
								break;
							}
							case CurveType::Logistic4P: {
								asymptoticSize = static_cast<const Logistic4PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
								break;
							}
							case CurveType::Gompertz: {
								asymptoticSize = static_cast<const GompertzCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
								break;
							}
							default: {
								asymptoticSize = 0.0;
								break;
							}
						}
						Output::cout("asymptoticSize: {}\n", asymptoticSize);
						Output::cout("Growth: {}\n", getTrait(Trait::growth));
						if(getSpecies()->getGrowthCurve()->getType() == CurveType::VonBertalanffy) {
							Output::cout("thisAnimalVonBertTime0: {}\n", static_cast<const VonBertalanffyCurve* const>(getSpecies()->getGrowthCurve())->getVonBertTime0());
						}
						exit(-1);
					}

					if(getSpecies()->hasEggClutchFromEquation())
					{
						clutchDryMass = (getSpecies()->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, getSpecies()->getForClutchMassScale())) / getSpecies()->getConversionToWetMass();
					}
					else //If it does not come from equation,
					{
					
						
						
						//arthro
						if(getSpecies()->hasIndeterminateGrowth()){

							double lengthAtThisAge = getValueGrowthCurve(currentAge, xmid);


							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
							
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(getSpecies()->getEggsPerBatch()*propAdultMass), getSpecies()->getEggsPerBatch()); 
							
							clutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
						}else{
							clutchDryMass = eggDryMassForClutch*getSpecies()->getEggsPerBatch();	
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
					double longevity = getSpecies()->getLongevitySinceMaturation()*ageOfFirstMaturation;
					double reproTime = longevity - ageOfFirstMaturation + getSpecies()->getPupaPeriodLength();
				
					daysForTargetReproduction = floor(reproTime/getSpecies()->getFemaleMaxReproductionEvents());
					
					
					//end athros


				}
				else //is male
				{
					
					//arthro - to make targets more continuous also for dinos
					double currentLength = getValueGrowthCurve(currentAge, xmid);
					double nextReproLength = getValueGrowthCurve(currentAge+1, xmid);
					
					
					double nextReproMass = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());	
					double currentMass =getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());

					if(getSpecies()->hasIndeterminateGrowth()){
						if(instar < getSpecies()->getNumberOfInstars()){
							massForNextReproduction = nextReproMass;
						}else{
							massForNextReproduction = currentMass;
						}
						//daysForTargetReproduction = ceil( ((getSpecies()->maleReproductionFactor * (massesVector[getSpecies()->numberOfInstars-1] - massesVector[getSpecies()->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
						daysForTargetReproduction = ceil(0.01);
					}else{
					
					massForNextReproduction = lastMassBeforeMaturationOrOviposition + (nextReproMass-currentMass); //lastMassBeforeMaturationOrOviposition + (nextReproMass-currentMass);
					//daysForTargetReproduction = ceil( ((getSpecies()->maleReproductionFactor * (massesVector[getSpecies()->numberOfInstars-1] - massesVector[getSpecies()->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
					daysForTargetReproduction = ceil(0.01);
					}
					//end arthro
				}

				if(currentAge==round(ageOfFirstMaturation)){//currentAge==ageOfFirstMaturation rep_count==0
				//arthro
				/* ageOfLastMoultOrReproduction = currentAge; //Dinosaurs removed this + ageOfLastMoultOrReproduction
					ageForNextReproduction = currentAge + getSpecies()->getPupaPeriodLength();  *///Dinosaurs removed this + ageOfLastMoultOrReproduction + daysForTargetReproduction;
					
/* 					cout << ageForNextReproduction << endl;
					cout << getSpecies()->getId() << endl;
					exit(-1); 
 */	
					setNewLifeStage(LifeStage::PUPA);
					/*cout << "pupaTimer: " << pupaTimer << endl;
					exit(-1);*/		/* + daysForTargetReproduction */
                    ageForNextReproduction = currentAge + getSpecies()->getPupaPeriodLength();


				}else{
				//arthro	
				/* ageOfLastMoultOrReproduction = currentAge;
					ageForNextReproduction = currentAge + daysForTargetReproduction; */
					ageForNextReproduction = currentAge + daysForTargetReproduction;
				}

			//arthro
			ageOfLastMoultOrReproduction = currentAge;
			//dinos2023 - to ensure that ageForNextReproduction does not surpass the next instar (year)
			if(instar < getSpecies()->getNumberOfInstars()){
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
			if(getSpecies()->hasIndeterminateGrowth() && (instar < getSpecies()->getNumberOfInstars())){
				if(currentAge > finalDevTimeVector[instar.getValue()-1]+1){//plus one day enough to mate
			    //cout << "rep_count: " << rep_count << endl;
				mature = false;
				}
			}
		}else{
		
		//Dinos2023 Recalculate this here because females that are initialized as adults
		//should use their factorEggMass trait too
		eggDryMassForClutch = getSpecies()->getEggDryMass()+getSpecies()->getEggDryMass()*getTrait(Trait::factorEggMass);

		double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*getSpecies()->getMaxPlasticityKVonBertalanffy();
		//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
		double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * getSpecies()->getMinPlasticityKVonBertalanffy();//Dinosaurs

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
			
			cout << getSpecies()->getId() << endl;
			cout << ageForNextReproduction << endl;

			exit(-1);
		}   */

		   /*  cout << getSpecies()->getId() << endl;
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

		
		if(investmentForReproduction >= massForNextReproduction &&  currentAge >= ageForNextReproduction  && getSpecies()->getEggsPerBatch()>0)  //Arthropods post-dinos July 2021  && getSpecies()->getEggsPerBatch()>0
		{

			
			
			


			/* cout << getSpecies()->getId() << endl;
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

			if(getSpecies()->getSexualType() != SexualType::diploid || (getSpecies()->getSexualType() == SexualType::diploid && isMated()))
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
				
				setNewLifeStage(LifeStage::REPRODUCING);

			/*if(currentAge >= ageForNextReproduction){
			cout << "investmentForReproduction: " << investmentForReproduction << endl;
			cout << "massForNextReproduction: " << massForNextReproduction << endl;
			exit(-1);
		    }*/

				
			
				//double expectedLengthAtNextAge = currentPostTempLinf*(1-exp(-currentPostTempGrowth*(ageOfMaturation+(1.0/timeStepsPerDay)-thisAnimalVonBertTime0)));
				//double expectedMassAtNextAge = getSpecies()->coefficientForMassA*pow(expectedLengthAtNextAge,getSpecies()->scaleForMassB);
				//double neededEnergyFromCurrentMassToNextAge = expectedMassAtNextAge - getDryMass();
				//double proportionOfTimeTheAnimalWasMoving = 0;
				//double expectedMinimumMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(getDryMass(), proportionOfTimeTheAnimalWasMoving);
				//double minimumPossibleVoracity = expectedMinimumMetabolicDryMassLoss + neededEnergyFromCurrentMassToNextAge;

//arthro and for dinos, why was egg size calcultaed again, is enough with the calculation at maturation
/* 				clutchDryMass = 0;
				double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
				eggDryMassForClutch = getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB())+getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB()) * traits[Trait::factorEggMass];

 				if(eggDryMassForClutch < 0){
					cout << "lengthAtBirth2: " << lengthAtBirth << endl;
					cout << "eggDryMassForClutch2: " << eggDryMassForClutch << endl;
					cout << "asymptoticSize:" << static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize() << endl;
					cout << "Growth:" << traits[Trait::growth] << endl;
					cout << "thisAnimalVonBertTime02: " << thisAnimalVonBertTime0 << endl;
					exit(-1);
				}

				if(lengthAtBirth < 0)
				{
					cout << "Animal id: " << this->getId() << " (" << this->getSpecies()->getScientificName() << ") - The length at birth has been calculated as a negative value. Check growth related variables." << endl;
				}*/


				if(getSpecies()->hasEggClutchFromEquation())
				{
					clutchDryMass = (getSpecies()->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, getSpecies()->getForClutchMassScale())) / getSpecies()->getConversionToWetMass();
				}
				else //If it does not come from equation,
				{
						//arthro
						if(getSpecies()->hasIndeterminateGrowth()){
		
							double lengthAtThisAge = getValueGrowthCurve(currentAge, xmid);
						
							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
												
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(getSpecies()->getEggsPerBatch()*propAdultMass), getSpecies()->getEggsPerBatch()); 
							
							clutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
						}else{
							clutchDryMass = eggDryMassForClutch*getSpecies()->getEggsPerBatch();	
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
				double longevity = getSpecies()->getLongevitySinceMaturation()*ageOfFirstMaturation;
				double reproTime = longevity - ageOfFirstMaturation;
				daysForTargetReproduction = floor(reproTime/getSpecies()->getFemaleMaxReproductionEvents());
				//end arthros

				ageForNextReproduction = currentAge + daysForTargetReproduction;
				ageOfLastMoultOrReproduction = currentAge;

			//dinos2023
			//ageForNextReproduction = ageForNextReproduction //,finalDevTimeVector[instar]+round((finalDevTimeVector[instar+1]-finalDevTimeVector[instar])/2));
			//end dinos2023

				//arthro
				

				double nextClutchDryMass;
				if(getSpecies()->hasEggClutchFromEquation())
				{
					nextClutchDryMass = (getSpecies()->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, getSpecies()->getForClutchMassScale())) / getSpecies()->getConversionToWetMass();
				}
				else //If it does not come from equation,
				{
						//arthro
						if(getSpecies()->hasIndeterminateGrowth()){
							double lengthAtThisAge = getValueGrowthCurve(ageForNextReproduction, xmid);

							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
											
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(getSpecies()->getEggsPerBatch()*propAdultMass), getSpecies()->getEggsPerBatch()); 
							
							nextClutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
							massForNextReproduction = (calculateDryMass()-clutchDryMass) + nextClutchDryMass;	

						}else{
							nextClutchDryMass = eggDryMassForClutch*getSpecies()->getEggsPerBatch();	
						    massForNextReproduction = massOfMaturationOrLastReproduction + nextClutchDryMass;	
				
						}
						
				}

				
				
				//end arthro

			}
			else if(currentAge >= ageForNextReproduction)
			{
				//massForNextReproduction = lastMassBeforeMaturationOrOviposition + (getSpecies()->maleReproductionFactor * (massesVector[getSpecies()->numberOfInstars-1] - massesVector[getSpecies()->numberOfInstars-2]));
				//daysForTargetReproduction = ceil( ((getSpecies()->maleReproductionFactor * (massesVector[getSpecies()->numberOfInstars-1] - massesVector[getSpecies()->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
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
	return lifeStage == LifeStage::ACTIVE;
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
		setNewLifeStage(LifeStage::ACTIVE);
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
		setNewLifeStage(LifeStage::ACTIVE);
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
		setNewLifeStage(LifeStage::ACTIVE);
	}
	else
	{
		handlingTimer--;
	}
}

void Animal::isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(float relativeHumidity)
{
	if (relativeHumidity >= getSpecies()->getMinRelativeHumidityThreshold())
	{
		if(pupaTimer > 0)
		{
			setNewLifeStage(LifeStage::PUPA);
		}
		/*
		else if(handlingTimer > 0)
		{
			lifeStage = HANDLING;
		}
		*/
		else
		{
			setNewLifeStage(LifeStage::ACTIVE);
		}
	}
	else
	{
		diapauseTimeSteps++;
	}
}

double Animal::getInterpolatedHuntedVoracity(double huntedVoracity)
{
	return Math_Functions::linearInterpolate01(huntedVoracity,getSpecies()->getMinVorHunted(),getSpecies()->getMaxVorHunted()); 
	//return ((huntedVoracity - getSpecies()->getMeanVorHunted()) / getSpecies()->getSdVoracityHunted());
}

double Animal::getInterpolatedHunterVoracity()
{
	return Math_Functions::linearInterpolate01(getTrait(Trait::voracity),getSpecies()->getMinVorHunter(),getSpecies()->getMaxVorHunter()); 
	//return ((traits[Trait::voracity] - getSpecies()->getMeanVoracityHunter()) / getSpecies()->getSdVoracityHunter());
}

double Animal::getInterpolatedVoracityProduct(double huntedVoracity)
{
	return Math_Functions::linearInterpolate01(getTrait(Trait::voracity)* huntedVoracity,getSpecies()->getMinVorXVor(),getSpecies()->getMaxVorXVor()); 
	/*return ((traits[Trait::voracity] * huntedVoracity - getSpecies()->getMeanVoracityXVoracity())
			/ getSpecies()->getSdVoracityXVoracity());*/
}

double Animal::getInterpolatedHuntedBodySize(double huntedBodySize)
{
	return Math_Functions::linearInterpolate01(huntedBodySize,getSpecies()->getMinSizeHunted(),getSpecies()->getMaxSizeHunted()); 
	//return ((huntedBodySize - getSpecies()->getMeanSizeHunted()) / getSpecies()->getSdSizeHunted());
}

double Animal::getInterpolatedHunterBodySize()
{
	return Math_Functions::linearInterpolate01(currentBodySize,getSpecies()->getMinSizeHunter(),getSpecies()->getMaxSizeHunter()); 
	//return ((currentBodySize - getSpecies()->getMeanSizeHunter()) / getSpecies()->getSdSizeHunter());
}

double Animal::getInterpolatedPDF(double probabilityDensityFunction)
{
	return Math_Functions::linearInterpolate01(probabilityDensityFunction,getSpecies()->getMinProbabilityDensityFunction(),getSpecies()->getMaxProbabilityDensityFunction());
	/*return ((probabilityDensityFunction - getSpecies()->getMeanProbabilityDensityFunction())
			/ getSpecies()->getSdProbabilityDensityFunction());*/
}

double Animal::getInterpolatedSpeedRatio(double huntedSpeed)
{

	return Math_Functions::linearInterpolate01(getTrait(Trait::speed)/huntedSpeed,getSpecies()->getMinSpeedRatio(),getSpecies()->getMaxSpeedRatio());
	/*return (((traits[Trait::speed] / huntedSpeed) - getSpecies()->getMeanSpeedRatio())
			/ getSpecies()->getSdSpeedRatio());*/

}

double calculateLogMassRatio(double hunterAnimalDryMass, double huntedAnimalDryMass)
{
	return log(hunterAnimalDryMass/huntedAnimalDryMass);
}

double Animal::getSearchAreaHunter()
{
	return ((getTrait(Trait::search_area) - getSpecies()->getMeanSearchAreaHunter())
			/ getSpecies()->getSdSearchAreaHunter());
}

double Animal::getSpeedHunter()
{
	return ((getTrait(Trait::speed) - getSpecies()->getMeanSpeedHunter()) / getSpecies()->getSdSpeedHunter());
}

void Animal::incrementEncountersWithPredator(const int &predatorId)
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

ostream& operator<<(std::ostream& os, const Animal& animal)
{
	os << static_cast<const Edible&>(animal)
	<< animal.getSpecies()->getScientificName() << "\t"
	<< animal.gender << "\t"
	<< animal.position->getX() << "\t"
	<< animal.position->getY() << "\t"
	<< animal.position->getZ() << "\t"
	<< animal.lifeStage << "\t"
	<< animal.instar << "\t"
	<< animal.pheno_ini << "\t"
	<< animal.date_egg << "\t"
	<< animal.age_first_rep << "\t"
	<< animal.rep_count << "\t"
	<< animal.fecundity << "\t"
	<< animal.getDateOfDeath() << "\t"
	<< animal.generationNumberFromFemaleParent << "\t"
	<< animal.generationNumberFromMaleParent << "\t"
	<< animal.ID_prt1 << "\t"
	<< animal.ID_prt2 << "\t"
	<< animal.todayEncountersWithPredators << "\t"
	<< animal.encounterEvents.size() << "\t"
	<< animal.days_digest << "\t"
	<< animal.voracity_ini << "\t"
	<< animal.search_area_ini << "\t"
	<< animal.speed_ini << "\t"
	<< animal.tank_ini << "\t"
	<< animal.pheno_ini << "\t"
	<< animal.getCurrentBodySize() << "\t"
	<< animal.calculateDryMass() << "\t";



	for (size_t i = 0; i < Trait::size(); ++i) {
		/*os << "CR" << i << "_1: " << *(chromosomes->at(i).first) << endl;
		os << "CR" << i << "_2: " << *(chromosomes->at(i).second) << endl;
		os << "TRT_" << i << ": " << traits.at(i) << endl;*/
		try
		{
			os << animal.getTrait((TraitType)i) << "\t";
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

void Animal::printVoracities(int timeStep, int timeStepsPerDay, ostream& voracitiesFile, SimulType simulType)
{
	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;
	double dryMassAfterAssim = calculateDryMass() + foodMass;
	double wetMassAfterAssim = dryMassAfterAssim * getSpecies()->getConversionToWetMass();
	double prop_time_mov;

	double lengthAtBirth = 0;
	double xmid = 0;						 

	if(getSpecies()->getGrowthCurve()->getType() == CurveType::Logistic){
		lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
	}

	if(lifeStage == LifeStage::SATIATED || lifeStage == LifeStage::HANDLING)
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
	double totalMetabolicDryMassLossAfterAssim = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, position->getTemperature(), prop_time_mov, timeStepsPerDay, simulType);
	if(totalMetabolicDryMassLossAfterAssim)										
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLossAfterAssim -= totalMetabolicDryMassLossAfterAssim * getSpecies()->getPercentageMetabolicDownregulation();
	}
	double dryMassAfterMetabLoss = (dryMassAfterAssim - totalMetabolicDryMassLossAfterAssim);

	double expectedLengthAtNextTimeStep = getValueGrowthCurve(currentAge+(round((1.0/(double)timeStepsPerDay))), xmid);



	double expectedMassAtNextTimeStep;


    if(instar == getSpecies()->getInstarFirstReproduction()){
    	expectedMassAtNextTimeStep = getSpecies()->getCoefficientForMassAforMature()*pow(expectedLengthAtNextTimeStep,getSpecies()->getScaleForMassBforMature());
    }else{
    	expectedMassAtNextTimeStep = getSpecies()->getCoefficientForMassA()*pow(expectedLengthAtNextTimeStep,getSpecies()->getScaleForMassB());
	}




	//Dinosaurs
	double diffFromExpectedMassAtNextTimeStep = expectedMassAtNextTimeStep - dryMassAfterMetabLoss;

  	double dinoLengthPredicted = 0;

	if(instar > 1) {
		dinoLengthPredicted = getValueGrowthCurve(currentAge, xmid);
	}
	else {
		dinoLengthPredicted = getValueGrowthCurve(0, xmid);
	}

	double dinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(dinoLengthPredicted,getSpecies()->getScaleForMassBforMature());

    double minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * getSpecies()->getMinPlasticityKVonBertalanffy();

    double nextDinoLengthPredicted = getValueGrowthCurve(currentAge+1, xmid);

	
	
	//double nextDinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());

	
	/*if(getId() == 2 && timeStep == 51){
    			cout << calculateDryMass() << endl;
    			exit(-1);
    		}*/

	double asymptoticSize;

	switch(getSpecies()->getGrowthCurve()->getType()) {
		case CurveType::VonBertalanffy: {
			asymptoticSize = static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->getAsymptoticSize();
			break;
		}
		case CurveType::Logistic: {
			asymptoticSize = static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize();
			break;
		}
		case CurveType::Logistic3P: {
			asymptoticSize = static_cast<const Logistic3PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
			break;
		}
		case CurveType::Logistic4P: {
			asymptoticSize = static_cast<const Logistic4PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
			break;
		}
		case CurveType::Gompertz: {
			asymptoticSize = static_cast<const GompertzCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
			break;
		}
		default: {
			asymptoticSize = 0.0;
			break;
		}
	}


	voracitiesFile << getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
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
	<< asymptoticSize << "\t"
	<< pseudoGrowth << "\t"
	<< asymptoticSize << "\t"
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
	<< getSpecies()->getScientificName() << "\t"
	<< generationNumberFromFemaleParent << "\t"
	<< generationNumberFromMaleParent << "\t"
	<< ID_prt1 << "\t"
	<< ID_prt2 << "\t";

	for (unsigned int i = 0; i < getSpecies()->getNumberOfVariableTraits(); ++i)
	{
		geneticsFile << *(genome.getHomologousChromosomes().at(i).first) << *(genome.getHomologousChromosomes().at(i).second);
	}
	geneticsFile << endl;
}

void Animal::printTraits(ostream& traitsFile)
{
	traitsFile << getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< generationNumberFromFemaleParent << "\t"
	<< generationNumberFromMaleParent << "\t"
	<< ID_prt1 << "\t"
	<< ID_prt2 << "\t";

	for (size_t i = 0; i < Trait::size(); ++i)
	{
		try
		{
			traitsFile << getTrait((TraitType)i) << "\t";
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
	for(size_t i = 0; i < getSpecies()->getNumberOfVariableTraits(); i++)
	{
		getSpecies()->setMinObservedPseudoValue(variableTraits[i], min(getSpecies()->getMinObservedPseudoValue(variableTraits[i]), getTrait(variableTraits[i])));
		getSpecies()->setMaxObservedPseudoValue(variableTraits[i], max(getSpecies()->getMaxObservedPseudoValue(variableTraits[i]), getTrait(variableTraits[i])));
	}
}

void Animal::sumPseudoGrowthMean()
{
	getSpecies()->setPseudoGrowthMean(getSpecies()->getPseudoGrowthMean() + pseudoGrowth);
}

void Animal::sumPseudoGrowthSd()
{
	getSpecies()->setPseudoGrowthSd(getSpecies()->getPseudoGrowthSd() + pow(pseudoGrowth - getSpecies()->getPseudoGrowthMean(), 2));
}
/*
void Animal::sumPseudoLengthMean()
{


	//growth cambia según dells a Topt = 35 (usando changedKmin changedKmax etc..)
	double vonBertLinf = linearInterpolate(traits[Trait::growth], getSpecies()->minRestrictedLimits[Trait::growth], getSpecies()->maxRestrictedLimits[Trait::growth], getSpecies()->vonBertLdistanceMin, getSpecies()->vonBertLdistanceMax);
	//vonBertLinf disminuye según tempSizeRuleConstant (usando Topt = 35)
	//finalDevTimeVector cambia usando dells (usamos la misma Topt = 35 otra vez)
	double pseudoVonBertMaxLength = vonBertLinf*(1-exp(-traits[Trait::growth]*(finalDevTimeVector[CADAINSTAR]-getSpecies()->vonBertTime0)));
	//calculamos mean y sd para pseudoVonBertMaxLength

	double normalizedPseudoVonBertMaxLength = (pseudoVonBertMaxLength - pseudoVonBertMaxLengthMean) / pseudoVonBertMaxLengthSd;
	vonBertMaxLength<-normalizedPseudoGrowth*cholMat[1,2]+normalizedPseudoVonBertMaxLength*cholMat[2,2]

	pseudoGrowth = traits[Trait::growth];
	getSpecies()->pseudoGrowthMean += pseudoGrowth;
}

void Animal::sumPseudoLengthSd()
{
	getSpecies()->pseudoGrowthSd += pow(pseudoGrowth - getSpecies()->pseudoGrowthMean, 2);
}
*/

void Animal::setNewLifeStage(const LifeStage newLifeStage)
{
	position->eraseAnimal(this, lifeStage);

	lifeStage = newLifeStage;
	position->addAnimal(this);
}

void Animal::setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath)
{
	setNewLifeStage(newLifeStage);

	switch (newLifeStage)
	{
	case LifeStage::STARVED:
		setDateOfDeath(dayOfDeath);
		break;
	case LifeStage::PREDATED:
		setDateOfDeath(dayOfDeath);
		break;
	case LifeStage::BACKGROUND:
		setDateOfDeath(dayOfDeath);
		break;
	case LifeStage::SENESCED:
		setDateOfDeath(dayOfDeath);
		break;
	case LifeStage::SHOCKED:
		setDateOfDeath(dayOfDeath);
		break;
	default:
		throwLineInfoException("In the new state the animal does not die");
		break;
	}
}

void Animal::setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath, int predatorId)
{
	setNewLifeStage(newLifeStage, dayOfDeath);

	switch (newLifeStage)
	{
	case LifeStage::PREDATED:
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
	for(size_t i = 0; i < getSpecies()->getNumberOfVariableTraits(); i++)
	{
		setTrait(variableTraits[i], Math_Functions::linearInterpolate(getTrait(variableTraits[i]), getSpecies()->getMinObservedPseudoValue(variableTraits[i]), getSpecies()->getMaxObservedPseudoValue(variableTraits[i]),
				getSpecies()->getMinRestrictedRange(variableTraits[i]), getSpecies()->getMaxRestrictedRange(variableTraits[i])));
		if(getTrait(variableTraits[i]) < getSpecies()->getMinRestrictedRange(variableTraits[i]) || getTrait(variableTraits[i]) > getSpecies()->getMaxRestrictedRange(variableTraits[i]))
		{
			//cout << "WARNING: individual " << id << " on species " << getSpecies()->getScientificName() << ": trait number " << i << ": " << traits[i] << " out of RESTRICTED RANGES: " << getSpecies()->minRestrictedRanges[i] << " - " << getSpecies()->maxRestrictedRanges[i] << endl << flush;
			isInsideRestrictedRanges = false;
		}
		if(getTrait(variableTraits[i]) < getSpecies()->getMinTraitLimit(variableTraits[i]) || getTrait(variableTraits[i]) > getSpecies()->getMaxTraitLimit(variableTraits[i]))
		{
			Output::coutFlush("WARNING: individual {} on species {}: trait number {}: {} out of LIMITS: {} - {}\n", getId(), getSpecies()->getScientificName(), variableTraits[i], getTrait(variableTraits[i]), getSpecies()->getMinTraitLimit(variableTraits[i]), getSpecies()->getMaxTraitLimit(variableTraits[i]));
			isViableAnimal = false;
		}
	}
	return make_pair(isInsideRestrictedRanges, isViableAnimal);
}

void Animal::calculateGrowthCurves(float temperature, ostream& tuneTraitsFile, bool printGrowthData, double ageAtInitialization) //Dinosaurs
{
	
	double lengthAtBirth = 0;
	double xmid = 0;						 

	if(getSpecies()->getGrowthCurve()->getType() == CurveType::Logistic){
		lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
	}

	//vonBertK is now the actual growth trait

	//Calculating asymptoticSize and changing growth curves according to LinfKcorr
	//Cholesky has been called AFTER the creation of each species and the matrix has been saved as cholMat
	//The creation of the cholMat is performed inside the setLinfKcorr function

	//Using Change_K (and Dells) for tuning Growth according to temperature
	//double tmin = 270;
	double tmax = 273 + getSpecies()->getTempFromLab();
	/*double newT = 273 + temperature;
	//Optimo para las curvas de crecimiento=35. Para Vor Speed Search será 25
	double Topt = 273 + getSpecies()->getTempOptGrowth();
	double activationEnergyGrowth = 0.65;//changed from 0.66 now following Dell, perhaps take it out as an entry parameter, but since is growth assume MTE

	double dellsNewT = dells(newT, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth);
	double changeKmin = getSpecies()->getMinTraitRange(Trait::growth)-0.000001;
	double changedKmin = linearInterpolate(dellsNewT, dells(xmin, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth), dells(xmax, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth), changeKmin, getSpecies()->getMinTraitRange(Trait::growth));
	double changedKmax = linearInterpolate(dellsNewT, dells(xmin, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth), dells(xmax, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth), changeKmin, getSpecies()->getMaxTraitRange(Trait::growth));
*/
	//This equals K<-interpol(Knorm, Kmin, Kmax)
	//currentPostTempGrowth = linearInterpolate(traits[Trait::growth], getSpecies()->getMinTraitRange(Trait::growth), getSpecies()->getMaxTraitRange(Trait::growth), changedKmin, changedKmax);

	//Decrease in asymptoticSize according to TempSizeRuleConstant
	double thisAnimalTempSizeRuleConstant = getSpecies()->getTempSizeRuleConstant();
	double degreesDifference = abs(temperature - getSpecies()->getTempFromLab());
	if(temperature > getSpecies()->getTempFromLab())
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant);
	}

	//Here the TSR rule is improved by directly affecting asymptotic body mass, not length.
	/*double preTsrLinfMass = getSpecies()->getCoefficientForMassAforMature()*pow(static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize(),getSpecies()->getScaleForMassBforMature());
	double postTsrLinfMass = preTsrLinfMass + thisAnimalTempSizeRuleConstant*degreesDifference*preTsrLinfMass;

	currentPostTempLinf = pow(postTsrLinfMass/getSpecies()->getCoefficientForMassAforMature(),1/getSpecies()->getScaleForMassBforMature());*/

	if(printGrowthData)
	{
		double asymptoticSize;

		switch(getSpecies()->getGrowthCurve()->getType()) {
			case CurveType::VonBertalanffy: {
				asymptoticSize = static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->getAsymptoticSize();
				break;
			}
			case CurveType::Logistic: {
				asymptoticSize = static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize();
				break;
			}
			case CurveType::Logistic3P: {
				asymptoticSize = static_cast<const Logistic3PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
				break;
			}
			case CurveType::Logistic4P: {
				asymptoticSize = static_cast<const Logistic4PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
				break;
			}
			case CurveType::Gompertz: {
				asymptoticSize = static_cast<const GompertzCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
				break;
			}
			default: {
				asymptoticSize = 0.0;
				break;
			}
		}


		tuneTraitsFile
		<< getTrait(Trait::growth) << "\t"
		<< asymptoticSize << "\t"; //<< currentPostTempLinf << "\t";
	}

    //Here to calculate the new dev time we need to calculate the mass of the adult after TSR has been applied
    double ageLastInstar = getAgeLastInstar();

	
    double maxLfromSpeciesLastInstar = getValueGrowthCurve(ageLastInstar, xmid);

    double massLastInstarForDevT = getSpecies()->getCoefficientForMassAforMature()*pow(maxLfromSpeciesLastInstar,getSpecies()->getScaleForMassBforMature());

    double postTSRMassLastInstarForDevT = massLastInstarForDevT + thisAnimalTempSizeRuleConstant*degreesDifference*massLastInstarForDevT;

    //transform to g in wetMass to adjust to the equation by Gillooly et al. 2012
    double massLastInstarForDevTinG = getSpecies()->getConversionToWetMass() * postTSRMassLastInstarForDevT * 1000;

    double devTime = exp(-0.11 * (temperature / (1 + temperature / 273)) + getSpecies()->getDevInter()) * pow(massLastInstarForDevTinG, 0.25);

	//double finalDevTimeProportion = devTime / ageLastInstar;

	double finalDevTimeProportion = 1; //Dinosaurs

	//Fixing t0 in BV
	//Original length without
	//Jordi: Are the three sentences below necessary anymore?
//	double maxLfromSpeciesMatureAge = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(matureAge-getSpecies()->getVonBertTime0())));
//	double thisAnimalMaxLfromSpeciesMatureAge = maxLfromSpeciesMatureAge + thisAnimalTempSizeRuleConstant*degreesDifference*maxLfromSpeciesMatureAge;
//	double thisAnimalMatureAge = matureAge*finalDevTimeProportion;

	if(eggDryMassAtBirth == -1)//for the new borns
	{
		
		//double lengthAtBirth = 0;
		//Only for initialization (value -1). The new offsprings will have factorEggMassFromMom correctly set as factorEggMass(FROM MOM).
		if(factorEggMassFromMom == -1)
		{
			factorEggMassFromMom = getTrait(Trait::factorEggMass);
		}

	/* 		if(ageAtInitialization < 1){ //Dinosaurs
			lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
			}else{
			lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(ageAtInitialization-thisAnimalVonBertTime0)));	
			}

		}else{ //newborn during simulation */

		//lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
		
		//}


		//eggDryMassAtBirth = getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB());
		eggDryMassAtBirth = getSpecies()->getEggDryMass() + getSpecies()->getEggDryMass() * factorEggMassFromMom;
		//traits[Trait::energy_tank] =  tank_ini * pow(eggDryMassAtBirth, getSpecies()->getBetaScaleTank());
		//currentBodySize = eggDryMassAtBirth - traits[Trait::energy_tank];

		
		double maxMassAtCurrentAge = eggDryMassAtBirth + eggDryMassAtBirth*getSpecies()->getMaxPlasticityKVonBertalanffy();
		//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
		double minMassAtCurrentAge = eggDryMassAtBirth - eggDryMassAtBirth * getSpecies()->getMinPlasticityKVonBertalanffy();//Dinosaurs

		//below final eggDryMass is set after the female eggMass trait, if it surpasses the
		//growing curve band limits, it is set to the limit  //Dinosaurs
		eggDryMassAtBirth = min(eggDryMassAtBirth, maxMassAtCurrentAge);
		eggDryMassAtBirth = max(eggDryMassAtBirth, minMassAtCurrentAge);

		setDryMass(eggDryMassAtBirth);
	}


	//Now we calculate the vector of Ls using asymptoticSize and von Bertalanfy function on each age
	//And we use each L as the next_L in the growth equations to calculate the next_M


/* 	lengthsVector = new double[getSpecies()->getNumberOfInstars()]; //Dinosaurs take out of the heap... better memory?
	massesVector = new double[getSpecies()->getNumberOfInstars()];
 */
	for (int i = 0; i < finalDevTimeVector.size(); ++i)
	{
		Instar instar(i+1);
		finalDevTimeVector[i] = getSpecies()->getDevTime(instar);
		
		lengthsVector[i] = getValueGrowthCurve(finalDevTimeVector[i], xmid);


		if(getSpecies()->getNumberOfInstars() == getSpecies()->getInstarFirstReproduction()){
			massesVector[i] = getSpecies()->getCoefficientForMassAforMature()*pow(lengthsVector[i],getSpecies()->getScaleForMassBforMature());
		}else{
			massesVector[i] = getSpecies()->getCoefficientForMassA()*pow(lengthsVector[i],getSpecies()->getScaleForMassB());
		}

		//TSR is applied directly here so the k value is effectively changed at the individual level
		massesVector[i] = massesVector[i] + massesVector[i]*thisAnimalTempSizeRuleConstant*degreesDifference;

	}

	for (int i = 0; i < finalDevTimeVector.size(); ++i)
	{
		Instar instar(i+1);
		finalDevTimeVector[i] = getSpecies()->getDevTime(instar)*finalDevTimeProportion;

		// cout << "finalDevTimeVector[i] :"<< finalDevTimeVector[i] << endl;
		// cout << "lengthsVector[i] :" << lengthsVector[i] << endl;
		// cout << "massesVector[i] :" << massesVector[i] << endl;

	}

		//arthro
		//ageOfFirstMaturation = finalDevTimeVector[getSpecies()->getInstarFirstReproduction()-2];

					//if(getSpecies()->getGrowthType() == 0){

						ageOfFirstMaturation = getAgeFirstReproduction();

					//}

					/*if(getSpecies()->getGrowthType() == 1){

						ageOfFirstMaturation = xmid;
					
					}*/


		//end arthro

//exit(-1);
}

//The function below is to grow the animals that need to be at a certain instar at the beginning of the simulation
float Animal::forceMolting(int timeStepsPerDay, double ageAtInitialization, Instar instarAtInitialization, SimulType simulType)
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

	if(getSpecies()->getGrowthCurve()->getType() == CurveType::Logistic){
		lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
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
		instar = Instar(getSpecies()->getNumberOfInstars());

		actualMoltingTimeVector.push_back(getAgeInInstar(instar));
		actualMoltingMassVector.push_back(getMassesInInstar(instar));

		while(ageAtInitialization < getAgeInInstar(instar))
		{
			instar.moveOnPreviousInstar();

			actualMoltingTimeVector.push_back(getAgeInInstar(instar));
			actualMoltingMassVector.push_back(getMassesInInstar(instar));
		}
	}

	//int maturationInstar = getSpecies()->getInstarFirstReproduction();
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
cout << getSpecies()->getInstarFirstReproduction() << endl;
exit(-1);
 */


	/* if(getSpecies()->hasIndeterminateGrowth() && (instar+2 > getSpecies()->getInstarFirstReproduction())){

	 if((getSpecies()->getInstarFirstReproduction() % 2 == 0) &&
	  ((instar+2 - getSpecies()->getInstarFirstReproduction()) % 2 == 0)){ //both even

		mature = true;

	 }else{ //at least one is odd

	  if((getSpecies()->getInstarFirstReproduction() % 2 > 0) &&
	   ((instar+2 - getSpecies()->getInstarFirstReproduction()) % 2 == 0)){ //second even

		mature = true;

	  }

	 }


	}else{ */

		//(instar+2 == getSpecies()->getInstarFirstReproduction()) ||
		if((instar >= getSpecies()->getInstarFirstReproduction()) || (instar == getSpecies()->getNumberOfInstars())){
			mature = true;
		}

	//}

	if(isMature()){
	
	
	  //if(getSpecies()->getGrowthType() == 0){
		
		if(instar == getSpecies()->getInstarFirstReproduction()){
			ageOfMaturation = ageAtInitialization;
			ageOfFirstMaturation = getAgeFirstReproduction(); //Dinos2023 - to prevent dates to close to target next maturation
		}else{
			ageOfMaturation = ageAtInitialization;
			ageOfFirstMaturation = getAgeFirstReproduction();
		}
	 // }

	  /*if(getSpecies()->getGrowthType() == 1){
		
		if(ageAtInitialization >= xmid){
							ageOfMaturation = xmid;
							ageOfFirstMaturation = xmid;
		}else{
							ageOfMaturation = 0;
							ageOfFirstMaturation = 0;
		}

	  }*/

	
	
	
	}

	//Decrease in asymptoticSize according to TempSizeRuleConstant
	double thisAnimalTempSizeRuleConstant = getSpecies()->getTempSizeRuleConstant();
	double degreesDifference = abs(temperatureCycleTemperature - getSpecies()->getTempFromLab());
	if(temperatureCycleTemperature > getSpecies()->getTempFromLab())
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant);
	}


	if(!isMature()){



		double expectedLengthAtCurrentAge = getValueGrowthCurve(ageAtInitialization, xmid);

		
		double expectedMassAtCurrentAge = getSpecies()->getCoefficientForMassA()*pow(expectedLengthAtCurrentAge,getSpecies()->getScaleForMassB());

		expectedMassAtCurrentAge = expectedMassAtCurrentAge + expectedMassAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;

		setDryMass(expectedMassAtCurrentAge);

		//to force initialization in juveniles 
		//to force initialization in juveniles - arthro - the values are dummy and won't be used unitl updated 
		double daysForPseudoTargetReproduction;
		daysForPseudoTargetReproduction = 1.1*currentAge; //1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = currentAge + daysForPseudoTargetReproduction;
		
		double currentLength = getValueGrowthCurve(currentAge, xmid);
		double nextReproLength = getValueGrowthCurve(ageForNextReproduction, xmid);
		
		massOfMaturationOrLastReproduction = getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());
		massForNextReproduction = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());
		//end arthro

	}else{//is mature


		double expectedLengthAtCurrentAge = getValueGrowthCurve(ageAtInitialization, xmid);

		double expectedMassAtCurrentAge = getSpecies()->getCoefficientForMassAforMature()*pow(expectedLengthAtCurrentAge,getSpecies()->getScaleForMassBforMature());

		expectedMassAtCurrentAge = expectedMassAtCurrentAge + expectedMassAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;

		setDryMass(expectedMassAtCurrentAge);


		//TODO ADD HERE ADJUSTMENTS FOR ENERGY TANK WHEN FORCING ADULTS!!
		//Dinosaurs double currentAge = -traits[Trait::pheno] + 1.0/timeStepsPerDay;
		massOfMaturationOrLastReproduction = getMassesInInstar(instar);
		ageOfMaturation = getAgeInInstar(instar);
		lastMassBeforeMaturationOrOviposition = getMassesInInstar(instar);
		double daysForTargetReproduction;
		if(gender == AnimalSpecies::GENDERS::FEMALE)
		{
			clutchDryMass=0;//Dinosaurs
			//double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
			eggDryMassForClutch = getSpecies()->getEggDryMass() + getSpecies()->getEggDryMass()* getTrait(Trait::factorEggMass);

			double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*getSpecies()->getMaxPlasticityKVonBertalanffy();
					//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
			double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * getSpecies()->getMinPlasticityKVonBertalanffy();//Dinosaurs

					//below final eggDryMass is set after the female eggMass trait, if it surpasses the
					//growing curve band limits, it is set to the limit  //Dinosaurs
			eggDryMassForClutch = min(eggDryMassForClutch, maxMassAtCurrentAge);
			eggDryMassForClutch = max(eggDryMassForClutch, minMassAtCurrentAge);
			
			
			if(getSpecies()->hasEggClutchFromEquation())
			{
				clutchDryMass = (getSpecies()->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, getSpecies()->getForClutchMassScale())) / getSpecies()->getConversionToWetMass();
			}
			else //If it does not come from equation,
			{
						//arthro
						if(getSpecies()->hasIndeterminateGrowth()){
							
							double lengthAtThisAge = getValueGrowthCurve(currentAge, xmid);

							
							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
						
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(getSpecies()->getEggsPerBatch()*propAdultMass), getSpecies()->getEggsPerBatch()); 
							
							clutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
						}else{
							clutchDryMass = eggDryMassForClutch*getSpecies()->getEggsPerBatch();	
						}
						//end arthro
			}
			massForNextReproduction = lastMassBeforeMaturationOrOviposition + clutchDryMass;
			//arthros
			//daysForTargetReproduction = ceil( (clutchDryMass*ageOfMaturation) / lastMassBeforeMaturationOrOviposition);
			double longevity = getSpecies()->getLongevitySinceMaturation()*ageOfFirstMaturation;
			double reproTime = longevity - ageOfFirstMaturation;
				
			daysForTargetReproduction = floor(reproTime/getSpecies()->getFemaleMaxReproductionEvents());
			//end arthros
			
			

			//daysForTargetReproduction = ceil(getSpecies()->getReproTimeFactor()*ageOfFirstMaturation);
		}
		else
		{
		  	//arthro - to make targets more continuous also for dinos
			double currentLength = getValueGrowthCurve(currentAge, xmid);
			double nextReproLength = getValueGrowthCurve(ageForNextReproduction, xmid);
    		
			double nextReproMass = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());	
			double currentMass = getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());

			if(getSpecies()->hasIndeterminateGrowth()){
				if(instar < getSpecies()->getNumberOfInstars()){
					massForNextReproduction = nextReproMass;
				}else{
					massForNextReproduction = currentMass;
				}
				//daysForTargetReproduction = ceil( ((getSpecies()->maleReproductionFactor * (massesVector[getSpecies()->numberOfInstars-1] - massesVector[getSpecies()->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
				daysForTargetReproduction = ceil(0.01);
			}else{
				massForNextReproduction = lastMassBeforeMaturationOrOviposition + (nextReproMass-currentMass);
				//daysForTargetReproduction = ceil( ((getSpecies()->maleReproductionFactor * (massesVector[getSpecies()->numberOfInstars-1] - massesVector[getSpecies()->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
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
		tuneTraits(-1, 1, temperatureCycleTemperature, 100, noStream2, false, true, simulType);
	}


	return temperatureCycleTemperature;
}


void Animal::setDryMass(const double &newDryMass)
{
	setTrait(Trait::energy_tank, tank_ini * pow(newDryMass, getSpecies()->getBetaScaleTank()));
	currentBodySize = newDryMass - getTrait(Trait::energy_tank);

	getSpecies()->updateMaximumDryMassObserved(newDryMass);
}

void Animal::setDryMass(const double &newEnergyTank, const double &newCurrentBodySize)
{
	setTrait(Trait::energy_tank, newEnergyTank);
	currentBodySize = newCurrentBodySize;

	getSpecies()->updateMaximumDryMassObserved(calculateDryMass());
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

	if(getSpecies()->getGrowthCurve()->getType() == CurveType::Logistic){
		lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature());  
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::growth));
	}

	for (int i = 0; i < getSpecies()->getDevTimeVector().size(); ++i)  //Dinosaurs for calculteGrowthCurves with ageAtInitialization != 0
	{
		Instar instar(i+1);
		finalDevTimeVector[i] = getSpecies()->getDevTime(instar);
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
		instar = Instar(getSpecies()->getNumberOfInstars());

		actualMoltingTimeVector.push_back(getAgeInInstar(instar));
		actualMoltingMassVector.push_back(getMassesInInstar(instar));

		while(ageAtInitialization < getAgeInInstar(instar))
		{
			instar.moveOnPreviousInstar();

			actualMoltingTimeVector.push_back(getAgeInInstar(instar));
			actualMoltingMassVector.push_back(getMassesInInstar(instar));
		}
	}


    double reproTime;
	double daysForTargetReproduction;
			
	//double addedAge = Random::randomIndex(daysForTargetReproduction);
	
    if(instar == 1){ //Dinosaurs and arthro - fixed for short-lifecycle mites - initialization is now done at random ages within the instar to generate reproduction variability
		
		ageAtInitialization = Random::randomIndex(finalDevTimeVector[0]);
	
	}else if((instar > 1) && (instar < getSpecies()->getNumberOfInstars())){
	
		ageAtInitialization = ageAtInitialization + Random::randomIndex(finalDevTimeVector[instar.getValue()]-getAgeInInstar(instar));	
	    
	} else { //is the last instar
		//age at initialization of animals in the last instar depends on longevity
		//cout << "before" << endl;
		//cout << ageAtInitialization << endl;
		
		//cout << ageAtInitialization << endl;
		
		// ageAtInitialization = ageAtInitialization + addedAge;
		
		ageAtInitialization = ageAtInitialization+Random::randomIndex((getAgeFirstReproduction()*getSpecies()->getLongevitySinceMaturation())-ageAtInitialization);
	} 
	
	calculateGrowthCurves(temperatureCycleTemperature, noStream, false, ageAtInitialization); //Dinosaurs
	
	currentAge = ageAtInitialization; //Dinosaurs
	//int maturationInstar = getSpecies()->getInstarFirstReproduction();
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
cout << getSpecies()->getInstarFirstReproduction() << endl;
exit(-1);
 */
	//if(getSpecies()->hasIndeterminateGrowth() && (instar+2 >= getSpecies()->getInstarFirstReproduction())){

	 //if((getSpecies()->getInstarFirstReproduction() % 2 == 0) &&
	 // ((instar+2 - getSpecies()->getInstarFirstReproduction()) % 2 == 0)){ //both even

		//mature = true;

	 //}else{ //at least one is odd

	  //if((getSpecies()->getInstarFirstReproduction() % 2 > 0) &&
	   //((instar+2 - getSpecies()->getInstarFirstReproduction()) % 2 == 0)){ //second even

		//mature = true;

	  //}/////*else{

		 ////if((getSpecies()->getInstarFirstReproduction() % 2 > 0) &&
		 ////(instar+2 == getSpecies()->getNumberOfInstars()+1)){ //first is odd

			 ////mature = true;

		 ////}
	  /////}*/
	  //mature = true;

	 //}


	//}else{

		//(instar+2 == getSpecies()->getInstarFirstReproduction()) ||
		if((instar >= getSpecies()->getInstarFirstReproduction()) || (instar == getSpecies()->getNumberOfInstars())){
			mature = true;
		}

	//}

	if(isMature()){
	 // if(getSpecies()->getGrowthType() == 0){
		
		//if(instar == getSpecies()->getInstarFirstReproduction()){
			ageOfMaturation = getAgeFirstReproduction();
			ageOfFirstMaturation = getAgeFirstReproduction(); //Dinos2023 - to prevent dates to close to target next maturation
		//}else{
			//ageOfMaturation = getAgeFirstReproduction();
			//ageOfFirstMaturation = getAgeFirstReproduction();
		//}
	  //}

	/* if(getSpecies()->getGrowthType() == 1){
		
		if(ageAtInitialization >= xmid){
							ageOfMaturation = xmid;
							ageOfFirstMaturation = xmid;
		}else{
							ageOfMaturation = 0;
							ageOfFirstMaturation = 0;
		}

	  }*/
	}

	//Decrease in asymptoticSize according to TempSizeRuleConstant
	double thisAnimalTempSizeRuleConstant = getSpecies()->getTempSizeRuleConstant();
	double degreesDifference = abs(temperatureCycleTemperature - getSpecies()->getTempFromLab());
	if(temperatureCycleTemperature > getSpecies()->getTempFromLab())
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant);
	}


	if(!isMature()){



		double expectedLengthAtCurrentAge = getValueGrowthCurve(ageAtInitialization, xmid);

		double expectedMassAtCurrentAge = getSpecies()->getCoefficientForMassA()*pow(expectedLengthAtCurrentAge,getSpecies()->getScaleForMassB());

		expectedMassAtCurrentAge = expectedMassAtCurrentAge + expectedMassAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;

		setDryMass(expectedMassAtCurrentAge);

		//to force initialization in juveniles - arthro - the values are dummy and won't be used unitl updated 
		double daysForPseudoTargetReproduction;
		daysForPseudoTargetReproduction = 1.1*currentAge; //1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = currentAge + daysForPseudoTargetReproduction;
		double currentLength = getValueGrowthCurve(currentAge, xmid);
		double nextReproLength = getValueGrowthCurve(ageForNextReproduction, xmid);

		massOfMaturationOrLastReproduction = getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());
		massForNextReproduction = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());
		//end arthro

	}else{//is mature


		double expectedLengthAtCurrentAge = getValueGrowthCurve(ageAtInitialization, xmid);

		double expectedMassAtCurrentAge = getSpecies()->getCoefficientForMassAforMature()*pow(expectedLengthAtCurrentAge,getSpecies()->getScaleForMassBforMature());

		expectedMassAtCurrentAge = expectedMassAtCurrentAge + expectedMassAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;

		setDryMass(expectedMassAtCurrentAge);


		//TODO ADD HERE ADJUSTMENTS FOR ENERGY TANK WHEN FORCING ADULTS!!
		//Dinosaurs double currentAge = -traits[Trait::pheno] + 1.0/timeStepsPerDay;
		
		
		double expectedLengthAtFormerAge = 0;

		if(ageAtInitialization > 0) {
			expectedLengthAtFormerAge = getValueGrowthCurve(ageAtInitialization-1, xmid);
		}
		else {
			expectedLengthAtFormerAge = getValueGrowthCurve(0, xmid);
		}



		double expectedMassAtFomerAge = getSpecies()->getCoefficientForMassAforMature()*pow(expectedLengthAtFormerAge,getSpecies()->getScaleForMassBforMature());
		
		massOfMaturationOrLastReproduction = getMassesInInstar(instar);
		//ageOfMaturation = getAgeInInstar(instar);
		lastMassBeforeMaturationOrOviposition = getMassesInInstar(instar);
		//double daysForTargetReproduction;

		if(gender == AnimalSpecies::GENDERS::FEMALE)
		{
			clutchDryMass=0;//Dinosaurs
			//double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::growth]*(0-thisAnimalVonBertTime0)));
			eggDryMassForClutch = getSpecies()->getEggDryMass() + getSpecies()->getEggDryMass()* getTrait(Trait::factorEggMass);

			double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*getSpecies()->getMaxPlasticityKVonBertalanffy();
			//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
			double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * getSpecies()->getMinPlasticityKVonBertalanffy();//Dinosaurs

			//below final eggDryMass is set after the female eggMass trait, if it surpasses the
			//growing curve band limits, it is set to the limit  //Dinosaurs
			eggDryMassForClutch = min(eggDryMassForClutch, maxMassAtCurrentAge);
			eggDryMassForClutch = max(eggDryMassForClutch, minMassAtCurrentAge);

			if(getSpecies()->hasEggClutchFromEquation())
			{
				clutchDryMass = (getSpecies()->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, getSpecies()->getForClutchMassScale())) / getSpecies()->getConversionToWetMass();
			}
			else //If it does not come from equation,
			{
						//arthro
						if(getSpecies()->hasIndeterminateGrowth()){

							double lengthAtThisAge = getValueGrowthCurve(currentAge, xmid);

							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
							
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = Math_Functions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
							round(getSpecies()->getEggsPerBatch()*propAdultMass), getSpecies()->getEggsPerBatch()); 
							
							clutchDryMass = eggDryMassForClutch*currentEggs;//for mites or others that lay only one egg per batch
						}else{
							clutchDryMass = eggDryMassForClutch*getSpecies()->getEggsPerBatch();	
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
		double longevity = getSpecies()->getLongevitySinceMaturation()*ageOfFirstMaturation;
		reproTime = longevity - ageOfFirstMaturation;
				
			daysForTargetReproduction = floor(reproTime/getSpecies()->getFemaleMaxReproductionEvents());
			//end arthros
			
 			
			//daysForTargetReproduction = ceil(getSpecies()->getReproTimeFactor()*ageOfFirstMaturation);
		}
		else
		{
				//arthro - to make targets more continuous also for dinos
				double currentLength = getValueGrowthCurve(currentAge, xmid);
				double nextReproLength = getValueGrowthCurve(ageForNextReproduction, xmid);
				
    			double nextReproMass = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());	
				double currentMass =getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());

				if(getSpecies()->hasIndeterminateGrowth()){
					if(instar < getSpecies()->getNumberOfInstars()){
					  	massForNextReproduction = nextReproMass;
					}else{
					  	massForNextReproduction = currentMass;
					}
					//daysForTargetReproduction = ceil( ((getSpecies()->maleReproductionFactor * (massesVector[getSpecies()->numberOfInstars-1] - massesVector[getSpecies()->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
					daysForTargetReproduction = ceil(0.01);
				}else{
					
					  massForNextReproduction = calculateDryMass() + (nextReproMass-currentMass);
					//daysForTargetReproduction = ceil( ((getSpecies()->maleReproductionFactor * (massesVector[getSpecies()->numberOfInstars-1] - massesVector[getSpecies()->numberOfInstars-2])*ageOfMaturation) / lastMassBeforeMaturationOrOviposition ));
					  daysForTargetReproduction = ceil(0.01);
				}
				//end arthro
		}

		//Dinosaurs - this allows dinosaurs to start reproducing readily 
		//at the beginning of the simulation
		ageOfLastMoultOrReproduction = ageAtInitialization;
		//if(getSpecies()->hasIndeterminateGrowth()){  //+ Random::randomIndex(getAgeInInstar(instar))
		ageForNextReproduction = ageAtInitialization + Random::randomIndex(daysForTargetReproduction); //add two days at least
									//

        /* cout << "Id            xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << getId() << endl;
		cout << gender << endl;
		cout << massForNextReproduction << endl;
		cout << calculateDryMass() << endl;
		cout << ageForNextReproduction << endl; 
		cout << currentAge << endl; */


	//}else{
			
		//}	

		//double daysActiveSinceMaturation = round(Random::randomUniform()*daysForTargetReproduction);
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


TerrainCellInterface* Animal::move(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, ostream& edibilitiesFile, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, double pdfThreshold, double muForPDF, double sigmaForPDF, vector<AnimalSpecies*> *existingAnimalSpecies, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{


	#ifdef _DEBUG
	Output::coutFlush("moving animal... \n");
	#endif

	// This condition checks if the animal has already been moved in the current timeStep
	if (lastDayMoved < timeStep)
	{
		updateBiomassExperiencedPerSpecies(timeStepsPerDay);
		handlingTimer = 0.0;
		
		sated = false; //Dinosaurs and arthropods after Dinos - do not know why¿? 

		// Assimilate leftover food
		assimilateLastHuntedAnimalAndComputeHandlingTime();

		// Movement occurs when there is no food mass left
		if(!isThereLeftoverFood()) 
		{
			oldSearchArea = getTrait(Trait::search_area);

			double biomassTrackedToday = 0.0;
			list<TerrainCellInterface *> cellsTrackedToday;
			list<Edible*> ediblesHasTriedToPredate;
			bool hasEvaluatedTheWholeWorld = false;

			searchInCurrentCell(timeStep, timeStepsPerDay, cellsTrackedToday, biomassTrackedToday, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, existingAnimalSpecies, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

			bool canMove = false;

			//Mature MALES will move now even when they are sated


			if(getSpecies()->isForceQuitCell()) {
				if(getTrait(Trait::search_area) < getPosition()->getSize()) {
					setTrait(Trait::search_area, 1.1*getPosition()->getSize());
				}
			}

			if(getTrait(Trait::search_area) >= getPosition()->getSize()) {
				canMove = true;
			}

			if(canMove)
			{
				TerrainCellInterface* targetNeighborToTravelTo;

				MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(position);

				if(macroCell != nullptr) {
					targetNeighborToTravelTo = new MacroTerrainCell(*macroCell);
				}
				else {
					targetNeighborToTravelTo = position;
				}

				while (handlingTimer < timeStepMaximumHandlingTimer && // No me queda claro para que sirve
					   steps < getTrait(Trait::search_area) - (0.5 * position->getSize()) && // If it hasn't moved beyond its search area
				       lifeStage == LifeStage::ACTIVE && // If it is active
					   days_digest == 0 && // Esta variable no se está utilizando
				       (
							!sated || // If it is not satisfied, it will have to look for food
							(sated && mature && gender == AnimalSpecies::GENDERS::MALE) // If it is sated, mature and a male, so it will look for a mate to breed with
					   )
				)
				{
					if(macroCell != nullptr) {
						if(*reinterpret_cast<MacroTerrainCell*>(targetNeighborToTravelTo) == *reinterpret_cast<MacroTerrainCell*>(position))
						{
							delete reinterpret_cast<MacroTerrainCell*>(targetNeighborToTravelTo);
							targetNeighborToTravelTo = searchTargetToTravelTo(hasEvaluatedTheWholeWorld, cellsTrackedToday, ediblesHasTriedToPredate, worldDepth, worldLength, worldWidth, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
						}
					}
					else {
						if(targetNeighborToTravelTo == position)
						{
							targetNeighborToTravelTo = searchTargetToTravelTo(hasEvaluatedTheWholeWorld, cellsTrackedToday, ediblesHasTriedToPredate, worldDepth, worldLength, worldWidth, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
						}
					}

					moveOneStep(targetNeighborToTravelTo);
					
					searchInCurrentCell(timeStep, timeStepsPerDay, cellsTrackedToday, biomassTrackedToday, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, existingAnimalSpecies, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
				}

				if(macroCell != nullptr) {
					delete reinterpret_cast<MacroTerrainCell*>(targetNeighborToTravelTo); 
				}
			}

			// IMPORTANT! steps might be higher than search_area just when the animal travels the LAST STEP in a diagonal direction.
			// Set the final number of steps performed in the movement
			if(steps > getTrait(Trait::search_area))
			{
				steps = getTrait(Trait::search_area);
			}

			// Update the number of days the animal has been without food
			switch (getHuntingMode()) {
				case HuntingMode::does_not_hunt: {
					break;
				}
				case HuntingMode::sit_and_wait:
				case HuntingMode::active_hunting: {
					if(foodMass > 0.0)
					{
						daysWithoutFood = 0;
					}
					else
					{
						daysWithoutFood += round(((double)timeStep/(double)timeStepsPerDay));
					}
					break;
				}
				default: {
					throwLineInfoException("Default case");
					break;
				}
			}

			// It indicates that the animal has already moved in this timestep.
			lastDayMoved = timeStep;
		}
	}

	return position;
}

bool edibleCompare(const std::pair<double, pair<Animal*, Edible*>>& firstElem, const std::pair<double, pair<Animal*, Edible*>>& secondElem) {
  return firstElem.first > secondElem.first;
}

bool Animal::searchInCurrentCell(int timeStep, int timeStepsPerDay, list<TerrainCellInterface *> &cellsTrackedToday, double &biomassTrackedToday, list<Edible*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, vector<AnimalSpecies*> *existingAnimalSpecies, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	// Count current position as tracked
	cellsTrackedToday.push_back(position);

	bool animalIsSenesced = searchAnimalToBreed(timeStep, timeStepsPerDay);

	if(!animalIsSenesced) {
		searchAnimalsAndResourceToEat(timeStep, timeStepsPerDay, biomassTrackedToday, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, existingAnimalSpecies, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
	}
}

bool Animal::searchAnimalToBreed(int timeStep, int timeStepsPerDay)
{
	// If the animal is mature and has not mated, find another animal to mate with
	if(isMature() && !isMated())
	{
		// List of states where you can search for animals
		vector<LifeStage> searchableLifeStages = {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE};

		// Get all the animals that can be mated from the current position
		vector<Animal*> searchableAnimals;
		for (const auto& lifeStage : searchableLifeStages) {
			auto lifeStageAnimals = position->getAnimalsBySpecies(lifeStage, getSpecies());

			for(auto &elem : lifeStageAnimals) {
				searchableAnimals.insert(searchableAnimals.end(), elem.first, elem.second);
			}
		}

		// Try to mate the animal with one of the possible animals in the cell
		for(auto animalsIt = searchableAnimals.begin(); animalsIt != searchableAnimals.end(); animalsIt++)
		{
			Animal* currentAnimal = *animalsIt;

			// If the mating conditions are met
			if(currentAnimal->isMature() && gender != currentAnimal->getGender() && !currentAnimal->isMated())
			{
				// Variables to identify the male and female at mating
				Animal *matedMale, *matedFemale;

				// Choose the role of each animal according to the sex of the animal doing the searching
				switch (gender)
				{
					case AnimalSpecies::GENDERS::MALE:
						matedMale = this;
						matedFemale = currentAnimal;
						break;
					case AnimalSpecies::GENDERS::FEMALE:
						matedMale = currentAnimal;
						matedFemale = this;
						break;
					case AnimalSpecies::GENDERS::HERMAPHRODITE:
						matedMale = currentAnimal;
						matedFemale = this;
						break;
					default:
						throwLineInfoException("Default case");
						break;
				}

				// The female retains the male genome
				matedFemale->setGenomeFromMatedMale(matedMale);

				// if the male has exceeded the maximum reproductive limit
				if(matedMale->postBreeding(timeStep, timeStepsPerDay))
				{
					// Change the status of the male to senesced
					matedMale->getPosition()->changeAnimalToSenesced(matedMale, timeStep);

					// If the male is the animal conducting the search, death of the animal is indicated
					if(matedMale == this)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Animal::searchAnimalsAndResourceToEat(int timeStep, int timeStepsPerDay, double &biomassTrackedToday, list<Edible*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, vector<AnimalSpecies*> *existingAnimalSpecies, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	// List of states where you can search for animals
	vector<LifeStage> searchableLifeStages;

	vector<pair<double, pair<Animal*, Edible*>>> ediblesByEdibility;


	// Identify all possible interactions between the animal and the other animals in the cell
	for(auto speciesIt = existingAnimalSpecies->begin(); speciesIt != existingAnimalSpecies->end(); speciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *speciesIt;
		
		// Check if the animal is prey for the species
		bool isSpeciePrey = currentAnimalSpecies->canEatAnimalSpecies(getSpecies()); 
		// Check if the species is prey for the animal
		bool isSpeciePredator = getSpecies()->canEatAnimalSpecies(currentAnimalSpecies);

		
		if(isSpeciePrey)
		{
			// Identify interactions with active animals of the species
			searchableLifeStages = {LifeStage::ACTIVE};
		}
		else if(!isSpeciePrey && isSpeciePredator)
		{
			// Identify interactions with all animals of the species with which it can interact
			searchableLifeStages = {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE};
		}
		else
		{
			// No interaction with an animal
			searchableLifeStages = {};
		}


		// Get all the animals that can interact at the current position.
		vector<Animal*> searchableAnimals;
		for (const auto& lifeStage : searchableLifeStages) {
			auto lifeStageAnimals = position->getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			for(auto &elem : lifeStageAnimals) {
				searchableAnimals.insert(searchableAnimals.end(), elem.first, elem.second);
			}
		}


		// Get information about interactions
		for(auto animalsIt = searchableAnimals.begin(); animalsIt != searchableAnimals.end(); animalsIt++)
		{
			Animal* currentAnimal = *animalsIt;

			if(currentAnimal != this)
			{
				// Variables indicating the type of interaction between the two animals
				bool isPredated = NULL, preyingOn = NULL;

				// Check if the animal qualifies as a predator
				if(isSated() || isExhausted())
				{
					preyingOn = false;
				}
				else
				{
					//Dinosaurs but elsewhere - to prevent unreasonable encounters and ingestion of very large prey
					double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/currentAnimal->calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
				
					if(probabilityDensityFunction < pdfThreshold) //Dinosaurs but elsewhere
					{
						preyingOn = false;
					}
				}

				// Check if the selected animal qualifies as a predator
				if(currentAnimal->isSated() || currentAnimal->isExhausted())
				{
					isPredated = false;
				}
				else
				{
					//Dinosaurs but elsewhere - to prevent unreasonable encounters and ingestion of very large prey
					double probabilityDensityFunction = exp(-0.5 * pow((log(currentAnimal->calculateWetMass()/calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
				
					if(probabilityDensityFunction < pdfThreshold) //Dinosaurs but elsewhere
					{
						isPredated = false;
					}
				}


				// Complete the interaction information if it is incomplete
				if(isPredated == NULL || preyingOn == NULL)
				{
					// Check if the animal is prey for the selected animal
					bool isPrey = currentAnimal->canEatEdible(this, ediblesHasTriedToPredate);
					// Check if the selected animal is prey for the animal
					bool isPredator = canEatEdible(currentAnimal, ediblesHasTriedToPredate);

					// If both without predators of the other
					if (isPrey && isPredator)
					{
						// Based on a probability, it is decided who will hunt whom
						if(currentAnimal->calculatePredationProbability(this, false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW) > calculatePredationProbability(currentAnimal, false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW))
						{
							isPredator = false;
						}
						else
						{
							isPrey = false;
						}
					}

					if(isPredated == NULL)
					{
						// If the selected animal hunts the animal
						if(isPrey)
						{
							isPredated = true;
						}
						else
						{
							isPredated = false;
						}
					}

					if(preyingOn == NULL)
					{
						// If the animal hunts the selected animal
						if(isPredator)
						{
							preyingOn = true;
						}
						else
						{
							preyingOn = false;
						}
					}
				}


				// Add interaction if possible
				if(isPredated)
				{
					ediblesByEdibility.push_back(make_pair(currentAnimal->calculateEdibilityValue(this, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(currentAnimal, this)));
				}
				else if(preyingOn)
				{
					ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentAnimal, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(this, currentAnimal)));
					biomassTrackedToday += currentAnimal->calculateDryMass();
				}
			}
		}
	}

	// If the animal is not sated and not exhausted
	if(!isSated() && !isExhausted())
	{
		// Identify all possible interactions between the animal and the resource in the cell
		for(auto &elem : position->getResources()) {
			for(auto resourceIt = elem->begin(); resourceIt != elem->end(); resourceIt++)
			{
				Resource* currentResource = *resourceIt;

				// If the resource specie is prey for the animal
				if(getSpecies()->canEatResourceSpecies(currentResource->getSpecies()))
				{
					// If the selected resource is prey for the animal
					if(canEatEdible(currentResource, ediblesHasTriedToPredate))
					{
						ediblesByEdibility.push_back(make_pair(calculateEdibilityValue(currentResource, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(this, currentResource)));
						biomassTrackedToday += currentResource->calculateDryMass();
					}
				}
			}
		}
	}

	// Sorting the elements by edibility
	std::sort(ediblesByEdibility.begin(), ediblesByEdibility.end(), edibleCompare);

	// Print interaction information
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
		<< (*ediblesIt).second.first->calculateEncounterProbability((*ediblesIt).second.second, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) << "\t"
		<< (*ediblesIt).second.first->calculatePredationProbability((*ediblesIt).second.second, false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW) << "\t"
		<< (*ediblesIt).second.first->getSpecies()->getEdiblePreference((*ediblesIt).second.second->getSpecies()->getId(), (*ediblesIt).second.first->getInstar(), (*ediblesIt).second.second->getInstar()) << "\t"
		<< (*ediblesIt).second.first->getMeanExperience((*ediblesIt).second.second->getSpecies()) << "\t"
		<< (*ediblesIt).first << endl;
	}
	

	// Variable indicating foraging success or failure
	bool hasEatenOnce = false;

	// Search for the edible to eat
	for(auto ediblesIt = ediblesByEdibility.begin(); !hasEatenOnce && ediblesIt != ediblesByEdibility.end(); ediblesIt++)
	{
		Animal* huntingAnimal = (*ediblesIt).second.first;
		Edible* targetAnimal = (*ediblesIt).second.second;


		// Calculate the probability of interaction of the animal with the edible it interacts with.
		float attackOrExposedAttackProbability = (huntingAnimal == this) ? huntingAnimal->getSpecies()->getAttackProbability() : targetAnimal->getSpecies()->getExposedAttackProbability();
		
		// Check if the predator can find the prey
		bool searchWasSuccessful = huntingAnimal->encounterEdible(targetAnimal, attackOrExposedAttackProbability, timeStep, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
		
		if(searchWasSuccessful)
		{
			//Steps only goes up when at least the animal HAS TRIED TO HUNT the prey. It goes up regardless of the success
			//The steps penalization is assumed depending on the search area and the longest diagonal
			
			// Check if the predator can hunt the prey
			bool huntWasSuccessful = huntingAnimal->predateEdible(targetAnimal, timeStep, false, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea);
			
			if(huntWasSuccessful)
			{
				// Successful search is indicated
				hasEatenOnce = true;

				// If the prey is the animal conducting the search, death of the animal is indicated
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
				// Counter attack only takes place if the target animal was not the current moving animal.
				// Check if the prey can become the predator
				if(targetAnimal->canEatEdible(huntingAnimal, ediblesHasTriedToPredate) && targetAnimal != this)
				{
					// Check if the new predator can hunt the old predator
					bool counterHuntWasSuccessful = targetAnimal->predateEdible(huntingAnimal, timeStep, true, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea);
					
					if(counterHuntWasSuccessful)
					{
						// Successful search is indicated
						hasEatenOnce = true;

						// If the old predator is the animal conducting the search, death of the animal is indicated
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

	#ifdef _DEBUG
		if(foodMass > traits[Trait::voracity])
		{
			cerr << "The food mass eaten was higher than the voracity value:" << endl;
			cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
			cerr << " - Food mass eaten: " << foodMass << endl;
			cerr << " - Voracity value: " << traits[Trait::voracity] << endl;
		}
	#endif

	// If the food found covers the animal's voracity
	if(round(foodMass*pow(10,7)) >= round(getTrait(Trait::voracity)*pow(10,7)))
	{
		// The amount of food is truncated to the animal's current voracity.
		foodMass = getTrait(Trait::voracity);

		// It indicates that the animal is sated
		sated = true;
	}

	// Searcher did not die, so return false
	return false;
}

void Animal::setGenomeFromMatedMale(const Animal* matedMale)
{
	mated = true;
	if(genomeFromMatedMale != NULL)	delete genomeFromMatedMale;
	genomeFromMatedMale = matedMale->getGenome().clone(getSpecies()->getRandomlyCreatedPositionsForChromosomes());										   
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
		//cout << getSpecies()->getScientificName() << endl;

		return true;

	}
	else
	{
		//this below is allow females with indeterminate growth to keep growing while reproducing
		//Dinos2023 - animals can now follow mature patterns, be mature while growing, does it work?
		//if we comment below indeterminate growth stops - thus, we need another formula to avoid excessive food targets
	 	if(instar < getSpecies()->getNumberOfInstars())
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
	// (getSpecies()->getEggDryMass() + getSpecies()->getEffDryMass() * traits[Trait::factorEggMass]));

	return floor(clutchDryMass / (getSpecies()->getEggDryMass() * (1 + getTrait(Trait::factorEggMass))));
}

list<Animal*> * Animal::breedAsexually( int objectiveEggsNumber,int timeStep, int timeStepsPerDay, float temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	double totalOffspringDryMass = 0.0;

	list<Animal*> * offspring = new list<Animal *>();

	//Asexual animals are always females (or treated as females) and they DO NOT perform meiosis.
	int offSpringGender = AnimalSpecies::GENDERS::FEMALE;
	int createdEggsNumber = 0;
	int generationDam;
	int generationSire;

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

		Gamete* femaleGameteSelected = genome.cloneFirstGameteFromHaploid();
		Gamete* maleGameteSelected = genome.cloneFirstGameteFromHaploid();

		TerrainCellInterface* newOffspringPosition;

		MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(position);

		if(macroCell != nullptr) {
			newOffspringPosition = new MacroTerrainCell(*macroCell);
		}
		else {
			newOffspringPosition = position;
		}

		Animal* newOffspring = new Animal(femaleGameteSelected, maleGameteSelected, getTrait(Trait::factorEggMass), newOffspringPosition, round(((double)timeStep/(double)timeStepsPerDay)), timeStepsPerDay, generationNumberFromFemaleParent + 1, generationNumberFromMatedMale + 1, getId(), idFromMatedMale, getSpecies(), offSpringGender, worldInterface, true);

		delete femaleGameteSelected;
		delete maleGameteSelected;

		pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newOffspring->interpolateTraits();

		//TODO ALWAYS print the traits after interpolating and before adjusting
		if(saveAnimalConstitutiveTraits) {
			newOffspring->printTraits(constitutiveTraitsFile);
		}

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
			newOffspring->doDefinitive();
			newOffspring->setExperiencePerSpecies();
		}
		else // non-viable offspring -> kill it
		{
			Output::cout("Deleting non-viable Animal with ID: {}\n", newOffspring->getId());
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
		setNewLifeStage(LifeStage::ACTIVE);
		//clearGenomeFromMatedMale();
		//CARE delete only when in TerrainCell
		//position->addAnimal(this);
		//position->eraseAnimal(this, REPRODUCING);
	}

	return offspring;
}


list<Animal*> * Animal::breedSexually( int objectiveEggsNumber,int timeStep, int timeStepsPerDay, float temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	double totalOffspringDryMass = 0.0;

	list<Animal*> * offspring = new list<Animal *>();

	int offSpringGender;
	int createdEggsNumber = 0;
	int generationDam;
	int generationSire;

	while( createdEggsNumber < objectiveEggsNumber )
	{
		Genome* offspringGenome = NULL;
		createdEggsNumber ++;  // DUDA: Debe hacerse antes o sólo si se crea el offspring?
		
		Gamete* femaleGameteSelected = genome.cloneFirstGameteFromHaploid();
		Gamete* maleGameteSelected = NULL;
		
		if(getSpecies()->getSexualType() == SexualType::diploid)
		{
			maleGameteSelected = genomeFromMatedMale->getRandomGameteFromMeiosis();
			//gender here depends on the species sexRatio
			offSpringGender = getSpecies()->getRandomGender();
		}
		else if(getSpecies()->getSexualType() == SexualType::haplodiploid)
		{
			if(isMated())
			{
				//Gender here depends on the species sexRatio
				offSpringGender = getSpecies()->getRandomGender();
				if(offSpringGender == AnimalSpecies::GENDERS::MALE)
				{
					//Males are haploid so they will use only the female gamete (duplicated for simplicity, in reality they have only one)
					maleGameteSelected = new Gamete(getSpecies()->getNumberOfChromosomes());
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
				maleGameteSelected = new Gamete(getSpecies()->getNumberOfChromosomes());
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

		TerrainCellInterface* newOffspringPosition;

		MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(position);

		if(macroCell != nullptr) {
			newOffspringPosition = new MacroTerrainCell(*macroCell);
		}
		else {
			newOffspringPosition = position;
		}

		Animal* newOffspring = new Animal(femaleGameteSelected, maleGameteSelected, getTrait(Trait::factorEggMass), newOffspringPosition, round(((double)timeStep/(double)timeStepsPerDay)), timeStepsPerDay, generationNumberFromFemaleParent + 1, generationNumberFromMatedMale + 1, getId(), idFromMatedMale, getSpecies(), offSpringGender, worldInterface, true);

		delete femaleGameteSelected;
		delete maleGameteSelected;

		pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newOffspring->interpolateTraits();

		//TODO ALWAYS print the traits after interpolating and before adjusting
		if(saveAnimalConstitutiveTraits) {
			newOffspring->printTraits(constitutiveTraitsFile);
		}

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
			newOffspring->doDefinitive();
			newOffspring->setExperiencePerSpecies();
		}
		else // non-viable offspring -> kill it
		{
			Output::cout("Deleting non-viable Animal with ID: {}\n", newOffspring->getId());
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
		setNewLifeStage(LifeStage::ACTIVE);
		//clearGenomeFromMatedMale();
		//CARE delete only when in TerrainCell
		//position->addAnimal(this);
		//position->eraseAnimal(this, REPRODUCING);
	}

	return offspring;
}

list<Animal*> * Animal::breed(int timeStep, int timeStepsPerDay, float temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	int currentEggBatchNumber = computeEggBatchNumber();

	// Stores animals to be born
	list<Animal*> * offspring = NULL;

	switch (getSpecies()->getSexualType())
	{
		case SexualType::asexual:
			offspring = breedAsexually(currentEggBatchNumber, timeStep, timeStepsPerDay, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		case SexualType::diploid:
			offspring = breedSexually(currentEggBatchNumber, timeStep, timeStepsPerDay, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		case SexualType::haplodiploid:
			offspring = breedSexually(currentEggBatchNumber, timeStep, timeStepsPerDay, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}

	return offspring;
}

TerrainCellInterface* Animal::searchTargetToTravelTo(bool &hasEvaluatedTheWholeWorld, std::list<TerrainCellInterface *> &cellsTrackedToday, const std::list<Edible*> &ediblesHasTriedToPredate, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	TerrainCellInterface* targetNeighborToTravelTo = position;

	//cout << getSpecies()->getScientificName() << "#" << id << ": SEARCH TARGET START... " << " stepsLeft: " << ((int) round((traits[Trait::search_area] - steps) / position->getSize())) << " searchAreaLeft: " << (traits[Trait::search_area] - steps) << endl << flush;
	int maxAnimalDistance = (int)round((getTrait(Trait::search_area) - steps)/position->getSize());
	int maxWorldX = max((int)round(static_cast<double>(worldWidth-position->getX()-1)/position->getSize()), (int)round(static_cast<double>(position->getX())/position->getSize()));
	int maxWorldY = max((int)round(static_cast<double>(worldLength-position->getY()-1)/position->getSize()), (int)round(static_cast<double>(position->getY())/position->getSize()));
	int maxWorldZ = max((int)round(static_cast<double>(worldDepth-position->getZ()-1)/position->getSize()), (int)round(static_cast<double>(position->getZ())/position->getSize()));
	int maxWorldDistance = max(max(maxWorldX, maxWorldY), maxWorldZ);
	int maxDistance = min(maxWorldDistance, maxAnimalDistance);
	//cout << getSpecies()->getScientificName() << "#" << id << ": position = " << position->getX() << "-" << position->getY() << "-" << position->getZ() << "; maxDistance = " << maxDistance << endl << flush;

	if(!hasEvaluatedTheWholeWorld)
	{
		for(int distance = 1; distance <= maxDistance && targetNeighborToTravelTo == position; distance++)
		{
			//cout << getSpecies()->getScientificName() << "#" << id << ": looking for resources at distance = " << distance << " of " << maxDistance << endl << flush;
			vector<TerrainCellInterface*> * neighbors = position->getCellsAtDistance(distance, getTrait(Trait::search_area) - steps + position->getSize(),this->getSpecies()->isMobile(), worldDepth, worldLength, worldWidth);
			list<TerrainCellInterface*>::iterator cellsTrackedTodayIt;
			//TODO what will happen if they have tracked all already?

			for(cellsTrackedTodayIt = cellsTrackedToday.begin(); cellsTrackedTodayIt != cellsTrackedToday.end(); cellsTrackedTodayIt++)
			{
				neighbors->erase(std::remove(neighbors->begin(), neighbors->end(), (*cellsTrackedTodayIt)), neighbors->end());
			}
			///////////////////////////////////////


			vector<TerrainCellEvaluation> bestEvaluations;
			bestEvaluations.push_back(TerrainCellEvaluation());

			for (vector<TerrainCellInterface*>::iterator neighborsIt = neighbors->begin(); neighborsIt != neighbors->end(); neighborsIt++)
			{
				TerrainCellInterface* currentTargetNeighbor = (*neighborsIt);

				bool availableCurrentTargetNeighbor = true;

				if (isMature() && gender == AnimalSpecies::GENDERS::MALE)
				{
					if(currentTargetNeighbor->getNumberOfMatureFemales(this) == 0)
					{
						availableCurrentTargetNeighbor = false;
					}
				}

				if(availableCurrentTargetNeighbor)
				{
					double edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass;
					tie(edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass) = currentTargetNeighbor->getCellEvaluation(this, ediblesHasTriedToPredate, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
					
					TerrainCellEvaluation currentEvaluation(getSpecies(), currentTargetNeighbor, edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass);

					if(bestEvaluations.front() == currentEvaluation) {
						bestEvaluations.push_back(currentEvaluation);
					}
					else if(bestEvaluations.front() < currentEvaluation) {
						bestEvaluations.clear();

						bestEvaluations.push_back(currentEvaluation);
					}
				}
			}

			if(bestEvaluations.front().cell != nullptr)
			{
				if(bestEvaluations.size() > 1) {
					targetNeighborToTravelTo = bestEvaluations[Random::randomIndex(bestEvaluations.size())].cell;
				}
				else {
					targetNeighborToTravelTo = bestEvaluations.front().cell;
				}
				neighbors->erase(std::remove(neighbors->begin(), neighbors->end(), targetNeighborToTravelTo), neighbors->end());
			}

			MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(getPosition());

			if(macroCell != nullptr) {
				for(auto &elem : *neighbors) {
					delete reinterpret_cast<MacroTerrainCell*>(elem);
				}
			}

			neighbors->clear();
			delete neighbors;
		}
		
		
		//Select a random direction if there are no animals around
		
		if(targetNeighborToTravelTo == position)
		{
			if(maxAnimalDistance >= maxWorldDistance)
			{
				hasEvaluatedTheWholeWorld = true;
			}
			vector<TerrainCellInterface*> * neighbors = position->getCellsAtDistance(maxDistance, DBL_MAX, this->getSpecies()->isMobile(), worldDepth, worldLength, worldWidth); //arthros and for dinos
			if(neighbors->size() > 0){
				targetNeighborToTravelTo = neighbors->at(Random::randomIndex(neighbors->size()));
				neighbors->erase(std::remove(neighbors->begin(), neighbors->end(), targetNeighborToTravelTo), neighbors->end());
			}

			MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(getPosition());

			if(macroCell != nullptr) {
				for(auto &elem : *neighbors) {
					delete reinterpret_cast<MacroTerrainCell*>(elem);
				}
			}

			neighbors->clear();
			delete neighbors;
		}
		//targetNeighborToTravelTo = position;
	}
	else //hasEvaluatedTheWholeWorld==true
	{
		vector<TerrainCellInterface*> * neighbors = position->getCellsAtDistance(maxDistance, DBL_MAX, this->getSpecies()->isMobile(), worldDepth, worldLength, worldWidth);
		if(neighbors->size() > 0){
			targetNeighborToTravelTo = neighbors->at(Random::randomIndex(neighbors->size()));
			neighbors->erase(std::remove(neighbors->begin(), neighbors->end(), targetNeighborToTravelTo), neighbors->end());
		}
		
		MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(getPosition());

		if(macroCell != nullptr) {
			for(auto &elem : *neighbors) {
				delete reinterpret_cast<MacroTerrainCell*>(elem);
			}
		}

		//targetNeighborToTravelTo = position;
		neighbors->clear();
		delete neighbors;
	}

	return targetNeighborToTravelTo;
}

void Animal::moveOneStep(const TerrainCellInterface* const targetNeighborToTravelTo)
{
	// Increase the number of movement attempts
	stepsAttempted++;

	TerrainCellInterface* cellToMoveTo = worldInterface->getCellByBearing(position, targetNeighborToTravelTo);
	
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
}

void Animal::dieFromBackground(int timeStep, bool growthAndReproTest)
{
	if(!growthAndReproTest) {
		if(getSpecies()->getProbabilityDeathFromBackground() > Random::randomUniform())
		{
			setNewLifeStage(LifeStage::BACKGROUND, timeStep);
		}
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

        /*double maxNextInstarLengthFromVBPlasticity = lengthsVector[instar+1] + getSpecies()->getMaxPlasticityKVonBertalanffy()*lengthsVector[instar+1];
        double maxNextInstarMassFromVBPlasticity = getSpecies()->getCoefficientForMassA()*pow(maxNextInstarLengthFromVBPlasticity,getSpecies()->getScaleForMassB());

        if(calculateDryMass() > maxNextInstarMassFromVBPlasticity){
        	foodMass=0;
        }else{
		if(calculateDryMass()+foodMass > maxNextInstarMassFromVBPlasticity){
		            cout << "dryMass + foodMass" << calculateDryMass()+foodMass << endl;
		            foodMass=maxNextInstarMassFromVBPlasticity-calculateDryMass();
		            cout << "dryMass + foodMass" << calculateDryMass()+foodMass << endl;
		}
        }*/
		/* if(getSpecies()->getId() == 1 && timeStep >30 && timeStep <40){
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
					getSpecies()->Q10DIGEST));
			eating = false;
		}
		 */
        if(round(foodMass*pow(10,7)) < round(getTrait(Trait::voracity)*pow(10,7)))
		{
			sated = false;
		}
		else
		{
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
	setNewLifeStage(LifeStage::PREDATED, timeStep, getId());
}


int Animal::getDaysDigest()
{
	return days_digest;
}

int Animal::getMaxReproductionEvents()
{
	if(gender == AnimalSpecies::GENDERS::FEMALE)
	{
		return getSpecies()->getFemaleMaxReproductionEvents();
	}
	else
	{
		return getSpecies()->getMaleMaxReproductionEvents();
	}
}

list<int> * Animal::getEncounterEvents()
{
	return &encounterEvents;
}

double Animal::calculatePredationProbability(Edible* edibleAnimal, bool retaliation, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW)
{
	double predationProbability = 1;
	if(edibleAnimal->getSpecies()->isMobile())
	{
		double interpolatedHunterVoracity = getInterpolatedHunterVoracity();

		double interpolatedSpeedRatio;

		if(edibleAnimal->getSpeed()>0.0){
		  interpolatedSpeedRatio = getInterpolatedSpeedRatio(edibleAnimal->getSpeed());
	    }else{//this is to circunvent NANs - the ratio is made 1
	      interpolatedSpeedRatio = getInterpolatedSpeedRatio(getTrait(Trait::speed));
	    }

		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/edibleAnimal->calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
		double interpolatedPDF = getInterpolatedPDF(probabilityDensityFunction);

/* 		if(getSpecies()->getScientificName()=="Tyrannosaurus_sp"){
			cout << probabilityDensityFunction << endl;
			cout << normalizedPDF << endl;
		}
 */


		if (getHuntingMode() == HuntingMode::active_hunting || retaliation)
		{
			predationProbability = 
					(predationSpeedRatioAH * interpolatedSpeedRatio +
					predationHunterVoracityAH * interpolatedHunterVoracity +
					predationProbabilityDensityFunctionAH * interpolatedPDF)/3;

		}
		else if (getHuntingMode() == HuntingMode::sit_and_wait)
		{
			predationProbability = 
					(predationSpeedRatioSAW * interpolatedSpeedRatio + //##spd_ratio does not matter anymore
					predationHunterVoracitySAW * interpolatedHunterVoracity +
					predationProbabilityDensityFunctionSAW * interpolatedPDF)/3;
		}
	}

/* 		if(getSpecies()->getScientificName()=="Tyrannosaurus_sp"){
			cout << predationProbability << endl;
		} */



	return predationProbability;
}

bool Animal::predateEdible(Edible* edibleToBePredated, int timeStep, bool retaliation, list<Edible*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea)
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
		double halfLongestDiagonalInsideCurrentCell = sqrt(3 * pow(position->getSize(), 2));
		double distanceToAdd = halfLongestDiagonalInsideCurrentCell*getTrait(Trait::search_area)/maxSearchArea;
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
		//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
		//cerr << " - Animal hunted off: " << huntedAnimal->getId() << "(" << huntedAnimal->getSpecies()->getScientificName() << ")" << endl;
	}
	huntedAnimalsToday.push_back(edibleToBePredated->getId());
	#endif
	//if (predationProbability > randomPredationProbability)
	//TODO attackProb y killProb van EN CADA ESPECIE

//getSpecies()->getKillProbability() > randomPredationProbability

    double prob = calculatePredationProbability(edibleToBePredated, retaliation, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW);//Dinosaur
//prob*
	if (((getSpecies()->getKillProbability() > randomPredationProbability) && edibleToBePredated->getSpecies()->isMobile()) || !edibleToBePredated->getSpecies()->isMobile()) ///arthropods + Dinosaur //
	{
		#ifdef _DEBUG
		if(find(predatedAnimalsToday.begin(), predatedAnimalsToday.end(), edibleToBePredated->getId()) != predatedAnimalsToday.end())
		{
			position->increaseMultipleSamePredatedAnimalToday();
			//cerr << "The animal predated the same animal at least twice the same day: " << endl;
			//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
			//cerr << " - Animal predated: " << huntedAnimal->getId() << "(" << huntedAnimal->getSpecies()->getScientificName() << ")" << endl;
		}
		predatedAnimalsToday.push_back(edibleToBePredated->getId());
		#endif

		getSpecies()->addPredationEventOnOtherSpecies(edibleToBePredated->getSpecies()->getId());
		eatenToday++;
		huntWasSuccessful = true;

		edibleToBePredatedProfitability = getSpecies()->getEdibleProfitability(edibleToBePredated->getSpecies()->getId(), getInstar(), edibleToBePredated->getInstar());

		//arthro and for dinos
	    double forNotToDepleteResource = getTrait(Trait::voracity)/(edibleToBePredatedProfitability+getTrait(Trait::assim));
		double leftovers;

        if(edibleToBePredated->isDepleted(forNotToDepleteResource)){ //resource leftover to partially fulfill voracity
	      leftovers = edibleToBePredated->anyLeft()*(edibleToBePredatedProfitability+getTrait(Trait::assim));
        }else{ //the entire voracity is available
		  leftovers = forNotToDepleteResource;
		}  

		//The target animal is cleaned up from its source lifestage vector. Or the resource is substracted.
		double fullDryMassToBeEaten = position->turnEdibleIntoDryMassToBeEaten(edibleToBePredated, timeStep, this, leftovers);
		//end arthro
		
	/*	cout << fullDryMassToBeEaten << endl;

		if(timeStep==10){
			exit(-1);
		}*/

		//arthro and for dinos commented to prevent segmentation fault if zero biomass - use it when minspores = 0 but need to check several parts
		/* if (edibleToBePredated->isExtinct())
		{
			position->deleteResource(edibleToBePredated);
			delete edibleToBePredated;
		} */

		//double profitableDryMassToBeEaten = fullDryMassToBeEaten * edibleToBePredatedProfitability;
		biomassExperiencedPerSpecies[edibleToBePredated->getSpecies()] += fullDryMassToBeEaten;
		
		//Assim with handling time will be calculated according to the FULL dry mass
		//It will be reduced to only profitableMass after handling calculations
		setFoodMassLeftForNextTimeStep(fullDryMassToBeEaten);
		lastHuntedAnimalDryMass = fullDryMassToBeEaten;
		assimilateLastHuntedAnimalAndComputeHandlingTime();
	}

	predationProbabilitiesFile << getIdStr() << "\t"
	<< edibleToBePredated->getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< edibleToBePredated->getSpecies()->getScientificName() << "\t"
	<< ((edibleToBePredated->isHunting()) ? 1 : 0) << "\t"
	<< calculateDryMass() << "\t"
	<< edibleToBePredated->calculateDryMass() << "\t"
	<< randomPredationProbability << "\t"
	<< prob << "\t"
	<< ((prob*getSpecies()->getKillProbability() > randomPredationProbability) ? 1 : 0) << "\t"
	<< endl;


	return huntWasSuccessful;
}

void Animal::setExperiencePerSpecies()
{
	/*
		Se debe llamar a esta función después de cada forceMolting o después de la 
		creación de los animales en el breed. Ya que debe estar definido el atributo "instar"
		para fijar la experiencia de cada especie
	*/

	for(auto it = getSpecies()->getEdibleResourceSpecies()->begin(); it != getSpecies()->getEdibleResourceSpecies()->end(); it++)
	{
		double preference = getSpecies()->getEdiblePreference((*it)->getId(), getInstar());
		
		biomassExperiencedPerSpecies[(*it)] = preference;
		meanExperiencesPerSpecies[(*it)] = preference;
	}

	for(auto it = getSpecies()->getEdibleAnimalSpecies()->begin(); it != getSpecies()->getEdibleAnimalSpecies()->end(); it++)
	{
		double preference = getSpecies()->getEdiblePreference((*it)->getId(), getInstar());

		biomassExperiencedPerSpecies[(*it)] = preference;
		meanExperiencesPerSpecies[(*it)] = preference;
	}
}

void Animal::updateBiomassExperiencedPerSpecies(int timeStepsPerDay)
{
	float totalExperiencedBiomass = 0;
	daysExperienced++;
	for(auto it = biomassExperiencedPerSpecies.begin(); it != biomassExperiencedPerSpecies.end(); it++)
	{
		totalExperiencedBiomass += (*it).second;
	}
	if(totalExperiencedBiomass > 0.0)
	{
		for(auto it = biomassExperiencedPerSpecies.begin(); it != biomassExperiencedPerSpecies.end(); it++)
		{
			(*it).second = (*it).second / totalExperiencedBiomass;
		}
	}

	double experienceInfluencePerTimeStep = getSpecies()->getExperienceInfluencePerDay() / timeStepsPerDay;

	/*
		To update the experience of a species, a weighted average is performed, where the variable 'experienceInfluencePerTimeStep' 
		determines whether recent experience or past experience is more important. If the value 'experienceInfluencePerTimeStep' is 
		close to 1, recent experience is more important.
	*/
	for(auto it = meanExperiencesPerSpecies.begin(); it != meanExperiencesPerSpecies.end(); it++)
	{
		//1.0 will be the base value, meaning it has not come across any animal yet
		(*it).second = biomassExperiencedPerSpecies[(*it).first] * experienceInfluencePerTimeStep + (*it).second * (1.0 - experienceInfluencePerTimeStep);
		//cout << (*it).second << endl;
	}

	//Jordi - 23/03/2022 - this "biomassExperiencedPerSpecies" needs to turn it back to the original biomass scale, otherwise only the last food item matters added to a 0-1 value
	if(totalExperiencedBiomass > 0.0)
	{
		for(auto it = biomassExperiencedPerSpecies.begin(); it != biomassExperiencedPerSpecies.end(); it++)
		{
			if(daysExperienced <= static_cast<unsigned int>(getTrait(Trait::memoryDepth))){ //this 5 needs to be taken out to the Json file - was already declared somwhere bu Gabi!!!
				(*it).second = (*it).second * totalExperiencedBiomass; //however, to do this well, it really needs to take account of all food items in the last 5 days
			}else{
				(*it).second = (*it).second * totalExperiencedBiomass; 
				(*it).second = (*it).second/static_cast<unsigned int>(getTrait(Trait::memoryDepth)); ///experience averaged over the last 5 days and continue experiencing from 0 days
				daysExperienced = 0;	
			}
		}
	}
}

const double& Animal::getFoodMassLeftForNextTimeStep() const
{
	return foodMassLeftForNextTimeStep;
}

void Animal::setFoodMassLeftForNextTimeStep(const double &foodMassLeftForNextTimeStep)
{
	this->foodMassLeftForNextTimeStep = foodMassLeftForNextTimeStep;
}

void Animal::decreaseFoodMassLeftForNextTimeStep(const double &foodToEliminate)
{
	foodMassLeftForNextTimeStep -= foodToEliminate;
}

bool Animal::isThereLeftoverFood() const
{
	return getFoodMassLeftForNextTimeStep() > DBL_EPSILON;
}

void Animal::assimilateLastHuntedAnimalAndComputeHandlingTime()
{
	if(isThereLeftoverFood() && timeStepMaximumHandlingTimer > 0.0)
	{
		double kelvinTemperature = position->getTemperature() + 273;
		double log_ratio = calculateLogMassRatio(calculateDryMass(), lastHuntedAnimalDryMass);
		double lnHandlingTime = -1814 + 0.7261*log_ratio + 12.04*kelvinTemperature + (-0.02006)*pow(kelvinTemperature, 2);
		double handlingTime = (1.0/exp(lnHandlingTime)) / (60 * 60 * 24); //Converted here to DAYS

		//TODO generalizar a timestep
		double handlingTimeToAdd = handlingTime * (getFoodMassLeftForNextTimeStep() / lastHuntedAnimalDryMass);

		double addedWouldBeHandlingTimer = handlingTimer + handlingTimeToAdd;
		double foodMassToAdd;
		if(addedWouldBeHandlingTimer <= timeStepMaximumHandlingTimer)
		{
			foodMassToAdd = getFoodMassLeftForNextTimeStep();
			//cout << "first" << endl;
		}
		else //addedWouldBeHandlingTimer > timeStepMaximumHandlingTimer
		{
			if((timeStepMaximumHandlingTimer - handlingTimer) > 0.0)
			{
				foodMassToAdd = (timeStepMaximumHandlingTimer - handlingTimer) * (getFoodMassLeftForNextTimeStep() / handlingTimeToAdd);
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
			decreaseFoodMassLeftForNextTimeStep(foodMassToAssimilate / (edibleToBePredatedProfitability + getTrait(Trait::assim)));
			foodMass = getTrait(Trait::voracity);
			sated = true;

			//cout << "SECOND   " << endl;

			//cout << sated << endl; 
		}
		else //0 mass left on the ground OR do not eat anything at all
		{
			foodMassToAssimilate = profitableFoodMassToAdd;
			decreaseFoodMassLeftForNextTimeStep(foodMassToAdd);
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

		double handlingTimeToAssimilate = handlingTime * ((foodMassToAssimilate / (edibleToBePredatedProfitability + getTrait(Trait::assim))) / lastHuntedAnimalDryMass);

		//traits[Trait::energy_tank] += traits[Trait::assim] * foodMassToAssimilate;
		handlingTimer += handlingTimeToAssimilate;
		if(timeStepMaximumHandlingTimer > 0.0)
		{
			setTrait(Trait::search_area, getTrait(Trait::search_area) - (handlingTimeToAssimilate / timeStepMaximumHandlingTimer) * timeStepMaximumSearchArea);
		}

		if(::isnan(foodMass) || ::isnan(handlingTimer))
		{
			Output::cout("Animal id: {} ({}) - The assimilation resulted in a NaN value for foodMass or handlingTimer.\n", this->getId(), this->getSpecies()->getScientificName());
		}

		if(!getSpecies()->getPreserveLeftovers()) 
		{
			setFoodMassLeftForNextTimeStep(0.0);
		}
	}
}

const double Animal::getInterpolatedDryMass() const
{
	return Math_Functions::linearInterpolate01(calculateDryMass(), getSpecies()->getMaximumDryMassObserved());
}

double Animal::calculatePredatoryRiskEdibilityValue(Edible* edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	double edibilityValue = (calculateEncounterProbability(edibleToBeEvaluated, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated->getSpecies()->getId(), getInstar(), edibleToBeEvaluated->getInstar())
			+ getMeanExperience(edibleToBeEvaluated->getSpecies()))/4;

	return edibilityValue;
}

double Animal::calculateEdibilityValue(Edible* edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	double edibilityValue = (calculateEncounterProbability(edibleToBeEvaluated, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated->getSpecies()->getId(), getInstar(), edibleToBeEvaluated->getInstar())
			+ getMeanExperience(edibleToBeEvaluated->getSpecies()))/4;

	return edibilityValue;
}

double Animal::calculateEdibilityValueWithMass(Edible* edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	double edibilityValue = edibleToBeEvaluated->getInterpolatedDryMass()
			* ((calculateEncounterProbability(edibleToBeEvaluated, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated->getSpecies()->getId(), getInstar(), edibleToBeEvaluated->getInstar())
			+ getMeanExperience(edibleToBeEvaluated->getSpecies()))/4);

	return edibilityValue;
}

double Animal::calculateEncounterProbability(Edible* edibleToBeEncountered, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	double encounterProbability = 1.0;
	if(edibleToBeEncountered->getSpecies()->isMobile())
	{
		double interpolatedHuntedVoracity = getInterpolatedHuntedVoracity(edibleToBeEncountered->getVoracity());
		double interpolatedHunterVoracity = getInterpolatedHunterVoracity();
		double interpolatedVoracityProduct = getInterpolatedVoracityProduct(edibleToBeEncountered->getVoracity());

		double interpolatedHuntedBodySize = getInterpolatedHuntedBodySize(edibleToBeEncountered->getCurrentBodySize());
		double interpolatedHunterBodySize = getInterpolatedHunterBodySize();

		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/edibleToBeEncountered->calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
		double interpolatedPDF = getInterpolatedPDF(probabilityDensityFunction);

		switch (getHuntingMode()) {
			case HuntingMode::sit_and_wait: {
				encounterProbability = 
					(encounterHuntedVoracitySAW * interpolatedHuntedVoracity +
					encounterHunterVoracitySAW * interpolatedHunterVoracity +
					encounterVoracitiesProductSAW * interpolatedVoracityProduct +
					encounterHunterSizeSAW * interpolatedHunterBodySize +
					encounterHuntedSizeSAW * interpolatedHuntedBodySize +
					encounterProbabilityDensityFunctionSAW * interpolatedPDF)/6;
				break;
			}
			case HuntingMode::active_hunting: {
				encounterProbability = 
					(encounterHuntedVoracityAH * interpolatedHuntedVoracity +
					encounterHunterVoracityAH * interpolatedHunterVoracity +
					encounterVoracitiesProductAH * interpolatedVoracityProduct +
					encounterHunterSizeAH * interpolatedHunterBodySize +
					encounterHuntedSizeAH * interpolatedHuntedBodySize +
					encounterProbabilityDensityFunctionAH * interpolatedPDF)/6;
				break;
			}
			case HuntingMode::does_not_hunt: {
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}
	}

	return encounterProbability;
}

bool Animal::encounterEdible(Edible* edibleToBeEncountered, float attackOrExposedAttackProbability, int day, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	double randomEncounterProbability = Random::randomUniform();

	encounterProbabilitiesFile << getIdStr() << "\t"
	<< edibleToBeEncountered->getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< edibleToBeEncountered->getSpecies()->getScientificName() << "\t"
	<< ((edibleToBeEncountered->isHunting()) ? 1 : 0) << "\t"
	<< calculateDryMass() << "\t"
	<< edibleToBeEncountered->calculateDryMass() << "\t"
	<< randomEncounterProbability << "\t"
	<< calculateEncounterProbability(edibleToBeEncountered, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) << "\t"
	<< attackOrExposedAttackProbability << "\t"											 
	<< ((attackOrExposedAttackProbability*calculateEncounterProbability(edibleToBeEncountered, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) > randomEncounterProbability)?1:0) << "\t"
	<< endl;

	#ifdef _DEBUG
	if(find(searchedAnimalsToday.begin(), searchedAnimalsToday.end(), edibleToBeEncountered->getId()) != searchedAnimalsToday.end())
	{
		position->increaseMultipleSameSearchedAnimalToday();
		//cerr << "The animal searched for the same animal at least twice the same day: " << endl;
		//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
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
			//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
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

bool Animal::canEatEdible(Edible* edibleToCheck, const list<Edible*> &ediblesHasTriedToPredate)
{
	if(getSpecies()->canEatEdible(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar()))
	{
		//double log_mass_ratio = getLog_mass_ratio(this, edibleToCheck);
		//cout << "LMR -> " << log_mass_ratio << endl;
		
		//cout << edibleToCheck->isDepleted(traits[Trait::voracity]) << endl;
		
		//if(traits[Trait::voracity] < 0){
		//cout << timeStep << endl;	
	//cout << timeStep << endl;	
		//cout << timeStep << endl;	
	//cout << timeStep << endl;	
		//cout << timeStep << endl;	
	//cout << timeStep << endl;	
		//cout << timeStep << endl;	
		
		//cout << traits[Trait::voracity] << endl;
		//cout << edibleToCheck->getId() << endl;
		//exit(-1);
		//}

		//this was incorrect in Gabi's version, he used just resource biomass vs min spore parameter
		//now considering everything the animal takes what is left by if available by decresing its voracity value
		double forNotToDepleteResource = getTrait(Trait::voracity)/(getSpecies()->getEdibleProfitability(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar())+getTrait(Trait::assim));

		//this to to feed on what is left if he entire voracity cannot be satisfied
		//Warning: this prevents this animal to fulfill entirely its original voracity (voracity shrinks) with and alternative
		//resource species if it were available - need to test this behavior
		
		
		//arthros and for dinos - this allows feeding on the leftovers without surpassing
		//the limit imposed by the minspores... so resource can grow back
		bool newVor = false;
		if(edibleToCheck->isDepleted(forNotToDepleteResource))
		{
			if(edibleToCheck->anyLeft()*(getSpecies()->getEdibleProfitability(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar())+getTrait(Trait::assim))>0){
				newVor = true;
			}
		}
			
		return !sated && !isExhausted() &&  
				(!edibleToCheck->isDepleted(forNotToDepleteResource) || newVor == true) &&
				edibleToCheck->getPredatedByID() == -1 && predatedByID == -1 &&
				!hasTriedToHunt((Animal*)edibleToCheck, ediblesHasTriedToPredate);
		// && log_mass_ratio >= position->theWorld->getMinLogMassRatio() && log_mass_ratio <= position->theWorld->getMaxLogMassRatio()/*6.678*/;
	}

	return false;
}

bool Animal::hasTriedToHunt(Animal* edibleToCheck, const list<Edible*> &ediblesHasTriedToPredate)
{
	return find(ediblesHasTriedToPredate.cbegin(), ediblesHasTriedToPredate.cend(), edibleToCheck) != ediblesHasTriedToPredate.cend();
}
