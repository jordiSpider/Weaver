/**
 * @file CellElement.h
 * @brief Defines the CellElement base class representing any patch-modifiable
 *        element within a terrain cell (moisture, resource, obstacle, etc.).
 *
 * This class provides the minimal interface and state required for elements that
 * can be modified by patches. Each element stores a patch priority determining
 * whether incoming patches are allowed to overwrite its current state.
 */

#ifndef CELL_ELEMENT_H_
#define CELL_ELEMENT_H_


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>



/**
 * @class CellElement
 * @brief Base class for any modifiable element contained within a terrain cell.
 *
 * A CellElement represents a component of a TerrainCell that can be modified by
 * environmental patches (moisture, resources, obstacles, habitat domain, etc.).
 * Each element stores a patch priority indicating whether a new patch is allowed
 * to overwrite the current state. Derived classes implement the actual element
 * behaviour, while CellElement manages patch priority and serialization.
 */
class CellElement
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes the element with a default patch priority (typically zero),
     * meaning no patch has been applied yet.
     */
    CellElement();

    /**
     * @brief Constructs the element with an explicit patch priority.
     *
     * @param patchPriority The priority of the patch that initialized this element.
     *                      Higher priorities overwrite lower ones.
     */
    CellElement(const size_t patchPriority);
    
    /**
     * @brief Virtual destructor.
     *
     * Ensures proper cleanup when derived cell elements are destroyed.
     */
    virtual ~CellElement();

    /**
     * @brief Deleted copy constructor.
     *
     * Cell elements are not copyable to avoid inconsistent patch priority
     * propagation across elements.
     */
    CellElement(const CellElement&) = delete;
    
    /**
     * @brief Deleted copy assignment operator.
     *
     * Prevents copying because patch priority integrity must be maintained.
     */
    CellElement& operator=(const CellElement&) = delete;

    /**
     * @brief Returns the current patch priority of this element.
     *
     * Patch priority is used to determine whether a new patch may overwrite
     * this element: a new patch only applies if its priority is greater than
     * the stored priority.
     *
     * @return The stored patch priority.
     */
    size_t getPatchPriority() const;

    /**
     * @brief Serializes this CellElement instance.
     *
     * Saves and loads the patch priority for persistence or distributed simulation.
     *
     * @tparam Archive The Boost.Serialization archive type.
     * @param ar Serialization archive.
     * @param version Serialization version number.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /// The patch priority that last modified this element.
    size_t patchPriority;

    /**
     * @brief Sets a new patch priority for the element.
     *
     * This method is typically called by PatchApplicator implementations when a
     * patch is successfully applied.
     *
     * @param newPatchPriority The priority assigned by the applied patch.
     */
    void setPatchPriority(const size_t newPatchPriority);
};

#endif /* CELL_ELEMENT_H_ */
