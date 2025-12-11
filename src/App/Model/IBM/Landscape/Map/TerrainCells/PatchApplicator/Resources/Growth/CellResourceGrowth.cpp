#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/Growth/CellResourceGrowth.h"

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.h"


using namespace std;




CellResourceGrowth::CellResourceGrowth()
	: CellResourceInterfaceGrowth()
{

}

CellResourceGrowth::CellResourceGrowth(CellResourceInterface* owner, ResourceSpeciesGrowth* const newSpeciesGrowth, ResourceSource* const newResourceSource)
	: CellResourceInterfaceGrowth(owner, newSpeciesGrowth), resourceSource(newResourceSource), fullCapacity(true)
{

}

CellResourceGrowth::~CellResourceGrowth()
{

}

void CellResourceGrowth::applySource(const bool competitionAmongResourceSpecies, const ResourceSource* const source)
{
	setResourceSource(source);

	setCurrentTotalWetMass(WetMass(getSource().getGrowthDynamics().getInitialValue()));

	updateFullCapacityStatus(competitionAmongResourceSpecies);
}

void CellResourceGrowth::setResourceSource(const ResourceSource* const source)
{
	resourceSource = source;
}

const ResourceSource& CellResourceGrowth::getSource() const
{
	return *resourceSource;
}

void CellResourceGrowth::setCurrentTotalWetMass(const WetMass& newCurrentTotalWetMass)
{
	WetMass newBiomass;

	if(newCurrentTotalWetMass < getMinimumEdibleBiomass()) {
		newBiomass = getMinimumEdibleBiomass();
	}
	else {
		newBiomass = newCurrentTotalWetMass;
	}

	static_cast<LeafPatchApplicator&>(getMutableOwner()->getMutableTerrainCell()->getMutablePatchApplicator()).removeResourceBiomass(getCurrentTotalWetMass());

	EdibleGrowth::setCurrentTotalWetMass(newBiomass);

	static_cast<LeafPatchApplicator&>(getMutableOwner()->getMutableTerrainCell()->getMutablePatchApplicator()).addResourceBiomass(getCurrentTotalWetMass());
}

void CellResourceGrowth::updateFullCapacityStatus(const bool competitionAmongResourceSpecies)
{
	if(competitionAmongResourceSpecies)
	{
		if(static_cast<const LeafPatchApplicator&>(getOwner()->getTerrainCell()->getPatchApplicator()).getTotalResourceBiomass() >= getOwner()->getTerrainCell()->getPatchApplicator().getCellMoisture().getTotalMaximumResourceCapacity())
		{
			static_cast<LeafPatchApplicator&>(getMutableOwner()->getMutableTerrainCell()->getMutablePatchApplicator()).setTotalFullCapacity(true);
		}
		else
		{
			static_cast<LeafPatchApplicator&>(getMutableOwner()->getMutableTerrainCell()->getMutablePatchApplicator()).setTotalFullCapacity(false);
		}
	}
	else
	{
		if(getCurrentTotalWetMass() >= getSource().getResourceMaximumCapacity())
		{
			setFullCapacity(true);
		}
		else
		{
			setFullCapacity(false);
		}
	}
}

bool CellResourceGrowth::isFullCapacity(const bool competitionAmongResourceSpecies) const
{
	if(competitionAmongResourceSpecies)
	{
		return static_cast<const LeafPatchApplicator&>(getOwner()->getTerrainCell()->getPatchApplicator()).isTotalFullCapacity();
	}
	else
	{
		return fullCapacity;
	}
}

void CellResourceGrowth::setFullCapacity(const bool newFullCapacityValue)
{
	fullCapacity = newFullCapacityValue;
}

WetMass CellResourceGrowth::growth(Landscape* const landscape, const PreciseDouble& moisture, const Temperature& temperature)
{
	WetMass excess(0.0);


	if(moisture >= getSpeciesGrowth()->getMinRH() && moisture <= getSpeciesGrowth()->getMaxRH())
	{
		const WetMass newBiomass(fmax(
			getSource().getGrowthDynamics().getValue(
				getCurrentTotalWetMass().getValue(), landscape->getTimeStepsPerDay(), 
				temperature, moisture, getSpeciesGrowth()->getMinRH(), getSpeciesGrowth()->getMaxRH()
			),
			0.0
		));


		if(newBiomass >= getCurrentTotalWetMass())
		{
			const WetMass availableCapacity = calculateAvailableCapacity(landscape->getCompetitionAmongResourceSpecies());

			const WetMass addedMass(newBiomass - getCurrentTotalWetMass());

			if(addedMass >= availableCapacity)
			{
				excess = addedMass - availableCapacity;
				setCurrentTotalWetMass(getCurrentTotalWetMass() + availableCapacity);
			}
			else
			{
				setCurrentTotalWetMass(newBiomass);
			}
		}
		else
		{
			setCurrentTotalWetMass(newBiomass);
		}


		updateFullCapacityStatus(landscape->getCompetitionAmongResourceSpecies());
	}


	return excess;
}

WetMass CellResourceGrowth::addBiomass(const WetMass& addedMass, const bool competitionAmongResourceSpecies)
{
	WetMass excess(0.0);


	if(!isFullCapacity(competitionAmongResourceSpecies))
	{
		const WetMass availableCapacity = calculateAvailableCapacity(competitionAmongResourceSpecies);

		if(addedMass >= availableCapacity)
		{
			excess = addedMass - availableCapacity;
			setCurrentTotalWetMass(getCurrentTotalWetMass() + availableCapacity);
		}
		else
		{
			setCurrentTotalWetMass(getCurrentTotalWetMass() + addedMass);
		}

		updateFullCapacityStatus(competitionAmongResourceSpecies);
	}


	return excess;
}

WetMass CellResourceGrowth::calculateAvailableCapacity(const bool competitionAmongResourceSpecies) const
{
	if(competitionAmongResourceSpecies)
	{
		return getOwner()->getTerrainCell()->getPatchApplicator().getCellMoisture().getTotalMaximumResourceCapacity() - static_cast<const LeafPatchApplicator&>(getOwner()->getTerrainCell()->getPatchApplicator()).getTotalResourceBiomass();
	}
	else
	{
		return getSource().getResourceMaximumCapacity() - getCurrentTotalWetMass();
	}
}

void CellResourceGrowth::substractBiomass(const DryMass& dryMassToBeSubstracted, const bool competitionAmongResourceSpecies)
{
	CellResourceInterfaceGrowth::substractBiomass(dryMassToBeSubstracted);

	if(isFullCapacity(competitionAmongResourceSpecies))
	{
		updateFullCapacityStatus(competitionAmongResourceSpecies);
	}
}

const WetMass& CellResourceGrowth::getMinimumEdibleBiomass() const
{
	return getSource().getMinimumEdibleBiomass();
}




BOOST_CLASS_EXPORT(CellResourceGrowth)

template <class Archive>
void CellResourceGrowth::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<CellResourceInterfaceGrowth>(*this);

	ar & fullCapacity;
}

// Specialisation
template void CellResourceGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellResourceGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellResourceGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellResourceGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
