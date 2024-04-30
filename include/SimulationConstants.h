/*
 * Constants.h
 *
 *  Created on: Aug 2, 2012
 *      Author: jrbcast
 */

#ifndef WEAVER_SIMULATIONCONSTANTS_H_
#define WEAVER_SIMULATIONCONSTANTS_H_

#include <ctime>

// This modifies each patch before creating it by randomly choosing its
// sigma between 1 and patch width and amplitude between minWater and maxWater
// If not specified, uses maximum
const int USE_RANDOM_AMPLITUDE = -1;
const int USE_RANDOM_SIGMA = -1;

#define STOICHIOMETRY_PRECISION 2	// Number of decimal digits to show when printing

const double MIN_T = 15;
const double MAX_T = 25;

const unsigned int DIMENSIONS = 3;

/*
#ifdef _TBB
	const unsigned int TBB_NUM_THREADS = 8;
	const unsigned int TBB_NUM_TASKS = 2;
#elif _PTHREAD
	const unsigned int PARTITIONS_PER_DIMENSION = 2;
#endif
*/

#endif /* SIMULATIONCONSTANTS_H */
