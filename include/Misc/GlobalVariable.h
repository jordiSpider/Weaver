/**
 * @file GlobalVariable.h
 * @brief Defines global constants and paths used throughout the simulation.
 *
 * This header provides the default working directory, schema folder paths, 
 * paths to JSON schema files, and some general constants for logging and formatting.
 */

#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_


#ifdef _WIN32
    #include <cstdlib>
    #include <string>
#endif

#include "Misc/PathUtils.h"


/**
 * @brief Default working directory of the application.
 */
const std::filesystem::path WORKING_DIRECTORY = getWorkingDirectory();

/**
 * @brief Folder containing all JSON schema files, organized by schema version.
 */
const std::filesystem::path SCHEMA_FOLDER = WORKING_DIRECTORY / std::filesystem::path("schema") / SCHEMA_VERSION;

/** @name JSON Schema Paths */
//@{
const std::filesystem::path RESOURCE_SCHEMA = SCHEMA_FOLDER / "resource.schema.json";                 /**< Path to resource schema file. */
const std::filesystem::path RESOURCE_PATCH_SCHEMA = SCHEMA_FOLDER / "resource_patch.schema.json";   /**< Path to resource patch schema file. */
const std::filesystem::path MOISTURE_PATCH_SCHEMA = SCHEMA_FOLDER / "moisture_patch.schema.json";   /**< Path to moisture patch schema file. */
const std::filesystem::path OBSTACLE_PATCH_SCHEMA = SCHEMA_FOLDER / "obstacle_patch.schema.json";   /**< Path to obstacle patch schema file. */
const std::filesystem::path HABITAT_DOMAIN_PATCH_SCHEMA = SCHEMA_FOLDER / "habitat_domain_patch.schema.json"; /**< Path to habitat domain patch schema file. */
const std::filesystem::path SPECIES_SCHEMA = SCHEMA_FOLDER / "species.schema.json";                 /**< Path to species schema file. */
const std::filesystem::path SIMULATION_PARAMS_SCHEMA = SCHEMA_FOLDER / "simulation_params.schema.json"; /**< Path to simulation parameters schema file. */
const std::filesystem::path LANDSCAPE_PARAMS_SCHEMA = SCHEMA_FOLDER / "landscape_params.schema.json";   /**< Path to landscape parameters schema file. */
//@}

/**
 * @brief Default folder name where resource patches are stored.
 */
#define RESOURCE_PATCH_FOLDER "patches"

/**
 * @brief Default filename for log files.
 */
#define LOG_FILENAME "log"

/**
 * @brief Default extension for log files.
 */
#define LOG_EXTENSION "txt"

/**
 * @brief Tab string used for indentation in logs or formatted output.
 */
#define TAB "    "

#endif // GLOBAL_VARIABLES_H_
