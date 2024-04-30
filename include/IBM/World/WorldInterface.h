#ifndef WORLD_INTERFACE_H_
#define WORLD_INTERFACE_H_


#include <string>
#include <unordered_map>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <ostream>
#include <boost/filesystem.hpp>

#include "IBM/World/Map/Patches/Moisture/MoisturePatch.h"
#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "Misc/Macros.h"
#include "Misc/EnumClass.h"


class WorldInterface
{
private:
    friend class boost::serialization::access;

public:
    enum class Type : unsigned int
	{
		Arthropods,
		Dinosaurs,
		Aquatic
	};

    static std::unique_ptr<WorldInterface> createInstance(nlohmann::json* jsonTree, nlohmann::json &worldConfig, boost::filesystem::path outputFolder, boost::filesystem::path configPath, int burnIn);
    static std::unique_ptr<WorldInterface> createInstance(const Type &worldType);


	WorldInterface();
	virtual ~WorldInterface();

    virtual void initializeAnimals()=0;
    virtual void saveOptimizationResult(boost::filesystem::path resultFolder)=0;
    virtual void evolveWorld(bool fromCheckpoint = false)=0;
    virtual double getMassRatio() const=0;
    virtual void saveAppliedMoisture(MoisturePatch &moisturePatch)=0;
    virtual const unsigned int getTimeStepsPerDay() const=0;
	virtual void printAnimalsAlongCells(const int day, const int simulationPoint) const=0;
    virtual double calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const=0;
    virtual double calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const=0;
    virtual double calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const=0;
    virtual const bool isDinosaurs() const;
    virtual bool getCompetitionAmongResourceSpecies() const=0;
    virtual const std::vector<ResourceSpecies*>& getExistingResourceSpecies() const=0;
    virtual std::vector<ResourceSpecies*>& getMutableExistingResourceSpecies()=0;
    virtual std::vector<AnimalSpecies*>& getMutableExistingAnimalSpecies()=0;
    virtual const std::vector<AnimalSpecies*>& getExistingAnimalSpecies() const=0;
    virtual std::vector<Species*>& getMutableExistingSpecies()=0;
    virtual const std::vector<Species*>& getExistingSpecies() const=0;
    virtual bool getSaveAnimalConstitutiveTraits() const=0;
    virtual std::ofstream& getConstitutiveTraitsFile()=0;
    virtual bool isGrowthAndReproTest() const=0;
    virtual double getPredictedSpeed(const AnimalInterface* const animal) const=0;
    virtual void increaseMovePrintBar()=0;
    virtual const boost::filesystem::path& getOutputFolder() const=0;
    virtual const Type getWorldType() const=0;
    virtual const id_type generateEdibleId()=0;
    virtual const id_type generateResourceId()=0;
    virtual const id_type generateAnimalId()=0;

    /**
     * @brief Serialize the WorldInterface object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, WorldInterface* &worldInterfacePtr, const unsigned int version);
    }
}

#endif // WORLD_INTERFACE_H_
