
include(${CMAKE_SOURCE_DIR}/cmake/embed_configuration_schemas.cmake)
weaver_embed_configuration_schemas("${CMAKE_BINARY_DIR}/resources")

# Dependencies used by the simulator and application libraries, but not by R.
find_package(cxxopts CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS geometry serialization)
find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h" REQUIRED)
find_package(TBB CONFIG REQUIRED)

target_sources(weaver_core_misc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Misc/PathUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Misc/ProgressBar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Misc/Utilities.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Misc/Maths/OptimisedOperators.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Misc/string/trim.cpp
)

target_precompile_headers(weaver_core_misc
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include/Exceptions/LineInfoException.h"
        "${CMAKE_SOURCE_DIR}/include/Misc/EnumClass.h"
        "${CMAKE_SOURCE_DIR}/include/Misc/CustomIndexedVector.h"
        "${CMAKE_SOURCE_DIR}/include/Misc/Maths/PreciseDouble.h"
)

target_precompile_headers(weaver_core_misc PUBLIC
    <boost/archive/text_iarchive.hpp>
    <boost/archive/text_oarchive.hpp>
    <boost/archive/binary_iarchive.hpp>
    <boost/archive/binary_oarchive.hpp>
)

target_link_libraries(weaver_core_misc
    PUBLIC
        weaver_options
        Boost::serialization
        Boost::geometry
        cxxopts::cxxopts
        TBB::tbb
        TBB::tbbmalloc
)

target_include_directories(weaver_core_misc PUBLIC "${RAPIDCSV_INCLUDE_DIRS}")




add_library(weaver_app_maths_physics)

target_sources(weaver_app_maths_physics
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Conversions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/GaussianDistribution.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/MathFunctions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Parabola.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Random.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/Dynamics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/MassBasedDynamics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/NonMassBasedDynamics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/Decay/Decay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/Decay/LinealDecay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/Decay/NonDecay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/MassBased/GrowthRateDynamics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/NonMassBased/CycleDynamics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/NonMassBased/NonDynamics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Maths/Dynamics/NonMassBased/PulseDynamics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Physics/Temperature.cpp
)

target_link_libraries(weaver_app_maths_physics
    PUBLIC
        weaver_options
        weaver_core_misc
)




add_library(weaver_app_landscape)

target_sources(weaver_app_landscape
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/AquaticLandscape.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/ArthropodsLandscape.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/DinosaursLandscape.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Landscape.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Map.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Navigation/RouteCandidateGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Navigation/RouteCollisionValidator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Navigation/ContinuousAStarRoutePlanner.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Navigation/EuclideanRouteCostPolicy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Navigation/RandomizedCandidateSelectionPolicy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Navigation/WaypointRouteFollowingPolicy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Geometry/Geometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Patch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Shape/CubicPatch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Shape/GaussianPatch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Shape/HomogeneousPatch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Shape/PatchShape.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Shape/RandomGaussianPatch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Shape/SphericalPatch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Source/HabitatDomainSource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Source/MoistureSource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Source/ObstacleSource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Source/PathSource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Patches/Source/ResourceSource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Points/PointContinuous.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/Points/PointMap.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/LivingBeings/Animals/SpatialTreeAnimal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/LeafTerrainCell.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/RootTerrainCell.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/BranchPatchApplicator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/LeafPatchApplicator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/SpatialTreePatchApplicator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/HabitatDomain/BranchCellHabitatDomain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Moisture/BranchCellMoisture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Obstacle/BranchCellObstacle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Resources/BranchCellResource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Resources/Growth/BranchCellResourceGrowth.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/AnimalSearchParams.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/CellValue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/EdibleSearchParams.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/ResourceSearchParams.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/CellElement.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/PatchApplicator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/HabitatDomain/CellHabitatDomain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/HabitatDomain/CellHabitatDomainInterface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoisture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoistureInterface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Obstacle/CellObstacle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Obstacle/CellObstacleInterface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/ResourceSignature.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/Growth/CellResourceGrowth.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/Growth/CellResourceInterfaceGrowth.cpp
)

target_link_libraries(weaver_app_landscape
    PUBLIC
        weaver_options
        weaver_core_misc
        weaver_app_maths_physics
        weaver_app_living_beings
        weaver_obstacle_engine
        unofficial::noise::noise-static
)




add_library(weaver_traits)

target_sources(weaver_traits
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/BaseTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/CellValueTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DecisionsTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/EdibilityValueWeightTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/EscapeProbabilityWeightTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/PredationProbabilityWeightTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/PreferencesTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/ProbabilityDensityFunctionTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/IndividualLevelTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/SpeciesLevelTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/TraitDefinitionSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TempSizeRuleTraitTemperatureSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TraitTemperatureSection.cpp
)

target_link_libraries(weaver_traits
    PUBLIC
        weaver_options
        weaver_core_misc
)




add_library(weaver_app_living_beings)

target_sources(weaver_app_living_beings
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Edible.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/StructuralUnits.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/TimeUnits.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/AnimalSignature.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Animal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/AnimalStatistical.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Generation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Prey.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Decisions/Decisions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Chromosome.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Correlosome.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Gamete.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Genetics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Genome.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTraitTemperatureSectionNone.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/TempSizeRuleIndividualTraitTemperatureSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalGrowth.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalNonStatisticalGrowth.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalStatisticalGrowth.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/ExponentialIndividualGrowthModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/LinearIndividualGrowthModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/Logistic4PIndividualGrowthModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/LogisticIndividualGrowthModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/VonBertalanffyIndividualGrowthModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpecies.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/AnimalSpeciesDecisions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/SensoryModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/AnimalDecisionProbability.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/CellQualityBiomass.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/DecisionProbability.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EdibilityValue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EdibleDecisionProbability.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EscapeProbability.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/PredationProbability.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Allele.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/AnimalSpeciesGenetics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Locus.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/Growth/AnimalSpeciesGrowth.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Animals/Species/OntogeneticLink.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Growth/EdibleGrowth.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Resources/Species/Growth/ResourceSpeciesGrowth.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Species/Species.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/Landscape/LivingBeings/Species/Growth/SpeciesGrowth.cpp
)

target_link_libraries(weaver_app_living_beings
    PUBLIC
        weaver_options
        weaver_core_misc
        weaver_app_maths_physics
)

if(UNIX AND NOT APPLE)
    target_link_libraries(weaver_app_living_beings PRIVATE
        -Wl,--whole-archive weaver_traits -Wl,--no-whole-archive
    )
else()
    target_link_libraries(weaver_app_living_beings PRIVATE "LINKER:/WHOLEARCHIVE:$<TARGET_FILE:weaver_traits>")
endif()




add_library(weaver_app_model)

target_sources(weaver_app_model
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/Model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/Model/IBM/IBM.cpp
)

target_link_libraries(weaver_app_model
    PUBLIC
        weaver_options
        weaver_core_misc
        weaver_config_validator
        weaver_app_landscape
        weaver_app_living_beings
)




add_library(weaver_app_view)

target_sources(weaver_app_view
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/App/IO/StorageBridge.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/View/Console.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/App/View/View.cpp
)

if(ENABLE_GUI)
    target_sources(weaver_app_view
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/App/View/GUI/GUI.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/App/View/GUI/Widgets/Checkbox.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/App/View/GUI/Widgets/LogTextBox.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/App/View/GUI/Widgets/Spinbox.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/App/View/GUI/Widgets/OptionSelector.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/App/View/GUI/Widgets/PathChooser.cpp
    )
else()
    target_sources(weaver_app_view
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/App/View/Console.cpp
    )
endif()

target_link_libraries(weaver_app_view
    PUBLIC
        weaver_options
        weaver_core_misc
        weaver_app_model
        weaver_app_living_beings
)
