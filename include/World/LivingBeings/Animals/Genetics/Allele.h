/*
 * Allele.h
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#ifndef ALLELE_H_
#define ALLELE_H_

#include <ostream>


class Allele
{
private:
	static unsigned int alleleId;

	const unsigned int id;
	const double value;
	const unsigned int alphabeticOrder;
public:
	explicit Allele(const double &value, const unsigned int alphabeticOrder);
	~Allele();

	// Getters
	inline const unsigned int getId() const { return id; }
	inline const double& getValue() const { return value; }
	inline const unsigned int getAlphabeticOrder() const { return alphabeticOrder; }
	
	friend std::ostream& operator<<(std::ostream& os, const Allele& chromosome);
};

#endif /* ALLELE_H_ */
