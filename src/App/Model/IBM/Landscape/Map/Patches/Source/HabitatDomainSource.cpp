#include "App/Model/IBM/Landscape/Map/Patches/Source/HabitatDomainSource.h"


using namespace std;
using json = nlohmann::json;


HabitatDomainSource::HabitatDomainSource(const json &config, const vector<AnimalSpecies*>& existingAnimalSpecies)
	: PathSource()
{
	for(const json& animalSpecies : config["affectedAnimalSpecies"])
	{
		vector<Instar> affectedInstars;

		for(const unsigned int instar : animalSpecies["affectedInstars"].get<vector<unsigned int>>())
		{
			affectedInstars.emplace_back(instar);
		}

		affectedAnimalSpecies.emplace_back(animalSpecies["name"].get<string>(), affectedInstars);
	}


	habitatDomainStatus.resize(existingAnimalSpecies.size());

    for(const auto animalSpecies : existingAnimalSpecies)
    {
        habitatDomainStatus[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getGrowthBuildingBlock().getNumberOfInstars(), false);
    }


	for(const auto& [name, affectedInstars] : affectedAnimalSpecies)
    {
		bool found = false;

		for(size_t i = 0; i < existingAnimalSpecies.size() && !found; i++)
		{
			if(name == existingAnimalSpecies[i]->getScientificName())
			{
				found = true;

				for(const Instar& instar : affectedInstars)
				{
					habitatDomainStatus[existingAnimalSpecies[i]->getAnimalSpeciesId()][instar] = true;
				}
			}
		}
	}
}

HabitatDomainSource::~HabitatDomainSource()
{
    
}

string HabitatDomainSource::showInfo() const
{
	ostringstream info;

	info << " - Habitat domain parameters:" << endl;

	info << "   - Affected animal species:" << endl;

	for(size_t i = 0; i < affectedAnimalSpecies.size(); i++)
	{
		if(i > 0)
		{
			info << endl;
		}

		info << "     - Animal species: \"" << affectedAnimalSpecies[i].first << "\"" << endl;

		info << "     - Affected instars = [";

		for(size_t j = 0; j < affectedAnimalSpecies[i].second.size(); j++)
		{
			if(j > 0)
			{
				info << ", ";
			}

			info << affectedAnimalSpecies[i].second[j];
		}

		info << "]" << endl;
	}

	return info.str();
}

PathSource::Type HabitatDomainSource::getType() const
{
	return Type::HabitatDomain;
}

const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>>& HabitatDomainSource::getHabitatDomainStatus() const
{
	return habitatDomainStatus;
}
