#ifndef RESOURCE_SEARCH_PARAMS_H_
#define RESOURCE_SEARCH_PARAMS_H_


#include <vector>
#include <unordered_set>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"



typedef std::unordered_set<id_type> SearchableResourceSpecies;



class ResourceSearchParams
{
private:
    friend class boost::serialization::access;

protected:
    SearchableResourceSpecies searchParams;

public:
    ResourceSearchParams();
    virtual ~ResourceSearchParams();

    void addSearchParams(const std::vector<id_type> &searchableResourceSpecies = {});

    const SearchableResourceSpecies& getSearchParams() const;

    /**
     * @brief Serialize the ResourceSearchParams object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* RESOURCE_SEARCH_PARAMS_H_ */
