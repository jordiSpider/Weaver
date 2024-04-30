#ifndef ANIMAL_H_
#define ANIMAL_H_


#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>
#include <climits>
#include <memory>

#include "IBM/World/LivingBeings/Animals/AnimalInterface.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"
#include "IBM/World/LivingBeings/Animals/AnimalGrowth.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"
#include "Misc/CustomIndexedVector.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Genome.h"
#include "IBM/World/Map/Points/PointContinuous.h"



class Animal: public AnimalInterface
{
protected:
    LifeStage lifeStage;

    AnimalSpecies::Gender::GenderValue gender;

    Genome genome;

    PointContinuous position;

    #ifdef DEBUG
        CustomIndexedVector<Trait::Type, CustomIndexedVector<TraitDefinitionSection::Elements, bool>> baseTraitsElementsInitialised;
    #endif

    CustomIndexedVector<Trait::Type, CustomIndexedVector<TraitDefinitionSection::Elements, double>> baseTraitsElements;

    #ifdef DEBUG
        CustomIndexedVector<Trait::Type, bool> finalTraitsValueInitialised;
        CustomIndexedVector<Trait::Type, unsigned int> finalTraitsValueLastTimeStep;
    #endif

	CustomIndexedVector<Trait::Type, double> finalTraitsValue;

    #ifdef DEBUG
        bool currentEnergyTankInitialised;
        unsigned int currentEnergyTankLastTimeStep;
    #endif

    double currentEnergyTank;

    AnimalGrowth* animalGrowth;

    std::map<Temperature, std::pair<double, double>> lowerTempSizeRuleConstantAccumulationVector;
	double lastLowerTempSizeRuleConstant;

	std::map<Temperature, std::pair<double, double>> upperTempSizeRuleConstantAccumulationVector;
	double lastUpperTempSizeRuleConstant;

    double currentAge;

    bool mature;

    void setFinalTraitValue(const Trait::Type& name, const double& newValue);

    void initTraits();

    void deleteHomologousCorrelosomes();

    const std::map<Temperature, std::pair<double, double>>& getLowerTempSizeRuleConstantAccumulationVector() const;
	const double& getLastLowerTempSizeRuleConstant() const;

	const std::map<Temperature, std::pair<double, double>>& getUpperTempSizeRuleConstantAccumulationVector() const;
	const double& getLastUpperTempSizeRuleConstant() const;

	void setLowerTempSizeRuleConstantAccumulationVector(const std::map<Temperature, std::pair<double, double>>& newLowerTempSizeRuleConstantAccumulationVector);
	void setLastLowerTempSizeRuleConstant(const double& newLastLowerTempSizeRuleConstant);
	void setUpperTempSizeRuleConstantAccumulationVector(const std::map<Temperature, std::pair<double, double>>& newUpperTempSizeRuleConstantAccumulationVector);
	void setLastUpperTempSizeRuleConstant(const double& newLastUpperTempSizeRuleConstant);

    void setOtherAttributes();

    #ifdef DEBUG
		void testSetBaseTraitElement(const Trait::Type name, const TraitDefinitionSection::Elements elementType);
        void testGetBaseTraitElement(const Trait::Type name, const TraitDefinitionSection::Elements elementType) const;

        void testSetFinalTraitValue(const Trait::Type name);
        void testGetFinalTraitValue(const Trait::Type name) const;

        void testModifyEnergyTank();
        void testGetCurrentEnergyTank() const;
	#endif

public:
    Animal(AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, 
        const LifeStage& lifeStage
    );
    Animal(AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, 
        const LifeStage& lifeStage, Gamete* const firstParentGamete, Gamete* const secondParentGamete,
        const AnimalSpecies::Gender::GenderValue& gender
    );

    virtual ~Animal();

    const AnimalSpecies* const getSpecies() const;
    AnimalSpecies* const getMutableSpecies();

    const LifeStage& getLifeStage() const;

    const Genome& getGenome() const;

    void tuneTraits(const Temperature& temperature);

    const PointContinuous& getPosition() const;
	PointContinuous& getMutablePosition();
	
    const double& getCurrentEnergyTank() const;

	void setPosition(const PointContinuous &newPosition);
    void setPosition(const std::vector<double> &newPosition);

    const unsigned int& getCellDepthOnActualInstar() const;

    const double getSpeed() const;

    const double getVoracity() const;

    void modifyEnergyTank(const double &newValue);

    void setCurrentAge(const double &newCurrentAge);
    const double& getCurrentAge() const;

    void setMatureValue(const bool isMature);
    const bool isMature() const;

    AnimalSpecies::Gender::GenderValue getGender() const;

    const AnimalGrowth* const getAnimalGrowth() const;
	AnimalGrowth* const getMutableAnimalGrowth();

    const CustomIndexedVector<Trait::Type, CustomIndexedVector<TraitDefinitionSection::Elements, double>>& getBaseTraitElementVector() const;

    const double& getBaseTraitElement(const Trait::Type name, const TraitDefinitionSection::Elements elementType) const;
	const double& getBaseTraitValue(const Trait::Type name) const;

    void setBaseTraitElement(const Trait::Type name, const TraitDefinitionSection::Elements elementType, const double& newValue);

    const double& getFinalTraitValue(const Trait::Type& name) const;

    const double calculateDryMass() const;

    const double calculateWetMass() const;
	const double calculateWetMass(const double &dryMass) const;
};

#endif // ANIMAL_H_
