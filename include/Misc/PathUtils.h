/**
 * @file PathUtils.h
 * @brief Utility functions for handling filesystem paths.
 *
 * This file defines functions for retrieving default output paths, resolving
 * relative paths, and safely accessing environment variables.
 */

#ifndef PATH_UTILS_H_
#define PATH_UTILS_H_

#include <filesystem>
#include <string>


/**
 * @brief Obtains the default folder path for program outputs.
 *
 * This function returns the standard output directory path used by the
 * application when no custom output path is specified.
 *
 * @return The default output folder as a std::filesystem::path.
 */
std::filesystem::path getDefaultOutputFolderPath();

/**
 * @brief Returns the current working directory.
 *
 * @return std::filesystem::path Current working directory path.
 */
std::filesystem::path getWorkingDirectory();

/**
 * @brief Resolves a path relative to a given working directory.
 *
 * If the input path is relative, it is resolved with respect to the
 * specified working directory. Absolute paths are returned as-is.
 *
 * @param workingDirectory Base directory for resolving relative paths.
 * @param input Path to resolve.
 * @return std::filesystem::path Resolved absolute path.
 */
std::filesystem::path resolvePath(const std::filesystem::path& workingDirectory, const std::filesystem::path& input);

#ifdef _WIN32
/**
 * @brief Safely retrieves the value of an environment variable.
 *
 * Returns the value of the environment variable if it exists, otherwise
 * returns an empty string.
 *
 * @param varName Name of the environment variable.
 * @return std::string Value of the environment variable or empty string.
 */
std::string getEnvSafe(const char* varName);
#endif

#endif // PATH_UTILS_H_
