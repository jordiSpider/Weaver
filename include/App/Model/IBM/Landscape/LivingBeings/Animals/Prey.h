/**
 * @file Prey.h
 * @brief Defines the Prey class, representing an individual prey item during a predation event.
 *
 * The Prey class encapsulates all the information required for handling predation
 * interactions between predators and prey. It stores ecological and physiological
 * attributes such as species identity, instar stage, dry mass, handling time,
 * and the amount of food (dry mass) that remains available during consumption.
 */

#ifndef PREY_H_
#define PREY_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Species/Species.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "Misc/Types.h"


/**
 * @class Prey
 * @brief Represents a prey item in the ecosystem, including its species, growth stage, and available biomass.
 *
 * The Prey class stores information about an individual prey item that can be consumed by predators.
 * It tracks the species ID, instar (growth stage), initial dry mass, handling time, and the current
 * food available for consumption. It also calculates the amount of food available per simulation timestep.
 */
class Prey
{
public:
	/**
     * @brief Default constructor. Initializes a Prey object with default values.
     */
	Prey();

	/**
     * @brief Constructs a Prey with specific parameters.
     * @param id The species identifier of the prey.
     * @param instar The growth stage of the prey.
     * @param dryMass The initial dry mass of the prey.
     * @param handlingTime The time required for a predator to consume the prey.
     * @param timeStepsPerDay The number of simulation timesteps per day.
     */
	explicit Prey(const Species::ID& id, const Instar& instar, const DryMass& dryMass, const Day& handlingTime, const PreciseDouble& timeStepsPerDay);

	/**
     * @brief Destructor.
     */
	virtual ~Prey();

	/// Copy constructor
	Prey(const Prey& other);

	/// Copy assignment operator
	Prey& operator=(const Prey& other);

	/**
     * @brief Returns the species ID of the prey.
     * @return Reference to the Species::ID.
     */
	const Species::ID& getId() const;

	/**
     * @brief Returns the instar (growth stage) of the prey.
     * @return Reference to the Instar.
     */
	const Instar& getInstar() const;

	/**
     * @brief Returns the initial dry mass of the prey.
     * @return Reference to the initial DryMass.
     */
	const DryMass& getInitialDryMass() const;

	/**
     * @brief Returns the handling time required for a predator to consume the prey.
     * @return Reference to the Day representing handling time.
     */
	const Day& getHandlingTime() const;

	/**
     * @brief Returns the remaining dry mass available for consumption.
     * @return Reference to the current DryMass available as food.
     */
	const DryMass& getFoodDryMass() const;

	/**
     * @brief Returns the dry mass available per simulation timestep.
     * @return Reference to the DryMass per timestep.
     */
	const DryMass& getFoodDryMassPerTimeStep() const;

	/**
     * @brief Reduces the available food by a given amount.
     * @param foodToEliminate The DryMass of food to subtract.
     */
	void decreaseFoodMass(const DryMass &foodToEliminate);

	/**
     * @brief Checks if there is any remaining food in the prey.
     * @return True if food is still available, false otherwise.
     */
	bool isThereLeftoverFood() const;

	/**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
	template <class Archive>
	void serialize(Archive &ar, const unsigned int version);

private:
	Species::ID id;                  ///< Species identifier of the prey
    Instar instar;                   ///< Growth stage of the prey
    DryMass initialDryMass;          ///< Initial dry mass of the prey
    Day handlingTime;                ///< Handling time required by a predator

    DryMass foodDryMass;             ///< Remaining dry mass available as food
    DryMass foodDryMassPerTimeStep;  ///< Dry mass available per simulation timestep
};

#endif /* PREY_H_ */
