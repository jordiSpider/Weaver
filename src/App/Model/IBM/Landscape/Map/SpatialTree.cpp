
#include "App/Model/IBM/Landscape/Map/SpatialTree.h"
#include "App/Model/IBM/Landscape/Landscape.h"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;



SpatialTree::SpatialTree()
    : Map()
{

}

SpatialTree::SpatialTree(const json &mapConfig, const json &, MoistureSource* const moistureBaseSource)
    : Map(mapConfig), mapDepth(calculateMapDepth()), 
      cellSizes(calculateCellSizes()),
      axisSizes(calculateAxisSizes()), totalNumberOfActiveAnimals(0)
{
    rootTerrainCell = new RootTerrainCell(cellSizes, mapDepth, new PointSpatialTree(), moistureBaseSource);
}

SpatialTree::~SpatialTree()
{
    delete rootTerrainCell;
}


unsigned int SpatialTree::calculateMapDepth() const
{
    return static_cast<unsigned int>(logMap(static_cast<double>(getNumberOfCellsPerAxis())).getValue()) + 1u;
}


PreciseDouble SpatialTree::logMap(const PreciseDouble &value) const
{
    return log(value) / log(SpatialTree::numbreOfSubdivisions);
}


vector<PreciseDouble> SpatialTree::calculateCellSizes() const
{
    vector<PreciseDouble> newCellSizes;

    for(unsigned int cellsPerAxis = getNumberOfCellsPerAxis(); cellsPerAxis > 1u; cellsPerAxis /= SpatialTree::numbreOfSubdivisions)
    {
        newCellSizes.push_back(getMinCellSize() * static_cast<double>(cellsPerAxis));
    }

    newCellSizes.push_back(getMinCellSize());

    return newCellSizes;
}

PointMap SpatialTree::obtainPointMap(const PointContinuous &point, const unsigned int depth) const
{
    vector<unsigned int> axisValues(DIMENSIONS);

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        axisValues[i] = static_cast<unsigned int>((getPositionAxisValue(point, i) / getCellSize(depth)).getValue());
    }

    return PointMap(axisValues);
}

vector<unsigned int> SpatialTree::calculateAxisSizes() const
{
    vector<unsigned int> newAxisSizes;

    for(unsigned int cellsPerAxis = 1u; cellsPerAxis <= getNumberOfCellsPerAxis(); cellsPerAxis *= SpatialTree::numbreOfSubdivisions)
    {
        newAxisSizes.push_back(cellsPerAxis);
    }

    return newAxisSizes;
}

void SpatialTree::increaseTotalNumberOfActiveAnimals()
{
    totalNumberOfActiveAnimals++;
}

void SpatialTree::decreaseTotalNumberOfActiveAnimals()
{
    totalNumberOfActiveAnimals--;
}

const unsigned int& SpatialTree::getTotalNumberOfActiveAnimals() const
{
    return totalNumberOfActiveAnimals;
}


const PreciseDouble& SpatialTree::getCellSize(const unsigned int depth) const
{
    return cellSizes[depth];
}

const unsigned int& SpatialTree::getAxisSize(const unsigned int depth) const
{
    return axisSizes[depth];
}

void SpatialTree::insertAnimal(Landscape* const landscape, Animal* const newAnimal)
{
    rootTerrainCell->insertAnimal(landscape, newAnimal);
}

void SpatialTree::update(const TimeStep& numberOfTimeSteps)
{
    rootTerrainCell->update(numberOfTimeSteps);
}


void SpatialTree::addAnimalSpecies(const AnimalSpecies& animalSpecies, const PreciseDouble& timeStepsPerDay)
{
    rootTerrainCell->addAnimalSpecies(animalSpecies, getAllAnimalsSearchParams(), timeStepsPerDay);


    for(const LifeStage &lifeStage : EnumClass<LifeStage>::getEnumValues())
    {
        lifeStageSearchParams[lifeStage].addSearchParams(
            {lifeStage}, {animalSpecies.getAnimalSpeciesId()}, 
            animalSpecies.getGrowthBuildingBlock().getInstarsRange(), EnumClass<Gender>::getEnumValues()
        );
    }

    allAnimalsSearchParams.addSearchParams(
        EnumClass<LifeStage>::getEnumValues(), {animalSpecies.getAnimalSpeciesId()},
        animalSpecies.getGrowthBuildingBlock().getInstarsRange(),
        EnumClass<Gender>::getEnumValues()
    );
}

void SpatialTree::addResourceSpecies(Landscape* const landscape, vector<vector<vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource, const PreciseDouble& timeStepsPerDay)
{
    rootTerrainCell->addResourceSpecies(landscape, landscapeResources, resourceSpecies, resourceBaseSource, getAllAnimalsSearchParams(), timeStepsPerDay);
}


bool SpatialTree::applyPatch(Landscape* const landscape, const Patch& patch)
{
    return rootTerrainCell->getMutablePatchApplicator().applyPatch(landscape, patch).second;
}



void SpatialTree::obtainGlobalTemperatureRange(unordered_set<Temperature>& globalTemperatureRange) const
{
    vector<Temperature> temperatureCycle;
    rootTerrainCell->getPatchApplicator().getCellMoisture().addTemperatureRange(globalTemperatureRange, temperatureCycle);
}


void SpatialTree::generatePointsRecursively(vector<PointSpatialTree>& points, const unsigned int sideLength, vector<unsigned int> &currentCoords, unsigned char axis, const PointSpatialTree& centralCoords, const vector<int> &initialCoords) const
{
    // Base case
    if(axis == DIMENSIONS) {
        if(currentCoords != centralCoords.getAxisValues())
        {
            points.emplace_back(currentCoords, centralCoords.getDepth());
        }
    }
    else
    {
        for(unsigned int i = 0; i < sideLength; ++i)
        {
            int axisValue = initialCoords[axis] + static_cast<int>(i);

            if(0 <= axisValue && axisValue < static_cast<const int&>(getAxisSize(centralCoords.getDepth())))
            {
                currentCoords[axis] = static_cast<unsigned int>(axisValue);
            
                vector<PointSpatialTree> subPoints;
                generatePointsRecursively(subPoints, sideLength, currentCoords, axis + 1u, centralCoords, initialCoords);
                for(auto &elem : subPoints)
                {
                    points.push_back(elem);
                }
            }
        }
    }
}

void SpatialTree::generatePointsRecursively(vector<PointSpatialTree>& points, const unsigned int sideLength, vector<unsigned int> &currentCoords, unsigned char axis, unsigned int depth, const vector<int> &initialCoords) const
{
    // Base case
    if(axis == DIMENSIONS) {
        points.emplace_back(currentCoords, depth);
    }
    else
    {
        for(unsigned int i = 0; i < sideLength; ++i)
        {
            int axisValue = initialCoords[axis] + static_cast<int>(i);

            if(0 <= axisValue && axisValue < static_cast<const int&>(getAxisSize(depth)))
            {
                currentCoords[axis] = static_cast<unsigned int>(axisValue);
            
                vector<PointSpatialTree> subPoints;
                generatePointsRecursively(subPoints, sideLength, currentCoords, axis + 1u, depth, initialCoords);
                for(auto &elem : subPoints)
                {
                    points.push_back(elem);
                }
            }
        }
    }
}


void SpatialTree::generatePoints(vector<PointSpatialTree>& points, const unsigned int sideLength, const PointSpatialTree& centralCoords, const vector<int> &initialCoords) const
{
    vector<unsigned int> currentCoords(DIMENSIONS, 0);
    generatePointsRecursively(points, sideLength, currentCoords, 0, centralCoords, initialCoords);
}

void SpatialTree::generatePoints(vector<PointSpatialTree>& points, const unsigned int sideLength, const vector<int> &initialCoords, unsigned int depth) const
{
    vector<unsigned int> currentCoords(DIMENSIONS, 0);
    generatePointsRecursively(points, sideLength, currentCoords, 0, depth, initialCoords);
}

void SpatialTree::obtainInhabitableTerrainCells()
{
    rootTerrainCell->obtainInhabitableTerrainCells(this);
}

void SpatialTree::obtainLeafAndBranchInhabitableTerrainCells(vector<vector<vector<TerrainCell*>::iterator>>& leafInhabitableTerrainCells, vector<vector<vector<TerrainCell*>::iterator>>& branchInhabitableTerrainCells)
{
    leafInhabitableTerrainCells.resize(getMapDepth());
    branchInhabitableTerrainCells.resize(getMapDepth());

    for(auto inhabitableTerrainCell = getMutableInhabitableTerrainCells().begin(); inhabitableTerrainCell != getMutableInhabitableTerrainCells().end(); ++inhabitableTerrainCell)
    {
        if(static_cast<SpatialTreeTerrainCell*>((*inhabitableTerrainCell))->isLeaf())
        {
            leafInhabitableTerrainCells.at(static_cast<const PointSpatialTree&>((*inhabitableTerrainCell)->getPosition()).getDepth()).push_back(inhabitableTerrainCell);
        }
        else
        {
            branchInhabitableTerrainCells.at(static_cast<const PointSpatialTree&>((*inhabitableTerrainCell)->getPosition()).getDepth()).push_back(inhabitableTerrainCell);
        }
    }
}



void SpatialTree::obtainSpeciesInhabitableTerrainCells(vector<CustomIndexedVector<Instar, vector<vector<TerrainCell*>::iterator>>>& mapSpeciesInhabitableTerrainCells, const vector<AnimalSpecies*>& existingAnimalSpecies, const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, std::vector<ResourceSpecies::ResourceID>>>& involvedResourceSpecies)
{
    vector<vector<vector<TerrainCell*>::iterator>> leafInhabitableTerrainCells, branchInhabitableTerrainCells;
    obtainLeafAndBranchInhabitableTerrainCells(leafInhabitableTerrainCells, branchInhabitableTerrainCells);

    mapSpeciesInhabitableTerrainCells.resize(existingAnimalSpecies.size());

    for(const AnimalSpecies* const &animalSpecies : existingAnimalSpecies)
    {
        mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getGrowthBuildingBlock().getNumberOfInstars());

        for(const Instar &instar : animalSpecies->getGrowthBuildingBlock().getInstarsRange())
        {
            for(unsigned int depth = 0; depth <= animalSpecies->getInstarCellDepth(instar); depth++)
            {
                for(auto &potencialInhabitableTerrainCell : leafInhabitableTerrainCells.at(depth))
                {
                    if(isSpeciesInhabitableTerrainCell(*animalSpecies, *potencialInhabitableTerrainCell, involvedResourceSpecies[animalSpecies->getAnimalSpeciesId()][instar]))
                    {
                        // Se añaden tantas referencia como celdas de la profundidad del instar ocupa la celda original
                        for(unsigned int i = 0; i < pow(2, (animalSpecies->getInstarCellDepth(instar)-depth)*DIMENSIONS); i++)
                        {
                            mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar].push_back(potencialInhabitableTerrainCell);
                        }
                    }
                }
            }

            for(auto &potencialInhabitableTerrainCell : branchInhabitableTerrainCells.at(animalSpecies->getInstarCellDepth(instar)))
            {
                if(isSpeciesInhabitableTerrainCell(*animalSpecies, *potencialInhabitableTerrainCell, involvedResourceSpecies[animalSpecies->getAnimalSpeciesId()][instar]))
                {
                    mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar].push_back(potencialInhabitableTerrainCell);
                }
            }
        }
    }
}


unsigned int SpatialTree::generateStatisticsPopulation(vector<CustomIndexedVector<Instar, vector<AnimalStatistical*>>>& animalsPopulation, View* view, Landscape* const landscape, vector<AnimalSpecies*>& existingAnimalSpecies, vector<CustomIndexedVector<Instar, vector<vector<TerrainCell*>::iterator>>> &mapSpeciesInhabitableTerrainCells, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    unsigned int populationSize = 0;
    animalsPopulation.resize(existingAnimalSpecies.size());

	for(AnimalSpecies* &animalSpecies : existingAnimalSpecies)
    {
        view->updateLog({" - Animal species ", animalSpecies->getScientificName(), "\n"});

        ProgressBar progressBar(view, animalSpecies->getGrowthBuildingBlock().getInstarsRange().size() * animalSpecies->getStatisticsIndividualsPerInstar());

        animalsPopulation.at(animalSpecies->getAnimalSpeciesId()).resize(animalSpecies->getGrowthBuildingBlock().getNumberOfInstars());

		for(const Instar &instar : animalSpecies->getGrowthBuildingBlock().getInstarsRange())
		{
			for(unsigned long individual = 0; individual < animalSpecies->getStatisticsIndividualsPerInstar(); individual++)
			{
				AnimalStatistical* newAnimal;

                // Get a random index from this species inhabitable cells
                size_t randomCellIndex = Random::randomIndex(mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar].size());
                
                
                auto newTerrainCell = (*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar][randomCellIndex])->randomInsertAnimal(landscape, instar, animalSpecies, true, false, false, actualTimeStep, timeStepsPerDay);
                
                if(get<0>(newTerrainCell))
                {
                    bool found = false;
                    for(unsigned int i = 0; i < mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar].size() && !found; i++)
                    {
                        if((*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar][i]) == get<1>(newTerrainCell))
                        {
                            (*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar][i]) = get<2>(newTerrainCell);
                            found = true;
                        }
                    }
                }

                newAnimal = static_cast<AnimalStatistical*>(get<3>(newTerrainCell));


				animalsPopulation.at(animalSpecies->getAnimalSpeciesId()).at(instar).push_back(newAnimal);
                populationSize++;


                progressBar.update();
			}
		}
	}

    return populationSize;
}

PointContinuous SpatialTree::calculateCenterPosition(const PointSpatialTree &cell, const PreciseDouble &size) const
{
    PointContinuous center;
    boost::geometry::assign_zero(center);

    PreciseDouble halfOfSize = size / 2.0;

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        setPositionAxisValue(center, i, static_cast<double>(cell.get(axis)) * size + halfOfSize);
    }

    return center;
}

unsigned int SpatialTree::getMapDepth() const
{
    return mapDepth;
}

size_t SpatialTree::generatePopulation(View* view, Landscape* const landscape, AnimalSpecies* currentAnimalSpecies, const CustomIndexedVector<Instar, unsigned int>& population, const CustomIndexedVector<Instar, vector<vector<TerrainCell*>::iterator>> &speciesInhabitableTerrainCells, const bool saveAnimalConstitutiveTraits, std::ostringstream& animalConstitutiveTraitsFile, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    size_t numberOfDiscardedIndividualsOutsideRestrictedRanges = 0;

    unsigned int totalInitialPopulation = 0;

    for(const unsigned int instarPopulation : population)
    {
        totalInitialPopulation += instarPopulation;
    }

    view->updateLog({"Creating ", to_string(totalInitialPopulation), " individuals of the species \"", currentAnimalSpecies->getScientificName(), "\"...\n"});

    ProgressBar progressBar(view, totalInitialPopulation);

    size_t randomCellIndex;

    for(const Instar &instar : currentAnimalSpecies->getGrowthBuildingBlock().getInstarsRange())
    {
        for (unsigned long individual = 0; individual < population[instar]; individual++)
        {
            AnimalNonStatistical* newAnimal;

            // Get a random index from this species inhabitable cells
            randomCellIndex = Random::randomIndex(speciesInhabitableTerrainCells[instar].size());

            auto newTerrainCell = (*speciesInhabitableTerrainCells[instar][randomCellIndex])->randomInsertAnimal(landscape, instar, currentAnimalSpecies, false, saveGenetics, saveMassInfo, actualTimeStep, timeStepsPerDay);

            if(get<0>(newTerrainCell))
            {
                bool found = false;
                for(unsigned int i = 0; i < speciesInhabitableTerrainCells[instar].size() && !found; i++)
                {
                    if((*speciesInhabitableTerrainCells[instar][i]) == get<1>(newTerrainCell))
                    {
                        (*speciesInhabitableTerrainCells[instar][i]) = get<2>(newTerrainCell);
                        found = true;
                    }
                }
            }

            newAnimal = static_cast<AnimalNonStatistical*>(get<3>(newTerrainCell));

            numberOfDiscardedIndividualsOutsideRestrictedRanges += get<4>(newTerrainCell);
            
            newAnimal->calculateGrowthCurves(timeStepsPerDay);
            newAnimal->forceMolting(landscape, actualTimeStep, timeStepsPerDay);

            if(saveAnimalConstitutiveTraits)
            {
                newAnimal->printTraits(animalConstitutiveTraitsFile);
            }

            progressBar.update();
        }
    }

	return numberOfDiscardedIndividualsOutsideRestrictedRanges;
}

void SpatialTree::deserializeSources(
        vector<pair<size_t, MoistureSource*>>& appliedMoisture, 
        std::vector<vector<pair<size_t, ResourceSource*>>>& appliedResource
    )
{
    rootTerrainCell->deserializeSources(appliedMoisture, appliedResource);
}

void SpatialTree::deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies, std::vector<AnimalSpecies*>& existingAnimalSpecies)
{
    rootTerrainCell->deserializeSpecies(existingResourceSpecies, existingAnimalSpecies);
}

string SpatialTree::getMapPositionHeader() const
{
    string header = static_cast<string>(magic_enum::enum_names<Axis>()[0]);

    for(unsigned int axis = 1; axis < DIMENSIONS; axis++)
    {
        header += "\t" + static_cast<string>(magic_enum::enum_names<Axis>()[axis]);
    }

    return header;
}


void SpatialTree::obtainResourceBiomass(vector<WetMass> &landscapeResourceBiomass) const
{
    for(size_t i = 0; i < rootTerrainCell->getPatchApplicator().getNumberOfResources(); i++)
    {
        landscapeResourceBiomass[i] = landscapeResourceBiomass[i] + rootTerrainCell->getPatchApplicator().getCellResource(i).getGrowthBuildingBlock().getCurrentTotalWetMass();
    }
}

void SpatialTree::registerEdibles(vector<vector<vector<CellResource*>>>& landscapeResources, vector<AnimalNonStatistical*>& landscapeAnimals)
{
    rootTerrainCell->registerEdibles(landscapeResources, landscapeAnimals);
}


void SpatialTree::saveResourceSpeciesSnapshot(ofstream &file, const ResourceSpecies* const &species) const
{
    rootTerrainCell->saveResourceSpeciesSnapshot(file, species);
}


void SpatialTree::saveWaterSnapshot(ofstream &file) const
{
    rootTerrainCell->saveWaterSnapshot(file);
}

SpatialTree::Type SpatialTree::getType() const
{
    return Type::SpatialTree;
}




BOOST_CLASS_EXPORT(SpatialTree)

template <class Archive>
void SpatialTree::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<Map>(*this);
    
    ar & mapDepth;
    ar & cellSizes;
    ar & axisSizes;
    ar & totalNumberOfActiveAnimals;

    ar & rootTerrainCell;
}

// Specialisation
template void SpatialTree::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpatialTree::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpatialTree::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpatialTree::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
