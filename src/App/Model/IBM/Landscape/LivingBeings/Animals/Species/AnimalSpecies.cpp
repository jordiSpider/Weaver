#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpecies.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Animal.h"



using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;



AnimalSpecies::AnimalSpecies()
	: Species()
{

}

AnimalSpecies::AnimalSpecies(const AnimalSpecies::ID& speciesId, const AnimalSpeciesID& animalSpeciesId, const json &info, const PreciseDouble& timeStepsPerDay, const PreciseDouble& pdfThreshold, const size_t numberOfExistingSpecies) 
	: Species(speciesId, info["name"].get<string>()), 
	  genetics(info["genetics"]),
	  animalSpeciesGrowth(
		info["growthModule"], static_cast<unsigned int>(info["individualsPerInstar"].size()), 
		info["conversionToWetMass"].get<std::vector<double>>(),
		Temperature(info["tempFromLab"].get<double>()), timeStepsPerDay
	  ),
	  decisions(info["decisions"], getGenetics(), getGrowthBuildingBlock().getNumberOfInstars(), pdfThreshold),
	  animalSpeciesId(animalSpeciesId), 
	  defaultHuntingMode(EnumClass<HuntingMode>::stringToEnumValue(info["defaultHuntingMode"].get<string>())),
	  breedSearchParams(EnumClass<Gender>::size()),
	  matureFemalesSearchParams(new AnimalSearchParams()), 
	  populationSearchParams(new AnimalSearchParams()), lifeStageSearchParams(EnumClass<LifeStage>::size()),
	  tempFromLab(info["tempFromLab"].get<double>()),
	  population(0), activatedHandling(info["activatedHandling"].get<bool>()),
	  sexualType(EnumClass<SexualType>::stringToEnumValue(info["sexualType"].get<string>())), 
	  forcePresenceAllResourcesInvolved(info["debug"]["forcePresenceAllResourcesInvolved"].get<bool>()),
	  preserveLeftovers(info["preserveLeftovers"].get<bool>()), YodzisA(info["YodzisA"].get<double>()), YodzisB(info["YodzisB"].get<double>())
{
	setPoreUsePerInstar(info["poreUsePerInstar"].get<std::vector<double>>());



	edibleSpecies = CustomIndexedVector<Instar, CustomIndexedVector<SpeciesType, std::unordered_map<ID, std::vector<Instar>>>>(getGrowthBuildingBlock().getNumberOfInstars(), vector<unordered_map<Species::ID, vector<Instar>>>(EnumClass<SpeciesType>::size()));
	preySearchParams = CustomIndexedVector<Instar, EdibleSearchParams>(getGrowthBuildingBlock().getNumberOfInstars());
	predatorSearchParams = CustomIndexedVector<Instar, AnimalSearchParams>(getGrowthBuildingBlock().getNumberOfInstars());
	maximumInteractionArea = CustomIndexedVector<Instar, PreciseDouble>(getGrowthBuildingBlock().getNumberOfInstars(), 0.0);




	maximumPredationEncountersPerDay = 0u;

	//Added for new growth_curves
	setIndividualsPerInstar(info["individualsPerInstar"]);

	setStatisticsIndividualsPerInstar(info["statisticsIndividualsPerInstar"]);

	setHabitatShiftBeforeBreeding(info["habitatShiftBeforeBreeding"], info["habitatShiftBeforeBreedingFactor"]);
	setHabitatShiftAfterBreeding(info["habitatShiftAfterBreeding"], info["habitatShiftAfterBreedingFactor"]);
	setMinRelativeHumidityThreshold(info["minRelativeHumidityThreshold"].get<vector<double>>());

	setPlasticityDueToConditionSearch(info["plasticityDueToConditionSearch"].get<double>());
	setPlasticityDueToConditionSpeed(info["plasticityDueToConditionSpeed"].get<double>());

	setTimeStepsWithoutFoodForMetabolicDownregulation(info["daysWithoutFoodForMetabolicDownregulation"].get<double>(), timeStepsPerDay);
	setPercentageMetabolicDownregulation(info["percentageMetabolicDownregulation"]);
	setPercentageCostForMetabolicDownregulationVoracity(info["percentageCostForMetabolicDownregulationVoracity"]);
	setPercentageCostForMetabolicDownregulationSearchArea(info["percentageCostForMetabolicDownregulationSearchArea"]);
	setPercentageCostForMetabolicDownregulationSpeed(info["percentageCostForMetabolicDownregulationSpeed"]);

	setSexRatio(info["sexRatio"]);
	setFemaleMaxReproductionEvents(info["femaleMaxReproductionEvents"]);
	setMaleMaxReproductionEvents(info["maleMaxReproductionEvents"]);
	setMaleMobility(info["maleMobility"].get<double>());
	setSurviveWithoutFood(info["debug"]["surviveWithoutFood"]);
	setActivityUnderPredationRisk(info["activityUnderPredationRisk"]);
	setProbabilityDeathFromBackground(info["probabilityDeathFromBackground"].get<std::vector<double>>());

	totFec = getFemaleMaxReproductionEvents() * getGrowthBuildingBlock().getEggsPerBatch();


	if(info["requiredRepresentAllPossibleAlleles"].get<bool>())
	{
		if(getTotalInitialPopulation() < getGenetics().getNumberOfAllelesPerLocus() * getGenetics().getNumberOfLociPerTrait())
		{
			throwLineInfoException("Error: The initial number of individuals is insufficient to represent all possible alleles for the species.");
		}
	}


	obtainBreedSearchParams();
	obtainMatureFemalesSearchParams();
	obtainPopulationSearchParams();
	obtainLifeStageSearchParams();
	obtainConspecificSearchParams();


	predationEventsOnOtherSpecies.resize(numberOfExistingSpecies, 0);
}

AnimalSpecies::~AnimalSpecies()
{
	delete matureFemalesSearchParams;

	delete populationSearchParams;
}

void AnimalSpecies::setPoreUsePerInstar(const vector<double>& newPoreUsePerInstar)
{
	for(const double value : newPoreUsePerInstar)
	{
		poreUsePerInstar.emplace_back(value);
	}
}

bool AnimalSpecies::getForcePresenceAllResourcesInvolved() const 
{ 
	return forcePresenceAllResourcesInvolved; 
}

bool AnimalSpecies::getPreserveLeftovers() const 
{ 
	return preserveLeftovers; 
}

const AnimalSpeciesID& AnimalSpecies::getAnimalSpeciesId() const 
{ 
	return animalSpeciesId; 
}

const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, OntogeneticLink>>& AnimalSpecies::getOntogeneticLinksPerInstar(const Instar &predatorInstar) const
{
	return edibleOntogeneticLink.at(predatorInstar);
}

const OntogeneticLink& AnimalSpecies::getEdibleOntogeneticLink(const Species::ID &preySpeciesId, const Instar &predatorInstar, const Instar &preyInstar) const
{
	return edibleOntogeneticLink.at(predatorInstar).at(preySpeciesId).at(preyInstar);
}

void AnimalSpecies::setOntogeneticLinks(View* view, const vector<Species*>& existingSpecies, rapidcsv::Document& ontogeneticLinksPreference, rapidcsv::Document& ontogeneticLinksProfitability)
{
	edibleOntogeneticLink.clear();

	edibleOntogeneticLink.resize(getGrowthBuildingBlock().getNumberOfInstars());

	for(const Instar& predatorInstar : getGrowthBuildingBlock().getInstarsRange())
	{
		edibleOntogeneticLink[predatorInstar].resize(existingSpecies.size());

		view->updateLog({" - Instar ", predatorInstar.to_string(), ":\n"});

		for(const auto& type : EnumClass<SpeciesType>::getEnumValues())
		{
			edibleSpecies[predatorInstar][type].clear();
		}

		PreciseDouble instarPreference = 0.0;

		string column = getScientificName() + "$" + predatorInstar.to_string();

		for(const Species* const preySpecies : existingSpecies)
		{
			edibleOntogeneticLink[predatorInstar][preySpecies->getId()].resize(preySpecies->getGrowthBuildingBlock().getNumberOfInstars());
		
			bool predatorEats = false;

			for(const Instar& preyInstar : preySpecies->getGrowthBuildingBlock().getInstarsRange())
			{
				string row = preySpecies->getScientificName() + "$" + preyInstar.to_string();

				PreciseDouble preference;

				try {
					preference = ontogeneticLinksPreference.GetCell<double>(column, row);
				} catch (const std::out_of_range&) {
					throwLineInfoException("Error: The cell (Row: " + row + ", Column: " + column + ") in 'ontogeneticLinksPreference.csv' does not exist.");
				}

				if(preference > 0.0)
				{
					instarPreference += preference;

					PreciseDouble profitability;

					try {
						profitability = ontogeneticLinksProfitability.GetCell<double>(column, row);
					} catch (const std::out_of_range&) {
						throwLineInfoException("Error: The cell (Row: " + row + ", Column: " + column + ") in 'ontogeneticLinksProfitability.csv' does not exist.");
					}

					edibleOntogeneticLink[predatorInstar][preySpecies->getId()][preyInstar].setInfo(preference,profitability);

					try
					{
						edibleSpecies[predatorInstar][preySpecies->getType()][preySpecies->getId()].push_back(preyInstar);
					}
					catch(const std::out_of_range&)
					{
						edibleSpecies[predatorInstar][preySpecies->getType()][preySpecies->getId()] = vector<Instar>();
						edibleSpecies[predatorInstar][preySpecies->getType()][preySpecies->getId()].push_back(preyInstar);
					}

					predatorEats = true;
				}
			}

			if(predatorEats)
			{
				switch(preySpecies->getType())
				{
					case SpeciesType::Animal:
						view->updateLog({"   * Animal: ", preySpecies->getScientificName(), "\n"});
						break;
					case SpeciesType::Resource:
						view->updateLog({"   * Resource: ", preySpecies->getScientificName(), "\n"});
						break;
					default:
						throwLineInfoException("Default case");
						break;
				}
			}
		}

		if(instarPreference > 1.0)
		{
			throwLineInfoException("Error: The total preference of instar " + predatorInstar.to_string() + " of '" + getScientificName() + "' animal species must less or equal to 1.");
		}
	}
}

void AnimalSpecies::calculateCellDepthPerInstar(const Map* const map)
{
	cellDepthPerInstar.reserve(poreUsePerInstar.size());
	
	for(const PreciseDouble &poreUse : poreUsePerInstar)
	{
		unsigned int actualDepth = static_cast<const SpatialTree* const>(map)->getMapDepth()-1u;

		while(poreUse > static_cast<const SpatialTree* const>(map)->getCellSize(actualDepth))
		{
			actualDepth--;
		}

		cellDepthPerInstar.push_back(actualDepth);
	}
}

const CustomIndexedVector<Instar, unsigned int>& AnimalSpecies::getCellDepthPerInstar() const
{
	return cellDepthPerInstar;
}

unsigned int AnimalSpecies::getInstarCellDepth(const Instar &instar) const
{
	return cellDepthPerInstar[instar];
}

const PreciseDouble& AnimalSpecies::getEdibleProfitability(const Species::ID &preySpeciesId, const Instar &predatorInstar, const Instar &preyInstar) const
{
	return getEdibleOntogeneticLink(preySpeciesId, predatorInstar, preyInstar).getProfitability();
}

bool AnimalSpecies::canEatEdible(const Species::ID &preySpeciesId, const Instar &predatorInstar, const Instar &preyInstar) const
{
	return getEdibleOntogeneticLink(preySpeciesId, predatorInstar, preyInstar).getEdible();
}

void AnimalSpecies::addPredationEventOnOtherSpecies(Species::ID predatedSpeciesId) 
{ 
	predationEventsOnOtherSpecies.at(predatedSpeciesId)++; 
}

void AnimalSpecies::addAnimalSpecies()
{
	predationEventsOnOtherSpecies.push_back(0);
}

void AnimalSpecies::addResourceSpecies()
{
	predationEventsOnOtherSpecies.push_back(0);
}

const unsigned int& AnimalSpecies::getPredationEventsOnOtherSpecies(Species::ID predatedSpeciesId) const
{
	return predationEventsOnOtherSpecies[predatedSpeciesId];
}

bool AnimalSpecies::eatsAnimalSpecies(const Instar &instar) const
{
	return !getInstarEdibleAnimalSpecies(instar).empty();
}

const PreciseDouble& AnimalSpecies::getHabitatShiftBeforeBreedingFactor() const
{
	return habitatShiftBeforeBreedingFactor;
}

const PreciseDouble& AnimalSpecies::getHabitatShiftAfterBreedingFactor() const
{
	return habitatShiftAfterBreedingFactor;
}

void AnimalSpecies::generateInvolvedResourceSpecies(const vector<Species*> &existingSpecies, const vector<AnimalSpecies*>& existingAnimalSpecies, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, std::vector<ResourceSpecies::ResourceID>>>& involvedResourceSpecies) const
{
	involvedResourceSpecies[getAnimalSpeciesId()].resize(getGrowthBuildingBlock().getNumberOfInstars());

	for(const Instar &instar : getGrowthBuildingBlock().getInstarsRange())
	{
		// Esta variable controla que no haya bucles infinitos debido a la depredación mutua entre dos instars
		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>> alreadyCheckedSpecies(existingAnimalSpecies.size());

		for(const AnimalSpecies* const animalSpecies : existingAnimalSpecies)
		{
			alreadyCheckedSpecies[animalSpecies->getAnimalSpeciesId()] = CustomIndexedVector<Instar, bool>(animalSpecies->getGrowthBuildingBlock().getNumberOfInstars(), false);
		}

		generateInstarInvolvedResourceSpecies(existingSpecies, instar, involvedResourceSpecies[getAnimalSpeciesId()][instar], alreadyCheckedSpecies);
		
		if(involvedResourceSpecies[getAnimalSpeciesId()][instar].empty())
		{
			throwLineInfoException("Each instar must have at least one resource species involved");
		}
	}
}

void AnimalSpecies::generateInstarInvolvedResourceSpecies(
		const vector<Species*>& existingSpecies, const Instar &instar, 
		std::vector<ResourceSpecies::ResourceID>& involvedResourceSpecies, 
		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>> &alreadyCheckedSpecies
	) const
{
	if(!alreadyCheckedSpecies[getAnimalSpeciesId()][instar])
	{
		alreadyCheckedSpecies[getAnimalSpeciesId()][instar] = true;

		for(const auto &preySpeciesTypeEntry : getInstarEdibleAnimalSpecies(instar))
		{
			for(const Instar &preyInstar : preySpeciesTypeEntry.second)
			{
				static_cast<AnimalSpecies*>(existingSpecies[preySpeciesTypeEntry.first])->generateInstarInvolvedResourceSpecies(existingSpecies, preyInstar, involvedResourceSpecies, alreadyCheckedSpecies);
			}
		}

		for(const auto &preySpeciesTypeEntry : getInstarEdibleResourceSpecies(instar))
		{
			involvedResourceSpecies.push_back(static_cast<ResourceSpecies*>(existingSpecies[preySpeciesTypeEntry.first])->getResourceSpeciesId());
		}
	}
} 

const EdibleSearchParams& AnimalSpecies::getPreySearchParams(const Instar &instar) const
{
	return preySearchParams[instar];
}

const AnimalSearchParams& AnimalSpecies::getPredatorSearchParams(const Instar &instar) const
{
	return predatorSearchParams[instar];
}

const AnimalSearchParams& AnimalSpecies::getConspecificSearchParams() const
{
	return conspecificSearchParams;
}

void AnimalSpecies::setHabitatShiftBeforeBreeding(const bool newHabitatShiftBeforeBreedingValue, const json &newHabitatShiftBeforeBreedingFactor)
{
	habitatShiftBeforeBreeding = newHabitatShiftBeforeBreedingValue;

	habitatShiftBeforeBreedingFactor = 0.0;

	if(habitatShiftBeforeBreeding)
	{
		habitatShiftBeforeBreedingFactor = newHabitatShiftBeforeBreedingFactor.get<double>();
	}
}

void AnimalSpecies::setIndividualsPerInstar(const vector<unsigned int>& newIndividualsPerInstar)
{ 
	individualsPerInstar = newIndividualsPerInstar;
}

void AnimalSpecies::setStatisticsIndividualsPerInstar(const unsigned int& statisticsIndividualsPerInstarValue) 
{ 
	statisticsIndividualsPerInstar = statisticsIndividualsPerInstarValue; 
}

void AnimalSpecies::setProbabilityDeathFromBackground(const vector<double>& newProbabilityDeathFromBackground) 
{ 
	for(const double value : newProbabilityDeathFromBackground)
	{
		probabilityDeathFromBackground.emplace_back(value);
	}
}

void AnimalSpecies::initializeGeneticFiles(const fs::path& geneticsFolder)
{
	genetics.initializeGeneticFiles(geneticsFolder / getScientificNameReplaced());
}

void AnimalSpecies::setHabitatShiftAfterBreeding(const bool newHabitatShiftAfterBreedingValue, const json &newHabitatShiftAfterBreedingFactor)
{
	habitatShiftAfterBreeding = newHabitatShiftAfterBreedingValue;

	habitatShiftAfterBreedingFactor = 0.0;

	if(habitatShiftAfterBreeding)
	{
		habitatShiftAfterBreedingFactor = newHabitatShiftAfterBreedingFactor.get<double>();
	}
}

bool AnimalSpecies::occursHabitatShiftBeforeBreeding() const
{
	return habitatShiftBeforeBreeding;
}

bool AnimalSpecies::occursHabitatShiftAfterBreeding() const
{
	return habitatShiftAfterBreeding;
}

const AnimalSearchParams& AnimalSpecies::getBreedSearchParams(const Gender &gender) const
{
	return breedSearchParams[gender];
}

void AnimalSpecies::obtainBreedSearchParams()
{
	for(const Gender &gender : EnumClass<Gender>::getEnumValues())
	{
		vector<Gender> searchableGender;

		switch(gender)
		{
			case Gender::MALE:
				searchableGender = {Gender::FEMALE};
				break;
			case Gender::FEMALE:
				searchableGender = {Gender::MALE};
				break;
			case Gender::HERMAPHRODITE:
				searchableGender = {Gender::HERMAPHRODITE};
				break;
			default:
				throwLineInfoException("Default case");
				break;
		}

		breedSearchParams[gender].addSearchParams(
			{LifeStage::ACTIVE, LifeStage::DIAPAUSE}, 
			{getAnimalSpeciesId()}, getGrowthBuildingBlock().getMatureInstarsRange(), searchableGender
		);
	}
}

void AnimalSpecies::obtainConspecificSearchParams()
{
	conspecificSearchParams.addSearchParams(
		{LifeStage::ACTIVE}, {getAnimalSpeciesId()}, 
		getGrowthBuildingBlock().getInstarsRange(),
		EnumClass<Gender>::getEnumValues()
	);
}

void AnimalSpecies::obtainPreyAndPredatorSearchParams(const vector<Species*>& existingSpecies, const vector<AnimalSpecies*>& existingAnimalSpecies, const vector<ResourceSpecies*>& existingResourceSpecies)
{
	for(const Instar &instar : getGrowthBuildingBlock().getInstarsRange())
	{
		preySearchParams[instar].clear();
		predatorSearchParams[instar].clear();

		for(const AnimalSpecies* const &otherAnimalSpecies : existingAnimalSpecies)
		{
			for(const Instar &otherAnimalSpeciesInstar : otherAnimalSpecies->getGrowthBuildingBlock().getInstarsRange())
			{
				// Check if the animal is prey for the species
				bool isSpeciePrey = otherAnimalSpecies->canEatEdible(getId(), otherAnimalSpeciesInstar, instar);
				// Check if the species is prey for the animal
				bool isSpeciePredator = canEatEdible(otherAnimalSpecies->getId(), instar, otherAnimalSpeciesInstar);
				
				if(isSpeciePrey)
				{
					predatorSearchParams[instar].addSearchParams(
						{LifeStage::ACTIVE}, {otherAnimalSpecies->getAnimalSpeciesId()},
						{otherAnimalSpeciesInstar}, EnumClass<Gender>::getEnumValues()
					);
				}
				
				if(isSpeciePredator)
				{
					preySearchParams[instar].addAnimalSearchParams(
						{LifeStage::ACTIVE, LifeStage::DIAPAUSE}, 
						{otherAnimalSpecies->getAnimalSpeciesId()},
						{otherAnimalSpeciesInstar}, EnumClass<Gender>::getEnumValues()
					);
				}
			}
		}

		for(const auto &instarEdibleResourceSpeciesEntry : getInstarEdibleResourceSpecies(instar))
		{
			preySearchParams[instar].addResourceSearchParams(
				existingResourceSpecies,
				{static_cast<ResourceSpecies*>(existingSpecies[instarEdibleResourceSpeciesEntry.first])->getResourceSpeciesId()}
			);
		}	
	}
}

const AnimalSearchParams& AnimalSpecies::getMatureFemalesSearchParams() const
{
	return *matureFemalesSearchParams;
}

void AnimalSpecies::generateInitialGenomesPool(const CustomIndexedVector<Instar, unsigned int>& initialPopulation, std::vector<Genome>& initialGenomesPool)
{
	unsigned int totalInitialPopulation = 0;

    for(const unsigned int instarPopulation : initialPopulation)
    {
        totalInitialPopulation += instarPopulation;
    }

	initialGenomesPool.reserve(totalInitialPopulation);


	for(unsigned int i = 0; i < totalInitialPopulation; i++) {
		initialGenomesPool.emplace_back(
			getGenetics().getLociPerTrait(), getGenetics().getRandomlyCreatedPositionsForChromosomes(),
			getGenetics().getNumberOfChromosomes(), getGenetics().getNumberOfLociPerChromosome(),
			getGenetics().getNumberOfChiasmasPerChromosome()
		);

		getMutableGenetics().updatePseudoValueRanges(initialGenomesPool.back());
	}
}

void AnimalSpecies::obtainMatureFemalesSearchParams()
{
	matureFemalesSearchParams->addSearchParams(
		{LifeStage::ACTIVE, LifeStage::DIAPAUSE},
		{getAnimalSpeciesId()}, getGrowthBuildingBlock().getMatureInstarsRange(), {Gender::FEMALE}
	);
}

const AnimalSearchParams& AnimalSpecies::getPopulationSearchParams() const
{
	return *populationSearchParams;
}

void AnimalSpecies::setTimeStepsWithoutFoodForMetabolicDownregulation(const PreciseDouble& daysWithoutFoodForMetabolicDownregulation, const PreciseDouble& timeStepsPerDay) 
{ 
	timeStepsWithoutFoodForMetabolicDownregulation = TimeStep(Day(daysWithoutFoodForMetabolicDownregulation), timeStepsPerDay); 
}

SpeciesType AnimalSpecies::getType() const
{
	return SpeciesType::Animal;
}

void AnimalSpecies::obtainPopulationSearchParams()
{
	populationSearchParams->addSearchParams(
		EnumClass<LifeStage>::getEnumValues(), {getAnimalSpeciesId()},
		getGrowthBuildingBlock().getInstarsRange(), EnumClass<Gender>::getEnumValues()
	);
}

const AnimalSearchParams& AnimalSpecies::getLifeStageSearchParams(const LifeStage &lifeStage) const
{
    return lifeStageSearchParams[lifeStage];
}

void AnimalSpecies::obtainLifeStageSearchParams()
{
    for(const auto &lifeStage : EnumClass<LifeStage>::getEnumValues())
    {
        lifeStageSearchParams[lifeStage].addSearchParams(
			{lifeStage}, {getAnimalSpeciesId()}, getGrowthBuildingBlock().getInstarsRange(),
			EnumClass<Gender>::getEnumValues()
		);
    }
}

const CustomIndexedVector<SpeciesType, unordered_map<Species::ID, vector<Instar>>>& AnimalSpecies::getInstarEdibleSpecies(const Instar &instar) const
{
	return edibleSpecies[instar];
}

const unordered_map<Species::ID, vector<Instar>>& AnimalSpecies::getInstarEdibleAnimalSpecies(const Instar &instar) const
{
	return edibleSpecies[instar][SpeciesType::Animal];
}

const unordered_map<Species::ID, vector<Instar>>& AnimalSpecies::getInstarEdibleResourceSpecies(const Instar &instar) const
{
	return edibleSpecies[instar][SpeciesType::Resource];
}

void AnimalSpecies::setMinRelativeHumidityThreshold(const vector<double>& newMinRelativeHumidityThreshold)
{
	for(const double value : newMinRelativeHumidityThreshold)
	{
		minRelativeHumidityThreshold.emplace_back(value);
	}
}

const AnimalSpeciesGrowth& AnimalSpecies::getGrowthBuildingBlock() const
{
	return animalSpeciesGrowth;
}

AnimalSpeciesGrowth& AnimalSpecies::getMutableGrowthBuildingBlock()
{
	return animalSpeciesGrowth;
}

const AnimalSpeciesDecisions* AnimalSpecies::getDecisionsBuildingBlock() const
{
	return &decisions;
}

AnimalSpeciesDecisions* AnimalSpecies::getMutableDecisionsBuildingBlock()
{
	return &decisions;
}

const PreciseDouble& AnimalSpecies::getYodzisA() const
{
	return YodzisA;
}

const PreciseDouble& AnimalSpecies::getYodzisB() const
{
	return YodzisB;
}

void AnimalSpecies::setPlasticityDueToConditionSearch(const PreciseDouble& plasticityDueToConditionSearchValue)
{
	if (plasticityDueToConditionSearchValue > 0)
	{
		plasticityDueToConditionSearch = plasticityDueToConditionSearchValue;
	}
	else
	{
		throwLineInfoException("For the species '" + getScientificName() + "':\nplasticityDueToConditionSearch must be a positive value. You entered " + plasticityDueToConditionSearchValue.to_string() + ". Exiting now.");
	}
}

void AnimalSpecies::setPlasticityDueToConditionSpeed(const PreciseDouble& plasticityDueToConditionSpeedValue)
{
	if (plasticityDueToConditionSpeedValue > 0)
	{
		plasticityDueToConditionSpeed = plasticityDueToConditionSpeedValue;
	}
	else
	{
		throwLineInfoException("For the species '" + getScientificName() + "':\nplasticityDueToConditionSpeed must be a positive value. You entered " + plasticityDueToConditionSpeedValue.to_string() + ". Exiting now.");
	}
}

void AnimalSpecies::setPercentageCostForMetabolicDownregulationVoracity(const float& percentageCostForMetabolicDownregulationVoracityValue) 
{ 
	percentageCostForMetabolicDownregulationVoracity = percentageCostForMetabolicDownregulationVoracityValue; 
}

void AnimalSpecies::setPercentageCostForMetabolicDownregulationSearchArea(const float& percentageCostForMetabolicDownregulationSearchAreaValue) 
{ 
	percentageCostForMetabolicDownregulationSearchArea = percentageCostForMetabolicDownregulationSearchAreaValue; 
}

void AnimalSpecies::setPercentageCostForMetabolicDownregulationSpeed(const float& percentageCostForMetabolicDownregulationSpeedValue) 
{ 
	percentageCostForMetabolicDownregulationSpeed = percentageCostForMetabolicDownregulationSpeedValue; 
}

float AnimalSpecies::getSexRatio() 
{ 
	return sexRatio; 
}

void AnimalSpecies::setSexRatio(const float& sexRatioValue) 
{ 
	sexRatio = sexRatioValue; 
}

void AnimalSpecies::setFemaleMaxReproductionEvents(const unsigned int& femaleMaxReproductionEventsValue) 
{ 
	femaleMaxReproductionEvents = femaleMaxReproductionEventsValue; 
}

void AnimalSpecies::setMaleMaxReproductionEvents(const unsigned int& maleMaxReproductionEventsValue) 
{ 
	maleMaxReproductionEvents = maleMaxReproductionEventsValue; 
}

void AnimalSpecies::updateMaximumPredationEncountersPerDay(const size_t& predationEncountersPerDay)
{
	maximumPredationEncountersPerDay = max(maximumPredationEncountersPerDay, predationEncountersPerDay);
}

const size_t& AnimalSpecies::getMaximumPredationEncountersPerDay() const
{
	return maximumPredationEncountersPerDay;
}

void AnimalSpecies::updatePopulationHistory(const TimeStep& stabilityCalculationInterval)
{
	if(TimeStep(static_cast<unsigned int>(populationHistory.size())) == stabilityCalculationInterval)
	{
		populationHistory.pop_front();
	}

	populationHistory.push_back(getPopulation());
}

void AnimalSpecies::calculateCoefficientOfVariation()
{
	PreciseDouble mean = std::accumulate(populationHistory.begin(), populationHistory.end(), 0.0) / static_cast<double>(populationHistory.size());

	if(mean == 0.0)
	{
		coefficientOfVariation = mean;
	}
	else
	{
		PreciseDouble sum_squared_diffs = 0;
		for (unsigned int value : populationHistory) {
			sum_squared_diffs += (value - mean) * (value - mean);
		}
		PreciseDouble std_dev = sqrt(sum_squared_diffs / static_cast<double>(populationHistory.size()));

		coefficientOfVariation = std_dev / mean;
	}
}

const PreciseDouble& AnimalSpecies::getCoefficientOfVariation() const
{
	return coefficientOfVariation;
}

unsigned int AnimalSpecies::getPopulation() const
{
	return population;
}

void AnimalSpecies::increasePopulation()
{
	population++;
}

void AnimalSpecies::decreasePopulation()
{
	population--;
}

Gender AnimalSpecies::getRandomGender() const
{
	Gender newAnimalGender = Gender::MALE;
	if(Random::randomUniform() < sexRatio || getSexualType() == SexualType::asexual)
	{
		newAnimalGender = Gender::FEMALE;
	}
	return newAnimalGender;
}

void AnimalSpecies::setMaleMobility(const PreciseDouble& maleMobilityValue) 
{ 
	maleMobility = maleMobilityValue; 
}

void AnimalSpecies::setSurviveWithoutFood(const bool surviveWithoutFoodValue) 
{ 
	surviveWithoutFood = surviveWithoutFoodValue; 
}

void AnimalSpecies::setActivityUnderPredationRisk(const float& activityUnderPredationRiskValue) 
{ 
	activityUnderPredationRisk = activityUnderPredationRiskValue; 
}

unsigned int AnimalSpecies::getTotalInitialPopulation() const
{
	unsigned int totalInitialPopulation = 0;

	for(const unsigned int instarPopulation : getIndividualsPerInstar())
	{
		totalInitialPopulation += instarPopulation;
	}

	return totalInitialPopulation;
}

unsigned int AnimalSpecies::getTotalInitialStatisticsPopulation()
{
	return getStatisticsIndividualsPerInstar() * getGrowthBuildingBlock().getNumberOfInstars();
}

void AnimalSpecies::updateMaximumInteractionArea(const Instar& instar, const PreciseDouble& newValue)
{
	maximumInteractionArea[instar] = fmax(maximumInteractionArea[instar], newValue);
}

const PreciseDouble& AnimalSpecies::getMaximumInteractionArea(const Instar& instar) const
{
	return maximumInteractionArea[instar];
}

void AnimalSpecies::interactionRanges(Animal& predator, Animal& prey)
{
	getMutableDecisionsBuildingBlock()->updateMaximumPDF(predator.getInstarToEvaluateCells(), PredationProbability::calculateProbabilityDensityFunction(predator, prey));

	PreciseDouble interactionAreaRadius = predator.applyAllometricModel(
		predator.getGenetics().getBaseIndividualTraits(BaseTraitType::coeffMassForInteractionRadius).getPhenotypicValue(),
		predator.getGenetics().getBaseIndividualTraits(BaseTraitType::scaleMassForInteractionRadius).getPhenotypicValue()
	);

	updateMaximumInteractionArea(predator.getGrowthBuildingBlock().getInstar(), interactionAreaRadius);
}

bool AnimalSpecies::getActivatedHandling() const
{
	return activatedHandling;
}

const PreciseDouble& AnimalSpecies::getTotFec() const
{
	return totFec;
}

const AnimalSpeciesGenetics& AnimalSpecies::getGenetics() const
{
	return genetics;
}

AnimalSpeciesGenetics& AnimalSpecies::getMutableGenetics()
{
	return genetics;
}


BOOST_CLASS_EXPORT(AnimalSpecies)

template <class Archive>
void AnimalSpecies::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<Species>(*this);
	
	ar & genetics;

	ar & animalSpeciesGrowth;

	ar & decisions;

	if (Archive::is_loading::value) {
		decisions.deserializeProbabilityFunctions();
	}

	ar & animalSpeciesId;
	ar & defaultHuntingMode;

	ar & edibleSpecies;

	ar & edibleOntogeneticLink;

	ar & preySearchParams;
	ar & predatorSearchParams;
	ar & conspecificSearchParams;

	ar & breedSearchParams;
	ar & matureFemalesSearchParams;
	ar & populationSearchParams;
	ar & lifeStageSearchParams;

	ar & predationEventsOnOtherSpecies;

	ar & tempFromLab;

	ar & populationHistory;

	ar & population;

	ar & coefficientOfVariation;

	ar & activatedHandling;

	ar & individualsPerInstar;
	ar & statisticsIndividualsPerInstar;

	ar & minRelativeHumidityThreshold;

	ar & plasticityDueToConditionSearch;
	ar & plasticityDueToConditionSpeed;

	ar & timeStepsWithoutFoodForMetabolicDownregulation;
	ar & percentageMetabolicDownregulation;
	ar & percentageCostForMetabolicDownregulationVoracity;
	ar & percentageCostForMetabolicDownregulationSearchArea;
	ar & percentageCostForMetabolicDownregulationSpeed;

	ar & sexualType;
	ar & sexRatio;
	ar & femaleMaxReproductionEvents;

	ar & totFec;

	ar & maleMaxReproductionEvents;
	ar & maleMobility;
	ar & surviveWithoutFood;
	ar & activityUnderPredationRisk;
	ar & probabilityDeathFromBackground;

	ar & habitatShiftBeforeBreeding;
	ar & habitatShiftAfterBreeding;

	ar & habitatShiftBeforeBreedingFactor;
	ar & habitatShiftAfterBreedingFactor;

	ar & poreUsePerInstar;
	ar & cellDepthPerInstar;

	ar & forcePresenceAllResourcesInvolved;
	ar & preserveLeftovers;

	ar & YodzisA;
	ar & YodzisB;

	ar & maximumPredationEncountersPerDay;

	ar & maximumInteractionArea;
}

// Specialisation
template void AnimalSpecies::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalSpecies::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalSpecies::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalSpecies::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
