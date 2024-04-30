#ifndef SPATIAL_TREE_ANIMAL_H_
#define SPATIAL_TREE_ANIMAL_H_


#include <memory>

#include "World/LivingBeings/Animals/Animal.h"
#include "World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "World/Map/TerrainCells/TerrainCellEvaluation.h"


class SpatialTreeAnimalFactory;


class SpatialTreeAnimal : public Animal
{
protected:
    void searchTargetToTravelTo(const double &perceptionArea, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
    void moveOneStep();
    AnimalInterface* createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, double eggMassAtBirth, double timeStep, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender,
			bool temporary=false);

public:
    SpatialTreeAnimal(double eggMassAtBirth, double timeStep, const Instar &instar, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender,
			bool temporary);
	SpatialTreeAnimal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, double eggMassAtBirth, double timeStep, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender,
			bool temporary);
    virtual ~SpatialTreeAnimal();
};

#endif /* SPATIAL_TREE_ANIMAL_H_ */
