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
 * textual visualization layer represented by the `View` class.
 */

#ifndef MODEL_H_
#define MODEL_H_


#include <string>
#include <filesystem>

#include "Misc/CustomIndexedVector.h"
#include "App/View/View.h"
#include "App/View/RunMode.h"
#include "Exceptions/LineInfoException.h"


/**
 * @class Model
 * @brief Abstract base class for all simulation models in the Weaver framework.
 *
 * The `Model` class establishes the interface that all derived simulation models must follow.
 * It encapsulates the essential lifecycle methods (`run`), provides a static factory method for
 * model creation, and maintains a reference to a `View` object for rendering or monitoring.
 *
 * The derived classes (e.g., `IBM`) implement specific simulation logic while conforming to
 * this standardized structure.
 *
 * ### Responsibilities
 * - Define the contract for simulation execution (`run`).
 * - Manage interaction with the visualization layer (`View`).
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
     * Creates and returns an instance of a specific model subclass
     * based on the given `Type`. The instance is initialized with
     * a reference to the `View` object.
     *
     * @param modelType The type of model to create (e.g., `Type::IBM`).
     * @param view Reference to the visualization or controller view.
     * @return Pointer to the newly created model instance.
     *
     * @throw LineInfoException If the provided model type is not supported.
     */
    static Model* createInstance(Type modelType, View& view);


    /**
     * @brief Constructs a new Model object.
     *
     * Initializes the reference to the associated `View`. This constructor
     * is called by derived classes such as `IBM`.
     *
     * @param view Reference to the view associated with the model.
     */
    Model(View& view);

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
     * The `Model` class is non-copyable to avoid unintended duplication
     * of simulation resources and view references.
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

protected:
    View& view; /**< Reference to the associated View object for visualization or logging. */
};

#endif // MODEL_H_