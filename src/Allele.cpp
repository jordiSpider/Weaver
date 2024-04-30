/*
 * Allele.cpp
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#include "Allele.h"

int Allele::alleleId = 0;

Allele::Allele(float value, float alphabeticOrder)
{
	this->id = alleleId;
	alleleId++;
	this->value = value;
	this->alphabeticOrder = alphabeticOrder;
}

Allele::Allele(const Allele* otherAllele)
{
	this->id = otherAllele->getId();
	this->value = otherAllele->getValue();
	this->alphabeticOrder = otherAllele->getAlphabeticOrder();
}

Allele::~Allele() {
	// Nothing to delete dynamically
}

int Allele::getId() const {
	return id;
}

float Allele::getValue() const {
	return value;
}

float Allele::getAlphabeticOrder() const {
	return alphabeticOrder;
}

Allele* Allele::clone() const {
	return new Allele(this);
}

//TODO Agregar el ID en esta impresion
ostream& operator<<(ostream& os, Allele& allele)
{
	os << allele.getValue();
	return os;
}
