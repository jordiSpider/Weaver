#ifndef SUMMARY_RESOURCE_H_
#define SUMMARY_RESOURCE_H_

#include <vector>
#include <numeric>

#include "World/LivingBeings/Resources/ResourceInterface.h"


// Forward declaration

class BranchTerrainCellInterface;



class SummaryResource : public ResourceInterface
{
protected:
    const unsigned int resourceSpeciesId;
    BranchTerrainCellInterface* const summaryTerrainCell;

public:
    SummaryResource(const unsigned int resourceSpeciesId, BranchTerrainCellInterface* const summaryTerrainCell);
    virtual ~SummaryResource();

    double substractBiomass(double dryMassToBeSubstracted) override;
    double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea, const double &minimumDryMass) override;
    double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea) override;
    void removeBiomass() override;
    void update();
    double getResourceMaximumCapacity() const;
    bool canSpread() const;
    const unsigned int getResourceSpeciesId() const;
    double addBiomass(double addedMass, const TerrainCellInterface* const source);
    double addBiomass(double addedMass, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions);
    void setFullCapacity(const bool newFullCapacityValue, const TerrainCellInterface* const source);
	void setFullCapacity(const bool newFullCapacityValue, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions);
};

#endif /* SUMMARY_RESOURCE_H_ */
