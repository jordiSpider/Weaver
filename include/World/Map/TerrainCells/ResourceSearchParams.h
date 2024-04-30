#ifndef RESOURCE_SEARCH_PARAMS_H_
#define RESOURCE_SEARCH_PARAMS_H_


#include <vector>
#include <unordered_set>

#include "World/LivingBeings/Resources/Species/ResourceSpecies.h"



typedef std::unordered_set<id_type> SearchableResourceSpecies;



class ResourceSearchParams
{
protected:
    SearchableResourceSpecies searchParams;

public:
    ResourceSearchParams();
    virtual ~ResourceSearchParams();

    void addSearchParams(const std::vector<id_type> &searchableResourceSpecies = {});

    const SearchableResourceSpecies& getSearchParams() const;
};

#endif /* RESOURCE_SEARCH_PARAMS_H_ */
