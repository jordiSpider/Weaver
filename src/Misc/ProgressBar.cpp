#include "Misc/ProgressBar.h"


using namespace std;


/**
 * @brief ProgressBar constructor.
 *
 * Initializes progress thresholds and prints the initial empty bar:
 * 
 * Example output:
 * ```
 * 0%|                                                  |100%
 *    #####################
 * ```
 *
 * The configuration automatically adjusts for cases where `maxCounter` is smaller
 * or larger than the display width (50 characters).
 *
 * @param view Pointer to a View instance used for rendering the bar.
 * @param maxCounter The total number of iterations to complete.
 */
ProgressBar::ProgressBar(View* view, const size_t maxCounter)
    : maxCounter(maxCounter), view(view), nextStep(1u), counter(0u)
{
	if(maxCounter < width)
	{
		// Small number of iterations: each iteration fills multiple characters.
		maxSteps = maxCounter;
		stepProgress = width / maxCounter;
		iterationsPerStep = 1u;
	}
	else
	{
		// Large number of iterations: each character represents several iterations.
		maxSteps = width;
		stepProgress = 1u;
		iterationsPerStep = maxCounter / width;
	}

	thresholdToNextStep = iterationsPerStep;

	// Initialize the bar in the output view.
	view->updateLog("0%|" + string(width, empty) + "|100%\n   ");
}

/**
 * @brief Advances the counter by one unit.
 *
 * Each call increments the progress counter. When the accumulated progress exceeds
 * the next step threshold, new filled characters are printed to the output view.
 *
 * The function prints only the necessary characters instead of redrawing the full
 * bar for efficiency.
 *
 * @note This function is designed for high-frequency updates, such as loops over
 * vector elements, where `maxCounter` corresponds to the vector’s size.
 */
void ProgressBar::update() {
	#ifdef DEBUG
	if(counter > maxCounter)
	{
		throwLineInfoException("Error: ProgressBar overflow.");
	}
	#endif

	counter++;

	string progress = "";

	// Fill segments as thresholds are reached.
	while(counter >= thresholdToNextStep)
	{
		nextStep++;
		progress += string(stepProgress, fill);

		// When reaching the end, complete the bar and add a newline.
		if(nextStep > maxSteps)
		{
			if(maxCounter < width)
			{
				progress += string(width - (stepProgress * maxSteps), fill);
			}

			progress += "\n";
		}

		// Update the threshold for the next step.
		if(nextStep == maxSteps)
		{
			thresholdToNextStep = maxCounter;
		}
		else
		{
			thresholdToNextStep += iterationsPerStep;
		}
	}

	// Output only when progress has advanced.
	if(!progress.empty())
	{
		view->updateLog(progress);
	}
}
