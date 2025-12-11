/**
 * @file trim.h
 * @brief Provides utility functions to trim whitespace from strings.
 *
 * This header defines functions to remove leading, trailing, or both leading
 * and trailing whitespace characters from std::string objects. The trimming
 * functions modify the string in-place where applicable or return a trimmed copy.
 */

#ifndef TRIM_H_
#define TRIM_H_

#include <algorithm> 
#include <cctype>
#include <locale>


/**
 * @brief Trims whitespace from the start (left) of the string in-place.
 *
 * Removes all leading whitespace characters (spaces, tabs, etc.) from the provided string.
 *
 * @param s The string to trim.
 */
void ltrim(std::string &s);

/**
 * @brief Trims whitespace from the end (right) of the string in-place.
 *
 * Removes all trailing whitespace characters (spaces, tabs, etc.) from the provided string.
 *
 * @param s The string to trim.
 */
void rtrim(std::string &s);

/**
 * @brief Trims whitespace from both ends of the string.
 *
 * Returns a new string with leading and trailing whitespace removed.
 *
 * @param s The input string to trim.
 * @return A trimmed copy of the input string.
 */
std::string trim(std::string s);

#endif /* TRIM_H_ */
