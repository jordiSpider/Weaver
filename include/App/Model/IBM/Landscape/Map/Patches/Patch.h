/**
 * @file Patch.h
 * @brief Defines the Patch class for representing environmental patches in a landscape.
 *
 * This file contains the Patch class and related types for managing spatial
 * patches that can affect the landscape or animal behavior. It includes coverage
 * calculations, priority handling, and serialization of patch sources.
 */

#ifndef PATCH_H_
#define PATCH_H_

#include <vector>

#include "App/Model/IBM/Landscape/Map/Patches/Shape/PatchShape.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/PathSource.h"
#include "Misc/EnumClass.h"


/**
 * @class Patch
 * @brief Represents a spatial patch in a landscape with a specific shape and priority.
 *
 * The Patch class stores information about an environmental patch, including its
 * source, geometric shape, priority, and associated filename. It provides methods
 * to check coverage of terrain cells, compare patches by priority, and access the source.
 */
class Patch {
public:
    /**
     * @brief Constructs a Patch from a JSON configuration and a source.
     * @param source Pointer to the PathSource object providing patch data.
     * @param patchConfig JSON configuration for the patch.
     * @param filename Name of the file associated with the patch.
     */
    Patch(PathSource* const source, const nlohmann::json &patchConfig, const std::string &filename);
    
    /**
     * @brief Constructs a Patch from a source, priority, and shape.
     * @param source Pointer to the PathSource object providing patch data.
     * @param priority Priority of the patch.
     * @param shape Pointer to the PatchShape describing its geometry.
     */
    Patch(PathSource* const source, const size_t priority, PatchShape* shape);

    /**
     * @brief Deleted copy constructor (Patch is non-copyable).
     */
    Patch(const Patch&) = delete;

    /**
     * @brief Deleted copy assignment operator (Patch is non-copyable).
     */
    Patch& operator=(const Patch&) = delete;

    /**
     * @brief Destructor.
     */
    virtual ~Patch();

    /**
     * @brief Checks the coverage level of this patch over a cell.
     * @param cellBounds RingModel representing the bounds of the terrain cell.
     * @return Coverage level of the patch over the cell (Null, Partial, Over50Percent, Full).
     */
    virtual Coverage checkCoverage(const RingModel* const cellBounds) const;

    /**
     * @brief Returns a description of the patch.
     * @return Description string.
     */
    const std::string getDescription() const;

    /**
     * @brief Returns the priority of the patch.
     * @return Priority as a size_t.
     */
    size_t getPriority() const;

    /**
     * @brief Returns a const reference to the patch's source.
     * @return Reference to the PathSource object.
     */
    const PathSource& getSource() const;

    /**
     * @brief Transfers ownership of the source pointer.
     * @return Pointer to the PathSource.
     */
    PathSource* moveSource();

    /**
     * @brief Compares two patches by priority.
     * @param other Another patch to compare with.
     * @return True if this patch has lower priority than the other.
     */
    virtual bool operator<(const Patch& other) const;

protected:
    /// @brief Priority of the patch (used for ordering).
    const size_t priority;

    /// @brief Pointer to the PathSource providing patch data.
    PathSource* source;

    /// @brief Pointer to the geometric shape of the patch.
    const PatchShape* const shape;

    /// @brief Filename associated with the patch.
    std::string filename;
};

/**
 * @struct ComparePatch
 * @brief Comparison functor for PatchPriorityQueue.
 *
 * This functor compares two patches by their priority for use in a priority queue.
 */
struct ComparePatch {
    /**
     * @brief Returns true if lhs has lower priority than rhs.
     * @param lhs Left-hand side patch.
     * @param rhs Right-hand side patch.
     * @return True if lhs < rhs.
     */
    bool operator()(const std::unique_ptr<Patch>& lhs, const std::unique_ptr<Patch>& rhs) const {
        return lhs->getPriority() < rhs->getPriority();
    }
};

/**
 * @typedef PatchPriorityQueue
 * @brief Priority queue of Patch objects ordered by ComparePatch.
 */
using PatchPriorityQueue = std::priority_queue<
    std::unique_ptr<Patch>,
    std::vector<std::unique_ptr<Patch>>,
    ComparePatch
>;

#endif /* PATCH_H_ */