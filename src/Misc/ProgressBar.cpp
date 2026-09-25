#include "Misc/ProgressBar.h"

#include "App/Manager/LogManager.h"


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
 * @param maxCounter The total number of iterations to complete.
 */
ProgressBar::ProgressBar(const size_t maxCounter)
    : maxCounter(maxCounter), nextStep(1u), counter(0u)
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
	LogManager::emit("0%|" + std::string(width, empty) + "|100%\n");
}

void ProgressBar::update(size_t increment) {
	#ifdef DEBUG
	if(counter > maxCounter)
	{
		throwLineInfoException("Error: ProgressBar overflow.");
	}
	#endif

	counter += increment;

	// Si el contador superó el máximo por redondeos o lotes, lo capamos al tope
	if (counter > maxCounter) {
		counter = maxCounter;
	}

	size_t stepsToPrint = 0;

	// Calcular cuántos pasos visuales se han superado en esta actualización
	while(counter >= thresholdToNextStep && nextStep <= maxSteps)
	{
		// Cada paso completado equivale a 'stepProgress' caracteres en la barra
		stepsToPrint += stepProgress;
		nextStep++;

		// Actualizar el umbral para el siguiente paso
		if (nextStep > maxSteps)
		{
			thresholdToNextStep = maxCounter;
		}
		else
		{
			thresholdToNextStep += iterationsPerStep;
		}
	}

	// Emitir salida solo si la barra realmente avanzó visualmente
	if (stepsToPrint > 0)
	{
		// Construimos la porción rellena basándonos en los pasos completados hasta ahora
		// 'nextStep - 1' nos da la cantidad de pasos totales completados con éxito
		size_t currentFillWidth = (nextStep - 1) * stepProgress;

		// Si ya llegamos al final del proceso por completo, forzamos el rellenado al ancho máximo
		if (counter >= maxCounter) {
			currentFillWidth = width;
		}

		// Asegurar de manera segura que nunca desborde el ancho visual configurado
		if (currentFillWidth > width) {
			currentFillWidth = width;
		}

		std::string progress(currentFillWidth, fill);
		LogManager::emit("0%|" + progress + std::string(width - currentFillWidth, empty) + "|100%\n");
	}
}
