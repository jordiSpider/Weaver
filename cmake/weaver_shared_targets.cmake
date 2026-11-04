# Shared build targets used by both the simulator and the R package.
# Include this file from the project root for every supported build mode.

find_package(magic_enum CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(unofficial-noise CONFIG REQUIRED)

set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

add_library(weaver_options INTERFACE)
target_include_directories(weaver_options INTERFACE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/resources"
)
target_link_libraries(weaver_options INTERFACE
    magic_enum::magic_enum
    nlohmann_json::nlohmann_json
    fmt::fmt
)

if(WIN32)
    target_compile_definitions(weaver_options INTERFACE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_options(weaver_options INTERFACE
        $<$<CONFIG:Debug>:/Wall /wd4866 /wd5262 /wd5045 /wd4820 /wd4514 /Od /Zi /MTd /EHsc /RTC1 /utf-8>
        $<$<CONFIG:Release>:/W4 /O2 /Ob2 /Ot /Gy /GL /DNDEBUG /MT /utf-8>
        $<$<CONFIG:RelWithDebInfo>:/W4 /O2 /Ob2 /Ot /Gy /GL /Z7 /DNDEBUG /MT /utf-8>
    )
    target_link_options(weaver_options INTERFACE
        $<$<CONFIG:Debug>:/DEBUG /INCREMENTAL>
        $<$<CONFIG:Release>:/INCREMENTAL:NO /LTCG /OPT:REF /OPT:ICF>
        $<$<CONFIG:RelWithDebInfo>:/DEBUG /LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO>
    )
else()
    target_compile_options(weaver_options INTERFACE
        $<$<CONFIG:Debug>:-Wall -Wextra -O0 -g -fno-omit-frame-pointer -fsanitize=address?$<BOOL:${ENABLE_ASAN}>:->
        $<$<CONFIG:Release>:-Wall -Wextra -O3 -mtune=native -flto -ffunction-sections -fdata-sections -funroll-loops -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:-Wall -Wextra -O3 -g -mtune=native -flto -fno-omit-frame-pointer -DNDEBUG>
    )
    target_link_options(weaver_options INTERFACE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Release>:-flto -Wl,--gc-sections -Wl,-O1>
        $<$<CONFIG:RelWithDebInfo>:-flto -g -Wl,--gc-sections>
    )
endif()

target_precompile_headers(weaver_options INTERFACE
    <nlohmann/json.hpp>
    <magic_enum/magic_enum.hpp>
)

add_library(weaver_exceptions STATIC
    "${CMAKE_SOURCE_DIR}/src/Exceptions/LineInfoException.cpp"
)
target_link_libraries(weaver_exceptions PUBLIC weaver_options)
set_target_properties(weaver_exceptions PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(BUILD_APP OR BUILD_BENCHMARKS)
    add_library(weaver_core_misc)
    target_link_libraries(weaver_core_misc PUBLIC weaver_options weaver_exceptions)
    set_target_properties(weaver_core_misc PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

set(_weaver_validator_sources
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Exceptions.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/ConfigurationValidator.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Operators.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Validator.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/Array.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/Boolean.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/Element.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/ElementFactory.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/Integer.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/Number.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/Object.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/String.cpp"
    "${CMAKE_SOURCE_DIR}/src/Misc/JsonValidator/Elements/Tuple.cpp"
)
add_library(weaver_config_validator SHARED ${_weaver_validator_sources})
target_link_libraries(weaver_config_validator PRIVATE weaver_exceptions PUBLIC weaver_options)
target_compile_definitions(weaver_config_validator PRIVATE WEAVER_CONFIG_VALIDATOR_BUILD)
set_target_properties(weaver_config_validator PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME "weaver_config_validator_core"
)
target_compile_features(weaver_config_validator PUBLIC cxx_std_20)

add_library(weaver_obstacle_engine SHARED
    "${CMAKE_SOURCE_DIR}/src/App/Model/IBM/Landscape/Map/Patches/ObstaclePorosityMaskGenerator.cpp"
)
target_link_libraries(weaver_obstacle_engine PUBLIC weaver_options unofficial::noise::noise-static)
target_compile_definitions(weaver_obstacle_engine PRIVATE WEAVER_OBSTACLE_ENGINE_BUILD)
set_target_properties(weaver_obstacle_engine PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME "weaver_obstacle_engine"
)
target_compile_features(weaver_obstacle_engine PUBLIC cxx_std_20)
