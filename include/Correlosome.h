
#ifndef CORRELOSOME_H_
#define CORRELOSOME_H_

#include <vector>
#include "Allele.h"



class Correlosome
{
private:
	std::vector<Allele*>* alleles;
public:
	Correlosome(std::vector<Allele*>* correlosomeAlleles);
    Correlosome(const unsigned int& numberOfLociPerChromosome);
    Correlosome(Correlosome* otherCorrelosome);

    virtual ~Correlosome();

    inline const std::vector<Allele*>* getAlleles() const { return alleles; }
	void setAllele(const Allele* allele, int lociPosition);
	Correlosome* clone();

    friend std::ostream& operator<<(std::ostream& os, Correlosome& correlosome);
};


#endif /* CORRELOSOME_H_ */
