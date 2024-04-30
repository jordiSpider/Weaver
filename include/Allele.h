/*
 * Allele.h
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#ifndef ALLELE_H_
#define ALLELE_H_

#include <ostream>
using namespace std;

class Allele
{
private:
	int id;
	float value;
	float alphabeticOrder;
public:
	Allele(float value, float alphabeticOrder);
	Allele(const Allele* otherAllele);
	virtual ~Allele();
	int getId() const;
	float getValue() const;
	float getAlphabeticOrder() const;
	Allele* clone() const;
	friend ostream& operator<<(ostream& os, Allele& chromosome);

private:
	static int alleleId;
};

#endif /* ALLELE_H_ */
