
=======
species
=======

It defines the configuration of a biological species, including its behavioral parameters, growth characteristics, and debugging options used in simulation.  

----------------------------------------------------------
Main Properties
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``version``
     - ``string``
     - Version of the species configuration schema.
   * - ``animal``
     - ``object``
     - Defines the main animal parameters, including biological traits, behavior, and debugging options.

----------------------------------------------------------
Object ``animal``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``name``
     - ``string``
     - Name identifying the animal species.
   * - ``defaultHuntingMode``
     - ``string (enum)``
     - Default hunting strategy used by the species. Possible values:
       - ``does_not_hunt`` – The animal does not engage in predation.
       - ``sit_and_wait`` – The animal waits passively for prey.
       - ``grazer`` – The animal continuously feeds on available resources.
   * - ``sexualType``
     - ``string (enum)``
     - Sexual reproduction mechanism. Possible values:
       - ``diploid`` – Standard diploid reproduction.
       - ``haplodiploid`` – Males are haploid and females are diploid.
       - ``asexual`` – Asexual reproduction (no mating).
   * - ``YodzisA``
     - ``number``
     - Coefficient *A* of the Yodzis and Innes metabolic model.
   * - ``YodzisB``
     - ``number``
     - Coefficient *B* of the Yodzis and Innes metabolic model.
   * - ``activatedHandling``
     - ``boolean``
     - Enables the handling time mechanism in the species' feeding process.
   * - ``preserveLeftovers``
     - ``boolean``
     - If ``true``, the animal preserves unconsumed resources between feeding events.
   * - ``sexRatio``
     - ``number``
     - Ratio of males to total individuals in the population.
   * - ``numberOfSimulatedAttacks``
     - ``integer``
     - Number of predation attacks simulated per time step. Must be greater than or equal to 0.
   * - ``conversionToWetMass``
     - ``array[number]``
     - Conversion factors from dry to wet mass for each developmental stage. Values must be strictly greater than 0.
   * - ``forDensitiesGrowth``
     - ``number``
     - Parameter for growth density calculations.
   * - ``forDensitiesEggSize``
     - ``number``
     - Parameter for egg size density calculations.
   * - ``forDensitiesA``
     - ``number``
     - Parameter *A* used in density-related calculations.
   * - ``forDensitiesB``
     - ``number``
     - Parameter *B* used in density-related calculations.
   * - ``femaleMaxReproductionEvents``
     - ``integer``
     - Maximum number of reproduction events a female can perform. Must be ≥ 0.
   * - ``maleMaxReproductionEvents``
     - ``integer``
     - Maximum number of reproduction events a male can perform. Must be ≥ 0.
   * - ``maleReproductionFactor``
     - ``number``
     - Factor influencing male reproduction success.
   * - ``maleMobility``
     - ``integer``
     - Mobility level of male individuals. Must be ≥ 0.
   * - ``activityUnderPredationRisk``
     - ``number``
     - Level of activity when exposed to predation risk.
   * - ``probabilityDeathFromBackground``
     - ``array[number]``
     - Background mortality probabilities for each stage. Values must be ≥ 0.
   * - ``minRelativeHumidityThreshold``
     - ``array[number]``
     - Minimum relative humidity thresholds for survival. Values must be ≥ 0.
   * - ``habitatShiftBeforeBreedingFactor``
     - ``number, nullable``
     - Factor modifying habitat shift behavior before breeding. Can be null.
   * - ``habitatShiftBeforeBreeding``
     - ``boolean``
     - Whether habitat shift occurs before breeding.
   * - ``habitatShiftAfterBreedingFactor``
     - ``number, nullable``
     - Factor modifying habitat shift behavior after breeding. Can be null.
   * - ``habitatShiftAfterBreeding``
     - ``boolean``
     - Whether habitat shift occurs after breeding.
   * - ``poreUsePerInstar``
     - ``array[number]``
     - Pore usage per instar stage, ordered in increasing values.
   * - ``individualsPerInstar``
     - ``array[integer]``
     - Number of individuals in each instar. Values must be ≥ 0.
   * - ``statisticsIndividualsPerInstar``
     - ``integer``
     - Number of individuals used for statistical calculations per instar. Must be ≥ 1.
   * - ``requiredRepresentAllPossibleAlleles``
     - ``boolean``
     - Indicate whether an initial number of individuals sufficient to represent all possible alleles of the species is required.
   * - ``tempFromLab``
     - ``number``
     - Laboratory temperature used for development experiments.
   * - ``plasticityDueToConditionSearch``
     - ``number``
     - Behavioral plasticity factor due to environmental conditions affecting search behavior.
   * - ``plasticityDueToConditionSpeed``
     - ``number``
     - Behavioral plasticity factor due to environmental conditions affecting movement speed.
   * - ``daysWithoutFoodForMetabolicDownregulation``
     - ``number``
     - Number of days without food before metabolic downregulation occurs.
   * - ``percentageMetabolicDownregulation``
     - ``number``
     - Percentage reduction in metabolism during downregulation.
   * - ``percentageCostForMetabolicDownregulationVoracity``
     - ``number``
     - Cost in voracity associated with metabolic downregulation.
   * - ``percentageCostForMetabolicDownregulationSearchArea``
     - ``number``
     - Cost in search area associated with metabolic downregulation.
   * - ``percentageCostForMetabolicDownregulationSpeed``
     - ``number``
     - Cost in movement speed associated with metabolic downregulation.
   * - ``coefficientForHandlingTimeA``
     - ``number``
     - Coefficient *A* used in handling time calculations.
   * - ``coefficientForHandlingTimeB``
     - ``number``
     - Coefficient *B* used in handling time calculations.
   * - ``coefficientForHandlingTimeC``
     - ``number``
     - Coefficient *C* used in handling time calculations.
   * - ``debug``
     - ``object``
     - Defines debugging options for simulation testing.
   * - ``decisions``
     - ``object``
     - (Submodule: :doc:`decisions_module`)  
       Defines probabilistic and behavioral decision models used by the species.
   * - ``genetics``
     - ``object``
     - (Submodule: :doc:`genetics_module`)  
       Defines the genetic architecture of the species.
   * - ``growthModule``
     - ``object``
     - (Submodule: :doc:`growth_module`)  
       Defines the biological growth parameters and reproductive strategies of the species.

----------------------------------------------------------
Object ``debug``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``surviveWithoutFood``
     - ``boolean``
     - If ``true``, animals do not die from starvation. Useful for debugging.
   * - ``forcePresenceAllResourcesInvolved``
     - ``boolean``
     - Forces the presence of all resources used by the species, ignoring environmental constraints.

----------------------------------------------------------
Submodules
----------------------------------------------------------

.. toctree::
   :maxdepth: 1

   decisions_module
   genetics_module
   growth_module

----------------------------------------------------------
Hierarchical Summary
----------------------------------------------------------

.. code-block:: text

    species
    ├── version: string
    └── animal: object
        ├── name: string
        ├── defaultHuntingMode: enum [does_not_hunt, sit_and_wait, grazer]
        ├── sexualType: enum [diploid, haplodiploid, asexual]
        ├── YodzisA: number
        ├── YodzisB: number
        ├── activatedHandling: boolean
        ├── preserveLeftovers: boolean
        ├── sexRatio: number
        ├── debug: object
        │   ├── surviveWithoutFood: boolean
        │   └── forcePresenceAllResourcesInvolved: boolean
        ├── decisions: object
        ├── genetics: object 
        └── growthModule: object
