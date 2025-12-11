/**
 * @file ProgressBar.h
 * @brief Lightweight textual progress bar utility for console or View output.
 *
 * This file defines the ProgressBar class, which displays a textual progress bar
 * that updates as a process advances. It is intended for tracking iterations over
 * a sequence or tasks in console-based or custom View-based environments.
 */

#ifndef PROGRESS_BAR_H_
#define PROGRESS_BAR_H_

#include <iostream>
#include <string>
#include <cmath>

#include "App/View/View.h"


/**
 * @class ProgressBar
 * @brief Displays a textual progress bar that updates as a process advances.
 *
 * The progress bar can be used to monitor operations iterating over a vector
 * or any process with a known number of steps. The output is sent through
 * a View interface, allowing flexible redirection to GUI, log file, or console.
 */
class ProgressBar {
public:
    /**
     * @brief Constructs a new ProgressBar instance.
     *
     * Initializes internal counters and determines step sizes for the visual bar.
     *
     * @param view Pointer to a View object for outputting progress.
     * @param maxCounter Total number of iterations or maximum progress count.
     */
    explicit ProgressBar(View* view, const size_t maxCounter);

    /// Deleted copy constructor.
    ProgressBar(const ProgressBar&) = delete;

    /// Deleted copy assignment operator.
    ProgressBar& operator=(const ProgressBar&) = delete;

    /**
     * @brief Advances the progress bar by one iteration.
     *
     * Increments the internal counter and updates the visual display if a
     * new threshold is reached. When the maximum is reached, the bar completes
     * and prints a newline.
     *
     * @throws LineInfoException (in DEBUG mode) if progress exceeds maxCounter.
     */
    void update();

private:
    static constexpr char fill = '#';   /**< Character representing completed progress */
    static constexpr char empty = ' ';  /**< Character representing remaining progress */
    static constexpr unsigned char width = 50; /**< Total visual width of the bar */

    const size_t maxCounter; /**< Maximum iteration count representing 100% progress */
    View* view;              /**< Output interface for printing the progress bar */
    size_t maxSteps;         /**< Maximum number of visual steps (<= width) */
    size_t stepProgress;     /**< Number of characters filled per progress step */
    size_t iterationsPerStep; /**< Number of iterations represented by one step */
    size_t nextStep;         /**< Number of completed steps at the next threshold */
    size_t counter;          /**< Current iteration count */
    size_t thresholdToNextStep; /**< Iteration count at which next visual step occurs */
};

#endif // PROGRESS_BAR_H_
