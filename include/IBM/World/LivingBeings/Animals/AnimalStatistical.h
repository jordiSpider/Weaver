#ifndef ANIMAL_STATISTICAL_H_
#define ANIMAL_STATISTICAL_H_


#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Animals/AnimalStatisticalGrowth.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "Misc/Macros.h"



class AnimalStatistical : public Animal
{
protected:
    
public:
    AnimalStatistical(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface);   

    virtual ~AnimalStatistical(); 

    const AnimalStatisticalGrowth* const getAnimalGrowth() const;
	AnimalStatisticalGrowth* const getMutableAnimalGrowth();

    int getPredatedByID() const;
    void incrementEncountersWithPredator(const int &predatorId);
    const bool isHunting() const;
    bool isDepleted(double foodDemand, const double &dryMass) const;
    double anyLeft(const double &dryMass) const;
    const double getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const;
    const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const;
    void setNewLifeStage(const LifeStage newLifeStage);
	void setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps);
	void setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps, int predatorId);
    double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea);
    bool canEatEdible(const EdibleInterface* const &edibleToCheck, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const;
    bool predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, const unsigned int numberOfTimeSteps, bool retaliation, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea);
};

#endif // ANIMAL_STATISTICAL_H_
