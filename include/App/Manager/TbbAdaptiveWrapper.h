#ifndef TBB_ADAPTIVE_WRAPPER_H_
#define TBB_ADAPTIVE_WRAPPER_H_

#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/blocked_range.h>
#include <oneapi/tbb/task_arena.h>
#include <iterator>

class TbbAdaptiveWrapper
{
public:
    /**
     * @brief Ejecuta una operación en paralelo o secuencial basándose en el crossover por hilo.
     * @param crossoverPerThread El valor obtenido del benchmark (ej. 100.25 animales por hilo)
     */
    template <typename Container, typename SeqOp, typename ParOp>
    static void Execute(Container& container, size_t startIndex, size_t endIndex, double crossoverPerThread, SeqOp seqOp, ParOp parOp)
    {
        // Salvaguarda: si los índices son inválidos o el rango es vacío, terminar inmediatamente
        if (startIndex >= endIndex || startIndex >= container.size()) {
            return;
        }

        // Ajustar el extremo final en caso de desbordamiento respecto al tamaño real del contenedor
        if (endIndex > container.size()) {
            endIndex = container.size();
        }

        const size_t totalElements = endIndex - startIndex;

        // 1. Obtener de forma dinámica las hebras máximas asignadas al contexto de TBB
        const size_t activeThreads = static_cast<size_t>(tbb::this_task_arena::max_concurrency());

        // 2. Calcular el umbral dinámico adaptado al hardware
        const size_t dynamicCrossoverThreshold = static_cast<size_t>(activeThreads * crossoverPerThread);

        // 3. Toma de decisión de baja latencia
        if (totalElements >= dynamicCrossoverThreshold && activeThreads > 1)
        {
            // La carga de trabajo justifica el paralelismo
            parOp(container, startIndex, endIndex);
        }
        else
        {
            // Ejecución secuencial óptima para evitar overhead
            seqOp(container, startIndex, endIndex);
        }
    }
};

#endif // TBB_ADAPTIVE_WRAPPER_H_
