
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/CellQualityBiomass.h"


#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"


using namespace std;
using json = nlohmann::json;


CellQualityBiomass::CellQualityBiomass()
{

}

CellQualityBiomass::CellQualityBiomass(const EdibilityValue* edibilityValue)
    : edibilityValue(edibilityValue)
{
    
}

CellQualityBiomass::~CellQualityBiomass()
{
    
}


PreciseDouble CellQualityBiomass::calculateValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    return edibilityValue->calculateValue(predator, prey, preyDryMass) * calculateProximityProbability(predator, prey);
}


void CellQualityBiomass::setEdibilityValue(const EdibilityValue* newEdibilityValue)
{
    edibilityValue = newEdibilityValue;
}


PreciseDouble CellQualityBiomass::calculateProximityProbability(const AnimalNonStatistical& predator, const Edible& prey) const
{
    PreciseDouble predatorPreyDistance;

    if(prey.getSpecies()->isMobile())
    {
        predatorPreyDistance = Geometry::calculateDistanceBetweenPoints(predator.getPosition(), static_cast<const AnimalNonStatistical&>(prey).getPosition());
    }
    else
    {
        predatorPreyDistance = Geometry::calculateDistanceBetweenPointAndPolygon(predator.getPosition(), prey.getTerrainCell()->getEffectiveArea());
    }

    if(predatorPreyDistance >= predator.getScopeAreaRadius())
    {
        return 0.0;
    }
    else
    {
        return 1.0 - (predatorPreyDistance / predator.getScopeAreaRadius());
    }
}



BOOST_CLASS_EXPORT(CellQualityBiomass)

template <class Archive>
void CellQualityBiomass::serialize(Archive &, const unsigned int) {
    
}


// Specialisation
template void CellQualityBiomass::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellQualityBiomass::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellQualityBiomass::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellQualityBiomass::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
