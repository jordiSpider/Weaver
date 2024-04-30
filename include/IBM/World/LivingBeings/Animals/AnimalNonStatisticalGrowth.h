#ifndef ANIMAL_NON_STATISTICAL_GROWTH_MODULE_H_
#define ANIMAL_NON_STATISTICAL_GROWTH_MODULE_H_


#include "IBM/World/LivingBeings/Animals/AnimalGrowth.h"
#include "Misc/CustomIndexedVector.h"


class AnimalNonStatistical;


class AnimalNonStatisticalGrowth : public AnimalGrowth {
public:
    AnimalNonStatisticalGrowth(AnimalNonStatistical* const animal, const double& factorEggMass, const double& actualTime);
    virtual ~AnimalNonStatisticalGrowth();

    const AnimalNonStatistical* const getAnimal() const;
    AnimalNonStatistical* const getMutableAnimal() const;

    void tune(std::ostream& tuneTraitsFile, bool printGrowthData);

    const CustomIndexedVector<Instar, double>& getInstarLengthsVector() const;
    const double& getInstarLength(const Instar& instar) const;
    
    const CustomIndexedVector<Instar, double>& getInstarMassesVector() const;
    const double& getInstarMass(const Instar& instar) const;

    
    const double getAgeOfFirstMaturation() const;

    void grow();

    void isReadyToResumeFromPupaOrDecreasePupaTimer();
    void isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(const double& relativeHumidity);

    const int getDiapauseTimeSteps() const;

    const double& getLongevity() const;

    //The function below is to grow the animals that need to be at a certain instar at the beginning of the simulation
    void forceMolting();

    const double& getMassForNextReproduction() const;

    void setMassForNextReproduction(const double& newMassForNextReproduction);

    const unsigned int computeEggBatchNumber() const;

    const int& getDateEgg() const;

    const double& getTotalReproMass() const;

    const double& getTankAtGrowth() const;

    void calculateGrowthCurves() override;

protected:
    AnimalNonStatistical* const animal;

    double longevity;

    int pupaTimer;

    int diapauseTimeSteps;

    double massForNextReproduction;

    double lastMassBeforeMaturationOrOviposition;

    double clutchDryMass;

    double tankAtGrowth;

    std::vector<double> actualMoltingTimeVector;
	std::vector<double> actualMoltingMassVector;

    int date_egg;

    double totalReproMass;


    void calculateLongevity();

    void calculateTotalReproMass();

    const double& getLastMassBeforeMaturationOrOviposition() const;
    void setLastMassBeforeMaturationOrOviposition(const double& newLastMassBeforeMaturationOrOviposition);

    const double calculateClutchDryMass(const double& individualDryMass) const;

    void setDateEgg(double newDateEgg);
};

#endif /* ANIMAL_NON_STATISTICAL_GROWTH_MODULE_H_ */
