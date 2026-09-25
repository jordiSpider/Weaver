
# =========================================================
# ⚙️ GLOBAL TARGET OPTIONS DEFINITION (INTERFACE TARGET)
# =========================================================

include(${CMAKE_SOURCE_DIR}/cmake/embed_json_to_header.cmake)

file(GLOB SCHEMA_JSON_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/schema/${SCHEMA_VERSION}/*.json")

foreach(JSON_FILE IN LISTS SCHEMA_JSON_FILES)
    # We call our automated function for each file
    embed_json_to_header("${JSON_FILE}" "${CMAKE_BINARY_DIR}/resources/schema")
endforeach()


# ---------------------------------------------------------
# 📂 Subprojects & Target Hierarchy Traversals
# ---------------------------------------------------------
if(BUILD_APP OR BUILD_TESTS)
    find_package(magic_enum CONFIG REQUIRED)
    find_package(cxxopts CONFIG REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(Boost REQUIRED COMPONENTS geometry serialization)
    find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
    find_package(TBB CONFIG REQUIRED)
    find_package(fmt CONFIG REQUIRED)

    set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)




    # Define an interface configuration target to propagate global build policies
    add_library(weaver_options INTERFACE)

    target_include_directories(weaver_options
        INTERFACE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_BINARY_DIR}/resources
            ${RAPIDCSV_INCLUDE_DIRS}
    )

    target_link_libraries(weaver_options
        INTERFACE
            Boost::serialization
            fmt::fmt
            magic_enum::magic_enum
            cxxopts::cxxopts
            nlohmann_json::nlohmann_json
            Boost::geometry
            TBB::tbb TBB::tbbmalloc
    )

    if(WIN32)
        target_compile_definitions(weaver_options INTERFACE 
            NOMINMAX
            WIN32_LEAN_AND_MEAN # Optimization: Accelerate compilation phases on Windows platforms
        )

        target_compile_options(weaver_options INTERFACE
            # DEBUG CONFIGURATION
            $<$<CONFIG:Debug>:/Wall /wd4866 /wd5262 /wd5045 /wd4820 /wd4514 /Od /Zi /MTd /EHsc /RTC1 /utf-8>
            # RELEASE CONFIGURATION
            $<$<CONFIG:Release>:/W4 /O2 /Ob2 /Ot /Gy /GL /DNDEBUG /MT /utf-8>
            # RELWITHDEBINFO CONFIGURATION
            $<$<CONFIG:RelWithDebInfo>:/W4 /O2 /Ob2 /Ot /Gy /GL /Z7 /DNDEBUG /MT /utf-8>
        )
        
        # Linker Directives (Typically required by MSVC for Link-Time Code Generation / LTCG)
        target_link_options(weaver_options INTERFACE
            # DEBUG CONFIGURATION
            $<$<CONFIG:Debug>:/DEBUG /INCREMENTAL>
            # RELEASE CONFIGURATION
            $<$<CONFIG:Release>:/INCREMENTAL:NO /LTCG /OPT:REF /OPT:ICF>
            # RELWITHDEBINFO CONFIGURATION
            $<$<CONFIG:RelWithDebInfo>:/DEBUG /LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO>
        )
    else()
        # GCC / Clang Toolchains
        target_compile_options(weaver_options INTERFACE
            # DEBUG CONFIGURATION
            $<$<CONFIG:Debug>:-Wall -Wextra -O0 -g -fno-omit-frame-pointer -fsanitize=address?$<BOOL:${ENABLE_ASAN}>:->
            # RELEASE CONFIGURATION
            $<$<CONFIG:Release>:-Wall -Wextra -O3 -mtune=native -flto -ffunction-sections -fdata-sections -funroll-loops -DNDEBUG>
            # RELWITHDEBINFO CONFIGURATION
            $<$<CONFIG:RelWithDebInfo>:-Wall -Wextra -O3 -g -mtune=native -flto -fno-omit-frame-pointer -DNDEBUG>
        )

        target_link_options(weaver_options INTERFACE
            # DEBUG CONFIGURATION
            $<$<CONFIG:Debug>:-g>
            # RELEASE CONFIGURATION
            $<$<CONFIG:Release>:-flto -Wl,--gc-sections -Wl,-O1>
            # RELWITHDEBINFO CONFIGURATION
            $<$<CONFIG:RelWithDebInfo>:-flto -g -Wl,--gc-sections>
        )
    endif()

    # Precompiled header: bundles the heaviest/most widely-included third-party and
    # foundational headers to cut redundant parsing across the ~340 translation units.
    target_precompile_headers(weaver_options
        INTERFACE
            <boost/archive/text_iarchive.hpp>
            <boost/archive/text_oarchive.hpp>
            <boost/archive/binary_iarchive.hpp>
            <boost/archive/binary_oarchive.hpp>
            <nlohmann/json.hpp>
            <magic_enum/magic_enum.hpp>
    )





    add_library(weaver_core_misc)

    target_sources(weaver_core_misc
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/PathUtils.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/ProgressBar.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/Utilities.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/Maths/OptimisedOperators.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Exceptions.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Operators.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Validator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/Array.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/Boolean.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/Element.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/ElementFactory.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/Integer.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/Number.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/Object.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/String.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/JsonValidator/Elements/Tuple.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Misc/string/trim.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Exceptions/LineInfoException.cpp
    )

    target_precompile_headers(weaver_core_misc
        PRIVATE
            "${CMAKE_SOURCE_DIR}/include/Exceptions/LineInfoException.h"
            "${CMAKE_SOURCE_DIR}/include/Misc/EnumClass.h"
            "${CMAKE_SOURCE_DIR}/include/Misc/CustomIndexedVector.h"
            "${CMAKE_SOURCE_DIR}/include/Misc/Maths/PreciseDouble.h"
    )

    target_link_libraries(weaver_core_misc
        PUBLIC
            weaver_options
    )




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
endif()

if(BUILD_APP)
    # 1. Always include the CLI target (serves as the core simulator foundation)
    add_subdirectory(cli)

    # 2. Conditionally append the Graphical User Interface (GUI) target layer
    if(ENABLE_GUI)
        add_subdirectory(gui)
    endif()
endif()
