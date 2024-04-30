
#include "IBM/World/Map/SpatialTree/SpatialTree.h"
#include "IBM/World/WorldInterface.h"

using namespace std;
using json = nlohmann::json;




SpatialTree::SpatialTree(const json &mapConfig, const json &spatialTreeConfig, WorldInterface* const worldInterface)
    : SpatialTreeInterface(mapConfig, worldInterface), mapDepth(calculateMapDepth()), 
      cellSizes(calculateCellSizes()),
      axisSizes(calculateAxisSizes()), totalNumberOfTerrainCells(calculateTotalNumberOfTerrainCells()), 
      rootTerrainCell(make_unique<RootTerrainCell>(this))
{
    
}


SpatialTree::~SpatialTree()
{
    
}


const unsigned int SpatialTree::calculateMapDepth() const
{
    return static_cast<unsigned int>(logMap(getNumberOfCellsPerAxis())) + 1;
}


double SpatialTree::logMap(const double &value) const
{
    return log(value) / log(SpatialTreeInterface::numbreOfSubdivisions);
}


vector<double> SpatialTree::calculateCellSizes() const
{
    vector<double> cellSizes;

    for(unsigned int cellsPerAxis = getNumberOfCellsPerAxis(); cellsPerAxis > 1; cellsPerAxis /= SpatialTreeInterface::numbreOfSubdivisions)
    {
        cellSizes.push_back(getMinCellSize() * static_cast<double>(cellsPerAxis));
    }

    cellSizes.push_back(getMinCellSize());

    return cellSizes;
}

PointMap SpatialTree::obtainPointMap(const PointContinuous &point, const unsigned int depth) const
{
    vector<unsigned int> axisValues(DIMENSIONS);

    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        axisValues[i] = static_cast<unsigned int>(getPositionAxisValue(point, i) / getCellSize(depth));
    }

    return PointMap(axisValues);
}

vector<unsigned int> SpatialTree::calculateAxisSizes() const
{
    vector<unsigned int> axisSizes;

    for(unsigned int cellsPerAxis = 1; cellsPerAxis <= getNumberOfCellsPerAxis(); cellsPerAxis *= SpatialTreeInterface::numbreOfSubdivisions)
    {
        axisSizes.push_back(cellsPerAxis);
    }

    return axisSizes;
}

unsigned int SpatialTree::calculateTotalNumberOfTerrainCells() const
{
    unsigned int totalNumberOfTerrainCells = 0;

    for(unsigned int depth = 0; depth < getMapDepth(); depth++)
    {
        totalNumberOfTerrainCells += pow(getAxisSize(depth), DIMENSIONS);
    }

    return totalNumberOfTerrainCells;
}

const unsigned int& SpatialTree::getTotalNumberOfTerrainCells() const
{
    return totalNumberOfTerrainCells;
}

SpatialTreeTerrainCellInterface* const SpatialTree::getCell(const PointMap &cellPos) const
{
    return getCell(static_cast<const PointSpatialTree &>(cellPos), rootTerrainCell.get());
}

SpatialTreeTerrainCellInterface* const SpatialTree::getCell(const PointSpatialTree &dstPos, SpatialTreeTerrainCellInterface* const &srcCell) const
{
    if(dstPos == srcCell->getPosition())
    {
        return srcCell;
    }
    else
    {
        SpatialTreeTerrainCellInterface* riseSrcCell = srcCell;

        while(!riseSrcCell->isChild(dstPos))
        {
            riseSrcCell = riseSrcCell->getMutableParent();
        }

        return riseSrcCell->getCell(dstPos);
    }
}


const double& SpatialTree::getCellSize(const unsigned int depth) const
{
    return cellSizes[depth];
}

const unsigned int& SpatialTree::getAxisSize(const unsigned int depth) const
{
    return axisSizes[depth];
}

void SpatialTree::insertAnimal(Animal* const newAnimal)
{
    rootTerrainCell->insertAnimal(newAnimal);
}

void SpatialTree::updateMoisture()
{
    rootTerrainCell->updateMoisture();
}

void SpatialTree::update(const unsigned int timeStep, ofstream &tuneTraitsFile)
{
    rootTerrainCell->update(timeStep, tuneTraitsFile);
}


void SpatialTree::applyObstaclePatch(const unique_ptr<const ObstaclePatch> &obstaclePatch)
{
    rootTerrainCell->checkObstaclePatch(*obstaclePatch.get());
}


bool SpatialTree::applyMoisturePatch(const MoisturePatch &moisturePatch)
{
    return rootTerrainCell->checkMoisturePatch(moisturePatch).second;
}


bool SpatialTree::applyResourcePatch(const ResourcePatch &resourcePatch)
{
    return rootTerrainCell->checkResourcePatch(resourcePatch);
}


unique_ptr<vector<TerrainCellInterface*>> SpatialTree::getResourceNeighbours(TerrainCellInterface* const &source, const unsigned int resourceSpeciesId, const unsigned int distance)
{
    #ifdef DEBUG
    if(distance < 1)
    {
        throwLineInfoException("The minimum required search distance is 1");
    }
    #endif

    auto potentialNeighbours = getManhattanNeighbours(source, distance);

    auto resourceNeighbours = make_unique<vector<TerrainCellInterface*>>();

    for(auto &neighbour : *potentialNeighbours)
    {
        if(neighbour->resourceIsPresent(resourceSpeciesId))
        {
            if(!neighbour->getResource(resourceSpeciesId)->isFullCapacity())
            {
                resourceNeighbours->push_back(neighbour);
            }
        }
    }

    return resourceNeighbours;
}

unique_ptr<vector<TerrainCellInterface*>> SpatialTree::getManhattanNeighbours(TerrainCellInterface* const &source, const unsigned int distance)
{
    #ifdef DEBUG
    if(distance < 1)
    {
        throwLineInfoException("The minimum required search distance is 1");
    }
    #endif

    vector<int> initialCoords(DIMENSIONS);

    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        initialCoords[i] = source->getPosition().get(axis) - distance;
    }

    auto neighboursPoints = generatePoints(2*distance + 1, static_cast<const PointSpatialTree&>(source->getPosition()), initialCoords);

    auto neighbours = make_unique<vector<TerrainCellInterface*>>(neighboursPoints->size());

    for(unsigned int i = 0; i < neighbours->size(); i++)
    {
        neighbours->at(i) = getCell(neighboursPoints->at(i), static_cast<SpatialTreeTerrainCellInterface*>(source));
    }

    return neighbours;
}


unique_ptr<vector<PointSpatialTree>> SpatialTree::generatePointsRecursively(const unsigned int sideLength, vector<unsigned int> &currentCoords, unsigned int axis, const PointSpatialTree& centralCoords, const vector<int> &initialCoords) const
{
    auto points = make_unique<vector<PointSpatialTree>>();

    // Base case
    if(axis == DIMENSIONS) {
        if(currentCoords != centralCoords.getAxisValues())
        {
            points->emplace_back(currentCoords, centralCoords.getDepth());
        }
    }
    else
    {
        for(unsigned int i = 0; i < sideLength; ++i)
        {
            int axisValue = initialCoords[axis] + i;

            if(0 <= axisValue && axisValue < static_cast<const int&>(getAxisSize(centralCoords.getDepth())))
            {
                currentCoords[axis] = static_cast<unsigned int>(axisValue);
            
                auto subPoints = generatePointsRecursively(sideLength, currentCoords, axis + 1, centralCoords, initialCoords);
                for(auto &elem : *subPoints)
                {
                    points->push_back(elem);
                }
            }
        }
    }
    
    return points;
}

unique_ptr<vector<PointSpatialTree>> SpatialTree::generatePointsRecursively(const unsigned int sideLength, vector<unsigned int> &currentCoords, unsigned int axis, unsigned int depth, const vector<int> &initialCoords) const
{
    auto points = make_unique<vector<PointSpatialTree>>();

    // Base case
    if(axis == DIMENSIONS) {
        points->emplace_back(currentCoords, depth);
    }
    else
    {
        for(unsigned int i = 0; i < sideLength; ++i)
        {
            int axisValue = initialCoords[axis] + i;

            if(0 <= axisValue && axisValue < static_cast<const int&>(getAxisSize(depth)))
            {
                currentCoords[axis] = static_cast<unsigned int>(axisValue);
            
                auto subPoints = generatePointsRecursively(sideLength, currentCoords, axis + 1, depth, initialCoords);
                for(auto &elem : *subPoints)
                {
                    points->push_back(elem);
                }
            }
        }
    }
    
    return points;
}


unique_ptr<vector<PointSpatialTree>> SpatialTree::generatePoints(const unsigned int sideLength, const PointSpatialTree& centralCoords, const vector<int> &initialCoords) const
{
    vector<unsigned int> currentCoords(DIMENSIONS, 0);
    return generatePointsRecursively(sideLength, currentCoords, 0, centralCoords, initialCoords);
}

unique_ptr<vector<PointSpatialTree>> SpatialTree::generatePoints(const unsigned int sideLength, const vector<int> &initialCoords, unsigned int depth) const
{
    vector<unsigned int> currentCoords(DIMENSIONS, 0);
    return generatePointsRecursively(sideLength, currentCoords, 0, depth, initialCoords);
}

void SpatialTree::obtainInhabitableTerrainCells()
{
    rootTerrainCell->obtainInhabitableTerrainCells();
}

pair<unique_ptr<vector<vector<vector<TerrainCellInterface *>::iterator>>>, unique_ptr<vector<vector<vector<TerrainCellInterface *>::iterator>>>> SpatialTree::obtainLeafAndBranchInhabitableTerrainCells()
{
    auto leafInhabitableTerrainCells = make_unique<vector<vector<vector<TerrainCellInterface *>::iterator>>>(getMapDepth());
    auto branchInhabitableTerrainCells = make_unique<vector<vector<vector<TerrainCellInterface *>::iterator>>>(getMapDepth());

    for(auto inhabitableTerrainCell = getMutableInhabitableTerrainCells().begin(); inhabitableTerrainCell != getMutableInhabitableTerrainCells().end(); ++inhabitableTerrainCell)
    {
        if(static_cast<SpatialTreeTerrainCellInterface*>((*inhabitableTerrainCell))->isLeaf())
        {
            leafInhabitableTerrainCells->at(static_cast<const PointSpatialTree&>((*inhabitableTerrainCell)->getPosition()).getDepth()).push_back(inhabitableTerrainCell);
        }
        else
        {
            branchInhabitableTerrainCells->at(static_cast<const PointSpatialTree&>((*inhabitableTerrainCell)->getPosition()).getDepth()).push_back(inhabitableTerrainCell);
        }
    }

    return make_pair(move(leafInhabitableTerrainCells), move(branchInhabitableTerrainCells));
}



unique_ptr<vector<InstarVector<vector<vector<TerrainCellInterface*>::iterator>>>> SpatialTree::obtainSpeciesInhabitableTerrainCells()
{
    unique_ptr<vector<vector<vector<TerrainCellInterface *>::iterator>>> leafInhabitableTerrainCells, branchInhabitableTerrainCells;

    tie(leafInhabitableTerrainCells, branchInhabitableTerrainCells) = obtainLeafAndBranchInhabitableTerrainCells();

    auto mapSpeciesInhabitableTerrainCells = make_unique<vector<InstarVector<vector<vector<TerrainCellInterface*>::iterator>>>>(getMutableWorldInterface()->getExistingAnimalSpecies().size());

    for(const auto &animalSpecies : getMutableWorldInterface()->getMutableExistingAnimalSpecies())
    {
        (*mapSpeciesInhabitableTerrainCells)[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getNumberOfInstars());

        for(const Instar &instar : animalSpecies->getInstarsRange())
        {
            for(unsigned int depth = 0; depth <= animalSpecies->getInstarCellDepth(instar); depth++)
            {
                for(auto &potencialInhabitableTerrainCell : leafInhabitableTerrainCells->at(depth))
                {
                    if(isSpeciesInhabitableTerrainCell(*animalSpecies, *potencialInhabitableTerrainCell, instar))
                    {
                        // Se añaden tantas referencia como celdas de la profundidad del instar ocupa la celda original
                        for(unsigned int i = 0; i < pow(2, (animalSpecies->getInstarCellDepth(instar)-depth)*DIMENSIONS); i++)
                        {
                            (*mapSpeciesInhabitableTerrainCells)[animalSpecies->getAnimalSpeciesId()][instar].push_back(potencialInhabitableTerrainCell);
                        }
                    }
                }
            }

            for(auto &potencialInhabitableTerrainCell : branchInhabitableTerrainCells->at(animalSpecies->getInstarCellDepth(instar)))
            {
                if(isSpeciesInhabitableTerrainCell(*animalSpecies, *potencialInhabitableTerrainCell, instar))
                {
                    (*mapSpeciesInhabitableTerrainCells)[animalSpecies->getAnimalSpeciesId()][instar].push_back(potencialInhabitableTerrainCell);
                }
            }
        }
    }

    return mapSpeciesInhabitableTerrainCells;
}

void SpatialTree::obtainMapCellsInfo(vector<pair<vector<double>, vector<unsigned int>>> &mapCellsInfo)
{
    rootTerrainCell->printCell(mapCellsInfo);
}

const unsigned int SpatialTree::calculateAnimalDepth(const AnimalInterface* const newAnimal)
{
    const double numberOfMinCells = newAnimal->getCurrentLength() / getMinCellSize();

    if(numberOfMinCells < 1.0)
    {
        return getMapDepth() - 1;
    }
    else
    {
        return getMapDepth() - ceil(logMap(numberOfMinCells)) - 1;
    }
}


pair<unique_ptr<vector<InstarVector<vector<AnimalInterface*>>>>, unsigned int> SpatialTree::generateStatisticsPopulation(vector<InstarVector<vector<vector<TerrainCellInterface*>::iterator>>> &mapSpeciesInhabitableTerrainCells)
{
    unsigned int populationSize = 0;
    auto animalsPopulation = make_unique<vector<InstarVector<vector<AnimalInterface*>>>>(getWorldInterface()->getExistingAnimalSpecies().size()); 

	for(auto &animalSpecies : getMutableWorldInterface()->getMutableExistingAnimalSpecies())
    {
        animalsPopulation->at(animalSpecies->getAnimalSpeciesId()).resize(animalSpecies->getNumberOfInstars());

		float percentageForPrinting = 0.1;
		float currentPercentageForPrinting = percentageForPrinting;

		unsigned int randomCellIndex;

        for(const Instar &instar : animalSpecies->getInstarsRange())
		{
			for(unsigned long individual = 0; individual < animalSpecies->getStatisticsIndividualsPerInstar(); individual++)
			{
				auto newAnimal = SpatialTreeAnimalFactory::createInstance(-1.0, 0.0, instar, 0, 0, -1,
						-1, animalSpecies, animalSpecies->getRandomGender());

                // Get a random index from this species inhabitable cells
                randomCellIndex = Random::randomIndex(mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][newAnimal->getInstar()].size());
                
                try
                {
                    auto newTerrainCell = (*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][newAnimal->getInstar()][randomCellIndex])->randomInsertAnimal(newAnimal);
                
                    if(get<0>(newTerrainCell))
                    {
                        bool found = false;
                        for(unsigned int i = 0; i < mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][newAnimal->getInstar()].size() && !found; i++)
                        {
                            if((*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][newAnimal->getInstar()][i]) == get<1>(newTerrainCell))
                            {
                                (*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][newAnimal->getInstar()][i]) = get<2>(newTerrainCell);
                                found = true;
                            }
                        }
                    }
                }
                catch(SpatialTreeTerrainCell::TemporalLeaf2Branch &e)
                {
                    (*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][newAnimal->getInstar()][randomCellIndex]) = static_cast<SpatialTreeTerrainCellInterface*>((*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][newAnimal->getInstar()][randomCellIndex]))->getMutableParent()->randomInsertAnimalOnChild(
                        newAnimal, (*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][newAnimal->getInstar()][randomCellIndex])
                    );
                }

				newAnimal->updateLimits();
				newAnimal->sumPseudoGrowthMean();

				animalsPopulation->at(animalSpecies->getAnimalSpeciesId()).at(instar).push_back(newAnimal);
                populationSize++;
			}

			if(instar.getValue() >= (animalSpecies->getNumberOfInstars()-1)*currentPercentageForPrinting)
			{
				cout << ">>>> " << (int)(currentPercentageForPrinting*100) << "%... " << endl;
				currentPercentageForPrinting += percentageForPrinting;
			}
		}

		animalSpecies->calculatePseudoGrowthMean();
	}

    return make_pair(move(animalsPopulation), populationSize);
}

PointContinuous SpatialTree::calculateCenterPosition(const PointSpatialTree &cell, const double &size) const
{
    PointContinuous center;

    double halfOfSize = size / 2.0;

    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        setPositionAxisValue(center, i, cell.get(axis) * size + halfOfSize);
    }

    return center;
}

const unsigned int SpatialTree::getMapDepth() const
{
    return mapDepth;
}

const int SpatialTree::generatePopulation(AnimalSpecies* currentAnimalSpecies, const InstarVector<vector<vector<TerrainCellInterface*>::iterator>> &speciesInhabitableTerrainCells)
{
    int numberOfDiscardedIndividualsOutsideRestrictedRanges = 0;

	cout << "Creating " << currentAnimalSpecies->getTotalInitialPopulation() << " individuals of the species \"" << currentAnimalSpecies->getScientificName() << "\"..." << endl;
	//cout << ">> Number of inhabitable cells (with involved resources): " << speciesInhabitableTerrainCells.size() << endl;

	float percentageForPrinting = 0.1;
	float currentPercentageForPrinting = percentageForPrinting;

	unsigned int randomCellIndex;

    for(const Instar &instar : currentAnimalSpecies->getInstarsRange())
	{
		for (unsigned long individual = 0; individual < currentAnimalSpecies->getInitialPopulation().at(instar); individual++)
		{
            auto newAnimal = SpatialTreeAnimalFactory::createInstance(-1.0, 0.0, instar, 0, 0, -1, -1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), true);

            pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();

			while(!isInsideRestrictedRangesAndIsViableOffSpring.first)
			{
				numberOfDiscardedIndividualsOutsideRestrictedRanges++;

				delete newAnimal;
				newAnimal = SpatialTreeAnimalFactory::createInstance(-1.0, 0.0, instar, 0, 0, -1, -1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), true);
                
				isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();
			}

			// Indicate that the animal created is the final animal, and therefore assign a final ID to it.
			newAnimal->doDefinitive();

            // Get a random index from this species inhabitable cells
            randomCellIndex = Random::randomIndex(speciesInhabitableTerrainCells[instar].size());

            try
            {
                auto newTerrainCell = (*speciesInhabitableTerrainCells[instar][randomCellIndex])->randomInsertAnimal(newAnimal);

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
            }
            catch(SpatialTreeTerrainCell::TemporalLeaf2Branch &e)
            {
                (*speciesInhabitableTerrainCells[instar][randomCellIndex]) = static_cast<SpatialTreeTerrainCellInterface*>((*speciesInhabitableTerrainCells[instar][randomCellIndex]))->getMutableParent()->randomInsertAnimalOnChild(
                    newAnimal, (*speciesInhabitableTerrainCells[instar][randomCellIndex])
                );
            }

            //ALWAYS print the traits after interpolating and before adjusting
			//newAnimal->printGenetics(geneticsFile);

			if(getWorldInterface()->getSaveAnimalConstitutiveTraits()) {
				newAnimal->printTraits(getMutableWorldInterface()->getConstitutiveTraitsFile());
			}
			
			//DONE Adjust traits now includes the first CalculateGrowthCurves call. For every animal.
			newAnimal->adjustTraits();
			newAnimal->forceMolting2();
			newAnimal->setExperiencePerSpecies();
		}

		if(instar.getValue() >= (currentAnimalSpecies->getInitialPopulation().size()-1)*currentPercentageForPrinting)
		{
			cout << ">> " << (int)(currentPercentageForPrinting*100) << "%... " << endl;
			currentPercentageForPrinting += percentageForPrinting;
		}
	}

	return numberOfDiscardedIndividualsOutsideRestrictedRanges;
}


void SpatialTree::eraseStatisticsPopulation()
{
    rootTerrainCell->eraseAllAnimals();
}


void SpatialTree::purgeDeadAnimals()
{
    rootTerrainCell->purgeDeadAnimals();
}


void SpatialTree::printAnimalsAlongCells(ofstream &file) const
{
    rootTerrainCell->printAnimals(file);
}


string SpatialTree::getMapPositionHeader() const
{
    string header = "";

    for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
    {
        header += static_cast<string>(magic_enum::enum_names<Axis>()[axis]) + "\t";
    }

    return header;
}


void SpatialTree::obtainWorldResourceBiomassAndAnimalsPopulation(vector<double> &worldResourceBiomass, vector<vector<unsigned int>> &worldAnimalsPopulation) const
{
    rootTerrainCell->obtainWorldResourceBiomassAndAnimalsPopulation(worldResourceBiomass, worldAnimalsPopulation);
}


void SpatialTree::obtainAnimalsPopulationAndGeneticsFrequencies(vector<vector<unsigned int>> &worldAnimalsPopulation, vector<vector<pair<vector<double>, vector<double>>>> &worldGeneticsFrequencies) const
{
    rootTerrainCell->obtainAnimalsPopulationAndGeneticsFrequencies(worldAnimalsPopulation, worldGeneticsFrequencies);
}


void SpatialTree::saveAnimalSpeciesSnapshot(ofstream &file, const AnimalSpecies* const &species) const
{
    rootTerrainCell->saveAnimalSpeciesSnapshot(file, species);
}


void SpatialTree::saveResourceSpeciesSnapshot(ofstream &file, const ResourceSpecies* const &species) const
{
    rootTerrainCell->saveResourceSpeciesSnapshot(file, species);
}


void SpatialTree::saveWaterSnapshot(ofstream &file) const
{
    rootTerrainCell->saveWaterSnapshot(file);
}

SearchableEdibles SpatialTree::getEdiblesOnRadius(
        TerrainCellInterface* const sourceTerrainCell, const PointContinuous &sourcePosition, const double &radius,
        const EdibleSearchParams &edibleSearchParams)
{
    return sourceTerrainCell->getEdiblesOnRadius(sourcePosition, radius, edibleSearchParams);
}

void SpatialTree::moveAnimals(int timeStep, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
    rootTerrainCell->moveAnimals(timeStep, encounterProbabilitiesFile, predationProbabilitiesFile, saveEdibilitiesFile, edibilitiesFile, exitTimeThreshold, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
}

void SpatialTree::performAnimalsActions(int timeStep, ostream& voracitiesFile, fs::path outputFolder, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
    rootTerrainCell->performAnimalsActions(timeStep, voracitiesFile, outputFolder, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
}
