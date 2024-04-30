#ifndef SPATIAL_TREE_ANIMAL_H_
#define SPATIAL_TREE_ANIMAL_H_


#include <memory>

#include "IBM/World/LivingBeings/Animals/AnimalNonStatistical.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "IBM/World/Map/TerrainCells/TerrainCellEvaluation.h"



class SpatialTreeAnimal : public AnimalNonStatistical
{
protected:
    void searchTargetToTravelTo(const double &perceptionArea, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
    void moveOneStep();
    AnimalNonStatistical* createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double factorEggMassFromMom, double actualTime, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender);

public:
    SpatialTreeAnimal(double timeStep, const Instar &instar, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface);
	SpatialTreeAnimal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double eggMassAtBirth, double timeStep, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender);
    virtual ~SpatialTreeAnimal();
};

#endif /* SPATIAL_TREE_ANIMAL_H_ */
