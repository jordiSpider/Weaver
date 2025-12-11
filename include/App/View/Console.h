/**
 * @file Console.h
 * @brief Defines the Console class for Weaver ecosystem simulator.
 *
 * This file contains the declaration of the Console class, which implements a
 * console-based interface for the Weaver simulator. The Console class derives
 * from the abstract View class and allows users to:
 * - Select simulation run modes.
 * - Provide input configuration and output folders.
 * - Decide whether to display simulation output.
 * - Start simulations and view logs via standard output.
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_


#include <string>
#include <iostream>

#ifdef __unix__
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>
#endif

#include "Misc/GlobalVariable.h"
#include "App/View/View.h"
#include "Misc/EnumClass.h"


/**
 * @class Console
 * @brief Console-based implementation of the View interface.
 *
 * Allows interaction with the user via standard input/output to select run modes,
 * input/output paths, and whether to display simulation output. Executes simulations
 * using the associated model and prints logs.
 */
class Console : public View
{
public:
    /**
     * @brief Constructs a Console object.
     */
    Console();

    /**
     * @brief Destructor for Console.
     */
    virtual ~Console();

    /**
     * @brief Runs the simulation in console mode.
     *
     * If parameters are empty, the user is prompted to provide them.
     * @param runMode Name of the run mode.
     * @param inputConfig Path to the input configuration directory.
     * @param outputFolder Path to the output directory.
     * @param silent True to suppress log output; false to display logs.
     */
    void run(const std::string& runMode, const std::string& inputConfig, const std::string& outputFolder, const bool silent);

    /**
     * @brief Prints a message to the console log if not in silent mode.
     * @param message Message text.
     * @param ignoreSilentMode If true, the message will be logged even in silent mode.
     */
    void updateLog(const std::string& message, bool ignoreSilentMode = false);

    /**
     * @brief Prints an error message to the console log if not in silent mode.
     * @param message Error message text.
     * @param ignoreSilentMode If true, the message will be logged even in silent mode.
     */
    void updateLogError(const std::string& message, bool ignoreSilentMode = false);

protected:
    bool silentMode; /**< Flag indicating if the console is in silent mode */

    /**
     * @brief Returns whether the console is in silent mode.
     * @return True if silent mode is active.
     */
    bool isSilentMode() const;

    /**
     * @brief Prompts the user to select a RunMode.
     * @return Selected RunMode.
     */
    RunMode selectRunMode() const;

    /**
     * @brief Checks if a folder path exists and is a directory.
     * @param folderPath Path to validate.
     * @return True if the path exists and is a directory.
     */
    bool isValidFolderPath(const std::filesystem::path& folderPath) const;

    /**
     * @brief Prompts the user to specify whether to display output.
     * @return True if output should be displayed.
     */
    bool requestShowOutput() const;

    /**
     * @brief Prompts the user to specify the input configuration folder.
     * @return Valid input configuration path.
     */
    std::filesystem::path requestInputConfig() const;

    /**
     * @brief Prompts the user to specify the output folder.
     * @return Valid output folder path.
     */
    std::filesystem::path requestOutputFolder() const;
};

#endif // CONSOLE_H_