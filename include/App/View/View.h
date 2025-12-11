/**
 * @file View.h
 * @brief Abstract base class for a view in the MVC pattern, handling logging and RunMode titles.
 *
 * This file defines the `View` class, which provides the interface for
 * views in the Weaver simulation framework. It includes methods for logging
 * messages, both normal and error messages, and maintains a mapping of
 * `RunMode` enum values to displayable titles.
 */

#ifndef VIEW_H_
#define VIEW_H_


#include <string>
#include <initializer_list>
#include <chrono>
#include <fstream>
#include <sstream>  
#include <iomanip> 
#include <ctime> 


#include "App/View/RunMode.h"
#include "Misc/EnumClass.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/PathUtils.h"



class Model;


/**
 * @class View
 * @brief Abstract base class representing a view in the MVC pattern.
 *
 * The `View` class is responsible for managing logs and providing
 * human-readable titles for simulation `RunMode`s. Derived classes
 * must implement methods to handle normal and error log messages.
 *
 * ### Responsibilities
 * - Maintain a reference to the associated `Model`.
 * - Provide mapping from `RunMode` enum values to string titles.
 * - Provide logging interface for normal and error messages.
 */
class View
{
public:
    /**
     * @brief Constructs a View object.
     *
     * Initializes internal data structures, including the run modes titles vector.
     */
    View();

    /**
     * @brief Destructor for View.
     *
     * Ensures proper cleanup of any resources in derived classes.
     */
    virtual ~View();

    /**
     * @brief Sets the model associated with this view.
     *
     * @param newModel Reference to the Model object to associate.
     */
    void setModel(Model& newModel);

    /**
     * @brief Returns a mapping of RunMode enum values to their display titles.
     *
     * @return Const reference to a CustomIndexedVector containing RunMode titles.
     */
    const CustomIndexedVector<RunMode, std::string>& getRunModesTitles() const;

    /**
     * @brief Updates the log with multiple messages.
     *
     * @param messages List of messages to append to the log.
     * @param ignoreSilentMode If true, the message will be logged even in silent mode.
     */
    void updateLog(std::initializer_list<std::string> messages, bool ignoreSilentMode = false);

    /**
     * @brief Updates the log with multiple error messages.
     *
     * @param messages List of error messages to append to the log.
     * @param ignoreSilentMode If true, the message will be logged even in silent mode.
     */
    void updateLogError(std::initializer_list<std::string> messages, bool ignoreSilentMode = false);

    /**
     * @brief Updates the log with a single message.
     *
     * This is a pure virtual function that must be implemented by derived classes.
     *
     * @param message Message to append to the log.
     * @param ignoreSilentMode If true, the message will be logged even in silent mode.
     */
    virtual void updateLog(const std::string& message, bool ignoreSilentMode = false)=0;

    /**
     * @brief Updates the log with a single error message.
     *
     * This is a pure virtual function that must be implemented by derived classes.
     *
     * @param message Error message to append to the log.
     * @param ignoreSilentMode If true, the message will be logged even in silent mode.
     */
    virtual void updateLogError(const std::string& message, bool ignoreSilentMode = false)=0;

protected:
    static const std::filesystem::path DEFAULT_OUTPUT_FOLDER; /**< Default output folder path */

    Model* model; /**< Pointer to the associated model */
    CustomIndexedVector<RunMode, std::string> runModesTitles; /**< Titles for each RunMode */
};

#endif // VIEW_H_