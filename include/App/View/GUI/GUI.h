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
#include <atomic>
#include <string>
#include <condition_variable>

#include "App/View/View.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"
#include "App/View/GUI/Widgets/PathChooser.h"
#include "App/View/GUI/Widgets/OptionSelector.h"
#include "App/View/GUI/Widgets/Checkbox.h"
#include "App/View/GUI/Widgets/LogTextbox.h"
#include "App/View/GUI/Widgets/Spinbox.h"




class SafeQueue {
private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;

public:
	bool empty() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.empty();
	}

    // Inserta un elemento y notifica a los hilos que esperan
    void push(const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        cv_.notify_one(); // Despierta a un hilo que esté esperando en pop()
    }

    // Saca un elemento. Si la cola está vacía, bloquea el hilo hasta que haya datos.
    std::string pop() {
        std::unique_lock<std::mutex> lock(mutex_);

        // Espera de forma eficiente mientras la cola esté vacía
        cv_.wait(lock, [this]() { return !queue_.empty(); });

        std::string value = queue_.front();
        queue_.pop();
        return value;
    }

    // Versión no bloqueante opcional: devuelve falso si está vacía
    bool try_pop(std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        value = queue_.front();
        queue_.pop();
        return true;
    }
};



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
    GUI(Model* newModel);

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

protected:
    nana::form fm;               /**< Main form window */
    nana::place layout;           /**< Layout manager for arranging widgets */

    OptionSelector runModeSelector;        /**< Dropdown for selecting run mode */
    PathChooser inputConfigPathChooser;    /**< Widget to select input config path */
    PathChooser outputFolderPathChooser;   /**< Widget to select output folder path */
    Checkbox verboseCheckbox;              /**< Checkbox to verbose simulation output */
    Checkbox enableDiskOutputMockCheckbox; /**< Checkbox to enable disk output mock */
	Spinbox threadsSpinbox;                /**< Spinbox to select number of threads */
    nana::button startSimulationButton;    /**< Button to start the simulation */
    LogTextbox log;                        /**< Textbox displaying logs */

    SafeQueue pendingLogs;                 /**< Thread-safe queue of pending log messages */
    nana::timer logUpdateTimer;            /**< Timer to periodically update log display */
    std::atomic<bool> simulationRunning{ false };

    /**
     * @brief Starts the simulation in a separate thread.
     */
    void startSimulation();
};

#endif // GUI_H_