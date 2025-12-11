/**
 * @file Coverage.h
 * @brief Defines the Coverage enumeration used to represent habitat or resource coverage levels.
 */

#ifndef COVERAGE_H_
#define COVERAGE_H_

/**
 * @enum Coverage
 * @brief Represents the degree of coverage of a resource or area.
 *
 * This enumeration is used to indicate how much of a terrain cell or resource
 * patch is covered or available. It is typically applied to quantify the
 * accessibility or abundance of resources in ecological simulations.
 */
enum Coverage : unsigned int
{
    Null = 0,        ///< No coverage.
    Partial = 1,     ///< Partial coverage (less than 50%).
    Over50Percent = 2, ///< Coverage over 50% but not complete.
    Full = 3         ///< Complete coverage (100%).
};

#endif /* COVERAGE_H_ */