/**
 * @file IBM.h
 * @brief Definition of the IBM class, representing the core individual-based model of the simulation.
 *
 * This file defines the IBM (Individual-Based Model) class, which extends the base Model class
 * to simulate ecosystem dynamics at the individual level. The class integrates landscape data,
 * random processes, and configurable parameters to drive the simulation. It supports profiling,
 * checkpointing, and input/output configuration handling.
 */

#ifndef IBM_H_
#define IBM_H_


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#ifdef USE_CPU_PROFILER
	#include "gperftools/profiler.h"
#endif

#ifdef USE_HEAP_PROFILER
	#include "gperftools/heap-profiler.h"
#endif

#include <thread>
#include <chrono>
#include <utility>

#include "Exceptions/LineInfoException.h"
#include "Misc/Utilities.h"
#include "Misc/GlobalVariable.h"
#include "App/Model/IBM/Maths/Random.h"
#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/Model.h"


/**
 * @class IBM
 * @brief Implements an Individual-Based Model (IBM) for ecosystem simulation.
 *
 * The IBM class represents a high-level model that simulates the behavior and
 * interactions of individual entities (e.g., animals, plants, or agents)
 * across a dynamic landscape. It derives from the base `Model` class and
 * serves as the core execution engine of the Weaver simulation framework.
 *
 * This model handles:
 * - Initialization of random number generators.
 * - Configuration loading from JSON or binary checkpoints.
 * - Interaction with the landscape and global parameters.
 * - Execution profiling (CPU and heap) for performance evaluation.
 */
class IBM : public Model
{
public:
	/**
     * @brief Constructs the IBM model and initializes its subsystems.
     *
     * The constructor links the model with a given `View` instance, which
     * may be used for visualization or real-time interaction.
     *
     * @param view Reference to the View object associated with the simulation.
     */
	IBM(View& view);

	/**
     * @brief Destroys the IBM model.
     *
     * Releases resources, handles profiler shutdown if active, and ensures
     * a clean termination of the simulation subsystems.
     */
    virtual ~IBM();

	/**
     * @brief Deleted copy constructor.
     *
     * The IBM model is non-copyable to prevent accidental duplication of
     * resources, threads, and internal simulation states.
     *
     * @param other Another IBM instance (unused).
     */
	IBM(const IBM& other) = delete;

	/**
     * @brief Deleted copy assignment operator.
     *
     * The IBM model cannot be assigned to another instance due to the
     * complexity and non-copyable nature of its internal state.
     *
     * @param other Another IBM instance (unused).
     * @return Reference to the current instance (deleted).
     */
    IBM& operator=(const IBM& other) = delete;

	/**
     * @brief Executes the IBM simulation using the provided configuration and output paths.
     *
     * This method represents the main simulation routine. It loads the input configuration,
     * initializes the landscape and random seeds, and performs the simulation loop
     * according to the specified run mode.
     *
     * Optionally, it can resume from an existing checkpoint and periodically save progress.
     *
     * @param runMode Specifies the execution mode.
     * @param inputConfigPath Path to the input configuration JSON file.
     * @param outputFolderPath Path to the folder where output data and logs will be stored.
     */
	void run(const RunMode runMode, const std::filesystem::path& inputConfigPath, const std::filesystem::path& outputFolderPath);

private:
	/**
     * @brief Checks for an existing checkpoint and returns its path if found.
     *
     * This function verifies whether a checkpoint file exists in the specified folder.
     * It can handle both binary and text-based checkpoint formats.
     *
     * @param checkpointFolderPath Path to the checkpoint directory.
     * @param isBinaryCheckpoint Indicates whether the checkpoint is in binary format.
     * @return A pair containing:
     *   - A boolean flag indicating if a checkpoint exists.
     *   - The path to the existing checkpoint file (if found).
     */
	std::pair<bool,std::filesystem::path> existsCheckpoint(const std::filesystem::path& checkpointFolderPath, const bool isBinaryCheckpoint);
};

#endif // IBM_H_