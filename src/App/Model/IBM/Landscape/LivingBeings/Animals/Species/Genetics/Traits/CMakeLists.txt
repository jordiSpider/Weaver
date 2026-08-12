
add_library(weaver_traits OBJECT)

target_sources(weaver_traits
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/BaseTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CellValueTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DecisionsTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EdibilityValueWeightTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EscapeProbabilityWeightTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PredationProbabilityWeightTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PreferencesTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ProbabilityDensityFunctionTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Trait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DefinitionSection/IndividualLevelTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DefinitionSection/SpeciesLevelTrait.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DefinitionSection/TraitDefinitionSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TemperatureSection/PawarTraitTemperatureSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TemperatureSection/TempSizeRuleTraitTemperatureSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TemperatureSection/TraitTemperatureSection.cpp
)

target_link_libraries(weaver_traits
    PUBLIC
        weaver_options
)
