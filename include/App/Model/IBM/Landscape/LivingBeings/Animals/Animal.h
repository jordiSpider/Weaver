/**
 * @file Animal.h
 * @brief Defines the Animal class, an abstract class representing animals in the ecosystem.
 *
 * The Animal class inherits from Edible and provides the base interface for all animals.
 * It manages species, life stage, genetics, position, gender, and growth.
 */

#ifndef ANIMAL_H_
#define ANIMAL_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Edible.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Genetics.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"



class TerrainCell;


/**
 * @class Animal
 * @brief Abstract base class representing an individual animal.
 *
 * This class stores general information about an animal, such as its species,
 * life stage, genetics, position, and gender. It defines pure virtual functions
 * for derived classes to implement behaviors such as voracity, growth, and
 * life stage transitions.
 */
class Animal: public Edible
{
public:
    /**
     * @brief Default constructor.
     */
    Animal();

    /**
     * @brief Constructor for a newly created animal.
     * @param id Unique identifier for the animal.
     * @param mySpecies Pointer to the animal's species.
     * @param terrainCell Pointer to the cell the animal occupies.
     * @param lifeStage Current life stage of the animal.
     * @param actualTimeStep Current simulation time step.
     */
    Animal(const EdibleID id, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, 
        const LifeStage& lifeStage, const TimeStep actualTimeStep
    );

    /**
     * @brief Constructor for an animal created via reproduction.
     * @param id Unique identifier for the animal.
     * @param mySpecies Pointer to the animal's species.
     * @param terrainCell Pointer to the cell the animal occupies.
     * @param lifeStage Current life stage of the animal.
     * @param firstParentGamete Pointer to the first parent's gamete.
     * @param secondParentGamete Pointer to the second parent's gamete.
     * @param gender Gender of the offspring.
     * @param actualTimeStep Current simulation time step.
     */
    Animal(const EdibleID id, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, 
        const LifeStage& lifeStage, Gamete* const firstParentGamete, Gamete* const secondParentGamete,
        const Gender& gender, const TimeStep actualTimeStep
    );

    /**
     * @brief Virtual destructor.
     */
    virtual ~Animal();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    Animal(const Animal&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent assignment.
     */
	Animal& operator=(const Animal&) = delete;

    /**
     * @brief Get a const pointer to the animal's species.
     * @return Pointer to the AnimalSpecies.
     */
    const AnimalSpecies* getSpecies() const;

    /**
     * @brief Get a mutable pointer to the animal's species.
     * @return Pointer to the AnimalSpecies.
     */
    AnimalSpecies* getMutableSpecies();

    /**
     * @brief Get the animal's current life stage.
     * @return Const reference to LifeStage.
     */
    const LifeStage& getLifeStage() const;

    /**
     * @brief Set a new species for the animal (overrides Edible).
     * @param newSpecies Pointer to the new species.
     */
    virtual void setSpecies(Species* newSpecies) override;

    /**
     * @brief Get a const reference to the animal's genetics.
     * @return Const reference to Genetics.
     */
    const Genetics& getGenetics() const;

    /**
     * @brief Get a mutable reference to the animal's genetics.
     * @return Reference to Genetics.
     */
    Genetics& getMutableGenetics();

    /**
     * @brief Get the remaining voracity of the animal.
     * @return Remaining voracity as PreciseDouble.
     *
     * Pure virtual: must be implemented in derived classes.
     */
    virtual PreciseDouble getRemainingVoracity() const=0;

    /**
     * @brief Add animal species to the simulation.
     * @param numberOfInstars Number of instars.
     * @param timeStepsPerDay Number of time steps per day.
     *
     * Pure virtual: must be implemented in derived classes.
     */
    virtual void addAnimalSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay)=0;

    /**
     * @brief Add resource species to the simulation.
     * @param numberOfInstars Number of instars.
     * @param timeStepsPerDay Number of time steps per day.
     *
     * Pure virtual: must be implemented in derived classes.
     */
    virtual void addResourceSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay)=0;

    /**
     * @brief Get the instar to evaluate the terrain cells.
     * @return Const reference to Instar.
     *
     * Pure virtual: must be implemented in derived classes.
     */
    virtual const Instar& getInstarToEvaluateCells() const=0;

    /**
     * @brief Apply an allometric model to the animal's traits.
     * @param coefficient Model coefficient.
     * @param scale Scaling factor.
     * @return Result of the allometric model as PreciseDouble.
     */
    PreciseDouble applyAllometricModel(const PreciseDouble& coefficient, const PreciseDouble& scale) const;

    /**
     * @brief Get a const reference to the animal's position.
     * @return Const reference to PointContinuous.
     */
    const PointContinuous& getPosition() const;

    /**
     * @brief Get a mutable reference to the animal's position.
     * @return Reference to PointContinuous.
     */
	PointContinuous& getMutablePosition();

    /**
     * @brief Get the animal's growth building block (const).
     * @return Const reference to AnimalGrowth.
     *
     * Pure virtual: must be implemented in derived classes.
     */
    virtual const AnimalGrowth& getGrowthBuildingBlock() const=0;

    /**
     * @brief Get the animal's growth building block (mutable).
     * @return Reference to AnimalGrowth.
     *
     * Pure virtual: must be implemented in derived classes.
     */
	virtual AnimalGrowth& getMutableGrowthBuildingBlock()=0;

    /**
     * @brief Set a new position for the animal.
     * @param newPosition New position as PointContinuous.
     */
	void setPosition(const PointContinuous &newPosition);

    /**
     * @brief Set a new position for the animal.
     * @param newPosition Vector of PreciseDouble coordinates.
     */
    void setPosition(const std::vector<PreciseDouble> &newPosition);

    /**
     * @brief Get the cell depth on the animal's current instar.
     * @return Depth as unsigned int.
     */
    unsigned int getCellDepthOnActualInstar() const;

    /**
     * @brief Get the animal's gender.
     * @return Gender of the animal.
     */
    Gender getGender() const;

    /**
     * @brief Set a new life stage for the animal (abstract version).
     * @param landscape Pointer to the landscape.
     * @param newLifeStage The new life stage.
     *
     * Pure virtual: must be implemented in derived classes.
     */
    virtual void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage)=0;
	
    /**
     * @brief Set a new life stage for the animal with a specified number of time steps.
     * @param landscape Pointer to the landscape.
     * @param newLifeStage The new life stage.
     * @param numberOfTimeSteps Number of time steps for transition.
     * @param timeStepsPerDay Number of time steps per day.
     *
     * Pure virtual: must be implemented in derived classes.
     */
    virtual void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay)=0;
	
    /**
     * @brief Set a new life stage for the animal considering predation.
     * @param landscape Pointer to the landscape.
     * @param newLifeStage The new life stage.
     * @param numberOfTimeSteps Number of time steps for transition.
     * @param predatorId ID of the predator.
     * @param timeStepsPerDay Number of time steps per day.
     *
     * Pure virtual: must be implemented in derived classes.
     */
    virtual void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, EdibleID predatorId, const PreciseDouble& timeStepsPerDay)=0;

    #ifdef DEBUG
        /**
         * @brief Deserialize debug variables (for debugging purposes only).
         */
        void deserializeDebugVariables();
    #endif

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    Genetics genetics;       /**< Genetics of the animal */
    LifeStage lifeStage;     /**< Current life stage */
    Gender gender;           /**< Animal gender */
    PointContinuous position;/**< Continuous position in the landscape */

    /**
     * @brief Check the minimum value for growth and enforce constraints if needed.
     */
    void checkGrowthMinimumValue() const;
};

#endif // ANIMAL_H_
