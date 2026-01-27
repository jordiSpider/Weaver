/**
 * @file Genome.h
 * @brief Defines the Genome class representing a diploid genome with homologous chromosomes and correlosomes.
 * 
 * This file contains the Genome class, which models a diploid genome composed
 * of homologous chromosomes and correlosomes. It provides constructors for
 * creating genomes from parent gametes or loci, methods for meiosis, cloning,
 * and accessing homologous structures, and serialization support.
 */

#ifndef GENOME_H_
#define GENOME_H_


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Locus.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Chromosome.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Correlosome.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Gamete.h"
#include "App/Model/IBM/Maths/Random.h"
#include "Exceptions/LineInfoException.h"


/**
 * @class Genome
 * @brief Represents a genome with homologous chromosomes and correlosomes.
 */
class Genome
{
public:
     /**
     * @brief Default constructor.
     */
     Genome();

	/**
     * @brief Constructor for the Genome class.
     * @param firstParentGamete Pointer to the first parent's gamete.
     * @param secondParentGamete Pointer to the second parent's gamete.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     * @param numberOfLociPerChromosome The number of loci per chromosome.
     * @param numberOfChiasmasPerChromosome The number of chiasmas per chromosome.
     */
	Genome(Gamete* const firstParentGamete, Gamete* const secondParentGamete, const std::vector<size_t> &randomlyCreatedPositionsForChromosomes, 
		   const size_t& numberOfLociPerChromosome, const size_t& numberOfChiasmasPerChromosome);
	
	/**
     * @brief Constructor for the Genome class.
     * @param lociPerTrait Vector of loci.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     * @param numberOfChromosomes The number of chromosomes.
     * @param numberOfLociPerChromosome The number of loci per chromosome.
     * @param numberOfChiasmasPerChromosome The number of chiasmas per chromosome.
     */
	Genome(const std::vector<std::vector<Locus>> &lociPerTrait, const std::vector<size_t> &randomlyCreatedPositionsForChromosomes, const size_t &numberOfChromosomes,
		   const size_t& numberOfLociPerChromosome, const size_t& numberOfChiasmasPerChromosome);
	
	/**
     * @brief Copy constructor for the Genome class.
     * @param otherGenome The genome to copy.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     */
	Genome(const Genome& otherGenome, const std::vector<size_t> &randomlyCreatedPositionsForChromosomes);

     /**
     * @brief Destructor for the Genome class.
     */
	virtual ~Genome();

     void initFromOther(const Genome& otherGenome, const std::vector<size_t> &randomlyCreatedPositionsForChromosomes);

    /**
     * @brief Get gametes from meiosis.
     * @param indexSelectedGamete The index of the selected gamete.
     * @return Pointer to the selected gamete.
     */
	Gamete* getGametesFromMeiosis(const size_t &indexSelectedGamete);
	
	/**
     * @brief Get a random gamete from meiosis.
     * @return Pointer to the randomly selected gamete.
     */
	Gamete * getRandomGameteFromMeiosis();
	
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
	const std::vector<std::pair<Chromosome*, Chromosome*> >& getHomologousChromosomes() const;
	
	/**
     * @brief Get the homologous correlosomes.
     * @return Vector of pairs of homologous correlosomes.
     */
	const std::vector<std::pair<Correlosome*, Correlosome*> >& getHomologousCorrelosomes() const;
	
	/**
     * @brief Delete homologous correlosomes.
     */
	void deleteHomologousCorrelosomes();
	
	/**
     * @brief Clone the genome.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     * @return Pointer to the cloned genome.
     */
	Genome* clone(const std::vector<size_t> &randomlyCreatedPositionsForChromosomes) const;

	/**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
     template<class Archive>
     void serialize(Archive & ar, const unsigned int version);

private:
     static constexpr unsigned int NUMBER_GAMETES_TO_GENERATE = 4; /**< Number of gametes generated per meiosis. */

	std::vector<std::pair<Chromosome*, Chromosome*> > homologousChromosomes; /**< Vector of homologous chromosome pairs. */
     std::vector<std::pair<Correlosome*, Correlosome*> > homologousCorrelosomes; /**< Vector of homologous correlosome pairs. */
     size_t numberOfLociPerChromosome; /**< Number of loci per chromosome. */
     size_t numberOfChiasmasPerChromosome; /**< Number of chiasmas per chromosome. */
	
	/**
     * @brief Create homologous correlosomes from homologous chromosomes.
     * @param randomlyCreatedPositionsForChromosomes Vector of randomly created positions for chromosomes.
     */
	void createHomologousCorrelosomesFromChromosomes(const std::vector<size_t> &randomlyCreatedPositionsForChromosomes);
};

#endif /* GENOME_H_ */
