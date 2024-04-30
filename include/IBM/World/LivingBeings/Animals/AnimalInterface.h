#ifndef ANIMAL_INTERFACE_H_
#define ANIMAL_INTERFACE_H_


#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Edible.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/Map/Geometry/Geometry.h"
#include "IBM/World/LivingBeings/LifeStage.h"


class TerrainCellInterface;


class AnimalInterface : public Edible
{
private:
	friend class boost::serialization::access;

public:
	AnimalInterface(Species* const mySpecies, const Instar &instar, const bool temporary, TerrainCellInterface* parentTerrainCellInterface = nullptr);
	virtual ~AnimalInterface();

	AnimalSpecies* const getSpecies() const;
	virtual bool isSated() const=0;
	virtual bool isExhausted() const=0;
	virtual const double getRemainingVoracity() const=0;
	virtual const double& getAssim() const=0;
	virtual const LifeStage& getLifeStage() const=0;
	virtual AnimalSpecies::Gender::GenderValue getGender() const=0;
	virtual void setPosition(const std::vector<double> &newPosition)=0;
	virtual void setPosition(const PointContinuous &newPosition)=0;
	virtual const PointContinuous& getPosition() const=0;
	virtual void updateDepth()=0;
	virtual const Genome& getGenome() const=0;
	virtual std::unique_ptr<std::list<AnimalInterface*>> breed(int timeStep, Temperature temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile)=0;
	virtual double calculateEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const=0;
	virtual double getMeanExperience(const unsigned int speciesId) const=0;
	virtual bool encounterEdible(EdibleInterface &searchedAnimal, const double &targetDryMass, float attackOrExposedAttackProbability, int timeStep, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const=0;
	virtual void isReadyToBeBorn(int timeStep, int timeStepsPerDay)=0;
	virtual void isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(double relativeHumidity)=0;
	virtual void isReadyToResumeFromPupaOrDecreasePupaTimer()=0;
	virtual void calculateGrowthCurves(Temperature temperature, std::ostream& tuneTraitsFile, bool printGrowthData, double ageAtInitialization)=0;
	virtual void increaseAge(int increase)=0;
	virtual void tuneTraits(int timeStep, int timeStepsPerDay, Temperature temperature, float relativeHumidity, std::ostream& tuneTraitsFile, bool printGrowthData, bool fromForceMolting1)=0;
	virtual void moveAnimal(int timeStep, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)=0;
	virtual double calculateEncounterProbability(const EdibleInterface &edibleToBeEncountered, const double &dryMass, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const=0;
	virtual double calculatePredationProbability(const EdibleInterface &edibleToBePredated, const double &targetDryMass, bool retaliation, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW) const=0;
	virtual bool isMature()=0;
	virtual bool isMated()=0;
	virtual void setGenomeFromMatedMale(const AnimalInterface* matedMale)=0;
	virtual bool postBreeding(int timeStep)=0;
	virtual const int& getGenerationNumberFromMaleParent() const=0;
	virtual const double getCurrentLength() const=0;
	virtual void dieFromBackground(int timeStep)=0;
	virtual void assimilateFoodMass(int timeStep)=0;
	virtual void metabolize(int timeStep)=0;
	virtual void grow(int timeStep)=0;
	virtual void printVoracities(int timeStep, std::ostream& voracitiesFile)=0;
	virtual std::pair<bool, bool> interpolateTraits()=0;
	virtual void printTraits(std::ostream& traitsFile)=0;
	virtual void adjustTraits()=0;
	virtual const double& getEggDryMassAtBirth() const=0;
	virtual void setMassAtBirth(double massAtBirth)=0;
	virtual void setExperiencePerSpecies()=0;
	virtual double calculatePredatoryRiskEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const=0;
	virtual bool hasTriedToHunt(AnimalInterface* edibleToCheck, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate) const=0;
	virtual double calculateEdibilityValueWithMass(const EdibleInterface &edibleToBeEvaluated, const double &dryMass, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const=0;
	virtual void updateLimits()=0;
	virtual void sumPseudoGrowthMean()=0;
	virtual Temperature forceMolting2()=0;
	virtual void sumPseudoGrowthSd()=0;
	virtual Temperature forceMolting()=0;
	virtual double getSearchArea()=0;
	virtual const unsigned int& getCellDepthOnActualInstar() const=0;
	virtual const double& getTrait(const Trait::Type& name) const=0;
	virtual const double& getSpeedAreaIni() const=0;
	virtual const bool isInBreedingZone() const=0;
	virtual void setInBreedingZone(const bool newInBreedingZoneValue)=0;

	/**
     * @brief Serialize the Trait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* ANIMAL_INTERFACE_H_ */
