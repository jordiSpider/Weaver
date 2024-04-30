/*
 * Genome.h
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#ifndef GENOME_H_
#define GENOME_H_

#include <algorithm>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Animals/Genetics/Locus.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Chromosome.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Correlosome.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Gamete.h"
#include "IBM/Maths/Random.h"
#include "Exceptions/LineInfoException.h"


/**
 * @class Genome
 * @brief Represents a genome with homologous chromosomes and correlosomes.
 */
class Genome
{
private:
	static constexpr unsigned int NUMBER_GAMETES_TO_GENERATE = 4;

	std::vector<std::pair<Chromosome*, Chromosome*> > homologousChromosomes; /**< Vector of pairs of homologous chromosomes. */
	std::vector<std::pair<Correlosome*, Correlosome*> > homologousCorrelosomes; /**< Vector of pairs of homologous correlosomes. */
	unsigned int numberOfLociPerChromosome;
	unsigned int numberOfChiasmasPerChromosome;
	
	/**
	* @brief Creates homologous correlosomes from homologous chromosomes.
	* @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
	*/
	void createHomologousCorrelosomesFromChromosomes(const std::vector<int> &randomlyCreatedPositionsForChromosomes);

	friend class boost::serialization::access;

public:
     Genome();
	/**
     * @brief Constructor for the Genome class.
     * @param firstParentGamete Pointer to the first parent's gamete.
     * @param secondParentGamete Pointer to the second parent's gamete.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     * @param numberOfLociPerChromosome The number of loci per chromosome.
     * @param numberOfChiasmasPerChromosome The number of chiasmas per chromosome.
     */
	Genome(Gamete* const firstParentGamete, Gamete* const secondParentGamete, const std::vector<int> &randomlyCreatedPositionsForChromosomes, 
		   const unsigned int& numberOfLociPerChromosome, const unsigned int& numberOfChiasmasPerChromosome);
	
	/**
     * @brief Constructor for the Genome class.
     * @param loci Vector of loci.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     * @param numberOfChromosomes The number of chromosomes.
     * @param numberOfLociPerChromosome The number of loci per chromosome.
     * @param numberOfChiasmasPerChromosome The number of chiasmas per chromosome.
     */
	Genome(const std::vector<Locus> &loci, const std::vector<int> &randomlyCreatedPositionsForChromosomes, const unsigned int &numberOfChromosomes,
		   const unsigned int& numberOfLociPerChromosome, const unsigned int& numberOfChiasmasPerChromosome);
	
	/**
     * @brief Copy constructor for the Genome class.
     * @param otherGenome The genome to copy.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     */
	Genome(const Genome& otherGenome, const std::vector<int> &randomlyCreatedPositionsForChromosomes);
	
	/**
     * @brief Destructor for the Genome class.
     */
	virtual ~Genome();

	//4 vectors (cells) of 20 chromosomes each. The animal will select one out of those 4 cells.
    /**
     * @brief Get gametes from meiosis.
     * @param indexSelectedGamete The index of the selected gamete.
     * @return Pointer to the selected gamete.
     */
	Gamete* getGametesFromMeiosis(const unsigned int &indexSelectedGamete);
	
	/**
     * @brief Get a random gamete from meiosis.
     * @return Pointer to the randomly selected gamete.
     */
	inline Gamete * getRandomGameteFromMeiosis() { return getGametesFromMeiosis(Random::randomIndex(NUMBER_GAMETES_TO_GENERATE)); }
	
	/**
     * @brief Clone the first gamete from the haploid.
     * @return Pointer to the cloned gamete.
     */
	Gamete * cloneFirstGameteFromHaploid() const;
	
	/**
     * @brief Clone the second gamete from the haploid.
     * @return Pointer to the cloned gamete.
     */
	Gamete * cloneSecondGameteFromHaploid() const;
	
	/**
     * @brief Get the homologous chromosomes.
     * @return Vector of pairs of homologous chromosomes.
     */
	inline const std::vector<std::pair<Chromosome*, Chromosome*> >& getHomologousChromosomes() const { return homologousChromosomes; }
	
	/**
     * @brief Get the homologous correlosomes.
     * @return Vector of pairs of homologous correlosomes.
     */
	inline const std::vector<std::pair<Correlosome*, Correlosome*> >& getHomologousCorrelosomes() const { return homologousCorrelosomes; }
	
	/**
     * @brief Delete homologous correlosomes.
     */
	void deleteHomologousCorrelosomes();
	
	/**
     * @brief Clone the genome.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     * @return Pointer to the cloned genome.
     */
	inline Genome* clone(const std::vector<int> &randomlyCreatedPositionsForChromosomes) const { return new Genome(*this, randomlyCreatedPositionsForChromosomes); }

	/**
     * @brief Serialize the Genome object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
     template<class Archive>
     void serialize(Archive & ar, const unsigned int version, const std::vector<Locus> &loci);
};

namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(
            Archive &ar, Genome* &genomePtr, const unsigned int version, const std::vector<Locus> &loci);
    }
}

#endif /* GENOME_H_ */
