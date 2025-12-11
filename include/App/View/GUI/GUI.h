/**
 * @file GUI.h
 * @brief Defines the GUI class for Weaver ecosystem simulator.
 *
 * This file contains the declaration of the GUI class, which implements a full
 * graphical user interface using the Nana library. The GUI allows users to:
 * - Select simulation run modes.
 * - Choose input configuration and output folders.
 * - Show or hide simulation output.
 * - Start simulations and view logs in real-time.
 *
 * The GUI class derives from the abstract View class and provides implementations
 * for logging methods.
 */

#ifndef GUI_H_
#define GUI_H_


#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/timer.hpp>
#include <thread>
#include <iostream>
#include <mutex>
#include <queue>

#include "App/View/View.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"
#include "App/View/GUI/Widgets/PathChooser.h"
#include "App/View/GUI/Widgets/OptionSelector.h"
#include "App/View/GUI/Widgets/Checkbox.h"
#include "App/View/GUI/Widgets/LogTextbox.h"



/**
 * @class GUI
 * @brief Graphical User Interface class derived from View.
 *
 * Implements a GUI using the Nana library. Allows selection of run mode, input/output paths,
 * toggling simulation output visibility, starting simulations, and displaying logs.
 */
class GUI : public View {
public:
    /**
     * @brief Constructs the GUI object and initializes all widgets.
     */
    GUI();

    /**
     * @brief Destructor for GUI.
     */
    virtual ~GUI();

    // Disable copy constructor and assignment
    GUI(const GUI& other) = delete;
	GUI& operator=(const GUI& other) = delete;

    /**
     * @brief Runs the GUI main loop and optionally selects a run mode.
     * @param runMode Name of the run mode to pre-select.
     */
    void run(const std::string& runMode);

    /**
     * @brief Appends a message to the log.
     * @param message Message text.
     * @param ignoreSilentMode If true, the message will be logged even in silent mode.
     */
    void updateLog(const std::string& message, bool ignoreSilentMode = false);

    /**
     * @brief Appends an error message to the log.
     * @param message Error message text.
     * @param ignoreSilentMode If true, the message will be logged even in silent mode.
     */
    void updateLogError(const std::string& message, bool ignoreSilentMode = false);

protected:
    nana::form fm;               /**< Main form window */
    nana::place layout;           /**< Layout manager for arranging widgets */

    OptionSelector runModeSelector;        /**< Dropdown for selecting run mode */
    PathChooser inputConfigPathChooser;    /**< Widget to select input config path */
    PathChooser outputFolderPathChooser;   /**< Widget to select output folder path */
    Checkbox showOutputCheckbox;           /**< Checkbox to show/hide simulation output */
    nana::button startSimulationButton;    /**< Button to start the simulation */
    LogTextbox log;                        /**< Textbox displaying logs */

    std::mutex logMutex;                    /**< Mutex for thread-safe log access */
    std::queue<std::string> pendingLogs;    /**< Queue of pending log messages */
    nana::timer logUpdateTimer;             /**< Timer to periodically update log display */

    /**
     * @brief Starts the simulation in a separate thread.
     */
    void startSimulation();
};

#endif // GUI_H_