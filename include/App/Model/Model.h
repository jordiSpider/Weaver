/**
 * @file Model.h
 * @brief Definition of the abstract base class for all simulation models in Weaver.
 *
 * The `Model` class defines the common interface for all simulation models used
 * within the Weaver ecosystem framework. It provides a base for specialized models
 * such as individual-based (IBM) simulations and offers a factory method for dynamic
 * model instantiation.
 *
 * This class acts as the bridge between the simulation logic and the graphical or
 * textual visualization layer.
 */

#ifndef MODEL_H_
#define MODEL_H_


#include <string>
#include <filesystem>

#include "Misc/CustomIndexedVector.h"
#include "App/Model/RunMode.h"
#include "Exceptions/LineInfoException.h"


/**
 * @class Model
 * @brief Abstract base class for all simulation models in the Weaver framework.
 *
 * The `Model` class establishes the interface that all derived simulation models must follow.
 * It encapsulates the essential lifecycle methods (`run`) and provides a static factory method for
 * model creation.
 *
 * The derived classes (e.g., `IBM`) implement specific simulation logic while conforming to
 * this standardized structure.
 *
 * ### Responsibilities
 * - Define the contract for simulation execution (`run`).
 * - Provide a common base for polymorphic use across different model types.
 */
class Model
{
public:
    /**
     * @brief Enumeration of supported model types.
     *
     * Defines all the simulation models available within the framework.
     * Currently, only the `IBM` (Individual-Based Model) type is implemented.
     */
    enum class Type {
        IBM  /**< Individual-Based Model */
    };


    /**
     * @brief Factory method for creating a model instance.
     *
     * @param modelType The type of model to create (e.g., `Type::IBM`).
     * @return Pointer to the newly created model instance.
     *
     * @throw LineInfoException If the provided model type is not supported.
     */
    static Model* createInstance(Type modelType);


    /**
     * @brief Constructs a new Model object.
     */
    Model();

    /**
     * @brief Virtual destructor for safe polymorphic cleanup.
     *
     * Ensures that derived classes are properly destroyed
     * when deleted through a pointer to `Model`.
     */
    virtual ~Model();

    /**
     * @brief Deleted copy constructor.
     *
     * @param other Another instance of Model (unused).
     */
    Model(const Model& other) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * Prevents assignment between `Model` instances to ensure resource integrity.
     *
     * @param other Another instance of Model (unused).
     * @return Reference to the current object (deleted).
     */
    Model& operator=(const Model& other) = delete;

    /**
     * @brief Runs the simulation with the specified parameters.
     *
     * This pure virtual function defines the main simulation routine that must
     * be implemented by derived classes (e.g., `IBM`). It handles reading
     * configuration files, setting up the simulation, and producing outputs.
     *
     * @param runMode Execution mode of the simulation (e.g., full run, resume, benchmark).
     * @param inputConfigPath Path to the directory or JSON file containing the input configuration.
     * @param outputFolderPath Path to the folder where output files will be generated.
     */
    virtual void run(const RunMode runMode, const std::filesystem::path& inputConfigPath, const std::filesystem::path& outputFolderPath)=0;
};

#endif // MODEL_H_