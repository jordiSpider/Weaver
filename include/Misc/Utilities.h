/**
 * @file Utilities.h
 * @brief Utility functions for creating, reading, and saving files, including JSON configuration files.
 *
 * This file provides helper functions to manage output files for simulation results,
 * as well as reading and validating JSON configuration files.
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <magic_enum/magic_enum.hpp>
#include <limits>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <regex>
#include <sstream>
#include <stack>
#include <set>
#include <string>
#include <chrono>
#include <filesystem>

#include "Types.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/JsonValidator/Validator.h"
#include "GlobalVariable.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"

/**
 * @brief Creates an output file at the specified path.
 *
 * @param file Reference to an ofstream object to open.
 * @param file_path Path where the file will be created.
 * @param openMode File open mode (default is std::ofstream::out).
 * @return The full path of the created file as a string.
 */
std::string createOutputFile(std::ofstream &file, std::filesystem::path file_path, std::ios_base::openmode openMode = std::ofstream::out);

/**
 * @brief Creates an output file combining root path, filename, and extension.
 *
 * @param file Reference to an ofstream object to open.
 * @param filenameRoot Root path where the file will be created.
 * @param filename Name of the file without extension.
 * @param extension File extension (e.g., ".txt", ".csv").
 * @param openMode File open mode (default is std::ofstream::out).
 * @return The full path of the created file as a string.
 */
std::string createOutputFile(std::ofstream &file, std::filesystem::path filenameRoot, std::string filename, std::string extension, std::ios_base::openmode openMode = std::ofstream::out);

/**
 * @brief Creates an output file with optional numbering based on simulation time steps.
 *
 * @param file Reference to an ofstream object to open.
 * @param filenameRoot Root path where the file will be created.
 * @param filename Name of the file without extension.
 * @param extension File extension (e.g., ".txt", ".csv").
 * @param numberOfTimeSteps Total number of simulation time steps.
 * @param recordEach Interval of time steps to record.
 * @param openMode File open mode (default is std::ofstream::out).
 * @return The full path of the created file as a string.
 */
std::string createOutputFile(std::ofstream &file, std::filesystem::path filenameRoot, std::string filename, std::string extension, const TimeStep numberOfTimeSteps, unsigned int recordEach, std::ios_base::openmode openMode = std::ofstream::out);

/**
 * @brief Reads a JSON configuration file and validates it.
 *
 * @param configPath Path to the JSON configuration file.
 * @param validator JsonValidator object used to validate the JSON structure.
 * @return The content of the configuration file as a nlohmann::json object.
 * @throws LineInfoException if the file cannot be read or is invalid.
 */
nlohmann::json readConfigFile(std::filesystem::path configPath, const JsonValidator& validator);

/**
 * @brief Saves a JSON object to a configuration file.
 *
 * @param configPath Path where the JSON file will be saved.
 * @param fileContent JSON content to save.
 * @throws LineInfoException if the file cannot be written.
 */
void saveConfigFile(std::filesystem::path configPath, nlohmann::json fileContent);

#endif // UTILITIES_H_
