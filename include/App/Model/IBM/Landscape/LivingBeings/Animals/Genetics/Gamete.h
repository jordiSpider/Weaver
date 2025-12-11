/**
 * @file Gamete.h
 * @brief Defines the Gamete class used in the genetic model.
 * 
 * This file contains the definition of the Gamete class, which represents
 * a gamete in the genetic model. A gamete consists of a variable number
 * of chromosomes and provides methods to access, modify, and manage them.
 */

#ifndef GAMETES_H_
#define GAMETES_H_

#include <vector>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Chromosome.h"


/**
 * @class Gamete
 * @brief Represents a gamete with a variable number of chromosomes.
 */
class Gamete
{
public:
    /**
     * @brief Constructor for the Gamete class.
     * @param numberOfChromosomes The initial number of chromosomes in the gamete.
     */
    Gamete(const size_t &numberOfChromosomes);

    /**
     * @brief Destructor for the Gamete class.
     */
    ~Gamete();

    /**
     * @brief Get the size (number of chromosomes) of the gamete.
     * @return The size of the gamete.
     */
    size_t size() const;
    
    /**
     * @brief Add a new chromosome to the gamete.
     * @param newChromosome The chromosome to add.
     */
    void pushChromosome(Chromosome *newChromosome);
    
    /**
     * @brief Get a chromosome at a specific position in the gamete.
     * @param position The position of the chromosome.
     * @return A pointer to the chromosome at the specified position.
     */
    Chromosome* getChromosome(const size_t& position);

private:
    std::vector<Chromosome *> chromosomes; /**< Vector of pointers to Chromosome objects. */
};

#endif
