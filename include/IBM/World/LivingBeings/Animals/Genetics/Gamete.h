#ifndef GAMETES_H_
#define GAMETES_H_

#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Chromosome.h"
#include "IBM/Maths/Random.h"


/**
 * @class Gamete
 * @brief Represents a gamete with a variable number of chromosomes.
 */
class Gamete
{
private:
    std::vector<Chromosome *> chromosomes; /**< Vector of pointers to Chromosome objects. */

    friend class boost::serialization::access;

public:
    /**
     * @brief Constructor for the Gamete class.
     * @param numberOfChromosomes The initial number of chromosomes in the gamete.
     */
    Gamete(const unsigned int &numberOfChromosomes);

    /**
     * @brief Destructor for the Gamete class.
     */
    ~Gamete();

    /**
     * @brief Get the size (number of chromosomes) of the gamete.
     * @return The size of the gamete.
     */
    inline unsigned int size() const { return chromosomes.size(); }
    
    /**
     * @brief Add a new chromosome to the gamete.
     * @param newChromosome The chromosome to add.
     */
    inline void pushChromosome(Chromosome *newChromosome) { chromosomes.emplace_back(newChromosome); }
    
    /**
     * @brief Get a chromosome at a specific position in the gamete.
     * @param position The position of the chromosome.
     * @return A pointer to the chromosome at the specified position.
     */
    inline Chromosome* getChromosome(const unsigned int& position) { return chromosomes.at(position); }

    /**
     * @brief Serialize the object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif
