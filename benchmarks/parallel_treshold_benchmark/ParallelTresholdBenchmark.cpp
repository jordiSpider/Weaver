
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <map>

#if defined(_MSC_VER)
#include <intrin.h> // <- CRÍTICO PARA MODO RELEASE EN WINDOWS (Intrínsecos de CPU)
#endif

#include <oneapi/tbb/blocked_range.h>
#include <oneapi/tbb/parallel_for.h>

#include <nlohmann/json.hpp>

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Manager/TbbManager.h"
#include "App/Manager/LogManager.h"

#include "schema/landscape_params_schema_json.h"

namespace
{
	struct LandscapeBenchmarkContext
	{
		std::unique_ptr<Landscape> landscape;
	};

	LandscapeBenchmarkContext createContext(const std::filesystem::path& configSubPath, std::size_t threads)
	{
		StorageBridge::setDiskOutputMockEnabled(true);
		LogManager::setHandler([](const LogManager::LogMessage&) {});

		
		TbbManager::configure_pool(threads, false);
		TbbManager::initialize_and_name_pool();


		const std::filesystem::path benchmarkConfigDir = BENCHMARK_CONFIG_DIR;

		const std::filesystem::path configPath = benchmarkConfigDir / configSubPath;

		JsonValidator landscapeValidator(EmbeddedResources::landscape_params_schema_json, "landscape_params_schema");

		nlohmann::json landscapeConfig = readConfigFile(configPath / "landscape_params.json", landscapeValidator);

		const std::filesystem::path outputPath = std::filesystem::temp_directory_path() / "parallel_treshold_benchmark_output";

		std::filesystem::create_directories(outputPath);

		LandscapeBenchmarkContext ctx;
		ctx.landscape.reset(Landscape::createInstance(landscapeConfig["landscape"]["simulationType"]));
		ctx.landscape->init(configPath, outputPath, false);
		return ctx;
	}

	class CrossoverOptimizer
	{
	public:
		// Diseñado para aceptar lambdas, funciones miembro o functors con firmas genéricas
		template <bool UseAnimals, typename SeqOp, typename ParOp>
		static void Execute(const std::string& benchmarkName, SeqOp seqOp, ParOp parOp, const std::filesystem::path& configSubPath)
		{
			auto ctx = createContext(configSubPath, THREADS);

			int low = MIN_SIZE;

			int high;
			if constexpr (UseAnimals)
			{
				high = std::min(MAX_SIZE, static_cast<int>(ctx.landscape->getLandscapeAnimals().size()));
			}
			else
			{
				high = std::min(MAX_SIZE, static_cast<int>(ctx.landscape->getLandscapeTerrainCells().size()));
			}

			int exactCrossover = -1;
			double optimalSeqTime = 0.0;
			double optimalParTime = 0.0;

			std::cout << "\n=======================================================\n";
			std::cout << " OPTIMIZADOR DE CRUCE: " << benchmarkName << "\n";
			std::cout << "=======================================================\n";
			std::cout << "Buscando tamaño crítico de población analítica...\n\n";

			// Algoritmo de búsqueda binaria O(log N)
			while (low <= high)
			{
				int mid = low + (high - low) / 2;

				// Evaluamos de forma aislada y estadística ambos paradigmas para el tamaño actual (mid)
				double seqMedian = measureRobustMedian<UseAnimals>(mid, seqOp, configSubPath, 5);
				double parMedian = measureRobustMedian<UseAnimals>(mid, parOp, configSubPath, 5);

				std::cout << "[N=" << std::setw(5) << mid << "] "
					<< "Secuencial: " << std::setw(7) << static_cast<int>(seqMedian) << " ns | "
					<< "Paralelo: " << std::setw(7) << static_cast<int>(parMedian) << " ns -> ";

				// CRITERIO ROBUSTO: El paralelo debe ganar por más del 5% de forma sostenida en la mediana
				if (parMedian < seqMedian && (seqMedian / parMedian) >= 1.05)
				{
					std::cout << "GANA TBB (Buscando umbral menor...)\n";
					exactCrossover = mid;
					optimalSeqTime = seqMedian;
					optimalParTime = parMedian;
					high = mid - 1; // Intentamos encontrar un punto de cruce aún más bajo
				}
				else
				{
					std::cout << "GANA SECUENCIAL (Buscando umbral mayor...)\n";
					low = mid + 1; // Necesitamos más carga para compensar el coste de los hilos
				}
			}

			PrintReport(exactCrossover, optimalSeqTime, optimalParTime);
		}

	private:
		static constexpr int MIN_SIZE = 1;
		static constexpr int MAX_SIZE = 2048;

		static constexpr std::size_t THREADS = 4;


		// Método crítico: garantiza el aislamiento clonando el contexto por cada pasada de medición
		template <bool UseAnimals, typename Op>
		static double measureRobustMedian(int populationSize, Op op, const std::filesystem::path& configSubPath, int runs = 10)
		{
			std::vector<double> samples;
			samples.reserve(runs);

			for (int r = 0; r < runs; ++r)
			{
				auto ctx = createContext(configSubPath, THREADS);

				std::vector<AnimalNonStatistical*>& animals = ctx.landscape->getLandscapeAnimals();
				std::vector<TerrainCell*>& cells = ctx.landscape->getLandscapeTerrainCells();

				// 2. MEDICIÓN QUIRÚRGICA DE LA OPERACIÓN SOBRE EL CONTEXTO AISLADO
				auto start = std::chrono::high_resolution_clock::now();

				if constexpr (UseAnimals)
				{
					op(*ctx.landscape, animals.begin(), animals.begin() + populationSize);
				}
				else
				{
					op(*ctx.landscape, cells.begin(), cells.begin() + populationSize);
				}

				auto end = std::chrono::high_resolution_clock::now();

				//----------------===================================================
				// PROTECCIÓN EN MODO RELEASE CONTRA CÓDIGO MUERTO (X64 COMPATIBLE)
				//----------------===================================================
#if defined(_MSC_VER)
				_ReadWriteBarrier(); // Fuerza a MSVC a volcar la memoria y ejecutar el bucle obligatoriamente
				volatile const void* memoryGuard = animals.data();
				(void)memoryGuard;
#else
				asm volatile("" : : "g"(animals.data()) : "memory");
#endif
				//----------------===================================================

				double durationNs = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
				samples.push_back(durationNs);
			}

			// 3. MÉTRICA DE LA MEDIANA ROBUSTA: Ordenamos las muestras y tomamos el elemento central
			// Esto descarta de forma matemática los picos causados por cambios de contexto del OS en hilos TBB
			std::sort(samples.begin(), samples.end());
			return samples[runs / 2];
		}

		static void PrintReport(int crossover, double seqTime, double parTime)
		{
			std::cout << "=======================================================\n";
			if (crossover != -1)
			{
				int crossoverPerThread = static_cast<int>(std::ceil(static_cast<double>(crossover) / static_cast<double>(THREADS)));

				std::cout << " [ANÁLISIS FINALIZADO] PUNTO DE CRUCE UNIVERSAL\n";
				std::cout << "=======================================================\n";
				std::cout << "-> Umbral óptimo (para " << THREADS << " hilos): " << crossover << " animales.\n";
				std::cout << "-> METRICA UNIVERSAL (Crossover por hilo): " << crossoverPerThread << " animales/hilo.\n";
				std::cout << "-> Mediana de Tiempo Secuencial: " << seqTime << " ns\n";
				std::cout << "-> Mediana de Tiempo Paralelo:   " << parTime << " ns\n";
				std::cout << "-> Incremento de Eficiencia: " << ((seqTime - parTime) / seqTime) * 100.0 << "%\n";
				std::cout << "\n* GUARDAR ESTE VALOR: " << crossoverPerThread << "\n";
			}
			else
			{
				std::cout << " [ANÁLISIS FINALIZADO]: El enfoque secuencial domina el espectro.\n";
			}
			std::cout << "=======================================================\n\n";
		}
	};

	// =========================================================================
	// DEFINICIÓN DE BLOQUES DE CÓDIGO (Tus casos de prueba reutilizables)
	// =========================================================================

	auto UpdateTimeStepsWithoutFood_Seq = [](Landscape&, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		for (auto it = begin; it != end; ++it) { (*it)->updateTimeStepsWithoutFood(); }
	};

	auto UpdateTimeStepsWithoutFood_Par = [](Landscape&, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		TbbManager::simulation_arena->execute([&]() {
			tbb::parallel_for(tbb::blocked_range<size_t>(0, static_cast<size_t>(std::distance(begin, end))),
				[&](const tbb::blocked_range<size_t>& r) {
					for (size_t i = r.begin(); i < r.end(); ++i) {
						(*(begin + i))->updateTimeStepsWithoutFood();
					}
				}
			);
		});
	};


	auto PrintAnimalsAlongCells_Seq = [](Landscape&, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		constexpr size_t estimatedAnimalLineSize = 2000u;

		size_t estimatedSize = static_cast<std::size_t>(std::distance(begin, end)) * estimatedAnimalLineSize;

		fmt::memory_buffer localStr;
		localStr.reserve(estimatedSize);

		for (auto it = begin; it != end; ++it) { (*it)->formatToBufferDirect(localStr); }
	};

	auto PrintAnimalsAlongCells_Par = [](Landscape&, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		TbbManager::simulation_arena->execute([&]() {
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());
			constexpr size_t estimatedAnimalLineSize = 2000u;
			const std::size_t n = static_cast<std::size_t>(std::distance(begin, end));

			size_t estimatedPerThreadSize = (n * estimatedAnimalLineSize) / maxThreadsInArena;

			tbb::enumerable_thread_specific<fmt::memory_buffer> localBuffersAnimals;

			tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
				[&](const tbb::blocked_range<size_t>& r) {
					fmt::memory_buffer& localStr = localBuffersAnimals.local();
					localStr.reserve(estimatedPerThreadSize);

					for (size_t i = r.begin(); i < r.end(); ++i) {
						(*(begin + i))->formatToBufferDirect(localStr);
					}
				}
			);
		});
	};


	auto PrintCellAlongCells_Seq = [](Landscape& landscape, std::vector<TerrainCell*>::iterator begin, std::vector<TerrainCell*>::iterator end) {
		constexpr size_t estimatedCellLineSize = 256u;
		const size_t numAnimalSpecies = landscape.getExistingAnimalSpecies().size();

		size_t estimatedSize = static_cast<std::size_t>(std::distance(begin, end)) * estimatedCellLineSize;

		fmt::memory_buffer localStr;
		localStr.reserve(estimatedSize);

		for (auto it = begin; it != end; ++it) { 
			const auto& cell = (*it);

			const PointMap& position = cell->getPosition();

			fmt::format_to(fmt::appender(localStr), "{}", position.get(magic_enum::enum_cast<Axis>(0).value()));

			for (unsigned int axis = 1; axis < DIMENSIONS; axis++)
			{
				fmt::format_to(fmt::appender(localStr), "\t{}", position.get(magic_enum::enum_cast<Axis>(axis).value()));
			}


			for (size_t j = 0; j < cell->getPatchApplicator().getNumberOfResources(); j++)
			{
				const CellResourceInterface& resource = cell->getPatchApplicator().getCellResource(j);

				fmt::format_to(fmt::appender(localStr), "\t{}\t{}",
					resource.getGrowthBuildingBlock().getCurrentTotalWetMass().getValue().getValue(),
					resource.calculateDryMassAvailable(true, nullptr, 0.0).getValue().getValue()
				);
			}


			std::vector<uint32_t> animalCounts(numAnimalSpecies, 0u);

			for (const auto* animal : landscape.getLandscapeAnimals()) {
				if (cell->isAnimalInside(animal->getPosition())) {
					animalCounts[animal->getAnimalSpeciesId()]++;
				}
			}

			for (uint32_t count : animalCounts)
			{
				fmt::format_to(fmt::appender(localStr), "\t{}", count);
			}


			localStr.push_back('\n');
		}
		};

	auto PrintCellAlongCells_Par = [](Landscape& landscape, std::vector<TerrainCell*>::iterator begin, std::vector<TerrainCell*>::iterator end) {
		TbbManager::simulation_arena->execute([&]() {
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());
			const std::size_t n = static_cast<std::size_t>(std::distance(begin, end));
			constexpr size_t estimatedCellLineSize = 256u;
			const size_t numAnimalSpecies = landscape.getExistingAnimalSpecies().size();

			size_t estimatedPerThreadSize = (n * estimatedCellLineSize) / maxThreadsInArena;

			tbb::enumerable_thread_specific<fmt::memory_buffer> localBuffersAnimals;

			tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
				[&](const tbb::blocked_range<size_t>& r) {
					fmt::memory_buffer& localStr = localBuffersAnimals.local();
					localStr.reserve(estimatedPerThreadSize);

					for (size_t i = r.begin(); i < r.end(); ++i) {
						const auto& cell = (*(begin + i));

						const PointMap& position = cell->getPosition();

						fmt::format_to(fmt::appender(localStr), "{}", position.get(magic_enum::enum_cast<Axis>(0).value()));

						for (unsigned int axis = 1; axis < DIMENSIONS; axis++)
						{
							fmt::format_to(fmt::appender(localStr), "\t{}", position.get(magic_enum::enum_cast<Axis>(axis).value()));
						}


						for (size_t j = 0; j < cell->getPatchApplicator().getNumberOfResources(); j++)
						{
							const CellResourceInterface& resource = cell->getPatchApplicator().getCellResource(j);

							fmt::format_to(fmt::appender(localStr), "\t{}\t{}",
								resource.getGrowthBuildingBlock().getCurrentTotalWetMass().getValue().getValue(),
								resource.calculateDryMassAvailable(true, nullptr, 0.0).getValue().getValue()
							);
						}


						std::vector<uint32_t> animalCounts(numAnimalSpecies, 0u);

						for (const auto* animal : landscape.getLandscapeAnimals()) {
							if (cell->isAnimalInside(animal->getPosition())) {
								animalCounts[animal->getAnimalSpeciesId()]++;
							}
						}

						for (uint32_t count : animalCounts)
						{
							fmt::format_to(fmt::appender(localStr), "\t{}", count);
						}


						localStr.push_back('\n');
					}
				}
			);
			});
		};


	auto InitControlVariables_Seq = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		for (auto it = begin; it != end; ++it) { (*it)->initControlVariables(landscape.getExistingSpecies()); }
		};

	auto InitControlVariables_Par = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		TbbManager::simulation_arena->execute([&]() {
			tbb::parallel_for(tbb::blocked_range<size_t>(0, static_cast<size_t>(std::distance(begin, end))),
				[&](const tbb::blocked_range<size_t>& r) {
					for (size_t i = r.begin(); i < r.end(); ++i) {
						(*(begin + i))->initControlVariables(landscape.getExistingSpecies());
					}
				}
			);
			});
		};


	auto ActionPlanning_Seq = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		constexpr size_t estimatedLineSize = 256u;

		size_t estimatedSize = static_cast<std::size_t>(std::distance(begin, end)) * estimatedLineSize;

		fmt::memory_buffer localStr;

		if (landscape.getSaveEdibilitiesFile()) {
			localStr.reserve(estimatedSize);
		}


		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> animalSpeciesMaximumPatchEdibilityValueGlobal(landscape.getExistingAnimalSpecies().size());
		
		for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
			animalSpeciesMaximumPatchEdibilityValueGlobal[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
		}

		
		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> animalSpeciesMaximumPatchPredationRiskGlobal(landscape.getExistingAnimalSpecies().size());
		
		for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
			animalSpeciesMaximumPatchPredationRiskGlobal[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
		}
		
		
		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> animalSpeciesMaximumPatchConspecificBiomassGlobal(landscape.getExistingAnimalSpecies().size());

		for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
			animalSpeciesMaximumPatchConspecificBiomassGlobal[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
		}

		
		const bool saveEd = landscape.getSaveEdibilitiesFile();
		const auto& timeStepsPerDay = landscape.getTimeStepsPerDay();

		for (auto it = begin; it != end; ++it) { 
			auto speciesId = (*it)->getAnimalSpeciesId();

			auto& maxEd = animalSpeciesMaximumPatchEdibilityValueGlobal[speciesId];
			auto& maxPred = animalSpeciesMaximumPatchPredationRiskGlobal[speciesId];
			auto& maxCons = animalSpeciesMaximumPatchConspecificBiomassGlobal[speciesId];

			(*it)->actionPlanning(
				&landscape,
				TimeStep(0),
				timeStepsPerDay,
				saveEd,
				localStr,
				maxEd,
				maxPred,
				maxCons
			);
		}
	};

	auto ActionPlanning_Par = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		TbbManager::simulation_arena->execute([&]() {
			const bool saveEd = landscape.getSaveEdibilitiesFile();
			const auto& timeStepsPerDay = landscape.getTimeStepsPerDay();


			tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumPatchEdibilityValueGlobalLocal(
				[&]() {
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(landscape.getExistingAnimalSpecies().size());

					for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
						localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
					}

					return localVector;
				}
			);

			tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumPatchPredationRiskGlobalLocal(
				[&]() {
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(landscape.getExistingAnimalSpecies().size());

					for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
						localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
					}

					return localVector;
				}
			);

			tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumPatchConspecificBiomassGlobalLocal(
				[&]() {
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(landscape.getExistingAnimalSpecies().size());

					for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
						localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
					}

					return localVector;
				}
			);


			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());
			constexpr size_t estimatedLineSize = 256u;
			const std::size_t n = static_cast<std::size_t>(std::distance(begin, end));

			size_t estimatedPerThreadSize = (n * estimatedLineSize) / maxThreadsInArena;

			tbb::enumerable_thread_specific<fmt::memory_buffer> localBuffersEdibilities;


			tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
				[&](const tbb::blocked_range<size_t>& r) {
					fmt::memory_buffer& localStr = localBuffersEdibilities.local();

					if (landscape.getSaveEdibilitiesFile()) {
						localStr.reserve(estimatedPerThreadSize);
					}


					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchEdibilityValueGlobal = maximumPatchEdibilityValueGlobalLocal.local();
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchPredationRiskGlobal = maximumPatchPredationRiskGlobalLocal.local();
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchConspecificBiomassGlobal = maximumPatchConspecificBiomassGlobalLocal.local();


					for (size_t i = r.begin(); i < r.end(); ++i) {
						auto speciesId = (*(begin + i))->getAnimalSpeciesId();

						auto& maxEd = animalSpeciesMaximumPatchEdibilityValueGlobal[speciesId];
						auto& maxPred = animalSpeciesMaximumPatchPredationRiskGlobal[speciesId];
						auto& maxCons = animalSpeciesMaximumPatchConspecificBiomassGlobal[speciesId];

						(*(begin + i))->actionPlanning(
							&landscape,
							TimeStep(0),
							timeStepsPerDay,
							saveEd,
							localStr,
							maxEd,
							maxPred,
							maxCons
						);
					}
				}
			);
			});
		};


	auto PrepareForActionExecution = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end, size_t& predateEnd, size_t& parallelEnd) {
		size_t activeElements = static_cast<std::size_t>(std::distance(begin, end));

		constexpr size_t estimatedLineSize = 256u;

		size_t estimatedSize = activeElements * estimatedLineSize;

		fmt::memory_buffer localStr;

		if (landscape.getSaveEdibilitiesFile()) {
			localStr.reserve(estimatedSize);
		}


		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> animalSpeciesMaximumPatchEdibilityValueGlobal(landscape.getExistingAnimalSpecies().size());

		for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
			animalSpeciesMaximumPatchEdibilityValueGlobal[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
		}


		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> animalSpeciesMaximumPatchPredationRiskGlobal(landscape.getExistingAnimalSpecies().size());

		for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
			animalSpeciesMaximumPatchPredationRiskGlobal[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
		}


		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> animalSpeciesMaximumPatchConspecificBiomassGlobal(landscape.getExistingAnimalSpecies().size());

		for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
			animalSpeciesMaximumPatchConspecificBiomassGlobal[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
		}


		const bool saveEd = landscape.getSaveEdibilitiesFile();
		const auto& timeStepsPerDay = landscape.getTimeStepsPerDay();

		for (auto it = begin; it != end; ++it) {
			auto speciesId = (*it)->getAnimalSpeciesId();

			auto& maxEd = animalSpeciesMaximumPatchEdibilityValueGlobal[speciesId];
			auto& maxPred = animalSpeciesMaximumPatchPredationRiskGlobal[speciesId];
			auto& maxCons = animalSpeciesMaximumPatchConspecificBiomassGlobal[speciesId];

			(*it)->actionPlanning(
				&landscape,
				TimeStep(0),
				timeStepsPerDay,
				saveEd,
				localStr,
				maxEd,
				maxPred,
				maxCons
			);
		}


		for (size_t i = 0; i < animalSpeciesMaximumPatchEdibilityValueGlobal.size(); ++i) {
			landscape.getMutableExistingAnimalSpecies()[i]->getMutableDecisionsBuildingBlock()->updateMaximumPatchEdibilityValueGlobal(animalSpeciesMaximumPatchEdibilityValueGlobal[i]);
		}

		for (size_t i = 0; i < animalSpeciesMaximumPatchPredationRiskGlobal.size(); ++i) {
			landscape.getMutableExistingAnimalSpecies()[i]->getMutableDecisionsBuildingBlock()->updateMaximumPatchPredationRiskGlobal(animalSpeciesMaximumPatchPredationRiskGlobal[i]);
		}

		for (size_t i = 0; i < animalSpeciesMaximumPatchConspecificBiomassGlobal.size(); ++i) {
			landscape.getMutableExistingAnimalSpecies()[i]->getMutableDecisionsBuildingBlock()->updateMaximumPatchConspecificBiomassGlobal(animalSpeciesMaximumPatchConspecificBiomassGlobal[i]);
		}


		predateEnd = 0;
		parallelEnd = activeElements;
		size_t i = 0;

		auto& landscapeAnimals = landscape.getLandscapeAnimals();

		while (i < parallelEnd) {
			auto action = landscapeAnimals[i]->getNextAction();

			if (action == AnimalNonStatistical::Action::PREDATE) {
				if (i != predateEnd) {
					std::swap(landscapeAnimals[i], landscapeAnimals[predateEnd]);
				}
				predateEnd++;
				i++;
			}
			else if (action == AnimalNonStatistical::Action::NONE) {
				// Mandamos los 'NONE' al final de la zona activa
				parallelEnd--;
				std::swap(landscapeAnimals[i], landscapeAnimals[parallelEnd]);
				// No incrementamos 'i' porque el elemento intercambiado desde 'parallelEnd' debe ser evaluado
			}
			else {
				// Es una acción común paralelizable
				i++;
			}
		}


		if (predateEnd > 0) {
			CustomIndexedVector<AnimalSpeciesID, uint64_t> animalSpeciesMaximumPredationEncountersPerDay(landscape.getExistingAnimalSpecies().size(), 0u);

			size_t predateEstimatedPerThreadSize = predateEnd * 256;

			fmt::memory_buffer predationProbabilitiesLocalStr;

			CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Species::ID, unsigned int>> predationEventsOnOtherSpeciesLocal(landscape.getExistingAnimalSpecies().size(), CustomIndexedVector<Species::ID, unsigned int>(landscape.getExistingSpecies().size(), 0u));


			for (size_t k = 0; k < predateEnd; k++) {
				CustomIndexedVector<Species::ID, unsigned int>& predationEventsOnOtherSpecies = predationEventsOnOtherSpeciesLocal[landscapeAnimals[k]->getSpecies()->getAnimalSpeciesId()];

				if (landscape.getSaveAnimalsEachDayPredationProbabilities()) {
					predationProbabilitiesLocalStr.reserve(predateEstimatedPerThreadSize);
				}

				landscapeAnimals[k]->predate(false, landscape.getSaveAnimalsEachDayPredationProbabilities(), predationProbabilitiesLocalStr,
					&landscape, TimeStep(0), landscape.getTimeStepsPerDay(), landscape.getCompetitionAmongResourceSpecies(), predationEventsOnOtherSpecies,
					animalSpeciesMaximumPredationEncountersPerDay);
			}

			for (size_t idx = 0; idx < animalSpeciesMaximumPredationEncountersPerDay.size(); ++idx) {
				landscape.getExistingAnimalSpecies()[idx]->updateMaximumPredationEncountersPerDay(animalSpeciesMaximumPredationEncountersPerDay[idx]);
			}
		}
	};

	auto ActionExecution_Seq = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		size_t predateEnd, parallelEnd;
		PrepareForActionExecution(landscape, begin, end, predateEnd, parallelEnd);


		if (predateEnd < parallelEnd) {
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());

			size_t nonPredateEstimatedPerThreadSize = ((parallelEnd - predateEnd) * 256) / maxThreadsInArena;

			fmt::memory_buffer activitiesLocalStr;
			fmt::memory_buffer movementsLocalStr;

			if (landscape.getSaveActivity()) {
				activitiesLocalStr.reserve(nonPredateEstimatedPerThreadSize);
			}

			if (landscape.getSaveMovements()) {
				movementsLocalStr.reserve(nonPredateEstimatedPerThreadSize);
			}

			auto& landscapeAnimals = landscape.getLandscapeAnimals();

			for (size_t idx = predateEnd; idx < parallelEnd; ++idx) {
				landscapeAnimals[idx]->actionExecution(&landscape, landscape.getSaveActivity(), activitiesLocalStr,
					TimeStep(0), landscape.getTimeStepsPerDay(), landscape.getSaveMovements(), movementsLocalStr);
			}
		}
	};

	auto ActionExecution_Par = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		size_t predateEnd, parallelEnd;
		PrepareForActionExecution(landscape, begin, end, predateEnd, parallelEnd);


		if (predateEnd < parallelEnd) {
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());

			size_t nonPredateEstimatedPerThreadSize = ((parallelEnd - predateEnd) * 256) / maxThreadsInArena;

			tbb::enumerable_thread_specific<fmt::memory_buffer> localBuffersActivities;
			tbb::enumerable_thread_specific<fmt::memory_buffer> localBuffersMovements;

			auto& landscapeAnimals = landscape.getLandscapeAnimals();

			tbb::parallel_for(
				tbb::blocked_range<size_t>(predateEnd, parallelEnd),
				[&](const tbb::blocked_range<size_t>& r) {
					fmt::memory_buffer& activitiesLocalStr = localBuffersActivities.local();
					fmt::memory_buffer& movementsLocalStr = localBuffersMovements.local();

					if (landscape.getSaveActivity()) {
						activitiesLocalStr.reserve(nonPredateEstimatedPerThreadSize);
					}

					if (landscape.getSaveMovements()) {
						movementsLocalStr.reserve(nonPredateEstimatedPerThreadSize);
					}

					for (size_t idx = r.begin(); idx < r.end(); ++idx) {
						landscapeAnimals[idx]->actionExecution(&landscape, landscape.getSaveActivity(), activitiesLocalStr,
							TimeStep(0), landscape.getTimeStepsPerDay(), landscape.getSaveMovements(), movementsLocalStr);
					}
				}
			);
		}
	};


	auto PerformAnimalsActions_Seq = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		constexpr size_t estimatedLineSize = 256u;

		size_t estimatedSize = static_cast<std::size_t>(std::distance(begin, end)) * estimatedLineSize;

		fmt::memory_buffer voracitiesLocalStr;

		if (landscape.getSaveAnimalsEachDayVoracities())
		{
			voracitiesLocalStr.reserve(estimatedSize);
		}

		CustomIndexedVector<AnimalSpeciesID, unsigned int> animalSpeciesPopulation(landscape.getExistingAnimalSpecies().size(), 0u);

		CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer> animalConstitutiveTraitsLocalStr(landscape.getExistingAnimalSpecies().size());

		CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>> animalSpeciesGeneticsLocalStr(landscape.getExistingAnimalSpecies().size());

		for (auto& animalSpecies : landscape.getExistingAnimalSpecies()) {
			animalSpeciesGeneticsLocalStr[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getGenetics().getIndividualLevelTraits().size());
		}


		for (auto it = begin; it != end; ++it) { 
			AnimalSpeciesID id = (*it)->getSpecies()->getAnimalSpeciesId();

			fmt::memory_buffer& animalSpeciesTraitsLocalStr = animalConstitutiveTraitsLocalStr[id];

			std::vector<fmt::memory_buffer>& geneticsLocalStr = animalSpeciesGeneticsLocalStr[id];

			unsigned int& population = animalSpeciesPopulation[id];

			if (landscape.getSaveAnimalConstitutiveTraits())
			{
				animalSpeciesTraitsLocalStr.reserve(estimatedSize);
			}

			if (landscape.getSaveGenetics()) {
				geneticsLocalStr.reserve(estimatedSize);
			}

			if ((*it)->getLifeStage() == LifeStage::ACTIVE)
			{
				(*it)->printVoracities(&landscape, voracitiesLocalStr, landscape.getTimeStepsPerDay());

				(*it)->dieFromBackground(&landscape, TimeStep(0), landscape.getTimeStepsPerDay(), landscape.isGrowthAndReproTest());
			}

			if ((*it)->getLifeStage() == LifeStage::ACTIVE)
			{
				(*it)->transferAssimilatedFoodToEnergyTank(TimeStep(0));

				(*it)->metabolize(&landscape, TimeStep(0));
			}

			if ((*it)->getLifeStage() == LifeStage::REPRODUCING)
			{
				if ((*it)->isInBreedingZone())
				{
					population += (*it)->breed(&landscape, TimeStep(0), landscape.getSaveGenetics(), geneticsLocalStr, landscape.getTimeStepsPerDay(), landscape.getSaveAnimalConstitutiveTraits(), animalSpeciesTraitsLocalStr);

					(*it)->setInBreedingZone(false);
				}
			}

			if ((*it)->getLifeStage() == LifeStage::ACTIVE)
			{
				(*it)->checkEnergyTank(&landscape, TimeStep(0), landscape.getTimeStepsPerDay());
			}
		}
	};

	auto PerformAnimalsActions_Par = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		TbbManager::simulation_arena->execute([&]() {
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());
			constexpr size_t estimatedLineSize = 256u;
			const std::size_t n = static_cast<std::size_t>(std::distance(begin, end));

			size_t estimatedPerThreadSize = (n * estimatedLineSize) / maxThreadsInArena;

			tbb::enumerable_thread_specific<fmt::memory_buffer> localBuffersVoracities;

			tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, unsigned int>> animalSpeciesPopulationLocal(landscape.getExistingAnimalSpecies().size(), 0u);

			tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer>> localBuffersAnimalConstitutiveTraits = tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer>>([&]() {
				CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer> newVector;
				newVector.resize(landscape.getExistingAnimalSpecies().size());
				return newVector;
			});

			tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>>> localBuffersGenetics = tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>>>([&]() {
				CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>> newVector;
				newVector.resize(landscape.getExistingAnimalSpecies().size());

				for (auto& animalSpecies : landscape.getExistingAnimalSpecies()) {
					newVector[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getGenetics().getIndividualLevelTraits().size());
				}

				return newVector;
			});


			tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
				[&](const tbb::blocked_range<size_t>& r) {
					fmt::memory_buffer& voracitiesLocalStr = localBuffersVoracities.local();

					CustomIndexedVector<AnimalSpeciesID, unsigned int>& animalSpeciesPopulation = animalSpeciesPopulationLocal.local();

					CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer>& animalConstitutiveTraitsLocalStr = localBuffersAnimalConstitutiveTraits.local();

					CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>>& animalSpeciesGeneticsLocalStr = localBuffersGenetics.local();

					if (landscape.getSaveAnimalsEachDayVoracities())
					{
						voracitiesLocalStr.reserve(estimatedPerThreadSize);
					}

					for (size_t i = r.begin(); i < r.end(); ++i) {
						AnimalSpeciesID id = (*(begin + i))->getSpecies()->getAnimalSpeciesId();

						fmt::memory_buffer& animalSpeciesTraitsLocalStr = animalConstitutiveTraitsLocalStr[id];

						std::vector<fmt::memory_buffer>& geneticsLocalStr = animalSpeciesGeneticsLocalStr[id];

						unsigned int& population = animalSpeciesPopulation[id];

						if (landscape.getSaveAnimalConstitutiveTraits())
						{
							animalSpeciesTraitsLocalStr.reserve(estimatedPerThreadSize);
						}

						if (landscape.getSaveGenetics()) {
							geneticsLocalStr.reserve(estimatedPerThreadSize);
						}

						if ((*(begin + i))->getLifeStage() == LifeStage::ACTIVE)
						{
							(*(begin + i))->printVoracities(&landscape, voracitiesLocalStr, landscape.getTimeStepsPerDay());

							(*(begin + i))->dieFromBackground(&landscape, TimeStep(0), landscape.getTimeStepsPerDay(), landscape.isGrowthAndReproTest());
						}

						if ((*(begin + i))->getLifeStage() == LifeStage::ACTIVE)
						{
							(*(begin + i))->transferAssimilatedFoodToEnergyTank(TimeStep(0));

							(*(begin + i))->metabolize(&landscape, TimeStep(0));
						}

						if ((*(begin + i))->getLifeStage() == LifeStage::REPRODUCING)
						{
							if ((*(begin + i))->isInBreedingZone())
							{
								population += (*(begin + i))->breed(&landscape, TimeStep(0), landscape.getSaveGenetics(), geneticsLocalStr, landscape.getTimeStepsPerDay(), landscape.getSaveAnimalConstitutiveTraits(), animalSpeciesTraitsLocalStr);

								(*(begin + i))->setInBreedingZone(false);
							}
						}

						if ((*(begin + i))->getLifeStage() == LifeStage::ACTIVE)
						{
							(*(begin + i))->checkEnergyTank(&landscape, TimeStep(0), landscape.getTimeStepsPerDay());
						}
					}
				}
			);
			});
		};


	auto UpdateMap_Seq = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		constexpr size_t estimatedLineSize = 256u;

		size_t estimatedSize = static_cast<std::size_t>(std::distance(begin, end)) * estimatedLineSize;

		fmt::memory_buffer localBuffersMassInfo;
		if (landscape.getSaveMassInfo()) {
			localBuffersMassInfo.reserve(estimatedSize);
		}


		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> maximumInteractionAreaLocal(landscape.getExistingAnimalSpecies().size());

		for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
			maximumInteractionAreaLocal[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
		}


		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> maximumVoracityLocal(landscape.getExistingAnimalSpecies().size());

		for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
			maximumVoracityLocal[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
		}


		for (auto it = begin; it != end; ++it) {
			AnimalSpeciesID id = (*it)->getSpecies()->getAnimalSpeciesId();

			CustomIndexedVector<Instar, PreciseDouble>& maximumInteractionArea = maximumInteractionAreaLocal[id];
			CustomIndexedVector<Instar, PreciseDouble>& maximumVoracity = maximumVoracityLocal[id];


			(*it)->resetControlVariables(TimeStep(0), landscape.getTimeStepsPerDay());

			if ((*it)->getLifeStage() != LifeStage::UNBORN)
			{
				(*it)->increaseAge(&landscape, TimeStep(0), landscape.getTimeStepsPerDay());
			}

			if ((*it)->getLifeStage() == LifeStage::UNBORN)
			{
				(*it)->isReadyToBeBorn(&landscape, landscape.getTimeStepsPerDay());
			}

			if ((*it)->getLifeStage() == LifeStage::DIAPAUSE)
			{
				(*it)->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(&landscape);
			}

			if ((*it)->getLifeStage() == LifeStage::PUPA)
			{
				(*it)->isReadyToResumeFromPupaOrDecreasePupaTimer(&landscape);
			}

			if ((*it)->getLifeStage() != LifeStage::UNBORN)
			{
				(*it)->tune(&landscape, landscape.getSaveMassInfo(), localBuffersMassInfo, TimeStep(0), landscape.getTimeStepsPerDay(), maximumInteractionArea, maximumVoracity);
			}

			if ((*it)->getLifeStage() == LifeStage::ACTIVE)
			{
				(*it)->grow(&landscape, TimeStep(0), landscape.getTimeStepsPerDay());
			}
		}
		};

	auto UpdateMap_Par = [](Landscape& landscape, std::vector<AnimalNonStatistical*>::iterator begin, std::vector<AnimalNonStatistical*>::iterator end) {
		TbbManager::simulation_arena->execute([&]() {
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());
			constexpr size_t estimatedLineSize = 256u;
			const std::size_t n = static_cast<std::size_t>(std::distance(begin, end));

			size_t estimatedPerThreadSize = (n * estimatedLineSize) / maxThreadsInArena;

			tbb::enumerable_thread_specific<fmt::memory_buffer> localBuffersMassInfo;

			tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumInteractionAreaLocal(
				[&]() {
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(landscape.getExistingAnimalSpecies().size());

					for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
						localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
					}

					return localVector;
				}
			);

			tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumVoracityLocal(
				[&]() {
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(landscape.getExistingAnimalSpecies().size());

					for (size_t i = 0; i < landscape.getExistingAnimalSpecies().size(); ++i) {
						localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(landscape.getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
					}

					return localVector;
				}
			);


			tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
				[&](const tbb::blocked_range<size_t>& r) {
					fmt::memory_buffer& localStr = localBuffersMassInfo.local();
					if (landscape.getSaveMassInfo()) {
						localStr.reserve(estimatedPerThreadSize);
					}

					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumInteractionArea = maximumInteractionAreaLocal.local();
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumVoracity = maximumVoracityLocal.local();


					for (size_t i = r.begin(); i < r.end(); ++i) {
						AnimalSpeciesID id = (*(begin + i))->getSpecies()->getAnimalSpeciesId();

						CustomIndexedVector<Instar, PreciseDouble>& maximumInteractionArea = animalSpeciesMaximumInteractionArea[id];
						CustomIndexedVector<Instar, PreciseDouble>& maximumVoracity = animalSpeciesMaximumVoracity[id];


						(*(begin + i))->resetControlVariables(TimeStep(0), landscape.getTimeStepsPerDay());

						if ((*(begin + i))->getLifeStage() != LifeStage::UNBORN)
						{
							(*(begin + i))->increaseAge(&landscape, TimeStep(0), landscape.getTimeStepsPerDay());
						}

						if ((*(begin + i))->getLifeStage() == LifeStage::UNBORN)
						{
							(*(begin + i))->isReadyToBeBorn(&landscape, landscape.getTimeStepsPerDay());
						}

						if ((*(begin + i))->getLifeStage() == LifeStage::DIAPAUSE)
						{
							(*(begin + i))->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(&landscape);
						}

						if ((*(begin + i))->getLifeStage() == LifeStage::PUPA)
						{
							(*(begin + i))->isReadyToResumeFromPupaOrDecreasePupaTimer(&landscape);
						}

						if ((*(begin + i))->getLifeStage() != LifeStage::UNBORN)
						{
							(*(begin + i))->tune(&landscape, landscape.getSaveMassInfo(), localStr, TimeStep(0), landscape.getTimeStepsPerDay(), maximumInteractionArea, maximumVoracity);
						}

						if ((*(begin + i))->getLifeStage() == LifeStage::ACTIVE)
						{
							(*(begin + i))->grow(&landscape, TimeStep(0), landscape.getTimeStepsPerDay());
						}
					}
				}
			);
			});
		};
}

int main(int, char**)
{
	CrossoverOptimizer::Execute<true>(
		"UpdateTimeStepsWithoutFood",
		UpdateTimeStepsWithoutFood_Seq,
		UpdateTimeStepsWithoutFood_Par,
		"config_base"
	);


	CrossoverOptimizer::Execute<true>(
		"PrintAnimalsAlongCells",
		PrintAnimalsAlongCells_Seq,
		PrintAnimalsAlongCells_Par,
		"config_base"
	);


	CrossoverOptimizer::Execute<false>(
		"PrintCellAlongCells",
		PrintCellAlongCells_Seq,
		PrintCellAlongCells_Par,
		"config_base"
	);


	CrossoverOptimizer::Execute<true>(
		"InitControlVariables",
		InitControlVariables_Seq,
		InitControlVariables_Par,
		"config_base"
	);

	CrossoverOptimizer::Execute<true>(
		"ActionPlanning",
		ActionPlanning_Seq,
		ActionPlanning_Par,
		"config_base"
	);


	CrossoverOptimizer::Execute<true>(
		"ActionExecution",
		ActionExecution_Seq,
		ActionExecution_Par,
		"config_base"
	);


	CrossoverOptimizer::Execute<true>(
		"PerformAnimalsActions",
		PerformAnimalsActions_Seq,
		PerformAnimalsActions_Par,
		"config_base"
	);


	CrossoverOptimizer::Execute<true>(
		"UpdateMap",
		UpdateMap_Seq,
		UpdateMap_Par,
		"config_base"
	);


	return 0;
}
