	/*
 * Animal.cpp
 *
 *  Created on: 20 Nov 2012
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"
#include "IBM/World/Map/MapInterface.h"
#include "IBM/World/WorldInterface.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"

using namespace std;
namespace bg = boost::geometry;



Animal::Animal(TerrainCellInterface* const terrainCellInterface)
	: AnimalInterface(terrainCellInterface)
{

}

Animal::Animal(double newFactorEggMassFromMom, double timeStep, const Instar &instar, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender gender,
		bool temporary) 
	: AnimalInterface(mySpecies, instar, temporary), currentAge(0), huntingMode(getSpecies()->getDefaultHuntingMode()), 
	  genome(getSpecies()->getLoci(), getSpecies()->getRandomlyCreatedPositionsForChromosomes(), getSpecies()->getNumberOfChromosomes(), 
	  getSpecies()->getNumberOfLociPerChromosome(), getSpecies()->getNumberOfChiasmasPerChromosome()),
	  traits(EnumClass<Trait::Type>::size(), 0.0), variableTraits(getSpecies()->getVariableTraits()), inHabitatShift(false), inBreedingZone(false), inHabitatShiftBeforeBreeding(false), inHabitatShiftAfterBreeding(false), atDestination(true), 
	  lifeStage(LifeStage::ACTIVE), biomassExperiencedPerSpecies(mySpecies->getWorldInterface()->getExistingAnimalSpecies().size(), 0.0),
	  meanExperiencesPerSpecies(mySpecies->getWorldInterface()->getExistingAnimalSpecies().size(), 0.0)
{
	if(!temporary) {
		mySpecies->getMutableWorldInterface()->generateAnimalId();
	}

	setOtherAttributes(newFactorEggMassFromMom, timeStep, g_numb_prt1,
					   g_numb_prt2, ID_prt1, ID_prt2, gender);
}


Animal::Animal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double newFactorEggMassFromMom, double timeStep, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender gender,
		bool temporary) 
	: AnimalInterface(mySpecies, Instar(), temporary, parentTerrainCellInterface), currentAge(0), huntingMode(getSpecies()->getDefaultHuntingMode()),
	  genome(firstParentGamete, secondParentGamete, getSpecies()->getRandomlyCreatedPositionsForChromosomes(), 
	  getSpecies()->getNumberOfLociPerChromosome(), getSpecies()->getNumberOfChiasmasPerChromosome()), 
	  variableTraits(getSpecies()->getVariableTraits()), inHabitatShift(false), inBreedingZone(false), inHabitatShiftBeforeBreeding(false), inHabitatShiftAfterBreeding(false), atDestination(true), 
	  lifeStage(LifeStage::UNBORN), biomassExperiencedPerSpecies(mySpecies->getWorldInterface()->getExistingAnimalSpecies().size(), 0.0),
	  meanExperiencesPerSpecies(mySpecies->getWorldInterface()->getExistingAnimalSpecies().size(), 0.0)
{
	if(!temporary) {
		getMutableTerrainCellInterface()->getMutableMapInterface().getMutableWorldInterface()->generateAnimalId();
	}

	setOtherAttributes(newFactorEggMassFromMom, timeStep, g_numb_prt1,
					   g_numb_prt2, ID_prt1, ID_prt2, gender);
}


void Animal::setOtherAttributes(double newFactorEggMassFromMom, double timeStep, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies::Gender gender)
{
	setEggDryMassAtBirth(-1.0);
	setFactorEggMassFromMom(newFactorEggMassFromMom);
	mated = false;
	genomeFromMatedMale = NULL;
	idFromMatedMale = -1;
	this->gender = gender;
	this->predatedByID = -1;
	this->setMassAtBirth(getSpecies()->getEggDryMass());

	traits.resize(EnumClass<Trait::Type>::size(), 0.0);
	getSpecies()->initializeFixedTraits(traits);

	steps = 0.0;
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
    daysExperienced = 0;

	actualMoltingTimeVector.reserve(getSpecies()->getNumberOfInstars()-1);
	actualMoltingMassVector.reserve(getSpecies()->getNumberOfInstars()-1);
	finalDevTimeVector.resize(getSpecies()->getNumberOfInstars()-1, -1);
	lengthsVector.resize(getSpecies()->getNumberOfInstars()-1, -1);
	massesVector.resize(getSpecies()->getNumberOfInstars()-1, -1);


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
	wetMassAtTheBeginningOfTheTimeStep = getEggDryMassAtBirth()*getSpecies()->getConversionToWetMass();
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
	pseudoGrowth = getTrait(Trait::Type::growth);

	growthCurveParams = CurveParams::createInstance(getSpecies()->getGrowthCurve()->getType()).release();
}


Animal::~Animal()
{
	if(genomeFromMatedMale != NULL)	delete genomeFromMatedMale;

	delete growthCurveParams;
}


void Animal::setFactorEggMassFromMom(const double &newFactorEggMassFromMom)
{
	factorEggMassFromMom = newFactorEggMassFromMom;
}

void Animal::setEggDryMassAtBirth(const double &newEggDryMassAtBirth)
{
	eggDryMassAtBirth = newEggDryMassAtBirth;
}

const double& Animal::getEggDryMassAtBirth() const 
{ 
	return eggDryMassAtBirth; 
}

const double& Animal::getFactorEggMassFromMom() const
{
	return factorEggMassFromMom;
}

void Animal::calculateFactorEggMassFromMom()
{
	setFactorEggMassFromMom(getTrait(Trait::Type::factorEggMass));
}

void Animal::calculateEggDryMassAtBirth()
{
	//double lengthAtBirth = 0;
	//Only for initialization (value -1). The new offsprings will have factorEggMassFromMom correctly set as factorEggMass(FROM MOM).
	if(getFactorEggMassFromMom() == -1)
	{
		calculateFactorEggMassFromMom();
	}

/* 		if(ageAtInitialization < 1){ //Dinosaurs
		lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(0-thisAnimalVonBertTime0)));
		}else{
		lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(ageAtInitialization-thisAnimalVonBertTime0)));	
		}

	}else{ //newborn during simulation */

	//lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(0-thisAnimalVonBertTime0)));
	
	//}


	//eggDryMassAtBirth = getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB());
	double baseEggDryMassAtBirth = getSpecies()->getEggDryMass() + getSpecies()->getEggDryMass() * getFactorEggMassFromMom();
	//traits[Trait::Type::energy_tank] =  tank_ini * pow(eggDryMassAtBirth, getSpecies()->getBetaScaleTank());
	//currentBodySize = eggDryMassAtBirth - traits[Trait::Type::energy_tank];

	
	double maxMassAtCurrentAge = baseEggDryMassAtBirth + baseEggDryMassAtBirth*getSpecies()->getMaxPlasticityKVonBertalanffy();
	//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
	double minMassAtCurrentAge = baseEggDryMassAtBirth - baseEggDryMassAtBirth*getSpecies()->getMinPlasticityKVonBertalanffy();//Dinosaurs

	//below final eggDryMass is set after the female eggMass trait, if it surpasses the
	//growing curve band limits, it is set to the limit  //Dinosaurs
	double minEggDryMassAtBirth = min(baseEggDryMassAtBirth, maxMassAtCurrentAge);
	double newEggDryMassAtBirth = max(minEggDryMassAtBirth, minMassAtCurrentAge);

	setEggDryMassAtBirth(newEggDryMassAtBirth);

	if(getEggDryMassAtBirth() <= 0.0 + numeric_limits<double>::epsilon())
	{
		throwLineInfoException("Attribute 'eggDryMassAtBirth' must have a value greater than 0");
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
		unsigned int trait_order = getSpecies()->getTraitOrder(variableTraits->at(i));
		moduleNumber = trait_order / getSpecies()->getTraitsPerModule();
		//The division is made using RHO. For every trait, the left side alleles of their own chromosomes must be added.
		for (int j = 0; j < getSpecies()->getRhoRangePerModule(moduleNumber); ++j) {
			if(genome.getHomologousCorrelosomes().at(trait_order).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(trait_order).second->getAllele(j)->getAlphabeticOrder())
			{
				traits[variableTraits->at(i)] += genome.getHomologousCorrelosomes().at(trait_order).first->getAllele(j)->getValue();
			}
			else
			{
				traits[variableTraits->at(i)] += genome.getHomologousCorrelosomes().at(trait_order).second->getAllele(j)->getValue();
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
					traits[variableTraits->at(i)] += genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).first->getAllele(j)->getValue();
				}
				else
				{
					traits[variableTraits->at(i)] += genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).second->getAllele(j)->getValue();
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
						traits[variableTraits->at(i)] += genome.getHomologousCorrelosomes().at(trait_order).first->getAllele(j)->getValue();
					}
					else
					{
						traits[variableTraits->at(i)] += genome.getHomologousCorrelosomes().at(trait_order).second->getAllele(j)->getValue();
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
						traits[variableTraits->at(i)] += (1.0 - genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).first->getAllele(j)->getValue());
					}
					else
					{
						traits[variableTraits->at(i)] += (1.0 - genome.getHomologousCorrelosomes().at(trait_order-distanceFromDominant).second->getAllele(j)->getValue());
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
	if(getSpecies()->getGrowthCurve()->getType() == Curve::Type::Logistic || getSpecies()->getGrowthCurve()->getType() == Curve::Type::VonBertalanffy)
	{
		//pseudoGrowthMean and pseudoGrowthSd have been calculated only using the first generation
		double normalizedPseudoGrowth = (pseudoGrowth - getSpecies()->getPseudoGrowthMean()) / getSpecies()->getPseudoGrowthSd();
		double valueFromNormalDistributionForCholesky = getSpecies()->getValueFromNormalDistributionY();

		//double Knorm = normalizedPseudoGrowth * getSpecies()->cholMat[0][0];
		double Linf = normalizedPseudoGrowth * getSpecies()->getValueFromCholMat(1,0) + valueFromNormalDistributionForCholesky * getSpecies()->getValueFromCholMat(1,1);
		//ASÍ ERA ANTES:
		//Linf = (getSpecies()->vonBertLinf/getSpecies()->vonBertKini)*traits[Trait::Type::growth]+getSpecies()->LinfKcorr*randomUniform();

		//Controlling the range of Linfs for adults
		//Here we use OriginLimits because pseudoGrowth was NOT interpolated
		double minNormalizedPseudoGrowth = (getSpecies()->getMinObservedPseudoValue(Trait::Type::growth) - getSpecies()->getPseudoGrowthMean()) / getSpecies()->getPseudoGrowthSd();
		double maxNormalizedPseudoGrowth = (getSpecies()->getMaxObservedPseudoValue(Trait::Type::growth) - getSpecies()->getPseudoGrowthMean()) / getSpecies()->getPseudoGrowthSd();
		
		//TODO -3.5 y 3.5 porque son los valores mínimos y máximos fijados artificialmente por nosotros para la distribución normal Y
		double minLinf = minNormalizedPseudoGrowth * getSpecies()->getValueFromCholMat(1,0) + (-3.5) * getSpecies()->getValueFromCholMat(1,1);
		double maxLinf = maxNormalizedPseudoGrowth * getSpecies()->getValueFromCholMat(1,0) + (3.5) * getSpecies()->getValueFromCholMat(1,1);

		switch(getSpecies()->getGrowthCurve()->getType()) {
			case Curve::Type::VonBertalanffy: {
				const VonBertalanffyCurve* const castGrowthCurve = static_cast<const VonBertalanffyCurve* const>(getSpecies()->getGrowthCurve());

				Linf = MathFunctions::linearInterpolate(Linf, minLinf, maxLinf, castGrowthCurve->getLdistanceMin(), castGrowthCurve->getLdistanceMax());

				static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->setAsymptoticSize(Linf);
				break;
			}
			case Curve::Type::Logistic: {
				const LogisticCurve* const castGrowthCurve = static_cast<const LogisticCurve* const>(getSpecies()->getGrowthCurve());

				Linf = MathFunctions::linearInterpolate(Linf, minLinf, maxLinf, castGrowthCurve->getLdistanceMin(), castGrowthCurve->getLdistanceMax());

				static_cast<LogisticCurveParams* const>(growthCurveParams)->setAsymptoticSize(Linf);
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}
	}

	pheno_ini = getTrait(Trait::Type::pheno);

	//DONE time is now a double
	//traits[Trait::Type::pheno] = round(traits[Trait::Type::pheno]);
	setTrait(Trait::Type::pheno, date_egg + getTrait(Trait::Type::pheno));

	//We work directly with the traits vector, and store these ini values for later use.
	//OLD size_ini = traits[Trait::Type::factorEggMass];
	voracity_ini = getTrait(Trait::Type::voracity);
	search_area_ini = getTrait(Trait::Type::search_area);
	speed_ini = getTrait(Trait::Type::speed);



	
	tank_ini = getTrait(Trait::Type::energy_tank);
	//OLD assim_ini = traits[Trait::Type::assim];

    //Dinosaurs fixed part ini - dummy initialization, it changes in calculateGrowthCurves() or in forceMolting() and forceMolting2()
	setDryMass(getSpecies()->getEggDryMass());

	//cout << traits[Trait::Type::energy_tank] << endl;
	//Dinosaurs fixed part end
	
	eggDryMassForClutch=getSpecies()->getEggDryMass(); //this is just a dummy initialization
}


void Animal::increaseAge(int increase)
{
	setCurrentAge(getCurrentAge() + increase);
}


const double& Animal::getTrait(const Trait::Type& name) const
{
	return traits[name];
}


void Animal::setTrait(const Trait::Type& name, const double& newValue)
{
	if(name != Trait::Type::factorEggMass && name != Trait::Type::pheno)
	{
		if(newValue < 0.0)
		{
			throwLineInfoException("The trait '" + string(EnumClass<Trait::Type>::to_string(name)) + "' will have a negative value.");
		}
	}

	traits[name] = newValue;
}


double Animal::getValueGrowthCurve(const double &age, const double &midpointValue) const
{
	growthCurveParams->setGrowthCoefficient(getTrait(Trait::Type::growth));
	growthCurveParams->setTime(age);
	switch(getSpecies()->getGrowthCurve()->getType()) {
		case Curve::Type::VonBertalanffy: {
			break;
		}
		case Curve::Type::Logistic: {
			static_cast<LogisticCurveParams* const>(growthCurveParams)->setMidpointValue(midpointValue);
			break;
		}
		case Curve::Type::Logistic3P: {
			break;
		}
		case Curve::Type::Logistic4P: {
			break;
		}
		case Curve::Type::Gompertz: {
			break;
		}
		case Curve::Type::Exponential: {
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}

	return getSpecies()->getGrowthCurve()->getValue(*growthCurveParams);
}

const double Animal::calculateWetMass() const
{ 
	return calculateWetMass(calculateDryMass());
}

const double Animal::calculateWetMass(const double &dryMass) const
{ 
	return getSpecies()->convertToWetMass(dryMass);
}

void Animal::tuneTraits(int timeStep, int timeStepsPerDay, Temperature temperature, float relativeHumidity, ostream& tuneTraitsFile, bool printGrowthData, bool fromForceMolting1)
{
	#ifdef DEBUG
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
	Temperature newT = temperature;
	//Optimo para las curvas de crecimiento=35. Para Vor Speed Search será 25
	Temperature ToptVoracity = getSpecies()->getTempOptVoracity();
	Temperature ToptSearch = getSpecies()->getTempOptSearch();
	Temperature ToptSpeed = getSpecies()->getTempOptSpeed();

	//the two values below are now going to be used again
	double preTsearch = search_area_ini*pow(calculateWetMass(), getSpecies()->getScaleForSearchArea());
	
	
	double preTspeed = getTerrainCellInterface()->getMapInterface().getWorldInterface()->getPredictedSpeed(this);


    wetMassAtTheBeginningOfTheTimeStep = calculateWetMass();

   //Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;

	

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
	traits[Trait::Type::energy_tank] = getSpecies()->getAssignedForMolt()*dinoTankPredicted;*/

	lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		
	//fix_xmid_K =  log((Asym-trex[1,"FC_Km"])/trex[1,"FC_Km"])*(1/target_K)
	
	double asymptoticSize = 0.0;
	switch(getSpecies()->getGrowthCurve()->getType()) {
		case Curve::Type::VonBertalanffy: {
			asymptoticSize = static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->getAsymptoticSize();
			break;
		}
		case Curve::Type::Logistic: {
			asymptoticSize = static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize();
			break;
		}
		default: {
			break;
		}
	}

	xmid = log((asymptoticSize-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::Type::growth));
       
	//Forcing continuous growth in Dinosaurs - warning this involves heavy investment in growth - reproduction?
   	double dinoLengthPredicted = getValueGrowthCurve(getCurrentAge(), xmid);
	//if(currentAge > 5){ //to surpass the critical young stages

	double dinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(dinoLengthPredicted,getSpecies()->getScaleForMassBforMature());

    //dinos2023 - moulting only if realized mass is > predicted mass

 	if(getTerrainCellInterface()->getMapInterface().getWorldInterface()->isDinosaurs() && getInstar() < Instar(getSpecies()->getNumberOfInstars())){
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
	traits[Trait::Type::energy_tank] = getSpecies()->getAssignedForMolt()*(calculateDryMass()-currentBodySize);
	//cout << "holar" << endl;
	}   */  
//END - not for arthropods only for debugging 

	//death criterion fixed to the true plasticity bands - Dinosaurs
	double minMassAtCurrentAge;
	double longevity = getSpecies()->getLongevitySinceMaturation()*ageOfFirstMaturation;

/* 	cout << ageOfFirstMaturation << endl;
	exit(-1); */

	if (getCurrentAge() > (0.15*longevity)){ //arthropod to prevent the tiny animals from dying too soon...
	minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * getSpecies()->getMinPlasticityKVonBertalanffy();
	}else{
    minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * 3 * getSpecies()->getMinPlasticityKVonBertalanffy();
	}
//Dinosaurs
/* 	double minMassAtCurrentAge = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(currentAge-thisAnimalVonBertTime0)));
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
		if(getCurrentAge() == 0 || getCurrentAge() == 1){
			nextDinoLengthPredicted = getValueGrowthCurve(1, xmid);
		}
		else {
			nextDinoLengthPredicted = getValueGrowthCurve(getCurrentAge() + 1, xmid);
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
		
		
		if(massForNextReproduction > calculateDryMass() && ageForNextReproduction >= getCurrentAge()){
		
			if(ageForNextReproduction > getCurrentAge()){
				slopeTarget = (massForNextReproduction - calculateDryMass())/(ageForNextReproduction-getCurrentAge());
				interceptTarget = calculateDryMass()-slopeTarget*getCurrentAge();
				nextDinoMassPredicted = interceptTarget + slopeTarget*(getCurrentAge()+1);

				//nextDinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());
				slopeTarget = 0.0;
				interceptTarget = 0.0;
			}else{//force currentAge-1 for calculations as to avoid nans
				slopeTarget = (massForNextReproduction - calculateDryMass())/(ageForNextReproduction-(getCurrentAge()-1));
				interceptTarget = calculateDryMass()-slopeTarget*(getCurrentAge()-1);
				nextDinoMassPredicted = interceptTarget + slopeTarget*getCurrentAge();
			    //nextDinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());
				
				
				slopeTarget = 0.0;
				interceptTarget = 0.0;
			}
				
		}else{
		
			if(massForNextReproduction < calculateDryMass() && ageForNextReproduction > getCurrentAge()){
				nextDinoMassPredicted = interceptTarget + slopeTarget*(getCurrentAge()+1); 
			}
			
			//here we force a positive slope to allow animals to keep feeding until the target is met 
			if(massForNextReproduction > calculateDryMass() && ageForNextReproduction < getCurrentAge()){
				slopeTarget = (massForNextReproduction-calculateDryMass())/(ageForNextReproduction-getCurrentAge());
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



		

		minTotalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, 0.0);

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

					/* double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(0-thisAnimalVonBertTime0)));
					eggDryMassForClutch = getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB())+getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB()) * traits[Trait::Type::factorEggMass];

					if(getSpecies()->hasEggClutchFromEquation())
					{
						clutchDryMass = (getSpecies()->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, getSpecies()->getForClutchMassScale())) / getSpecies()->getConversionToWetMass();
					}
					else //If it does not come from equation,
					{
						clutchDryMass = eggDryMassForClutch*getSpecies()->getEggsPerBatch(); //clutch dry mass is identical to the mass of an egg - done for mites or others that only lay one egg at a time.
					} */




		minTotalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, 0.0);

		minNextDinoMassPredicted = nextDinoMassPredicted - nextDinoMassPredicted * getSpecies()->getMinPlasticityKVonBertalanffy();

	maxNextDinoMassPredicted = nextDinoMassPredicted + nextDinoMassPredicted * (getSpecies()->getMaxPlasticityKVonBertalanffy()*1.0);

		
		targetMass = maxNextDinoMassPredicted - calculateDryMass();

		if(targetMass<0){

		if(getCurrentAge() > ageForNextReproduction){
			targetMass = fabs(maxNextDinoMassPredicted - calculateDryMass());		
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
	double postTvor = MathFunctions::useJohnsonLewis(
		newT, getTrait(Trait::Type::actE_vor), getSpecies()->getTrait(Trait::Type::voracity)->getLowerThreshold(),
		getSpecies()->getTrait(Trait::Type::voracity)->getUpperThreshold(), ToptVoracity, getSpecies()->getEdVoracity(),
		voracity_ini
	);

    /*cout << "newT: " << newT << endl;
    cout << "dellsNewT: " << dellsNewT << endl;
    cout << "dellsMinT: " << dellsMinT << endl;
    cout << "dellsMaxT: " << dellsMaxT << endl;

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

	setTrait(Trait::Type::voracity, preFinalVoracity);
	//below we ensure that voracity does not get into negative values, if for instance voracity_ini is negative


    preFinalVoracity = max(getTrait(Trait::Type::voracity),0.0);

    //preFinalVoracity = min(preFinalVoracity+calculateDryMass(),maxNextInstarMassFromVBPlasticity)-calculateDryMass();

	if(preFinalVoracity > 0 && nextDinoMassPredicted > 0.00000001) //when slope and interceptTarget = 0
	{
	//BELOW WE INCLUDE PLASTICITY DUE TO CONDITION EFFECTS FOR VORACITY
	//double massToTarget = maxNextInstarMassFromVBPlasticity - calculateDryMass();

    //0 is the mass to achieve when the animal is actually on the target
    double fullTrajectoryMass = maxNextDinoMassPredicted - minNextDinoMassPredicted;

	double finalVoracity = MathFunctions::linearInterpolate(
    		targetMass,
			0,
			fullTrajectoryMass,
			preFinalVoracity - preFinalVoracity * getSpecies()->getMinPlasticityDueToConditionVor(),
			preFinalVoracity + preFinalVoracity * getSpecies()->getMaxPlasticityDueToConditionVor()
			);

				
    //below we ensure that voracity does not get into negative values
	setTrait(Trait::Type::voracity, max(finalVoracity, 0.0));

	//cout << "here: " << traits[Trait::Type::voracity]/calculateDryMass() << endl;

    //BELOW WE INCLUDE TEMP TUNING FOR SEARCH AREA AND SPEED
	postTsearch = MathFunctions::useJohnsonLewis(
		newT, getTrait(Trait::Type::actE_search), getSpecies()->getTrait(Trait::Type::search_area)->getLowerThreshold(),
		getSpecies()->getTrait(Trait::Type::search_area)->getUpperThreshold(), ToptSearch, getSpecies()->getEdSearch(),
		preTsearch
	);
	
	postTsearch = preTsearch; //this is to have more predictive mobilities

	postTspeed = MathFunctions::useJohnsonLewis(
		newT, getTrait(Trait::Type::actE_speed), getSpecies()->getTrait(Trait::Type::speed)->getLowerThreshold(),
		getSpecies()->getTrait(Trait::Type::speed)->getUpperThreshold(), ToptSpeed, getSpecies()->getEdSpeed(),
		preTspeed
	);

    //BELOW WE INCLUDE CONDITION-DEPENDENT PLASTICITY FOR SEARCH AREA AND SPEED
    finalSearch = MathFunctions::linearInterpolate(
    		targetMass,
    		0,
			fullTrajectoryMass,
			postTsearch - postTsearch*getSpecies()->getMinPlasticityDueToConditionSearch(),
			postTsearch + postTsearch*getSpecies()->getMaxPlasticityDueToConditionSearch());

    finalSearch = postTsearch; //this is to have more predictive mobilities
   
    maxPostTsearch = postTsearch + postTsearch * getSpecies()->getMaxPlasticityDueToConditionSearch();
	setTrait(Trait::Type::search_area, finalSearch);

    //traits[Trait::Type::search_area] = preTsearch;

    finalSpeed = MathFunctions::linearInterpolate(
    		targetMass,
			0,
			fullTrajectoryMass,
			postTspeed - postTspeed * getSpecies()->getMinPlasticityDueToConditionSpeed(),
			postTspeed + postTspeed * getSpecies()->getMaxPlasticityDueToConditionSpeed()
			);

	setTrait(Trait::Type::speed, finalSpeed);

/* if(getId() == 3 && timeStep==15){
		cout << nextDinoMassPredicted << endl;
		//exit(-1);
		}  */

	if(::isnan(finalSpeed) || ::isnan(finalSearch) || ::isnan(finalVoracity) || ::isinf(finalSpeed) || ::isinf(finalSearch) || ::isinf(finalVoracity))
	{
		setNewLifeStage(LifeStage::SHOCKED, timeStep);
		return;
		/*
		cout << "Animal id: " << getId() << " (" << this->getSpecies()->getScientificName() << ") - The growing curve resulted in a NaN value." << endl;
		cout << finalVoracity << endl;
		cout << finalSpeed << endl;
		cout << finalSearch << endl;
		exit(-1);
		*/
	}

	

	//below we ensure that search area does not get into negative values
	setTrait(Trait::Type::search_area, max(finalSearch, 0.0));
	//below we ensure that speed does not get into negative values
	setTrait(Trait::Type::speed, max(finalSpeed, 0.0));
	//Jordi ends here::::

	 //below we establish a decrease in activity due to previous encounters with predators
    //the punishment on the traits depends on the parameter decreaseOnTraitsDueToEncounters
	double vorAfterEncounters = getTrait(Trait::Type::voracity) / (getSpecies()->getDecreaseOnTraitsDueToEncounters()*todayEncountersWithPredators+1);
	double searchAfterEncounters = getTrait(Trait::Type::search_area) / (getSpecies()->getDecreaseOnTraitsDueToEncounters()*todayEncountersWithPredators+1);

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

	setTrait(Trait::Type::voracity, vorAfterEncounters);

	setTrait(Trait::Type::search_area, searchAfterEncounters);

	}else{

		setTrait(Trait::Type::search_area, 0.0);

		setTrait(Trait::Type::speed, 0.0);

		setTrait(Trait::Type::voracity, preFinalVoracity);
      

	}
	timeStepMaximumHandlingTimer = 1.0;
	timeStepMaximumSearchArea = getTrait(Trait::Type::search_area);



	
	
	//Dinos2023 - this should occur only for Dinos or large vertebrates but needs to be tested for invertebrates
	//We truncate voracity using the allometric equation of Garland 1983 for terrestrial mammals 
	double newVoracity = getTerrainCellInterface()->getMapInterface().getWorldInterface()->calculateNewVoracity(calculateWetMass(), getSpecies()->getConversionToWetMass());
	
	if(newVoracity < getTrait(Trait::Type::voracity))
	{
		setTrait(Trait::Type::voracity, newVoracity);
	} 
	//end Dinos2023


	//We assume that encounters DO NOT AFFECT the animal speed - although in reality stamina should be affected
	//However this affects also to the predators so we do not consider this for now
	//traits[AnimalSpecies::TraitOrderConverter::speed] = traits[AnimalSpecies::TraitOrderConverter::speed] / (getSpecies()->decreaseOnTraitsDueToEncounters*todayEncountersWithPredators+1);

	//traits[Trait::Type::energy_tank] < 0
	if(getTrait(Trait::Type::energy_tank) <= numeric_limits<double>::epsilon()) //Dinosaurs calculateDryMass() < minMassAtCurrentAge
	{
		if(getSpecies()->isSurviveWithoutFood()) {
			setEnergyTank(0.1);
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
	

	if(isMature() && gender == AnimalSpecies::Gender::MALE)
	{
		setTrait(Trait::Type::search_area, getTrait(Trait::Type::search_area) * getSpecies()->getMaleMobility());
	}

	//TODO PARA LOS CRECIMIENTO INDETERMINADO, SE DEJAN POR AHORA QUE SIGAN MOVIENDOSE INFINITO
	//TODO EN UN FUTURO SE HARÁ PARA LOS INDETERMINADO, DEJANDO QUE SIGAN CRECIENDO SI ALCANZAN EL TIEMPO DE LONGEVITY
	if(getCurrentAge() >= ageOfFirstMaturation * getSpecies()->getLongevitySinceMaturation())
	{
		setNewLifeStage(LifeStage::SENESCED, timeStep);
	}


	//below it means that metabolic downregulation only exists for predators, such as spiders
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		setTrait(Trait::Type::voracity, getTrait(Trait::Type::voracity) - getSpecies()->getPercentageCostForMetabolicDownregulationVoracity() * getTrait(Trait::Type::voracity));
		setTrait(Trait::Type::search_area, getTrait(Trait::Type::search_area) - getSpecies()->getPercentageCostForMetabolicDownregulationSearchArea() * getTrait(Trait::Type::search_area));
		setTrait(Trait::Type::speed, getTrait(Trait::Type::speed) - getSpecies()->getPercentageCostForMetabolicDownregulationSpeed() * getTrait(Trait::Type::speed));
	}

	//In this version the shock_resistance trait involves Maximum Critical Temperature (CTmax)
	if(getTrait(Trait::Type::shock_resistance)<(temperature.getTemperatureKelvin()))
	{
		setNewLifeStage(LifeStage::SHOCKED, timeStep);
	}

	if(relativeHumidity < getSpecies()->getMinRelativeHumidityThreshold())
	{
		setNewLifeStage(LifeStage::DIAPAUSE);
	}



	if(printGrowthData)
	{
		tuneTraitsFile << this->getIdStr() << "\t"
		<< getSpecies()->getScientificName() << "\t"
		<< lifeStage << "\t"
		<< getCurrentAge() << "\t"
		<< getInstar() << "\t"
		<< currentBodySize << "\t"
		<< getTrait(Trait::Type::energy_tank) << "\t"
		<< tankAtGrowth << "\t" //done to document excessInvestment behavior
		<< calculateDryMass() << "\t"
		<< isMature() << "\t"
		<< minMassAtCurrentAge << "\t"
		<< finalJMinVB << "\t"
		<< finalJMaxVB << "\t"
		<< voracity_ini << "\t"
		<< calculateDryMass()+finalJMinVB << "\t"
		<< calculateDryMass()+getTrait(Trait::Type::voracity) << "\t"
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
		<< getTrait(Trait::Type::voracity) << "\t"
		<< getTrait(Trait::Type::search_area) << "\t"
		<< getTrait(Trait::Type::speed) << "\t"
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


const int Animal::getTodayEncountersWithPredators() const
{
	return todayEncountersWithPredators;
}


double Animal::getTotalMetabolicDryMassLoss(double wetMass, double proportionOfTimeTheAnimalWasMoving)
{
	double totalMetabolicDryMassLoss = getTerrainCellInterface()->getMapInterface().getWorldInterface()->calculateTotalMetabolicDryMassLoss(wetMass, proportionOfTimeTheAnimalWasMoving, this);

	totalMetabolicDryMassLoss = totalMetabolicDryMassLoss / getTerrainCellInterface()->getMapInterface().getWorldInterface()->getTimeStepsPerDay();

	return totalMetabolicDryMassLoss;
}

void Animal::setEnergyTank(const double &newValue)
{
	double valueToSet = (newValue < 0.0) ? 0.0 : newValue;

	setTrait(Trait::Type::energy_tank, valueToSet);
}

//TODO parametro para que se ejecuten cada X timesteps

void Animal::metabolize(int timeStep)
{
	#ifdef DEBUG
		double lastEnergyTank = traits[Trait::Type::energy_tank];
		double lastBodySize = traits[Trait::Type::factorEggMass];
	#endif

	//double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;

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
	steps = 0.0;
	stepsAttempted = 0;

	double totalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, proportionOfTimeTheAnimalWasMoving);

	//Downregulation only here, do not change this into getMetabolicDryMassLoss because it would alter the expected loss in tuneTraits
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLoss -= totalMetabolicDryMassLoss * getSpecies()->getPercentageMetabolicDownregulation();
	}

	setEnergyTank(getTrait(Trait::Type::energy_tank) - totalMetabolicDryMassLoss);

	#ifdef DEBUG
		double afterLossEnergyTank = traits[Trait::Type::energy_tank];
		if(afterLossEnergyTank >= lastEnergyTank)
		{
			cout << "The metabolic loss was 0 or positive:" << endl;
			cout << " - Animal: " << getId() << "(" << getSpecies()->getScientificName() << ")" << endl;
			cout << " - Last energy tank: " << lastEnergyTank << endl;
			cout << " - After loss energy tank: " << afterLossEnergyTank << endl;
		}
	#endif
}

double Animal::getMeanExperience(const unsigned int speciesId) const
{
	return meanExperiencesPerSpecies.at(speciesId); 
}


const LifeStage& Animal::getLifeStage() const 
{ 
	return lifeStage; 
}


const double Animal::getCurrentLength() const
{
	double xmid = 0.0;

	if(getSpecies()->getGrowthCurve()->getType() == Curve::Type::Logistic){
		double lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::Type::growth));
	}

	return getValueGrowthCurve(getCurrentAge(), xmid);
}

//TODO parametro para que se ejecuten cada X timesteps

void Animal::grow(int timeStep)
{
	#ifdef DEBUG
		double lastEnergyTank = traits[Trait::Type::energy_tank];
		double lastBodySize = traits[Trait::Type::factorEggMass];
	#endif

	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;

	double lengthAtBirth = 0;
	double xmid = 0;					
	double propAdultMass = 0;
	double currentEggs = 0;
	

	if(getSpecies()->getGrowthCurve()->getType() == Curve::Type::Logistic){
		double lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::Type::growth));
	}
		
	if(!isMature())
	{


	//to force initialization in juveniles - arthro - the values are dummy and won't be used unitl updated 
		double daysForPseudoTargetReproduction;
		daysForPseudoTargetReproduction = 1.1*getCurrentAge();//1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = getCurrentAge() + daysForPseudoTargetReproduction;
		double currentLength = getValueGrowthCurve(getCurrentAge(), xmid);
		double nextReproLength = getValueGrowthCurve(ageForNextReproduction, xmid);


		massOfMaturationOrLastReproduction = getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());
		massForNextReproduction = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());
		//end arthro

		//arthro - assignedForMolt is considered for molting or investment could be lower than the actual target
		if(calculateDryMass() >= massesVector[getInstar().getValue()] && getCurrentAge() >= finalDevTimeVector[getInstar().getValue()])
		{

			/* if(currentAge > 1){
			cout << currentAge << endl;
			exit(-1);
			} */

			//massesVector[instar+1]

			double investment = calculateDryMass()*getSpecies()->getAssignedForMolt();
			double next_tank = tank_ini * pow(massesVector[getInstar().getValue()], getSpecies()->getBetaScaleTank());
			double next_size = massesVector[getInstar().getValue()] - next_tank;
			double excessInvestment = investment-massesVector[getInstar().getValue()];
			lastMassBeforeMaturationOrOviposition = calculateDryMass();

			/* cout << excessInvestment << endl;
			cout << next_tank << endl;  */
			//exit(-1);
		
			setDryMass(
				next_tank + excessInvestment*(1-getSpecies()->getExcessInvestInSize()),
				next_size + excessInvestment*getSpecies()->getExcessInvestInSize()
			);
			
			tankAtGrowth = getTrait(Trait::Type::energy_tank);

			#ifdef DEBUG
				double afterMoultEnergyTank = traits[Trait::Type::energy_tank];
				double afterMoultBodySize = traits[Trait::Type::factorEggMass];
				if(afterMoultEnergyTank >= lastEnergyTank)
				{
					cout << "The energy tank after moult was higher than before:" << endl;
					cout << " - Animal: " << getId() << "(" << getSpecies()->getScientificName() << ")" << endl;
					cout << " - Last energy tank: " << lastEnergyTank << endl;
					cout << " - After moult energy tank: " << afterMoultEnergyTank << endl;
				}
				if(afterMoultBodySize <= lastBodySize)
				{
					cout << "The body size after moult was lower than before: " << endl;
					cout << " - Animal: " << getId() << "(" << getSpecies()->getScientificName() << ")" << endl;
					cout << " - Last body size: " << lastBodySize << endl;
					cout << " - After moult body size: " << afterMoultBodySize << endl;
				}
			#endif
			
			moveOnNextInstar();

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
				if((getInstar() >= getSpecies()->getInstarFirstReproduction()) || (getInstar() == Instar(getSpecies()->getNumberOfInstars())))
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

			
			actualMoltingTimeVector.push_back(getCurrentAge()+1);
			//arthro really for dinos actualMoltingMassVector.push_back(investment-excessInvestment);
			ageOfLastMoultOrReproduction = getCurrentAge();



			if(isMature())
			{


				//TODO TIMESTEP
				//This gets decremented in activateAnimals at the beginning of each day.
				massOfMaturationOrLastReproduction = calculateDryMass();

				//arthros but very important for repro in Dinos!!!! ageOfFirstMaturation by age not by instar
				 /* cout << currentAge << endl;
				cout << finalDevTimeVector[(getSpecies()->getInstarFirstReproduction()-2)] << endl;
				exit(-1);  */

				if(getCurrentAge() == getAgeFirstReproduction()){ //instar+2 == getSpecies()->getInstarFirstReproduction()){
				 	
					//if(getSpecies()->getGrowthType() == 0){

						ageOfMaturation = getCurrentAge();
						
						ageOfFirstMaturation = getCurrentAge();

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
				if(getCurrentAge()==round(ageOfFirstMaturation)) // rep_count==0
				{
					pupaTimer = getSpecies()->getPupaPeriodLength()*getTerrainCellInterface()->getMapInterface().getWorldInterface()->getTimeStepsPerDay();
				}
				//double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;
				//DONE RepFactor for different genders
				double daysForTargetReproduction;
				if(gender == AnimalSpecies::Gender::FEMALE)
				{

					//Dinosaurs
					//and arthros we remove eggDryMass from script because it may not work - early death from starvation
					//double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(0-thisAnimalVonBertTime0)));
					eggDryMassForClutch = getSpecies()->getEggDryMass()+getSpecies()->getEggDryMass()*getTrait(Trait::Type::factorEggMass);
				
					double maxMassAtCurrentAge = eggDryMassForClutch + eggDryMassForClutch*getSpecies()->getMaxPlasticityKVonBertalanffy();
					//below is a shorter limit for death - if minPlasticityKVonBertalanffy == 0, death occurs when total body mass = body size
					double minMassAtCurrentAge = eggDryMassForClutch - eggDryMassForClutch * getSpecies()->getMinPlasticityKVonBertalanffy();//Dinosaurs

					//below final eggDryMass is set after the female eggMass trait, if it surpasses the
					//growing curve band limits, it is set to the limit  //Dinosaurs
					eggDryMassForClutch = min(eggDryMassForClutch, maxMassAtCurrentAge);
					eggDryMassForClutch = max(eggDryMassForClutch, minMassAtCurrentAge);
				
				/* double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(0-thisAnimalVonBertTime0)));
				eggDryMassForClutch = getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB())+getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB()) * traits[Trait::Type::factorEggMass];
 */

					if(eggDryMassForClutch < 0){
						//cout << "lengthAtBirth: " << lengthAtBirth << endl;
						cout <<"eggDryMassForClutch: " << eggDryMassForClutch << endl;
						double asymptoticSize;
						switch(getSpecies()->getGrowthCurve()->getType()) {
							case Curve::Type::VonBertalanffy: {
								asymptoticSize = static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->getAsymptoticSize();
								break;
							}
							case Curve::Type::Logistic: {
								asymptoticSize = static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize();
								break;
							}
							case Curve::Type::Logistic3P: {
								asymptoticSize = static_cast<const Logistic3PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
								break;
							}
							case Curve::Type::Logistic4P: {
								asymptoticSize = static_cast<const Logistic4PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
								break;
							}
							case Curve::Type::Gompertz: {
								asymptoticSize = static_cast<const GompertzCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
								break;
							}
							default: {
								asymptoticSize = 0.0;
								break;
							}
						}
						cout << "asymptoticSize: " << asymptoticSize << endl;
						cout << "Growth: " << getTrait(Trait::Type::growth) << endl;
						if(getSpecies()->getGrowthCurve()->getType() == Curve::Type::VonBertalanffy) {
							cout << "thisAnimalVonBertTime0: " << static_cast<const VonBertalanffyCurve* const>(getSpecies()->getGrowthCurve())->getVonBertTime0() << endl;
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

							double lengthAtThisAge = getValueGrowthCurve(getCurrentAge(), xmid);


							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
							
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = MathFunctions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
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
					double currentLength = getValueGrowthCurve(getCurrentAge(), xmid);
					double nextReproLength = getValueGrowthCurve(getCurrentAge()+1, xmid);
					
					
					double nextReproMass = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());	
					double currentMass =getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());

					if(getSpecies()->hasIndeterminateGrowth()){
						if(getInstar() < Instar(getSpecies()->getNumberOfInstars())){
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

				if(getCurrentAge()==round(ageOfFirstMaturation)){//currentAge==ageOfFirstMaturation rep_count==0
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
                    ageForNextReproduction = getCurrentAge() + getSpecies()->getPupaPeriodLength();


				}else{
				//arthro	
				/* ageOfLastMoultOrReproduction = currentAge;
					ageForNextReproduction = currentAge + daysForTargetReproduction; */
					ageForNextReproduction = getCurrentAge() + daysForTargetReproduction;
				}

			//arthro
			ageOfLastMoultOrReproduction = getCurrentAge();
			//dinos2023 - to ensure that ageForNextReproduction does not surpass the next instar (year)
			if(getInstar() < Instar(getSpecies()->getNumberOfInstars())){
				ageForNextReproduction = min(ageForNextReproduction,finalDevTimeVector[getInstar().getValue()-1]+round(0.9*(finalDevTimeVector[getInstar().getValue()]-finalDevTimeVector[getInstar().getValue()-1])));
			}

			//end dinos2023
	

			}
		}
	}
	else //isMature() == true
	{
		//Jordi: the following is to force males to molt whether they mate or not - otherwise they stop growing if the do not mate
		if(gender == AnimalSpecies::Gender::MALE){
			if(getSpecies()->hasIndeterminateGrowth() && (getInstar() < Instar(getSpecies()->getNumberOfInstars()))){
				if(getCurrentAge() > finalDevTimeVector[getInstar().getValue()-1]+1){//plus one day enough to mate
			    //cout << "rep_count: " << rep_count << endl;
				mature = false;
				}
			}
		}else{
		
		//Dinos2023 Recalculate this here because females that are initialized as adults
		//should use their factorEggMass trait too
		eggDryMassForClutch = getSpecies()->getEggDryMass()+getSpecies()->getEggDryMass()*getTrait(Trait::Type::factorEggMass);

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
		////Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;

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

		
		if(investmentForReproduction >= massForNextReproduction &&  getCurrentAge() >= ageForNextReproduction  && getSpecies()->getEggsPerBatch()>0)  //Arthropods post-dinos July 2021  && getSpecies()->getEggsPerBatch()>0
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
				double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(0-thisAnimalVonBertTime0)));
				eggDryMassForClutch = getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB())+getSpecies()->getCoefficientForMassA()*pow(lengthAtBirth,getSpecies()->getScaleForMassB()) * traits[Trait::Type::factorEggMass];

 				if(eggDryMassForClutch < 0){
					cout << "lengthAtBirth2: " << lengthAtBirth << endl;
					cout << "eggDryMassForClutch2: " << eggDryMassForClutch << endl;
					cout << "asymptoticSize:" << static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize() << endl;
					cout << "Growth:" << traits[Trait::Type::growth] << endl;
					cout << "thisAnimalVonBertTime02: " << thisAnimalVonBertTime0 << endl;
					exit(-1);
				}

				if(lengthAtBirth < 0)
				{
					cout << "Animal id: " << getId() << " (" << this->getSpecies()->getScientificName() << ") - The length at birth has been calculated as a negative value. Check growth related variables." << endl;
				}*/


				if(getSpecies()->hasEggClutchFromEquation())
				{
					clutchDryMass = (getSpecies()->getForClutchMassCoefficient() * pow(lastMassBeforeMaturationOrOviposition, getSpecies()->getForClutchMassScale())) / getSpecies()->getConversionToWetMass();
				}
				else //If it does not come from equation,
				{
						//arthro
						if(getSpecies()->hasIndeterminateGrowth()){
		
							double lengthAtThisAge = getValueGrowthCurve(getCurrentAge(), xmid);
						
							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
												
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = MathFunctions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
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

				ageForNextReproduction = getCurrentAge() + daysForTargetReproduction;
				ageOfLastMoultOrReproduction = getCurrentAge();

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
							
							currentEggs = MathFunctions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
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
			else if(getCurrentAge() >= ageForNextReproduction)
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
	double thisCurrentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - getTrait(Trait::Type::pheno) + 1.0/timeStepsPerDay;
	if (thisCurrentAge > 0)
	{
		setNewLifeStage(LifeStage::ACTIVE);
		setCurrentAge(round(thisCurrentAge));
		ageOfLastMoultOrReproduction = thisCurrentAge;//Dinosaurs end
	}
}

const double& Animal::getCurrentAge() const
{
	return currentAge;
}

void Animal::setCurrentAge(const double &newCurrentAge)
{
	currentAge = newCurrentAge;
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


void Animal::isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(double relativeHumidity)
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


double Animal::getInterpolatedHuntedVoracity(double huntedVoracity) const
{
	return MathFunctions::linearInterpolate01(huntedVoracity,getSpecies()->getMinVorHunted(),getSpecies()->getMaxVorHunted()); 
	//return ((huntedVoracity - getSpecies()->getMeanVorHunted()) / getSpecies()->getSdVoracityHunted());
}


double Animal::getInterpolatedHunterVoracity() const
{
	return MathFunctions::linearInterpolate01(getTrait(Trait::Type::voracity),getSpecies()->getMinVorHunter(),getSpecies()->getMaxVorHunter()); 
	//return ((traits[Trait::Type::voracity] - getSpecies()->getMeanVoracityHunter()) / getSpecies()->getSdVoracityHunter());
}


double Animal::getInterpolatedVoracityProduct(double huntedVoracity) const
{
	return MathFunctions::linearInterpolate01(getTrait(Trait::Type::voracity)* huntedVoracity,getSpecies()->getMinVorXVor(),getSpecies()->getMaxVorXVor()); 
	/*return ((traits[Trait::Type::voracity] * huntedVoracity - getSpecies()->getMeanVoracityXVoracity())
			/ getSpecies()->getSdVoracityXVoracity());*/
}


double Animal::getInterpolatedHuntedBodySize(double huntedBodySize) const
{
	return MathFunctions::linearInterpolate01(huntedBodySize,getSpecies()->getMinSizeHunted(),getSpecies()->getMaxSizeHunted()); 
	//return ((huntedBodySize - getSpecies()->getMeanSizeHunted()) / getSpecies()->getSdSizeHunted());
}


double Animal::getInterpolatedHunterBodySize() const
{
	return MathFunctions::linearInterpolate01(currentBodySize,getSpecies()->getMinSizeHunter(),getSpecies()->getMaxSizeHunter()); 
	//return ((currentBodySize - getSpecies()->getMeanSizeHunter()) / getSpecies()->getSdSizeHunter());
}


double Animal::getInterpolatedPDF(double probabilityDensityFunction) const
{
	return MathFunctions::linearInterpolate01(probabilityDensityFunction,getSpecies()->getMinProbabilityDensityFunction(),getSpecies()->getMaxProbabilityDensityFunction());
	/*return ((probabilityDensityFunction - getSpecies()->getMeanProbabilityDensityFunction())
			/ getSpecies()->getSdProbabilityDensityFunction());*/
}


double Animal::getInterpolatedSpeedRatio(double huntedSpeed) const
{

	return MathFunctions::linearInterpolate01(getTrait(Trait::Type::speed)/huntedSpeed,getSpecies()->getMinSpeedRatio(),getSpecies()->getMaxSpeedRatio());
	/*return (((traits[Trait::Type::speed] / huntedSpeed) - getSpecies()->getMeanSpeedRatio())
			/ getSpecies()->getSdSpeedRatio());*/

}


double calculateLogMassRatio(double hunterAnimalDryMass, double huntedAnimalDryMass)
{
	return log(hunterAnimalDryMass/huntedAnimalDryMass);
}


double Animal::getSearchAreaHunter()
{
	return ((getTrait(Trait::Type::search_area) - getSpecies()->getMeanSearchAreaHunter())
			/ getSpecies()->getSdSearchAreaHunter());
}


double Animal::getSpeedHunter()
{
	return ((getTrait(Trait::Type::speed) - getSpecies()->getMeanSpeedHunter()) / getSpecies()->getSdSpeedHunter());
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

const double& Animal::getPhenoIni() const
{
	return pheno_ini;
}

const int& Animal::getDateEgg() const
{
	return date_egg;
}

const int& Animal::getAgeFirstRep() const
{
	return age_first_rep;
}

const int& Animal::getRepCount() const
{
	return rep_count;
}

const int& Animal::getFecundity() const
{
	return fecundity;
}

const int& Animal::getGenerationNumberFromFemaleParent() const
{
	return generationNumberFromFemaleParent;
}

const int& Animal::getGenerationNumberFromMaleParent() const
{
	return generationNumberFromMaleParent;
}

const int& Animal::getID_prt1() const
{
	return ID_prt1;
}

const int& Animal::getID_prt2() const
{
	return ID_prt2;
}

const double& Animal::getVoracityIni() const
{
	return voracity_ini;
}

const double& Animal::getSearchAreaIni() const
{
	return search_area_ini;
}

const double& Animal::getSpeedAreaIni() const
{
	return speed_ini;
}

const double& Animal::getTankIni() const
{
	return tank_ini;
}

ostream& operator<<(std::ostream& os, const Animal& animal)
{
	os << static_cast<const Edible&>(animal)
	<< animal.getSpecies()->getScientificName() << "\t"
	<< static_cast<unsigned int>(animal.getGender()) << "\t";

	for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
	{
		os << getPositionAxisValue(animal.getPosition(), axis) << "\t";
	}

	os << animal.getLifeStage() << "\t"
	<< animal.getInstar() << "\t"
	<< animal.getPhenoIni() << "\t"
	<< animal.getDateEgg() << "\t"
	<< animal.getAgeFirstRep() << "\t"
	<< animal.getRepCount() << "\t"
	<< animal.getFecundity() << "\t"
	<< animal.getDateOfDeath() << "\t"
	<< animal.getGenerationNumberFromFemaleParent() << "\t"
	<< animal.getGenerationNumberFromMaleParent() << "\t"
	<< animal.getID_prt1() << "\t"
	<< animal.getID_prt2() << "\t"
	<< animal.getTodayEncountersWithPredators() << "\t"
	<< animal.getEncounterEvents()->size() << "\t"
	<< animal.getDaysDigest() << "\t"
	<< animal.getVoracityIni() << "\t"
	<< animal.getSearchAreaIni() << "\t"
	<< animal.getSpeedAreaIni() << "\t"
	<< animal.getTankIni() << "\t"
	<< animal.getPhenoIni() << "\t"
	<< animal.getCurrentBodySize() << "\t"
	<< animal.calculateDryMass() << "\t";

	for (size_t i = 0; i < EnumClass<Trait::Type>::size(); ++i)
	{
		try
		{
			os << animal.getTrait((Trait::Type)i) << "\t";
		}
		catch(const LineInfoException& e)
		{
			os << -1 << "\t";
		}
	}

	return os;
}


const double Animal::getRemainingVoracity() const
{ 
	return max(getTrait(Trait::Type::voracity) - foodMass, 0.0); 
}


void Animal::printVoracities(int timeStep, ostream& voracitiesFile)
{
	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;
	double dryMassAfterAssim = calculateDryMass() + foodMass;
	double wetMassAfterAssim = dryMassAfterAssim * getSpecies()->getConversionToWetMass();
	double prop_time_mov;

	double lengthAtBirth = 0;
	double xmid = 0;						 

	if(getSpecies()->getGrowthCurve()->getType() == Curve::Type::Logistic){
		double lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::Type::growth));
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
	double totalMetabolicDryMassLossAfterAssim = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, prop_time_mov);
	if(totalMetabolicDryMassLossAfterAssim)										
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLossAfterAssim -= totalMetabolicDryMassLossAfterAssim * getSpecies()->getPercentageMetabolicDownregulation();
	}
	double dryMassAfterMetabLoss = (dryMassAfterAssim - totalMetabolicDryMassLossAfterAssim);

	double expectedLengthAtNextTimeStep = getValueGrowthCurve(getCurrentAge()+(round((1.0/(double)getTerrainCellInterface()->getMapInterface().getWorldInterface()->getTimeStepsPerDay()))), xmid);



	double expectedMassAtNextTimeStep;


    if(getInstar() == getSpecies()->getInstarFirstReproduction()){
    	expectedMassAtNextTimeStep = getSpecies()->getCoefficientForMassAforMature()*pow(expectedLengthAtNextTimeStep,getSpecies()->getScaleForMassBforMature());
    }else{
    	expectedMassAtNextTimeStep = getSpecies()->getCoefficientForMassA()*pow(expectedLengthAtNextTimeStep,getSpecies()->getScaleForMassB());
	}




	//Dinosaurs
	double diffFromExpectedMassAtNextTimeStep = expectedMassAtNextTimeStep - dryMassAfterMetabLoss;

  	double dinoLengthPredicted = 0;

	if(getInstar() > Instar(1)) {
		dinoLengthPredicted = getValueGrowthCurve(getCurrentAge(), xmid);
	}
	else {
		dinoLengthPredicted = getValueGrowthCurve(0, xmid);
	}

	double dinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(dinoLengthPredicted,getSpecies()->getScaleForMassBforMature());

    double minMassAtCurrentAge = dinoMassPredicted - dinoMassPredicted * getSpecies()->getMinPlasticityKVonBertalanffy();

    double nextDinoLengthPredicted = getValueGrowthCurve(getCurrentAge()+1, xmid);

	
	
	//double nextDinoMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());

	
	/*if(getId() == 2 && timeStep == 51){
    			cout << calculateDryMass() << endl;
    			exit(-1);
    		}*/

	double asymptoticSize;

	switch(getSpecies()->getGrowthCurve()->getType()) {
		case Curve::Type::VonBertalanffy: {
			asymptoticSize = static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->getAsymptoticSize();
			break;
		}
		case Curve::Type::Logistic: {
			asymptoticSize = static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize();
			break;
		}
		case Curve::Type::Logistic3P: {
			asymptoticSize = static_cast<const Logistic3PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
			break;
		}
		case Curve::Type::Logistic4P: {
			asymptoticSize = static_cast<const Logistic4PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
			break;
		}
		case Curve::Type::Gompertz: {
			asymptoticSize = static_cast<const GompertzCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
			break;
		}
		default: {
			asymptoticSize = 0.0;
			break;
		}
	}


	voracitiesFile << this->getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< lifeStage << "\t"
	<< getCurrentAge()-1 << "\t" //Dinosaurs - it has been updated in tuneTraits
	<< getInstar() << "\t"
	<< currentBodySize << "\t"
	<< getTrait(Trait::Type::energy_tank) << "\t"
	<< calculateDryMass() << "\t"
	<< nextDinoMassPredicted << "\t" //Dinosaures
	<< minMassAtCurrentAge << "\t"
	<< getTrait(Trait::Type::voracity) << "\t"
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
	<< getTrait(Trait::Type::voracity) / calculateDryMass() << "\t"
	<< magic_enum::enum_name(gender) << "\t"
	<< mated << "\t"
	<< eggDryMassForClutch << "\t"
	<< getTrait(Trait::Type::growth) << "\t"
	<< asymptoticSize << "\t"
	<< pseudoGrowth << "\t"
	<< asymptoticSize << "\t"
	<< getTrait(Trait::Type::factorEggMass) << "\t"
	<< getEggDryMassAtBirth() << "\t"
	<< getDateOfDeath() << "\t"
	<< ageOfFirstMaturation << "\t"
	<< rep_count << "\t";

	voracitiesFile << endl;
}

void Animal::printGenetics(ostream& geneticsFile)
{
	geneticsFile << this->getIdStr() << "\t"
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
	traitsFile << this->getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< generationNumberFromFemaleParent << "\t"
	<< generationNumberFromMaleParent << "\t"
	<< ID_prt1 << "\t"
	<< ID_prt2 << "\t";

	for (size_t i = 0; i < EnumClass<Trait::Type>::size(); ++i)
	{
		try
		{
			traitsFile << getTrait((Trait::Type)i) << "\t";
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
		getSpecies()->setMinObservedPseudoValue(variableTraits->at(i), min(getSpecies()->getMinObservedPseudoValue(variableTraits->at(i)), getTrait(variableTraits->at(i))));
		getSpecies()->setMaxObservedPseudoValue(variableTraits->at(i), max(getSpecies()->getMaxObservedPseudoValue(variableTraits->at(i)), getTrait(variableTraits->at(i))));
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
	double vonBertLinf = linearInterpolate(traits[Trait::Type::growth], getSpecies()->minRestrictedLimits[Trait::Type::growth], getSpecies()->maxRestrictedLimits[Trait::Type::growth], getSpecies()->vonBertLdistanceMin, getSpecies()->vonBertLdistanceMax);
	//vonBertLinf disminuye según tempSizeRuleConstant (usando Topt = 35)
	//finalDevTimeVector cambia usando dells (usamos la misma Topt = 35 otra vez)
	double pseudoVonBertMaxLength = vonBertLinf*(1-exp(-traits[Trait::Type::growth]*(finalDevTimeVector[CADAINSTAR]-getSpecies()->vonBertTime0)));
	//calculamos mean y sd para pseudoVonBertMaxLength

	double normalizedPseudoVonBertMaxLength = (pseudoVonBertMaxLength - pseudoVonBertMaxLengthMean) / pseudoVonBertMaxLengthSd;
	vonBertMaxLength<-normalizedPseudoGrowth*cholMat[1,2]+normalizedPseudoVonBertMaxLength*cholMat[2,2]

	pseudoGrowth = traits[Trait::Type::growth];
	getSpecies()->pseudoGrowthMean += pseudoGrowth;
}

void Animal::sumPseudoLengthSd()
{
	getSpecies()->pseudoGrowthSd += pow(pseudoGrowth - getSpecies()->pseudoGrowthMean, 2);
}
*/


void Animal::setNewLifeStage(const LifeStage newLifeStage)
{
	switch (newLifeStage)
	{
	case LifeStage::REPRODUCING:
		if(getSpecies()->occursHabitatShiftBeforeBreeding())
		{
			setInHabitatShiftBeforeBreeding(true);
		}
		else
		{
			setInBreedingZone(true);
		}
		break;
	default:
		break;
	}


	getMutableTerrainCellInterface()->eraseAnimal(this);

	lifeStage = newLifeStage;
	getMutableTerrainCellInterface()->insertAnimal(this);
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
		setTrait(variableTraits->at(i), MathFunctions::linearInterpolate(
				getTrait(variableTraits->at(i)), 
				getSpecies()->getMinObservedPseudoValue(variableTraits->at(i)), getSpecies()->getMaxObservedPseudoValue(variableTraits->at(i)),
				getSpecies()->getMinRestrictedRange(variableTraits->at(i)), getSpecies()->getMaxRestrictedRange(variableTraits->at(i))
			)
		);
		
		if(getTrait(variableTraits->at(i)) < getSpecies()->getMinRestrictedRange(variableTraits->at(i)) || getTrait(variableTraits->at(i)) > getSpecies()->getMaxRestrictedRange(variableTraits->at(i)))
		{
			//cout << "WARNING: individual " << id << " on species " << getSpecies()->getScientificName() << ": trait number " << i << ": " << traits[i] << " out of RESTRICTED RANGES: " << getSpecies()->minRestrictedRanges[i] << " - " << getSpecies()->maxRestrictedRanges[i] << endl << flush;
			isInsideRestrictedRanges = false;
		}
		if(getTrait(variableTraits->at(i)) < getSpecies()->getMinTraitLimit(variableTraits->at(i)) || getTrait(variableTraits->at(i)) > getSpecies()->getMaxTraitLimit(variableTraits->at(i)))
		{
			cout << "WARNING: individual " << getId() << " on species " << getSpecies()->getScientificName() << ": trait number " << EnumClass<Trait::Type>::to_string(variableTraits->at(i)) << ": " << getTrait(variableTraits->at(i)) << " out of LIMITS: " << getSpecies()->getMinTraitLimit(variableTraits->at(i)) << " - " << getSpecies()->getMaxTraitLimit(variableTraits->at(i)) << endl << flush;
			isViableAnimal = false;
		}
	}
	return make_pair(isInsideRestrictedRanges, isViableAnimal);
}


void Animal::calculateGrowthCurves(Temperature temperature, ostream& tuneTraitsFile, bool printGrowthData, double ageAtInitialization) //Dinosaurs
{
	
	double lengthAtBirth = 0;
	double xmid = 0;						 

	if(getSpecies()->getGrowthCurve()->getType() == Curve::Type::Logistic){
		double lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::Type::growth));
	}

	//vonBertK is now the actual growth trait

	//Calculating asymptoticSize and changing growth curves according to LinfKcorr
	//Cholesky has been called AFTER the creation of each species and the matrix has been saved as cholMat
	//The creation of the cholMat is performed inside the setLinfKcorr function

	//Using Change_K (and Dells) for tuning Growth according to temperature
	//double tmin = 270;
	double tmax = getSpecies()->getTempFromLab().getTemperatureKelvin();
	/*double newT = 273 + temperature;
	//Optimo para las curvas de crecimiento=35. Para Vor Speed Search será 25
	double Topt = 273 + getSpecies()->getTempOptGrowth();
	double activationEnergyGrowth = 0.65;//changed from 0.66 now following Dell, perhaps take it out as an entry parameter, but since is growth assume MTE

	double dellsNewT = johnsonLewis(newT, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth);
	double changeKmin = getSpecies()->getMinTraitRange(Trait::Type::growth)-0.000001;
	double changedKmin = linearInterpolate(dellsNewT, johnsonLewis(xmin, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth), johnsonLewis(xmax, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth), changeKmin, getSpecies()->getMinTraitRange(Trait::Type::growth));
	double changedKmax = linearInterpolate(dellsNewT, johnsonLewis(xmin, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth), johnsonLewis(xmax, Topt, getSpecies()->getEdGrowth(), activationEnergyGrowth), changeKmin, getSpecies()->getMaxTraitRange(Trait::Type::growth));
*/
	//This equals K<-interpol(Knorm, Kmin, Kmax)
	//currentPostTempGrowth = linearInterpolate(traits[Trait::Type::growth], getSpecies()->getMinTraitRange(Trait::Type::growth), getSpecies()->getMaxTraitRange(Trait::Type::growth), changedKmin, changedKmax);

	//Decrease in asymptoticSize according to TempSizeRuleConstant
	double thisAnimalTempSizeRuleConstant = getSpecies()->getTempSizeRuleConstant();
	double degreesDifference = fabs(temperature.getTemperatureCelsius() - getSpecies()->getTempFromLab().getTemperatureCelsius());
	
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
			case Curve::Type::VonBertalanffy: {
				asymptoticSize = static_cast<VonBertalanffyCurveParams* const>(growthCurveParams)->getAsymptoticSize();
				break;
			}
			case Curve::Type::Logistic: {
				asymptoticSize = static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize();
				break;
			}
			case Curve::Type::Logistic3P: {
				asymptoticSize = static_cast<const Logistic3PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
				break;
			}
			case Curve::Type::Logistic4P: {
				asymptoticSize = static_cast<const Logistic4PCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
				break;
			}
			case Curve::Type::Gompertz: {
				asymptoticSize = static_cast<const GompertzCurve* const>(getSpecies()->getGrowthCurve())->getAsymptoticSize();
				break;
			}
			default: {
				asymptoticSize = 0.0;
				break;
			}
		}


		tuneTraitsFile
		<< getTrait(Trait::Type::growth) << "\t"
		<< asymptoticSize << "\t"; //<< currentPostTempLinf << "\t";
	}

    //Here to calculate the new dev time we need to calculate the mass of the adult after TSR has been applied
    double ageLastInstar = getAgeLastInstar();

	

    double maxLfromSpeciesLastInstar = getValueGrowthCurve(ageLastInstar, xmid);

    double massLastInstarForDevT = getSpecies()->getCoefficientForMassAforMature()*pow(maxLfromSpeciesLastInstar,getSpecies()->getScaleForMassBforMature());

    double postTSRMassLastInstarForDevT = massLastInstarForDevT + thisAnimalTempSizeRuleConstant*degreesDifference*massLastInstarForDevT;

    //transform to g in wetMass to adjust to the equation by Gillooly et al. 2012
    double massLastInstarForDevTinG = getSpecies()->getConversionToWetMass() * postTSRMassLastInstarForDevT * 1000;

	double devTime = exp(-0.11 * (temperature.getTemperatureCelsius() / (1 + temperature.getTemperatureCelsius() / 273)) + getSpecies()->getDevInter()) * pow(massLastInstarForDevTinG, 0.25);

		//double finalDevTimeProportion = devTime / ageLastInstar;

	double finalDevTimeProportion = 1; //Dinosaurs

	//Fixing t0 in BV
	//Original length without
	//Jordi: Are the three sentences below necessary anymore?
//	double maxLfromSpeciesMatureAge = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(matureAge-getSpecies()->getVonBertTime0())));
//	double thisAnimalMaxLfromSpeciesMatureAge = maxLfromSpeciesMatureAge + thisAnimalTempSizeRuleConstant*degreesDifference*maxLfromSpeciesMatureAge;
//	double thisAnimalMatureAge = matureAge*finalDevTimeProportion;

	if(getEggDryMassAtBirth() == -1)//for the new borns
	{
		calculateEggDryMassAtBirth();

		setDryMass(getEggDryMassAtBirth());
	}


	//Now we calculate the vector of Ls using asymptoticSize and von Bertalanfy function on each age
	//And we use each L as the next_L in the growth equations to calculate the next_M


/* 	lengthsVector = new double[getSpecies()->getNumberOfInstars()]; //Dinosaurs take out of the heap... better memory?
	massesVector = new double[getSpecies()->getNumberOfInstars()];
 */
	for(unsigned int i = 0; i < finalDevTimeVector.size(); ++i)
	{
		Instar instar(i+1);
		finalDevTimeVector[i] = getSpecies()->getDevTime(instar);
		
		lengthsVector[i] = getValueGrowthCurve(finalDevTimeVector[i], xmid);


		if(Instar(getSpecies()->getNumberOfInstars()) == getSpecies()->getInstarFirstReproduction()){
			massesVector[i] = getSpecies()->getCoefficientForMassAforMature()*pow(lengthsVector[i],getSpecies()->getScaleForMassBforMature());
		}else{
			massesVector[i] = getSpecies()->getCoefficientForMassA()*pow(lengthsVector[i],getSpecies()->getScaleForMassB());
		}

		//TSR is applied directly here so the k value is effectively changed at the individual level
		massesVector[i] = massesVector[i] + massesVector[i]*thisAnimalTempSizeRuleConstant*degreesDifference;
	
		}

	for(unsigned int i = 0; i < finalDevTimeVector.size(); ++i)
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

void Animal::setInstar(const Instar& newInstar)
{
	getMutableTerrainCellInterface()->eraseAnimal(this);

	Edible::setInstar(newInstar);

	updateDepth();

	setInHabitatShift(getSpecies()->habitatShiftOccurs(getInstar()));

	getMutableTerrainCellInterface()->insertAnimal(this);
}

void Animal::moveOnNextInstar()
{
	Instar actualInstar = getInstar();
	actualInstar.moveOnNextInstar();
	setInstar(actualInstar);
}

void Animal::setInHabitatShift(const bool newInHabitatShiftValue)
{
	inHabitatShift = newInHabitatShiftValue;
}

void Animal::setInBreedingZone(const bool newInBreedingZoneValue)
{
	inBreedingZone = newInBreedingZoneValue;
}

void Animal::setInHabitatShiftBeforeBreeding(const bool newInHabitatShiftBeforeBreedingValue)
{
	inHabitatShiftBeforeBreeding = newInHabitatShiftBeforeBreedingValue;
}

void Animal::setInHabitatShiftAfterBreeding(const bool newInHabitatShiftAfterBreedingValue)
{
	inHabitatShiftAfterBreeding = newInHabitatShiftAfterBreedingValue;
}

void Animal::setAtDestination(const bool newAtDestinationValue)
{
	atDestination = newAtDestinationValue;
}

void Animal::setTargetNeighborToTravelTo(const std::pair<PointMap, PointContinuous> newTargetNeighborToTravelTo)
{
	targetNeighborToTravelTo = newTargetNeighborToTravelTo;
}

const bool Animal::isInHabitatShift() const
{
	return inHabitatShift;
}

const bool Animal::isInBreedingZone() const
{
	return inBreedingZone;
}

const bool Animal::isInHabitatShiftBeforeBreeding() const
{
	return inHabitatShiftBeforeBreeding;
}

const bool Animal::isInHabitatShiftAfterBreeding() const
{
	return inHabitatShiftAfterBreeding;
}

const bool Animal::isAtDestination() const
{
	return atDestination;
}

const std::pair<PointMap, PointContinuous>& Animal::getTargetNeighborToTravelTo() const
{
	return targetNeighborToTravelTo;
}

void Animal::updateDepth()
{
	TerrainCellInterface* terrainCellToMigrate = getMutableTerrainCellInterface();

	while(static_cast<const PointSpatialTree &>(terrainCellToMigrate->getPosition()).getDepth() != getCellDepthOnActualInstar())
	{
		TerrainCellInterface* newTerrainCellToMigrate = static_cast<SpatialTreeTerrainCellInterface *const>(terrainCellToMigrate)->getMutableParent();
	
		if(newTerrainCellToMigrate == nullptr && getCurrentLength() > newTerrainCellToMigrate->getSize())
		{
			throwLineInfoException("The animal's dimensions are greater than those of the map");
		}
		
		terrainCellToMigrate = newTerrainCellToMigrate;
	}

	setTerrainCellInterface(terrainCellToMigrate);
}

const unsigned int& Animal::getCellDepthOnActualInstar() const
{
	return getSpecies()->getCellDepthPerInstar()[getInstar()];
}

//The function below is to grow the animals that need to be at a certain instar at the beginning of the simulation

Temperature Animal::forceMolting()
{
	Temperature temperatureCycleTemperature = getTerrainCellInterface()->getMoistureInfo()->getRandomTemperature();
	ofstream noStream;
	calculateGrowthCurves(temperatureCycleTemperature, noStream, false, -1);//Dinosaurs

	double lengthAtBirth = 0;
	double xmid = 0;				
	double propAdultMass = 0;
	double currentEggs = 0;		 

	if(getSpecies()->getGrowthCurve()->getType() == Curve::Type::Logistic){
		double lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature()); 
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::Type::growth));
	}



	double ageAtInitialization = 0.0;
	if(getInstar() > Instar(1))
	{
		ageAtInitialization = getAgeInInstar(getInstar());
	}



	eggDryMassForClutch = 0.0;

	if(ageAtInitialization >= finalDevTimeVector[0])
	{
		for(unsigned int instarValue = getSpecies()->getNumberOfInstars(); instarValue > getInstar().getValue(); instarValue--)
		{
			Instar actualInstar(instarValue);

			actualMoltingTimeVector.push_back(getAgeInInstar(actualInstar));
			actualMoltingMassVector.push_back(getMassesInInstar(actualInstar));
		}
	}

	//int maturationInstar = getSpecies()->getInstarFirstReproduction();
	//ageOfMaturation = finalDevTimeVector[maturationInstar];
	//cout << maturationInstar << endl;

	//La edad del animal en negativo para que los calculos de tuneTraits sean correctos
	date_egg = -ageAtInitialization - getTrait(Trait::Type::pheno);
	setTrait(Trait::Type::pheno, -ageAtInitialization);

	ageOfLastMoultOrReproduction = 0;
	if(getInstar() > Instar(1))
	{
		ageOfLastMoultOrReproduction = getAgeInInstar(getInstar());
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
		if((getInstar() >= getSpecies()->getInstarFirstReproduction()) || (getInstar() == Instar(getSpecies()->getNumberOfInstars()))){
			mature = true;
		}

	//}

	if(isMature()){
	
	
	  //if(getSpecies()->getGrowthType() == 0){
		
		if(getInstar() == getSpecies()->getInstarFirstReproduction()){
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
	double degreesDifference = fabs(temperatureCycleTemperature.getTemperatureCelsius() - getSpecies()->getTempFromLab().getTemperatureCelsius());
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
		daysForPseudoTargetReproduction = 1.1*getCurrentAge(); //1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = getCurrentAge() + daysForPseudoTargetReproduction;
		
		double currentLength = getValueGrowthCurve(getCurrentAge(), xmid);
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
		//Dinosaurs double currentAge = -traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;
		massOfMaturationOrLastReproduction = getMassesInInstar(getInstar());
		ageOfMaturation = getAgeInInstar(getInstar());
		lastMassBeforeMaturationOrOviposition = getMassesInInstar(getInstar());
		double daysForTargetReproduction;
		if(gender == AnimalSpecies::Gender::FEMALE)
		{
			clutchDryMass=0;//Dinosaurs
			//double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(0-thisAnimalVonBertTime0)));
			eggDryMassForClutch = getSpecies()->getEggDryMass() + getSpecies()->getEggDryMass()* getTrait(Trait::Type::factorEggMass);

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
							
							double lengthAtThisAge = getValueGrowthCurve(getCurrentAge(), xmid);

							
							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
						
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = MathFunctions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
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
			double currentLength = getValueGrowthCurve(getCurrentAge(), xmid);
			double nextReproLength = getValueGrowthCurve(ageForNextReproduction, xmid);
    		
			double nextReproMass = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());	
			double currentMass = getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());

			if(getSpecies()->hasIndeterminateGrowth()){
				if(getInstar() < Instar(getSpecies()->getNumberOfInstars())){
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

		
		ageOfLastMoultOrReproduction = getAgeInInstar(getInstar());
		//arthro
		ageForNextReproduction = ageAtInitialization + daysForTargetReproduction;
		//end arthro
		double daysActiveSinceMaturation = round(Random::randomUniform()*daysForTargetReproduction);
		//expectedMassAtCurrentAge = massOfMaturationOrLastReproduction; // + daysActiveSinceMaturation * ((massForNextReproduction - massOfMaturationOrLastReproduction) / (ageForNextReproduction - ageOfLastMoultOrReproduction));
		//traits[Trait::Type::energy_tank] = expectedMassAtCurrentAge - currentBodySize;

/*		if(getId() == 23){
			cout << "ageOfLastMoultOrReproduction: " << ageOfLastMoultOrReproduction << endl;
			cout << "ageAtInitialization: " << ageAtInitialization << endl;
			cout << "daysForTargetReproduction: " << daysForTargetReproduction << endl;
	    	cout << "hello: " << traits[Trait::Type::energy_tank] << endl;
	    }*/


		ofstream noStream2;
		tuneTraits(-1, 1, temperatureCycleTemperature, 100, noStream2, false, true);
	}


	return temperatureCycleTemperature;
}


void Animal::setDryMass(const double &newDryMass)
{
	setEnergyTank(tank_ini * pow(newDryMass, getSpecies()->getBetaScaleTank()));
	currentBodySize = newDryMass - getTrait(Trait::Type::energy_tank);

	getSpecies()->updateMaximumDryMassObserved(newDryMass);
}


void Animal::setDryMass(const double &newEnergyTank, const double &newCurrentBodySize)
{
	setEnergyTank(newEnergyTank);
	currentBodySize = newCurrentBodySize;

	getSpecies()->updateMaximumDryMassObserved(calculateDryMass());
}

//The function below is to grow the animals that need to be at a certain instar at the beginning of the simulation

Temperature Animal::forceMolting2()
{
	Temperature temperatureCycleTemperature = getTerrainCellInterface()->getMoistureInfo()->getRandomTemperature();
	ofstream noStream;

	double lengthAtBirth = 0;
	double xmid = 0;					
	double propAdultMass = 0;
	double currentEggs = 0;		 	 

	if(getSpecies()->getGrowthCurve()->getType() == Curve::Type::Logistic){
		double lengthAtBirth = pow((getMassAtBirth()/getSpecies()->getCoefficientForMassAforMature()),1/getSpecies()->getScaleForMassBforMature());  
		xmid = log((static_cast<LogisticCurveParams* const>(growthCurveParams)->getAsymptoticSize()-lengthAtBirth)/lengthAtBirth)*(1/getTrait(Trait::Type::growth));
	}

	for(unsigned int i = 0; i < getSpecies()->getDevTimeVector().size(); ++i)  //Dinosaurs for calculteGrowthCurves with ageAtInitialization != 0
	{
		Instar instar(i+1);
		finalDevTimeVector[i] = getSpecies()->getDevTime(instar);
	}

	

	double ageAtInitialization = 0.0;
	if(getInstar() > Instar(1))
	{
		ageAtInitialization = getAgeInInstar(getInstar());
	}


	eggDryMassForClutch = 0.0;

	

	if(ageAtInitialization >= finalDevTimeVector[0])
	{
		for(unsigned int instarValue = getSpecies()->getNumberOfInstars(); instarValue > getInstar().getValue(); instarValue--)
		{
			Instar actualInstar(instarValue);

			actualMoltingTimeVector.push_back(getAgeInInstar(actualInstar));
			actualMoltingMassVector.push_back(getMassesInInstar(actualInstar));
		}
	}


    double reproTime;
	double daysForTargetReproduction;
			
	//double addedAge = Random::randomIndex(daysForTargetReproduction);
	
    if(getInstar() == Instar(1)){ //Dinosaurs and arthro - fixed for short-lifecycle mites - initialization is now done at random ages within the instar to generate reproduction variability
		
		ageAtInitialization = Random::randomIndex(finalDevTimeVector[0]);
	
	}else if(getInstar() < Instar(getSpecies()->getNumberOfInstars())){
		ageAtInitialization = ageAtInitialization + Random::randomIndex(finalDevTimeVector[getInstar().getValue()]-getAgeInInstar(getInstar()));	
	    
	} else { //is the last instar
		//age at initialization of animals in the last instar depends on longevity
		//cout << "before" << endl;
		//cout << ageAtInitialization << endl;
		
		//cout << ageAtInitialization << endl;
		
		// ageAtInitialization = ageAtInitialization + addedAge;
		
		ageAtInitialization = ageAtInitialization+Random::randomIndex((getAgeFirstReproduction()*getSpecies()->getLongevitySinceMaturation())-ageAtInitialization);
	} 
	
	calculateGrowthCurves(temperatureCycleTemperature, noStream, false, ageAtInitialization); //Dinosaurs
	
	setCurrentAge(ageAtInitialization); //Dinosaurs
	//int maturationInstar = getSpecies()->getInstarFirstReproduction();
	//ageOfMaturation = finalDevTimeVector[maturationInstar];
	//cout << maturationInstar << endl;

	//La edad del animal en negativo para que los calculos de tuneTraits sean correctos
	date_egg = -ageAtInitialization - getTrait(Trait::Type::pheno);
	setTrait(Trait::Type::pheno, -ageAtInitialization);

	ageOfLastMoultOrReproduction = 0;
	if(getInstar() > Instar(1))
	{
		ageOfLastMoultOrReproduction = getAgeInInstar(getInstar());
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
		if((getInstar() >= getSpecies()->getInstarFirstReproduction()) || (getInstar() == Instar(getSpecies()->getNumberOfInstars()))){
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
	double degreesDifference = fabs(temperatureCycleTemperature.getTemperatureCelsius() - getSpecies()->getTempFromLab().getTemperatureCelsius());
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
		daysForPseudoTargetReproduction = 1.1*getCurrentAge(); //1.1 because reproTimeFactor is no longer in use
		ageForNextReproduction = getCurrentAge() + daysForPseudoTargetReproduction;
		double currentLength = getValueGrowthCurve(getCurrentAge(), xmid);
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
		//Dinosaurs double currentAge = -traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;
		
		
		double expectedLengthAtFormerAge = 0;

		if(ageAtInitialization > 0) {
			expectedLengthAtFormerAge = getValueGrowthCurve(ageAtInitialization-1, xmid);
		}
		else {
			expectedLengthAtFormerAge = getValueGrowthCurve(0, xmid);
		}



		double expectedMassAtFomerAge = getSpecies()->getCoefficientForMassAforMature()*pow(expectedLengthAtFormerAge,getSpecies()->getScaleForMassBforMature());
		
		massOfMaturationOrLastReproduction = getMassesInInstar(getInstar());
		//ageOfMaturation = getAgeInInstar(instar);
		lastMassBeforeMaturationOrOviposition = getMassesInInstar(getInstar());
		//double daysForTargetReproduction;

		if(gender == AnimalSpecies::Gender::FEMALE)
		{
			clutchDryMass=0;//Dinosaurs
			//double lengthAtBirth = static_cast<const LogisticCurveParams *>(growthCurveParams)->getAsymptoticSize()*(1-exp(-traits[Trait::Type::growth]*(0-thisAnimalVonBertTime0)));
			eggDryMassForClutch = getSpecies()->getEggDryMass() + getSpecies()->getEggDryMass()* getTrait(Trait::Type::factorEggMass);

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

							double lengthAtThisAge = getValueGrowthCurve(getCurrentAge(), xmid);

							double massAtThisAge = getSpecies()->getCoefficientForMassAforMature()*pow(lengthAtThisAge,getSpecies()->getScaleForMassBforMature());
							
							propAdultMass = getMassesFirstReproduction()/getMassesLastInstar();
							
							currentEggs = MathFunctions::linearInterpolate(massAtThisAge,getMassesFirstReproduction(),getMassesLastInstar(),
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
				double currentLength = getValueGrowthCurve(getCurrentAge(), xmid);
				double nextReproLength = getValueGrowthCurve(ageForNextReproduction, xmid);
				
    			double nextReproMass = getSpecies()->getCoefficientForMassAforMature()*pow(nextReproLength,getSpecies()->getScaleForMassBforMature());	
				double currentMass =getSpecies()->getCoefficientForMassAforMature()*pow(currentLength,getSpecies()->getScaleForMassBforMature());

				if(getSpecies()->hasIndeterminateGrowth()){
					if(getInstar() < Instar(getSpecies()->getNumberOfInstars())){
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
		//traits[Trait::Type::energy_tank] = expectedMassAtCurrentAge - currentBodySize;

/*		if(getId() == 23){
			cout << "ageOfLastMoultOrReproduction: " << ageOfLastMoultOrReproduction << endl;
			cout << "ageAtInitialization: " << ageAtInitialization << endl;
			cout << "daysForTargetReproduction: " << daysForTargetReproduction << endl;
	    	cout << "hello: " << traits[Trait::Type::energy_tank] << endl;
	    }*/


		ofstream noStream2;
		//tuneTraits(-1, 1, temperatureCycleTemperature, 100, noStream2, false);
	
	}

	return temperatureCycleTemperature;
}


void Animal::moveAnimal(int timeStep, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{


	#ifdef DEBUG
	//Output::coutFlush("moving animal... \n");
	#endif

	// This condition checks if the animal has already been moved in the current timeStep
	if (lastDayMoved < timeStep)
	{
		updateBiomassExperiencedPerSpecies();
		handlingTimer = 0.0;
		
		sated = false; //Dinosaurs and arthropods after Dinos - do not know why¿? 

		// Assimilate leftover food
		assimilateLastHuntedAnimalAndComputeHandlingTime();

		// Movement occurs when there is no food mass left
		if(!isThereLeftoverFood()) 
		{
			oldSearchArea = getTrait(Trait::Type::search_area);

			double biomassTrackedToday = 0.0;
			list<const EdibleInterface*> ediblesHasTriedToPredate;

			bool animalIsSenesced = searchAnimalToBreed(timeStep);

			if(!animalIsSenesced) {
				searchAnimalsAndResourceToEat(timeStep, biomassTrackedToday, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
			}

			//Mature MALES will move now even when they are sated

			if(isInHabitatShift() && (isInHabitatShiftBeforeBreeding() || isInHabitatShiftAfterBreeding()))
			{
				setInHabitatShift(false);
			}

			while (handlingTimer < timeStepMaximumHandlingTimer && // No me queda claro para que sirve
					steps < getTrait(Trait::Type::search_area) && // If it hasn't moved beyond its search area
					lifeStage == LifeStage::ACTIVE && // If it is active
					days_digest == 0 && // Esta variable no se está utilizando
					(
						!sated || // If it is not satisfied, it will have to look for food
						(sated && mature && gender == AnimalSpecies::Gender::MALE) // If it is sated, mature and a male, so it will look for a mate to breed with
					)
			)
			{
				double currentPerceptionArea = getTrait(Trait::Type::perception_area);

				if(isInHabitatShift() || isInHabitatShiftBeforeBreeding() || isInHabitatShiftAfterBreeding())
				{
					if(isInHabitatShift())
					{
						currentPerceptionArea *= getSpecies()->getHabitatShiftFactor();
						setInHabitatShift(false);
					}
					else if(isInHabitatShiftBeforeBreeding())
					{
						currentPerceptionArea *= getSpecies()->getHabitatShiftBeforeBreedingFactor();
						setInHabitatShiftBeforeBreeding(false);
					}
					else if(isInHabitatShiftAfterBreeding())
					{
						currentPerceptionArea *= getSpecies()->getHabitatShiftAfterBreedingFactor();
						setInHabitatShiftAfterBreeding(false);
					}

					setAtDestination(true);
				}


				if(isAtDestination())
				{
					searchTargetToTravelTo(currentPerceptionArea, ediblesHasTriedToPredate, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
					setAtDestination(false);
				}

				moveOneStep();

				if(isAtDestination() && getLifeStage() == LifeStage::REPRODUCING)
				{
					setInBreedingZone(true);
				}
				
				animalIsSenesced = searchAnimalToBreed(timeStep);

				if(!animalIsSenesced) {
					searchAnimalsAndResourceToEat(timeStep, biomassTrackedToday, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
				}
			}

			// IMPORTANT! steps might be higher than search_area just when the animal travels the LAST STEP in a diagonal direction.
			// Set the final number of steps performed in the movement
			if(steps > getTrait(Trait::Type::search_area))
			{
				steps = getTrait(Trait::Type::search_area);
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
						daysWithoutFood += round(((double)timeStep/getTerrainCellInterface()->getMapInterface().getWorldInterface()->getTimeStepsPerDay()));
					}
					break;
				}
				default: {
					throwLineInfoException("Default case");
					break;
				}
			}
		}

		// It indicates that the animal has already moved in this timestep.
		lastDayMoved = timeStep;
	}
}

bool edibleCompare(const tuple<double, pair<AnimalInterface*, EdibleInterface*>, double>& firstElem, const tuple<double, pair<AnimalInterface*, EdibleInterface*>, double>& secondElem) {
  return get<0>(firstElem) > get<0>(secondElem);
}

const double& Animal::getAssim() const
{ 
	return getTrait(Trait::Type::assim); 
}

bool Animal::checkBreed(int timeStep, AnimalInterface &otherAnimal)
{
	// Variables to identify the male and female at mating
	AnimalInterface *matedMale, *matedFemale;

	// Choose the role of each animal according to the sex of the animal doing the searching
	switch(getGender())
	{
		case AnimalSpecies::Gender::MALE:
			matedMale = this;
			matedFemale = &otherAnimal;
			break;
		case AnimalSpecies::Gender::FEMALE:
			matedMale = &otherAnimal;
			matedFemale = this;
			break;
		case AnimalSpecies::Gender::HERMAPHRODITE:
			matedMale = &otherAnimal;
			matedFemale = this;
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}

	// The female retains the male genome
	matedFemale->setGenomeFromMatedMale(matedMale);

	// if the male has exceeded the maximum reproductive limit
	if(matedMale->postBreeding(timeStep))
	{
		// Change the status of the male to senesced
		matedMale->getMutableTerrainCellInterface()->changeAnimalToSenesced(matedMale, timeStep);

		// If the male is the animal conducting the search, death of the animal is indicated
		if(matedMale == this)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Animal::searchAnimalToBreed(int timeStep)
{
	// If the animal is mature and has not mated, find another animal to mate with
	if(isMature() && !isMated())
	{
		unique_ptr<PartialCoverageAnimals> partialCoverageAnimals;
		unique_ptr<FullCoverageAnimals> fullCoverageAnimals;

		tie(partialCoverageAnimals, fullCoverageAnimals) = getMutableTerrainCellInterface()->getAnimalsOnRadius(
			getPosition(), getTrait(Trait::Type::interaction_area), getSpecies()->getBreedSearchParams(getGender())
		);

		// Try to mate the animal with one of the possible animals
		for(auto &terrainCellAnimals : *fullCoverageAnimals)
		{
			for(auto &animalsVector : *terrainCellAnimals)
			{
				for(auto &animal : *animalsVector)
				{
					// If the mating conditions are met
					if(!animal->isMated())
					{
						return checkBreed(timeStep, *animal);
					}
				}
			}
		}

		for(auto &terrainCellAnimals : *partialCoverageAnimals)
		{
			for(auto &animal : *terrainCellAnimals)
			{
				// If the mating conditions are met
				if(!animal->isMated())
				{
					return checkBreed(timeStep, *animal);
				}
			}
		}
	}

	return false;
}

bool Animal::isSated() const
{ 
	return sated; 
}

bool Animal::isExhausted() const 
{ 
	return steps >= getTrait(Trait::Type::search_area); 
}

pair<bool, bool> Animal::checkAnimalInteraction(const AnimalInterface &otherAnimal, list<const EdibleInterface*> &ediblesHasTriedToPredate, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	// Variables indicating the type of interaction between the two animals
	bool setIsPredated = false, setPreyingOn = false;
	bool isPredated, preyingOn;

	// Check if the animal qualifies as a predator
	if(isSated() || isExhausted())
	{
		setPreyingOn = true;
		preyingOn = false;
	}
	else
	{
		//Dinosaurs but elsewhere - to prevent unreasonable encounters and ingestion of very large prey
		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/otherAnimal.calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	
		if(probabilityDensityFunction < pdfThreshold) //Dinosaurs but elsewhere
		{
			setPreyingOn = true;
			preyingOn = false;
		}
	}

	// Check if the selected animal qualifies as a predator
	if(otherAnimal.isSated() || otherAnimal.isExhausted())
	{
		setIsPredated = true;
		isPredated = false;
	}
	else
	{
		//Dinosaurs but elsewhere - to prevent unreasonable encounters and ingestion of very large prey
		double probabilityDensityFunction = exp(-0.5 * pow((log(otherAnimal.calculateWetMass()/calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	
		if(probabilityDensityFunction < pdfThreshold) //Dinosaurs but elsewhere
		{
			setIsPredated = true;
			isPredated = false;
		}
	}


	// Complete the interaction information if it is incomplete
	if(!setIsPredated || !setPreyingOn)
	{
		// Check if the animal is prey for the selected animal
		bool isPrey = otherAnimal.canEatEdible(this, ediblesHasTriedToPredate, calculateDryMass());
		// Check if the selected animal is prey for the animal
		bool isPredator = canEatEdible(&otherAnimal, ediblesHasTriedToPredate, otherAnimal.calculateDryMass());

		// If both without predators of the other
		if (isPrey && isPredator)
		{
			// Based on a probability, it is decided who will hunt whom
			if(otherAnimal.calculatePredationProbability(*this, calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW) > calculatePredationProbability(otherAnimal, otherAnimal.calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW))
			{
				isPredator = false;
			}
			else
			{
				isPrey = false;
			}
		}

		if(!setIsPredated)
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

		if(!setPreyingOn)
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

	return make_pair(isPredated, preyingOn);
}

bool Animal::searchAnimalsAndResourceToEat(int timeStep, double &biomassTrackedToday, list<const EdibleInterface*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	vector<tuple<double, pair<AnimalInterface*, EdibleInterface*>, double>> ediblesByEdibility;

	// Get all the animals that can interact within the perception area.
	auto searchableEdibles = getTerrainCellInterface()->getMutableMapInterface().getEdiblesOnRadius(
		getMutableTerrainCellInterface(), getPosition(), getTrait(Trait::Type::interaction_area), 
		getSpecies()->getEdibleSearchParams(getInstar())
	);

	// Identify all possible interactions between the animal and the other edibles in the perception radius
	auto searchableResources = move(searchableEdibles.first);
	auto searchableAnimals = move(searchableEdibles.second);

	auto partialCoverageAnimals = move(searchableAnimals.first);
	auto fullCoverageAnimals = move(searchableAnimals.second);

	for(auto &terrainCellAnimals : *fullCoverageAnimals)
	{
		for(auto &animalsVector : *terrainCellAnimals)
		{
			for(auto &animal : *animalsVector)
			{
				if(animal != this)
				{
					bool isPredated, preyingOn;
					tie(isPredated, preyingOn) = checkAnimalInteraction(*animal, ediblesHasTriedToPredate, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

					// Add interaction if possible
					if(isPredated)
					{
						ediblesByEdibility.push_back(tuple<double, pair<AnimalInterface*, EdibleInterface*>, double>(animal->calculateEdibilityValue(*this, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(animal, this), calculateDryMass()));
					}
					else if(preyingOn)
					{
						ediblesByEdibility.push_back(tuple<double, pair<AnimalInterface*, EdibleInterface*>, double>(calculateEdibilityValue(*animal, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(this, animal), animal->calculateDryMass()));
						biomassTrackedToday += animal->calculateDryMass();
					}
				}
			}
		}
	}

	for(auto &terrainCellAnimals : *partialCoverageAnimals)
	{
		for(auto &animal : *terrainCellAnimals)
		{
			if(animal != this)
			{
				bool isPredated, preyingOn;
				tie(isPredated, preyingOn) = checkAnimalInteraction(*animal, ediblesHasTriedToPredate, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

				// Add interaction if possible
				if(isPredated)
				{
					ediblesByEdibility.push_back(tuple<double, pair<AnimalInterface*, EdibleInterface*>, double>(animal->calculateEdibilityValue(*this, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(animal, this), calculateDryMass()));
				}
				else if(preyingOn)
				{
					ediblesByEdibility.push_back(tuple<double, pair<AnimalInterface*, EdibleInterface*>, double>(calculateEdibilityValue(*animal, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(this, animal), animal->calculateDryMass()));
					biomassTrackedToday += animal->calculateDryMass();
				}
			}
		}
	}

	if(!isSated() && !isExhausted())
	{
		for(auto& [resource, dryMass] : *searchableResources)
		{
			// If the selected resource is prey for the animal
			if(canEatEdible(resource, ediblesHasTriedToPredate, dryMass))
			{
				ediblesByEdibility.push_back(tuple<double, pair<AnimalInterface*, EdibleInterface*>, double>(calculateEdibilityValue(*resource, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(this, resource), dryMass));
				biomassTrackedToday += dryMass;
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
		<< get<1>(*ediblesIt).first->getId() << "\t"
		<< get<1>(*ediblesIt).first->getSpecies()->getScientificName() << "\t"
		<< get<1>(*ediblesIt).first->calculateDryMass() << "\t"
		<< get<1>(*ediblesIt).second->getId() << "\t"
		<< get<1>(*ediblesIt).second->getSpecies()->getScientificName() << "\t"
		<< get<2>(*ediblesIt) << "\t"
		<< get<1>(*ediblesIt).first->calculateEncounterProbability(*get<1>(*ediblesIt).second, get<1>(*ediblesIt).second->calculateDryMass(), muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) << "\t"
		<< get<1>(*ediblesIt).first->calculatePredationProbability(*get<1>(*ediblesIt).second, get<1>(*ediblesIt).second->calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW) << "\t"
		<< get<1>(*ediblesIt).first->getSpecies()->getEdiblePreference(get<1>(*ediblesIt).second->getSpecies()->getId(), get<1>(*ediblesIt).first->getInstar(), get<1>(*ediblesIt).second->getInstar()) << "\t"
		<< get<1>(*ediblesIt).first->getMeanExperience(get<1>(*ediblesIt).second->getSpecies()->getId()) << "\t"
		<< get<0>(*ediblesIt) << endl;
	}
	

	// Variable indicating foraging success or failure
	bool hasEatenOnce = false;

	unique_ptr<Ring> perceptionArea;

	if(!searchableResources->empty())
	{
		perceptionArea = Geometry::makeSphere(getPosition(), getTrait(Trait::Type::interaction_area));
	}
	else
	{
		perceptionArea = nullptr;
	}

	// Search for the edible to eat
	for(auto ediblesIt = ediblesByEdibility.begin(); !hasEatenOnce && ediblesIt != ediblesByEdibility.end(); ediblesIt++)
	{
		auto huntingAnimal = get<1>(*ediblesIt).first;
		auto targetAnimal = get<1>(*ediblesIt).second;
		auto targetDryMass = get<2>(*ediblesIt);


		// Calculate the probability of interaction of the animal with the edible it interacts with.
		float attackOrExposedAttackProbability = (huntingAnimal == this) ? huntingAnimal->getSpecies()->getAttackProbability() : targetAnimal->getSpecies()->getExposedAttackProbability();
		
		// Check if the predator can find the prey
		bool searchWasSuccessful = huntingAnimal->encounterEdible(*targetAnimal, targetDryMass, attackOrExposedAttackProbability, timeStep, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
		
		if(searchWasSuccessful)
		{
			//Steps only goes up when at least the animal HAS TRIED TO HUNT the prey. It goes up regardless of the success
			//The steps penalization is assumed depending on the search area and the longest diagonal
			
			// Check if the predator can hunt the prey
			bool huntWasSuccessful = huntingAnimal->predateEdible(*targetAnimal, targetDryMass, perceptionArea.get(), timeStep, false, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea);
			
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
				if(targetAnimal->canEatEdible(huntingAnimal, ediblesHasTriedToPredate, huntingAnimal->calculateDryMass()) && targetAnimal != this)
				{
					// Check if the new predator can hunt the old predator
					bool counterHuntWasSuccessful = targetAnimal->predateEdible(*huntingAnimal, huntingAnimal->calculateDryMass(), perceptionArea.get(), timeStep, true, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea);
					
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

	#ifdef DEBUG
		if(foodMass > traits[Trait::Type::voracity])
		{
			cout << "The food mass eaten was higher than the voracity value:" << endl;
			cout << " - Animal: " << getId() << "(" << getSpecies()->getScientificName() << ")" << endl;
			cout << " - Food mass eaten: " << foodMass << endl;
			cout << " - Voracity value: " << traits[Trait::Type::voracity] << endl;
		}
	#endif

	// If the food found covers the animal's voracity
	if(round(foodMass*pow(10,7)) >= round(getTrait(Trait::Type::voracity)*pow(10,7)))
	{
		// The amount of food is truncated to the animal's current voracity.
		foodMass = getTrait(Trait::Type::voracity);

		// It indicates that the animal is sated
		sated = true;
	}

	// Searcher did not die, so return false
	return false;
}


void Animal::setGenomeFromMatedMale(const AnimalInterface* matedMale)
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


bool Animal::postBreeding(int timeStep)
{
	rep_count++;

	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::Type::pheno] + 1.0/timeStepsPerDay;



	ageOfLastMoultOrReproduction = getCurrentAge();

	if (rep_count == 1)
	{
		age_first_rep = getCurrentAge();
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
	 	if(getInstar() < Instar(getSpecies()->getNumberOfInstars()))
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
	// (getSpecies()->getEggDryMass() + getSpecies()->getEffDryMass() * traits[Trait::Type::factorEggMass]));

	return floor(clutchDryMass / (getSpecies()->getEggDryMass() * (1 + getTrait(Trait::Type::factorEggMass))));
}


unique_ptr<list<AnimalInterface*>> Animal::breedAsexually( int objectiveEggsNumber,int timeStep, Temperature temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	double totalOffspringDryMass = 0.0;

	auto offspring = make_unique<list<AnimalInterface*>>();

	//Asexual animals are always females (or treated as females) and they DO NOT perform meiosis.
	AnimalSpecies::Gender offSpringGender = AnimalSpecies::Gender::FEMALE;
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
		createdEggsNumber ++;
		generationDam = generationNumberFromFemaleParent + 1;
		generationSire = generationNumberFromFemaleParent + 1;

		Gamete* femaleGameteSelected = genome.cloneFirstGameteFromHaploid();
		Gamete* maleGameteSelected = genome.cloneFirstGameteFromHaploid();

		AnimalInterface* newOffspring = createOffspring(femaleGameteSelected, maleGameteSelected, getMutableTerrainCellInterface(), getTrait(Trait::Type::factorEggMass), timeStep, generationNumberFromFemaleParent + 1, generationNumberFromMatedMale + 1, getId(), idFromMatedMale, getSpecies(), offSpringGender, true);

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
			cout << "Deleting non-viable Animal with ID: " << newOffspring->getId() << endl;
			delete newOffspring;
		}
	}

	
		
	fecundity += offspring->size();

	setEnergyTank(getTrait(Trait::Type::energy_tank) - totalOffspringDryMass);

	//lastMassBeforeMaturationOrOviposition = calculateDryMass() + traits[Trait::Type::energy_tank];
	//We recalculate the massForNextReproduction here? Not now tho, check this...
	//massForNextReproduction = massOfMaturationOrLastReproduction + clutchDryMass;
	//massOfMaturationOrLastReproduction = getDryMass();

	if(postBreeding(timeStep))
	{
		getMutableTerrainCellInterface()->changeAnimalToSenesced(this, timeStep);
	}
	else
	{
		setNewLifeStage(LifeStage::ACTIVE);
		//clearGenomeFromMatedMale();
		//CARE delete only when in TerrainCell
		//position->insertAnimal(this);
		//position->eraseAnimal(this, REPRODUCING);
	}

	return offspring;
}


unique_ptr<list<AnimalInterface*>> Animal::breedSexually( int objectiveEggsNumber,int timeStep, Temperature temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	double totalOffspringDryMass = 0.0;

	auto offspring = make_unique<list<AnimalInterface*>>();

	AnimalSpecies::Gender offSpringGender = AnimalSpecies::Gender::FEMALE;
	int createdEggsNumber = 0;
	int generationDam;
	int generationSire;

	while( createdEggsNumber < objectiveEggsNumber )
	{
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
				if(offSpringGender == AnimalSpecies::Gender::MALE)
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
				offSpringGender = AnimalSpecies::Gender::MALE;
			}
		}

		generationDam = generationNumberFromFemaleParent + 1; 
		generationSire = generationNumberFromMatedMale + 1;

		AnimalInterface* newOffspring = createOffspring(femaleGameteSelected, maleGameteSelected, getMutableTerrainCellInterface(), getTrait(Trait::Type::factorEggMass), timeStep, generationNumberFromFemaleParent + 1, generationNumberFromMatedMale + 1, getId(), idFromMatedMale, getSpecies(), offSpringGender, true);

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
			cout << "Deleting non-viable Animal with ID: " << newOffspring->getId() << endl;
			delete newOffspring;
		}
	}

	fecundity += offspring->size();

	setEnergyTank(getTrait(Trait::Type::energy_tank) - totalOffspringDryMass);

	//lastMassBeforeMaturationOrOviposition = calculateDryMass() + traits[Trait::Type::energy_tank];
	//We recalculate the massForNextReproduction here? Not now tho, check this...
	//massForNextReproduction = massOfMaturationOrLastReproduction + clutchDryMass;
	//massOfMaturationOrLastReproduction = getDryMass();

	if(postBreeding(timeStep))
	{
		getMutableTerrainCellInterface()->changeAnimalToSenesced(this, timeStep);
	}
	else
	{
		setNewLifeStage(LifeStage::ACTIVE);
		//clearGenomeFromMatedMale();
		//CARE delete only when in TerrainCell
		//position->insertAnimal(this);
		//position->eraseAnimal(this, REPRODUCING);
	}

	return offspring;
}


unique_ptr<list<AnimalInterface*>> Animal::breed(int timeStep, Temperature temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	int currentEggBatchNumber = computeEggBatchNumber();

	switch (getSpecies()->getSexualType())
	{
		case SexualType::asexual:
			return breedAsexually(currentEggBatchNumber, timeStep, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		case SexualType::diploid:
			return breedSexually(currentEggBatchNumber, timeStep, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		case SexualType::haplodiploid:
			return breedSexually(currentEggBatchNumber, timeStep, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}

	if(getSpecies()->occursHabitatShiftAfterBreeding())
	{
		setInHabitatShiftAfterBreeding(true);
	}
}


void Animal::dieFromBackground(int timeStep)
{
	if(!getTerrainCellInterface()->getMapInterface().getWorldInterface()->isGrowthAndReproTest()) {
		if(getSpecies()->getProbabilityDeathFromBackground() > Random::randomUniform())
		{
			setNewLifeStage(LifeStage::BACKGROUND, timeStep);
		}
	}
}


void Animal::assimilateFoodMass(int timeStep)
{
	if(foodMass > 0)
	{
		#ifdef DEBUG
		if(foodMass < traits[Trait::Type::voracity])
		{
			//cout << "The animal has only eaten " << foodMass << "(" << eatenToday << " preys)" << " out of " << traits[Trait::Type::voracity] << "(" << steps << " steps)" << endl;
		}
		#endif

		/*
		if(food_mass / traits[Trait::Type::energy_tank] > 4) {
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
			cout << traits[Trait::Type::energy_tank] << endl;
		}	 */
		
		//double demand = traits[Trait::Type::voracity] + traits[Trait::Type::energy_tank] 
		
		setEnergyTank(getTrait(Trait::Type::energy_tank) + foodMass);//*0.2; YA ESTÁ RESUELTO CUANDO COMEN!!

		
		//cout << "hola: " << endl;



		/*
		double daysDigestWithoutTemp;
		if (eating)
		{
			//TODO 2 RAW VALUES HERE 0.05 AND 0.15
			daysDigestWithoutTemp = linearInterpolate(food_mass/traits[Trait::Type::voracity], 0, 1, 1, 3);

			days_digest = ceil(daysDigestWithoutTemp * linearInterpolate(position->temperature, MIN_T, MAX_T, 1,
					getSpecies()->Q10DIGEST));
			eating = false;
		}
		 */
        if(round(foodMass*pow(10,7)) < round(getTrait(Trait::Type::voracity)*pow(10,7)))
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


int Animal::getDaysDigest() const
{
	return days_digest;
}


int Animal::getMaxReproductionEvents()
{
	if(gender == AnimalSpecies::Gender::FEMALE)
	{
		return getSpecies()->getFemaleMaxReproductionEvents();
	}
	else
	{
		return getSpecies()->getMaleMaxReproductionEvents();
	}
}


const list<int> * const Animal::getEncounterEvents() const
{
	return &encounterEvents;
}


double Animal::calculatePredationProbability(const EdibleInterface &edibleAnimal, const double &targetDryMass, bool retaliation, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW) const
{
	double predationProbability = 1.0;
	if(edibleAnimal.getSpecies()->isMobile())
	{
		double interpolatedHunterVoracity = getInterpolatedHunterVoracity();

		double interpolatedSpeedRatio;

		if(edibleAnimal.getSpeed()>0.0){
		  interpolatedSpeedRatio = getInterpolatedSpeedRatio(edibleAnimal.getSpeed());
	    }else{//this is to circunvent NANs - the ratio is made 1
	      interpolatedSpeedRatio = getInterpolatedSpeedRatio(getTrait(Trait::Type::speed));
	    }

		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/edibleAnimal.calculateWetMass(targetDryMass)) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
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


bool Animal::predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, int timeStep, bool retaliation, list<const EdibleInterface*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea)
{
	if(edibleToBePredated.getSpecies()->isMobile())
	{
		ediblesHasTriedToPredate.push_back(&edibleToBePredated);
	}
	bool huntWasSuccessful = false;
	//Steps only goes up when at least the animal HAS TRIED TO HUNT the prey. It goes up regardless of the success
	//The steps penalization is assumed depending on the search area and the longest diagonal

	if(!retaliation && edibleToBePredated.getSpecies()->isMobile())
	{
		double halfLongestDiagonalInsideCurrentCell = sqrt(3 * pow(this->getTerrainCellInterface()->getSize(), 2));
		double distanceToAdd = halfLongestDiagonalInsideCurrentCell*getTrait(Trait::Type::search_area)/maxSearchArea;
		steps += distanceToAdd;
		if(timeStepMaximumSearchArea > 0.0)
		{
			timeStepMaximumHandlingTimer -= distanceToAdd / timeStepMaximumSearchArea;
		} else {
			timeStepMaximumHandlingTimer = 0.0;
		}
		timeStepMaximumHandlingTimer = max(timeStepMaximumHandlingTimer, 0.0);
		steps = min(steps, getTrait(Trait::Type::search_area));
	}

	//arthros and for dinos!!!!
	double randomPredationProbability = Random::randomUniform();

	#ifdef DEBUG
	if(find(huntedAnimalsToday.begin(), huntedAnimalsToday.end(), edibleToBePredated.getId()) != huntedAnimalsToday.end())
	{
		//getTerrainCellInterface()->increaseMultipleSameHuntedAnimalToday();


		//cout << "The animal hunted off the same animal at least twice the same day: " << endl;
		//cout << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
		//cout << " - Animal hunted off: " << huntedAnimal->getId() << "(" << huntedAnimal->getSpecies()->getScientificName() << ")" << endl;
	}
	huntedAnimalsToday.push_back(edibleToBePredated.getId());
	#endif
	//if (predationProbability > randomPredationProbability)
	//TODO attackProb y killProb van EN CADA ESPECIE

//getSpecies()->getKillProbability() > randomPredationProbability

    double prob = calculatePredationProbability(edibleToBePredated, targetDryMass, retaliation, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW);//Dinosaur
//prob*
	if (((getSpecies()->getKillProbability() > randomPredationProbability) && edibleToBePredated.getSpecies()->isMobile()) || !edibleToBePredated.getSpecies()->isMobile()) ///arthropods + Dinosaur //
	{
		#ifdef DEBUG
		if(find(predatedAnimalsToday.begin(), predatedAnimalsToday.end(), edibleToBePredated.getId()) != predatedAnimalsToday.end())
		{
			//getTerrainCellInterface()->increaseMultipleSamePredatedAnimalToday();


			//cerr << "The animal predated the same animal at least twice the same day: " << endl;
			//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
			//cerr << " - Animal predated: " << huntedAnimal->getId() << "(" << huntedAnimal->getSpecies()->getScientificName() << ")" << endl;
		}
		predatedAnimalsToday.push_back(edibleToBePredated.getId());
		#endif

		getSpecies()->addPredationEventOnOtherSpecies(edibleToBePredated.getSpecies()->getId());
		eatenToday++;
		huntWasSuccessful = true;

		edibleToBePredatedProfitability = getSpecies()->getEdibleProfitability(edibleToBePredated.getSpecies()->getId(), getInstar(), edibleToBePredated.getInstar());

		//arthro and for dinos
	    double forNotToDepleteResource = getTrait(Trait::Type::voracity)/(edibleToBePredatedProfitability+getTrait(Trait::Type::assim));
		double leftovers;

        if(edibleToBePredated.isDepleted(forNotToDepleteResource, targetDryMass)){ //resource leftover to partially fulfill voracity
	      leftovers = edibleToBePredated.anyLeft(targetDryMass)*(edibleToBePredatedProfitability+getTrait(Trait::Type::assim));
        }else{ //the entire voracity is available
		  leftovers = forNotToDepleteResource;
		}  

		//The target animal is cleaned up from its source lifestage vector. Or the resource is substracted.
		double fullDryMassToBeEaten = getMutableTerrainCellInterface()->turnEdibleIntoDryMassToBeEaten(edibleToBePredated, targetDryMass, perceptionArea, timeStep, this, leftovers);
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
		biomassExperiencedPerSpecies[edibleToBePredated.getSpecies()->getId()] += fullDryMassToBeEaten;
		
		//Assim with handling time will be calculated according to the FULL dry mass
		//It will be reduced to only profitableMass after handling calculations
		setFoodMassLeftForNextTimeStep(fullDryMassToBeEaten);
		lastHuntedAnimalDryMass = fullDryMassToBeEaten;
		assimilateLastHuntedAnimalAndComputeHandlingTime();
	}

	predationProbabilitiesFile << this->getIdStr() << "\t"
	<< edibleToBePredated.getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< edibleToBePredated.getSpecies()->getScientificName() << "\t"
	<< ((edibleToBePredated.isHunting()) ? 1 : 0) << "\t"
	<< calculateDryMass() << "\t"
	<< targetDryMass << "\t"
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

	for(const auto &[edibleResourceSpecies, edibleInstars] : getSpecies()->getInstarEdibleResourceSpecies(getInstar()))
	{
		ResourceSpecies* resourceSpecies = getTerrainCellInterface()->getMapInterface().getWorldInterface()->getExistingResourceSpecies().at(edibleResourceSpecies);

		double preference = getSpecies()->getEdiblePreference(resourceSpecies->getId(), getInstar());
		
		biomassExperiencedPerSpecies[resourceSpecies->getId()] = preference;
		meanExperiencesPerSpecies[resourceSpecies->getId()] = preference;
	}

	for(const auto &[edibleAnimalSpecies, edibleInstars] : getSpecies()->getInstarEdibleAnimalSpecies(getInstar()))
	{
		AnimalSpecies* animalSpecies = getTerrainCellInterface()->getMapInterface().getWorldInterface()->getExistingAnimalSpecies().at(edibleAnimalSpecies);

		double preference = getSpecies()->getEdiblePreference(animalSpecies->getId(), getInstar());

		biomassExperiencedPerSpecies[animalSpecies->getId()] = preference;
		meanExperiencesPerSpecies[animalSpecies->getId()] = preference;
	}
}


void Animal::updateBiomassExperiencedPerSpecies()
{
	float totalExperiencedBiomass = 0;
	daysExperienced++;

	for(const auto &biomassExperienced : biomassExperiencedPerSpecies)
	{
		totalExperiencedBiomass += biomassExperienced;
	}

	if(totalExperiencedBiomass > 0.0)
	{
		for(auto &biomassExperienced : biomassExperiencedPerSpecies)
		{
			biomassExperienced /= totalExperiencedBiomass;
		}
	}

	double experienceInfluencePerTimeStep = this->getSpecies()->getExperienceInfluencePerDay() / getTerrainCellInterface()->getMapInterface().getWorldInterface()->getTimeStepsPerDay();

	/*
		To update the experience of a species, a weighted average is performed, where the variable 'experienceInfluencePerTimeStep' 
		determines whether recent experience or past experience is more important. If the value 'experienceInfluencePerTimeStep' is 
		close to 1, recent experience is more important.
	*/
	for(unsigned int i = 0; i < meanExperiencesPerSpecies.size(); i++)
	{
		//1.0 will be the base value, meaning it has not come across any animal yet
		meanExperiencesPerSpecies[i] = biomassExperiencedPerSpecies[i] * experienceInfluencePerTimeStep + meanExperiencesPerSpecies[i] * (1.0 - experienceInfluencePerTimeStep);
		//cout << (*it).second << endl;
	}

	//Jordi - 23/03/2022 - this "biomassExperiencedPerSpecies" needs to turn it back to the original biomass scale, otherwise only the last food item matters added to a 0-1 value
	if(totalExperiencedBiomass > 0.0)
	{
		for(auto &biomassExperienced : biomassExperiencedPerSpecies)
		{
			if(daysExperienced <= static_cast<unsigned int>(getTrait(Trait::Type::memoryDepth))){ //this 5 needs to be taken out to the Json file - was already declared somwhere bu Gabi!!!
				biomassExperienced *= totalExperiencedBiomass; //however, to do this well, it really needs to take account of all food items in the last 5 days
			}else{
				biomassExperienced *= totalExperiencedBiomass; 
				biomassExperienced /= static_cast<unsigned int>(getTrait(Trait::Type::memoryDepth)); ///experience averaged over the last 5 days and continue experiencing from 0 days
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
		double kelvinTemperature = this->getTerrainCellInterface()->getMoistureInfo()->getTemperature().getTemperatureKelvin();
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
		double profitableFoodMassToAdd = foodMassToAdd * (edibleToBePredatedProfitability + getTrait(Trait::Type::assim));

		double foodMassToAssimilate;

		double addedWouldBeFoodMass = foodMass + profitableFoodMassToAdd;
		if(round(addedWouldBeFoodMass*pow(10,7)) >= round(getTrait(Trait::Type::voracity)*pow(10,7)))
		{

			foodMassToAssimilate = getTrait(Trait::Type::voracity) - foodMass;
			decreaseFoodMassLeftForNextTimeStep(foodMassToAssimilate / (edibleToBePredatedProfitability + getTrait(Trait::Type::assim)));
			foodMass = getTrait(Trait::Type::voracity);
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
				foodMass = getTrait(Trait::Type::voracity);
				sated = true;

			/* cout << "THIRD   " << endl;
			cout << sated << endl; */
			}
			/*cout << "THIRD   " << endl;
			cout << sated << endl; */

		}

		double handlingTimeToAssimilate = handlingTime * ((foodMassToAssimilate / (edibleToBePredatedProfitability + getTrait(Trait::Type::assim))) / lastHuntedAnimalDryMass);

		//traits[Trait::Type::energy_tank] += traits[Trait::Type::assim] * foodMassToAssimilate;
		handlingTimer += handlingTimeToAssimilate;
		if(timeStepMaximumHandlingTimer > 0.0)
		{
			setTrait(Trait::Type::search_area, getTrait(Trait::Type::search_area) - (handlingTimeToAssimilate / timeStepMaximumHandlingTimer) * timeStepMaximumSearchArea);
		}

		if(::isnan(foodMass) || ::isnan(handlingTimer))
		{
			cout << "Animal id: " << getId() << " (" << this->getSpecies()->getScientificName() << ") - The assimilation resulted in a NaN value for foodMass or handlingTimer." << endl;
		}

		if(!getSpecies()->getPreserveLeftovers()) 
		{
			setFoodMassLeftForNextTimeStep(0.0);
		}
	}
}


const double Animal::getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const
{
	return MathFunctions::linearInterpolate01(dryMass, getSpecies()->getMaximumDryMassObserved());
}


double Animal::calculatePredatoryRiskEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double edibilityValue = (calculateEncounterProbability(edibleToBeEvaluated, edibleToBeEvaluated.calculateDryMass(), muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, edibleToBeEvaluated.calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated.getSpecies()->getId(), getInstar(), edibleToBeEvaluated.getInstar())
			+ getMeanExperience(edibleToBeEvaluated.getSpecies()->getId()))/4;

	return edibilityValue;
}


double Animal::calculateEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double edibilityValue = (calculateEncounterProbability(edibleToBeEvaluated, edibleToBeEvaluated.calculateDryMass(), muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, edibleToBeEvaluated.calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated.getSpecies()->getId(), getInstar(), edibleToBeEvaluated.getInstar())
			+ getMeanExperience(edibleToBeEvaluated.getSpecies()->getId()))/4;

	return edibilityValue;
}


double Animal::calculateEdibilityValueWithMass(const EdibleInterface &edibleToBeEvaluated, const double &dryMass, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double edibilityValue = edibleToBeEvaluated.getInterpolatedDryMass(static_cast<const PointSpatialTree &>(getTerrainCellInterface()->getPosition()).getDepth(), dryMass)
			* ((calculateEncounterProbability(edibleToBeEvaluated, dryMass, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, dryMass, false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated.getSpecies()->getId(), getInstar(), edibleToBeEvaluated.getInstar())
			+ getMeanExperience(edibleToBeEvaluated.getSpecies()->getId()))/4);

	return edibilityValue;
}


double Animal::calculateEncounterProbability(const EdibleInterface &edibleToBeEncountered, const double &dryMass, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double encounterProbability = 1.0;
	if(edibleToBeEncountered.getSpecies()->isMobile())
	{
		double interpolatedHuntedVoracity = getInterpolatedHuntedVoracity(edibleToBeEncountered.getVoracity());
		double interpolatedHunterVoracity = getInterpolatedHunterVoracity();
		double interpolatedVoracityProduct = getInterpolatedVoracityProduct(edibleToBeEncountered.getVoracity());

		double interpolatedHuntedBodySize = getInterpolatedHuntedBodySize(edibleToBeEncountered.getCurrentBodySize());
		double interpolatedHunterBodySize = getInterpolatedHunterBodySize();

		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/edibleToBeEncountered.getSpecies()->convertToWetMass(dryMass)) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
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


const PointContinuous& Animal::getPosition() const
{
	return position;
}

PointContinuous& Animal::getMutablePosition()
{
	return position;
}


void Animal::setPosition(const vector<double> &newPosition)
{
	for(unsigned int i = 0; i < DIMENSIONS; i++)
	{
		setPositionAxisValue(position, i, newPosition[i]);
	}
}

void Animal::setPosition(const PointContinuous &newPosition)
{
	position = newPosition;
}


bool Animal::encounterEdible(EdibleInterface &edibleToBeEncountered, const double &targetDryMass, float attackOrExposedAttackProbability, int day, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double randomEncounterProbability = Random::randomUniform();

	encounterProbabilitiesFile << this->getIdStr() << "\t"
	<< edibleToBeEncountered.getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< edibleToBeEncountered.getSpecies()->getScientificName() << "\t"
	<< ((edibleToBeEncountered.isHunting()) ? 1 : 0) << "\t"
	<< calculateDryMass() << "\t"
	<< targetDryMass << "\t"
	<< randomEncounterProbability << "\t"
	<< calculateEncounterProbability(edibleToBeEncountered, targetDryMass, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) << "\t"
	<< attackOrExposedAttackProbability << "\t"											 
	<< ((attackOrExposedAttackProbability*calculateEncounterProbability(edibleToBeEncountered, targetDryMass, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) > randomEncounterProbability)?1:0) << "\t"
	<< endl;

	#ifdef DEBUG
	if(find(searchedAnimalsToday.begin(), searchedAnimalsToday.end(), edibleToBeEncountered.getId()) != searchedAnimalsToday.end())
	{
		//getTerrainCellInterface()->increaseMultipleSameSearchedAnimalToday();


		//cerr << "The animal searched for the same animal at least twice the same day: " << endl;
		//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
		//cerr << " - Animal searched for: " << searchedAnimal->getId() << "(" << searchedAnimal->getSpecies()->getScientificName() << ")" << endl;
	}


	//searchedAnimalsToday.push_back(edibleToBeEncountered.getId());
	#endif
	//if (encounterProbability > randomEncounterProbability)
	//*calculateEncounterProbability(edibleToBeEncountered)
	if ((attackOrExposedAttackProbability > randomEncounterProbability) || !edibleToBeEncountered.getSpecies()->isMobile())
	{
		#ifdef DEBUG
		if(find(encounteredAnimalsToday.begin(), encounteredAnimalsToday.end(), edibleToBeEncountered.getId()) != encounteredAnimalsToday.end())
		{
			//getTerrainCellInterface()->increaseMultipleSameEncounteredAnimalToday();


			//cerr << "The animal encountered the same animal at least twice the same day: " << endl;
			//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
			//cerr << " - Animal encountered: " << searchedAnimal->getId() << "(" << searchedAnimal->getSpecies()->getScientificName() << ")" << endl;
		}

		
		//encounteredAnimalsToday.push_back(edibleToBeEncountered.getId());
		#endif
		// if encounter occurs
		if(edibleToBeEncountered.getSpecies()->isMobile()){
		edibleToBeEncountered.incrementEncountersWithPredator(getId());
		}
		//incrementEncounters_prey();
		return true;
	}

	return false;
}


bool Animal::canEatEdible(const EdibleInterface* const &edibleToCheck, const list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const
{
	if(getSpecies()->canEatEdible(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar()))
	{
		//double log_mass_ratio = getLog_mass_ratio(this, edibleToCheck);
		//cout << "LMR -> " << log_mass_ratio << endl;
		
		//cout << edibleToCheck->isDepleted(traits[Trait::Type::voracity]) << endl;
		
		//if(traits[Trait::Type::voracity] < 0){
		//cout << timeStep << endl;	
	//cout << timeStep << endl;	
		//cout << timeStep << endl;	
	//cout << timeStep << endl;	
		//cout << timeStep << endl;	
	//cout << timeStep << endl;	
		//cout << timeStep << endl;	
		
		//cout << traits[Trait::Type::voracity] << endl;
		//cout << edibleToCheck->getId() << endl;
		//exit(-1);
		//}

		//this was incorrect in Gabi's version, he used just resource biomass vs min spore parameter
		//now considering everything the animal takes what is left by if available by decresing its voracity value
		double forNotToDepleteResource = getTrait(Trait::Type::voracity)/(getSpecies()->getEdibleProfitability(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar())+getTrait(Trait::Type::assim));

		//this to to feed on what is left if he entire voracity cannot be satisfied
		//Warning: this prevents this animal to fulfill entirely its original voracity (voracity shrinks) with and alternative
		//resource species if it were available - need to test this behavior
		
		
		//arthros and for dinos - this allows feeding on the leftovers without surpassing
		//the limit imposed by the minspores... so resource can grow back
		bool newVor = false;
		if(edibleToCheck->isDepleted(forNotToDepleteResource, dryMass))
		{
			if(edibleToCheck->anyLeft(dryMass)*(getSpecies()->getEdibleProfitability(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar())+getTrait(Trait::Type::assim))>0){
				newVor = true;
			}
		}
			
		return !sated && !isExhausted() &&  
				(!edibleToCheck->isDepleted(forNotToDepleteResource, dryMass) || newVor == true) &&
				edibleToCheck->getPredatedByID() == -1 && predatedByID == -1 &&
				!hasTriedToHunt((Animal*)edibleToCheck, ediblesHasTriedToPredate);
		// && log_mass_ratio >= position->theWorld->getMinLogMassRatio() && log_mass_ratio <= position->theWorld->getMaxLogMassRatio()/*6.678*/;
	}

	return false;
}

void Animal::doDefinitive() 
{ 
	Edible::doDefinitive();
	getMutableTerrainCellInterface()->getMutableMapInterface().getMutableWorldInterface()->generateAnimalId();
}

bool Animal::hasTriedToHunt(AnimalInterface* edibleToCheck, const list<const EdibleInterface*> &ediblesHasTriedToPredate) const
{
	return find(ediblesHasTriedToPredate.cbegin(), ediblesHasTriedToPredate.cend(), edibleToCheck) != ediblesHasTriedToPredate.cend();
}

template <class Archive>
void Animal::serialize(Archive &ar, const unsigned int version) {
	ar & boost::serialization::base_object<AnimalInterface>(*this);

    #ifdef DEBUG
	ar & searchedAnimalsToday;
	ar & encounteredAnimalsToday;
	ar & huntedAnimalsToday;
	ar & predatedAnimalsToday;
	#endif

	ar & nextDinoMassPredicted;
	ar & currentAge;
	ar & daysExperienced;
	ar & tankAtGrowth;
	ar & slopeTarget;
	ar & interceptTarget;
	ar & foodMass;
	ar & finalJMaxVB;
	ar & maxNextInstarMassFromVBPlasticity;
	ar & edibleToBePredatedProfitability;
	ar & foodMassLeftForNextTimeStep;
	ar & lastHuntedAnimalDryMass;
	ar & days_digest;


	ar & eatenToday;
	ar & wetMassAtTheBeginningOfTheTimeStep;
	ar & oldSearchArea;

	ar & huntingMode;
	genome.serialize(ar, version, getSpecies()->getLoci());
	ar & eggDryMassAtBirth;
	ar & factorEggMassFromMom;
	ar & traits;
	variableTraits = getSpecies()->getVariableTraits();

	ar & actualMoltingTimeVector;
	ar & actualMoltingMassVector;
	ar & finalDevTimeVector;
	ar & lengthsVector; 
	ar & massesVector;

	ar & currentBodySize;
	ar & currentPostTempGrowth;
	ar & currentPostTempLinf;

	ar & maxPostTvor;
	ar & minPostTvor;
	ar & maxPostTsearch;
	ar & minPostTsearch;
	ar & maxPostTspeed;
	ar & minPostTspeed;

	ar & position;

	ar & inHabitatShift;

	ar & inBreedingZone;

	ar & inHabitatShiftBeforeBreeding;
	ar & inHabitatShiftAfterBreeding;

	ar & atDestination;
	ar & targetNeighborToTravelTo;

	ar & lastMassBeforeMaturationOrOviposition;
	ar & clutchDryMass;
	ar & massOfMaturationOrLastReproduction;
	ar & ageOfMaturation;
	ar & ageOfFirstMaturation;
	ar & ageForNextReproduction;
	ar & massForNextReproduction;
	ar & eggDryMassForClutch;
	ar & massAtBirth;
	ar & idFromMatedMale;
	ar & generationNumberFromMatedMale;
	ar & mated;

	bool haveGenomeFromMatedMale;
	if(!Archive::is_loading::value) 
	{
		haveGenomeFromMatedMale = genomeFromMatedMale != NULL;
	}
	ar & haveGenomeFromMatedMale;

	if(haveGenomeFromMatedMale)
	{
		boost::serialization::serialize(ar, genomeFromMatedMale, version, getSpecies()->getLoci());
	}
	else
	{
		genomeFromMatedMale = NULL;
	}
	ar & pseudoGrowth;

	ar & steps;

	ar & stepsAttempted;

	ar & lifeStage;

	ar & encounterEvents;
	ar & predatedByID;
	ar & biomassExperiencedPerSpecies;
	ar & meanExperiencesPerSpecies;

	ar & todayEncountersWithPredators;
	ar & daysWithoutFood;
	ar & mature;
	ar & sated;
	ar & pupaTimer;
	ar & handlingTimer;
	ar & timeStepMaximumHandlingTimer;
	ar & timeStepMaximumSearchArea;
	ar & diapauseTimeSteps;
	ar & ageOfLastMoultOrReproduction;
	ar & dateOfDeath;
	ar & lastDayMoved;

	ar & pheno_ini;
	ar & date_egg;

	ar & tank_ini;
	ar & speed_ini;
	ar & assim_ini;

	ar & voracity_ini;
	ar & search_area_ini;

	ar & generationNumberFromFemaleParent;
	ar & generationNumberFromMaleParent;
	ar & ID_prt1; //TODO puntero al padre mejor? o no? esto se usa?
	ar & ID_prt2;
	ar & rep_count;
	ar & fecundity;
	ar & age_first_rep;
	ar & gender;

	boost::serialization::serialize(ar, growthCurveParams, version, getSpecies()->getGrowthCurve()->getType());
}             

// Specialisation
template void Animal::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Animal::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Animal::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Animal::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
