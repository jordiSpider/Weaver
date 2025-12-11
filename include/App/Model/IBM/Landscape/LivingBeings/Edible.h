/**
 * @file Edible.h
 * @brief Defines the base class for edible entities in the simulation.
 */

#ifndef EDIBLE_H_
#define EDIBLE_H_


#include <list>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Species/Species.h"
#include "App/Model/IBM/Landscape/LivingBeings/Growth/EdibleGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/EdibleID.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"



class TerrainCell;
class Map;
class Landscape;


/**
 * @class Edible
 * @brief Represents a generic edible entity within a terrain cell.
 *
 * The Edible class serves as a base class for all resources or prey that animals
 * can consume. It stores information about its unique identifier, associated species,
 * and the terrain cell it belongs to. Subclasses implement growth, predation, and
 * biomass conversion.
 */
class Edible {
public:
	/// Default constructor
	Edible();

	/**
     * @brief Constructs an Edible with a specific ID, species, and terrain cell.
     * @param id Unique EdibleID
     * @param mySpecies Pointer to the associated Species
     * @param terrainCell Pointer to the terrain cell containing this edible
     */
	Edible(const EdibleID id, Species* const mySpecies, TerrainCell* terrainCell);
	
	/// Virtual destructor
	virtual ~Edible();

	/// Deleted copy constructor
	Edible(const Edible&) = delete;

	/// Deleted copy assignment operator
	Edible& operator=(const Edible&) = delete;

	/**
     * @brief Returns the unique identifier of this edible.
     * @return Reference to the EdibleID
     */
	const EdibleID& getId() const;

	/**
     * @brief Returns the associated species (const).
     * @return Pointer to the Species
     */
	virtual const Species* getSpecies() const;

	/**
     * @brief Returns the associated species (mutable).
     * @return Pointer to the Species
     */
	virtual Species* getMutableSpecies();

	/**
     * @brief Sets the species associated with this edible.
     * @param newSpecies Pointer to the new Species
     */
	virtual void setSpecies(Species* newSpecies);

	/**
     * @brief Sets the terrain cell where this edible is located.
     * @param newTerrainCell Pointer to the new TerrainCell
     */
	virtual void setTerrainCell(TerrainCell* newTerrainCell);

     /**
      * @brief Returns the terrain cell where this edible object is currently located.
      *
      * Provides read-only access to the TerrainCell pointer.
      *
      * @return Pointer to the current TerrainCell.
      */
	const TerrainCell* getTerrainCell() const;

	/**
     * @brief Returns the terrain cell where this edible resides (mutable).
     * @return Pointer to the TerrainCell
     */
	TerrainCell* getMutableTerrainCell() const;

	/**
     * @brief Returns the growth building block for this edible (const).
     * @return Reference to the EdibleGrowth
     */
	virtual const EdibleGrowth& getGrowthBuildingBlock() const=0;

	/**
     * @brief Returns the growth building block for this edible (mutable).
     * @return Reference to the EdibleGrowth
     */
	virtual EdibleGrowth& getMutableGrowthBuildingBlock()=0;

	/// Increases the number of predation encounters
	virtual void increasePredationEncounters()=0;

	/// Returns whether this edible is currently being hunted
	virtual bool isHunting() const=0;

	/**
     * @brief Converts part of the edible into dry mass for a predator.
     * @param targetDryMass Amount of dry mass targeted for consumption
     * @param predatorRemainingVoracity Remaining voracity of the predator
     * @return Actual dry mass obtained
     */
	virtual DryMass turnIntoDryMass(const DryMass &targetDryMass, const PreciseDouble& predatorRemainingVoracity) const=0;

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
	/// Unique identifier for this edible
    EdibleID id;

    /// Pointer to the species associated with this edible
    Species* mySpecies;

    /// Pointer to the terrain cell containing this edible
    TerrainCell* terrainCell;

	/// Sets the species growth block (implemented in derived classes)
	virtual void setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth)=0;
};

#endif /* EDIBLE_H_ */
