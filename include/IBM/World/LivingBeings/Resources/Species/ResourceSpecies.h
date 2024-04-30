#ifndef WEAVER_RESOURCESPECIES_H_
#define WEAVER_RESOURCESPECIES_H_

#include <math.h>
#include <unordered_map>
#include <string>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/Maths/Parabola.h"
#include "IBM/World/LivingBeings/Species.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/Physics/Temperature.h"
#include "Misc/Macros.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"


class ResourceSpecies: public Species
{
public:
	enum class Type {
		BasalResource, 
		NoBasalResource
	};

	class ResourceID
	{
	public:
		ResourceID();
		ResourceID(const id_type& value);

		const id_type& getValue() const;
		operator size_t() const;

		template <class Archive>
    	void serialize(Archive &ar, const unsigned int version);

	private:
		id_type value;
	};
	

	static std::unique_ptr<ResourceSpecies> createInstance(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const nlohmann::json &resourceSpeciesInfo, WorldInterface* const worldInterface);
	static std::unique_ptr<ResourceSpecies> createInstance(const Type& resourceSpeciesType, WorldInterface* const worldInterface);


	ResourceSpecies(WorldInterface* const worldInterface);
	ResourceSpecies(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const nlohmann::json &resourceSpeciesInfo, WorldInterface* const worldInterface);
	virtual ~ResourceSpecies();

	const ResourceID& getResourceSpeciesId() const;
	virtual const Type getResourceSpeciesType() const=0;

	double getCellMass() const;
	bool getVariableIntrinsicRateOfIncrease() const;
	double getMaxRScale() const;
	void setCellMass(const double& cellMass);
	void setGrowingRateParams(const double& minHR, const double& maxHR, const double& maxRscale, const bool& variableIntrinsicRateOfIncrease);
	void setMinimumEdibleBiomass(const double& minimumNumberOfEdibleSpores, const double& cellMass);
	virtual double getMinHR() const;
	virtual double getMaxHR() const;
	virtual double getRateOfGrowth(Temperature temperature, double moisture, double massRatio);
	virtual double getMinimumEdibleBiomass() const;
	virtual bool isMobile() const;
	virtual float getAttackProbability() const;
	virtual float getExposedAttackProbability() const;
	virtual float getKillProbability() const;
	virtual float getAttackAvoidingProbability() const;
	virtual float getKillAvoidingProbability() const;
	void updateK_Density(const double &resourceMaximumCapacityDensity);
	void generateInstarInvolvedResourceSpecies(
		const Instar &instar, std::vector<ResourceSpecies::ResourceID> &instarInvolvedResourceSpecies, 
		std::vector<CustomIndexedVector<Instar, bool>> &alreadyCheckedSpecies
	);
	double calculateInstarK_Density(const Instar &instar, std::vector<CustomIndexedVector<Instar, bool>> &checkedAnimalSpecies, 
		std::vector<CustomIndexedVector<Instar, bool>> &cannibalismAnimalSpecies
	);

	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
	ResourceID resourceSpeciesId;
	double cellMass;

	double ACTIVATION_ENERGY;
	double NORMALIZATION_B;

	// Moisture limits for resource grow in % percent
	double minHR;
	double maxHR;

	// Maximum growing rate
	double maxR;
	double maxRScale;
	bool variableIntrinsicRateOfIncrease;
	double minimumEdibleBiomass;
	Parabola growingParabola;
};


namespace boost {
    namespace serialization {
		/**
         * @brief Serialize a ResourceSpecies pointer.
         *
         * This function handles the serialization and deserialization of a ResourceSpecies pointer.
         *
         * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
         * @param ar The archive to use.
         * @param resourceSpeciesPtr The ResourceSpecies pointer to serialize or deserialize.
         * @param version The version of the serialization format.
         */
		template <class Archive>
        void serialize(Archive &ar, ResourceSpecies* &resourceSpeciesPtr, const unsigned int version, WorldInterface* const newWorldInterface);
	}
}

template <>
struct std::hash<ResourceSpecies::ResourceID> {
	std::size_t operator()(const ResourceSpecies::ResourceID& resourceID) const;
};

#endif /* WEAVER_RESOURCESPECIES_H_ */
