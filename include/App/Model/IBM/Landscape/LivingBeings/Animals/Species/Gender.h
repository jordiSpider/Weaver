/**
 * @file Gender.h
 * @brief Defines the Gender enumeration representing the sex of an individual.
 */

#ifndef GENDER_H_
#define GENDER_H_

/**
 * @enum Gender
 * @brief Represents the biological sex of an individual.
 */
enum class Gender : unsigned char
{
	MALE,         /**< Male individual. */
    FEMALE,       /**< Female individual. */
    HERMAPHRODITE /**< Individual possessing both male and female reproductive organs. */
};

#endif /* GENDER_H_ */
