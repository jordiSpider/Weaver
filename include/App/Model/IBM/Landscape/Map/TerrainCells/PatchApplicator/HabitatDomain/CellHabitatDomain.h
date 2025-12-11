/**
 * @file CellHabitatDomain.h
 * @brief Declares the CellHabitatDomain class, a concrete implementation of
 *        CellHabitatDomainInterface representing the habitat-domain state
 *        of a terrain cell.
 *
 * This class does not add new behavior over CellHabitatDomainInterface,
 * but provides a concrete instantiable type used within the landscape
 * to store the habitat-domain properties of each cell. It supports patch
 * application, serialization, and priority-based overwriting of habitat
 * information.
 */


#ifndef CELL_HABITAT_DOMAIN_H_
#define CELL_HABITAT_DOMAIN_H_



#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/HabitatDomain/CellHabitatDomainInterface.h"


/**
 * @class CellHabitatDomain
 * @brief Concrete implementation of CellHabitatDomainInterface representing 
 *        the habitat-domain properties of a terrain cell.
 *
 * The CellHabitatDomain class stores, updates and serializes habitat-domain
 * information for a single terrain cell. A habitat domain encodes which 
 * animal species and instars consider this cell as suitable habitat.
 *
 * This class does not introduce additional behavior beyond what is provided
 * by CellHabitatDomainInterface, but serves as the actual instantiated type
 * used for each terrain cell in the landscape.
 *
 * Habitat-domain information is patch-based: every modification is applied
 * according to a patch priority system. Higher-priority patches overwrite
 * lower-priority ones, ensuring consistent and deterministic updates when
 * multiple sources affect the same cell.
 *
 * The class also supports Boost serialization, enabling the habitat-domain
 * configuration to be saved and restored in simulation snapshots.
 */
class CellHabitatDomain : public CellHabitatDomainInterface
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes an empty habitat-domain structure with default patch priority (0).
     */
    CellHabitatDomain();

    /**
     * @brief Constructor with explicit patch priority.
     *
     * @param newHabitatDomainPatchPriority Initial patch priority applied to habitat-domain data.
     */
    CellHabitatDomain(const size_t newHabitatDomainPatchPriority);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~CellHabitatDomain();

    /// Deleted copy constructor (CellElement-derived objects cannot be copied).
    CellHabitatDomain(const CellHabitatDomain&) = delete;

    /// Deleted assignment operator.
    CellHabitatDomain& operator=(const CellHabitatDomain&) = delete;

    /**
     * @brief Serializes the CellHabitatDomain object.
     *
     * Serialization includes the patch priority and the full
     * habitat-domain status inherited from CellHabitatDomainInterface.
     *
     * @tparam Archive Boost serialization archive type.
     * @param ar Archive used for reading/writing.
     * @param version Serialization version (currently unused).
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* CELL_HABITAT_DOMAIN_H_ */
