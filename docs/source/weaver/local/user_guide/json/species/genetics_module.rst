
===============
Genetics Module
===============

It defines the genetic architecture of each species, specifying the number of loci and alleles 
per trait, the recombination parameters, and the configuration of modules and traits 
that determine how genetic variation influences phenotypic traits in the simulation.

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
   * - ``numberOfLociPerTrait``
     - ``integer``
     - Number of loci defining each trait. Minimum value: ``0``.
   * - ``numberOfAllelesPerLocus``
     - ``integer``
     - Number of alleles per locus. Minimum value: ``0``.
   * - ``numberOfChiasmasPerChromosome``
     - ``integer``
     - Number of chiasmas per chromosome. Minimum value: ``0``.
   * - ``modules``
     - ``object``
     - Defines the modular genetic organization.
   * - ``modifyAlleles``
     - ``array[object]``
     - Describes allele modification events applied to certain loci or traits.
   * - ``traits``
     - ``object``
     - Describes the genetic definition of traits and their temperature dependence.

----------------------------------------------------------
Object ``modules``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``traitsPerModule``
     - ``integer``
     - Number of traits per genetic module. Minimum value: ``0``.
   * - ``correlationCoefficientRhoPerModule``
     - ``array[number]``
     - Correlation coefficients (ρ) describing genetic linkage among traits within a module.

----------------------------------------------------------
Object ``modifyAlleles``
----------------------------------------------------------

Properties of each element:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``affectedAlleles``
     - ``number``
     - Identifier of the affected allele.
   * - ``timesAlleles``
     - ``number``
     - Multiplicative factor applied to the alleles. Minimum: ``0.0``.
   * - ``affectedLocus``
     - ``array[number]``
     - List of locus indices (between 0 and 1) affected by the modification.
   * - ``affectedTraits``
     - ``array[object]``
     - List of trait elements influenced by the modification.

----------------------------------------------------------
Object ``affectedTraits``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``trait``
     - ``string``
     - Name of the trait affected.
   * - ``element``
     - ``string (enum)``
     - Component of the trait affected. Possible values:
       - ``value``
       - ``activationEnergy``
       - ``energyDecay``
       - ``temperatureOptimal``
       - ``temperatureRef``

----------------------------------------------------------
Object ``traits``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``individualLevelTraitsOrder``
     - ``array[object]``
     - Defines the order and structure of individual-level traits.
   * - ``definition``
     - ``object``
     - Contains the complete definitions of each trait type.

----------------------------------------------------------
Object ``individualLevelTraitsOrder``
----------------------------------------------------------

Properties of each element:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``trait``
     - ``string``
     - Name of the individual-level trait.
   * - ``element``
     - ``string (enum)``
     - Component of the trait. Possible values:
       - ``value``
       - ``activationEnergy``
       - ``energyDecay``
       - ``temperatureOptimal``
       - ``temperatureRef``

----------------------------------------------------------
Object ``definition``
----------------------------------------------------------

Contains the definition of the different types of traits.

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``base``
     - ``object``
     - A diverse set of traits with different functionalities.
   * - ``decisions``
     - ``object``
     - Traits of weights that influence decisions.
   * - ``probabilityDensityFunction``
     - ``object``
     - Traits of the coefficients of probability density functions.
   * - ``cellValue``
     - ``object``
     - Weights that influence the value of cells.


----------------------------------------------------------
Object ``base``
----------------------------------------------------------

Within this section, three types of traits are defined depending on the definition of their temperature subsection.

The traits without temperature definition are as follows:

- energy_tank

The traits with temperature size rule as their temperature definition are as follows:

- lengthAtMaturation

The traits with dells as the temperature definition are as follows:

- growth
- eggDevTime
- factorEggMass
- assim
- voracityProportion
- met_rate
- shock_resistance
- actE_met
- memoryDepth
- coeffMassForScopeRadius
- scaleMassForScopeRadius
- coeffMassForInteractionRadius
- scaleMassForInteractionRadius
- coeffMassForSearchRadius
- scaleMassForSearchRadius
- coeffMassForSpeed
- scaleMassForSpeed
- devTime
- pupaPeriodTime
- longevitySinceMaturation
- dummy
- eggFertility


----------------------------------------------------------
Object ``decisions``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``escapeProbabilityWeight``
     - ``object``
     - Factors influencing the probability of escape.
   * - ``predationProbabilityWeight``
     - ``object``
     - Factors influencing the probability of predation.
   * - ``edibilityValueWeight``
     - ``object``
     - Factors that influence edibility value.
   * - ``preferences``
     - ``object``
     - Factors influencing preferences.


----------------------------------------------------------
Object ``escapeProbabilityWeight``
----------------------------------------------------------

The weights defined in this section use traits with dells as the temperature definition.

The weights are as follows:

- Pvelocity
- PattackDistance

----------------------------------------------------------
Object ``predationProbabilityWeight``
----------------------------------------------------------

The weights defined in this section use traits with dells as the temperature definition.

The weights are as follows:

- Preach
- Ppdf
- PvorPred

----------------------------------------------------------
Object ``edibilityValueWeight``
----------------------------------------------------------

The weights defined in this section use traits with dells as the temperature definition.

The weights are as follows:

- Pp

----------------------------------------------------------
Object ``preferences``
----------------------------------------------------------

The weights defined in this section use traits with dells as the temperature definition.

The weights are as follows:

- experienceInfluenceWithEdibles

----------------------------------------------------------
Object ``probabilityDensityFunction``
----------------------------------------------------------

The coefficients defined in this section use traits with dells as the temperature definition.

The coefficients are as follows:

- muForPDF
- sigmaForPDF

----------------------------------------------------------
Object ``cellValue``
----------------------------------------------------------

The weights defined in this section use traits with dells as the temperature definition.

The weights are as follows:

- cellEvaluationBiomass
- cellEvaluationProConspecific
- conspecificWeight


----------------------------------------------------------
Object ``<trait-non-temperature-section>``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``definitionType``
     - ``string (enum)``
     - Type of definition. Possible values:
       - ``SpeciesLevel``
       - ``IndividualLevel``
   * - ``speciesLevelParams``
     - ``object``
     - Species-level parameters.
   * - ``individualLevelParams``
     - ``object``
     - Individual-level parameters.


----------------------------------------------------------
Object ``speciesLevelParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``value``
     - ``number``
     - Fixed species-level trait value.


----------------------------------------------------------
Object ``individualLevelParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``limits``
     - ``object``
     - Limits for individual-level trait values.
   * - ``ranges``
     - ``object``
     - Ranges for individual-level trait values.
   * - ``restrictValue``
     - ``number``
     - Restriction factor for individual-level trait values (between ``0.0`` and ``1.0``).


----------------------------------------------------------
Object ``limits``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``min``
     - ``number``
     - Minimum limit for individual-level trait values. Minimum value: ``0.0``.
   * - ``max``
     - ``number``
     - Maximum limit for individual-level trait values. Minimum value: ``0.0``.


----------------------------------------------------------
Object ``ranges``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``min``
     - ``number``
     - Minimum range for individual-level trait values.
   * - ``max``
     - ``number``
     - Maximum range for individual-level trait values.



----------------------------------------------------------
Object ``<trait-temperature-size-rule-section>``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``definitionType``
     - ``string (enum)``
     - Type of definition. Possible values:
       - ``SpeciesLevel``
       - ``IndividualLevel``
   * - ``speciesLevelParams``
     - ``object``
     - Species-level parameters.
   * - ``individualLevelParams``
     - ``object``
     - Individual-level parameters.
   * - ``temperature``
     - ``object``
     - Temperature dependence parameters.



-------------------------------------------------------------------
Object ``temperature`` of ``<trait-temperature-size-rule-section>``
-------------------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``dependent``
     - ``boolean``
     - Indicates if the trait is temperature-dependent.
   * - ``tempSizeRuleVector``
     - ``array[object]``
     - Array defining the temperature-size rule.


----------------------------------------------------------
Object ``tempSizeRuleVector``
----------------------------------------------------------

Properties of each element:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``temperature``
     - ``number``
     - Temperature value (greater than ``0.0``).
   * - ``dryMass``
     - ``number``
     - Dry mass value (greater than ``0.0``).


----------------------------------------------------------
Object ``<trait-dells-section>``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``definitionType``
     - ``string (enum)``
     - Type of definition. Possible values:
       - ``SpeciesLevel``
       - ``IndividualLevel``
   * - ``speciesLevelParams``
     - ``object``
     - Species-level parameters.
   * - ``individualLevelParams``
     - ``object``
     - Individual-level parameters.
   * - ``temperature``
     - ``object``
     - Temperature dependence parameters.



-------------------------------------------------------------------
Object ``temperature`` of ``<trait-dells-section>``
-------------------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``dependent``
     - ``boolean``
     - Indicates if the trait is temperature-dependent.
   * - ``activationEnergy``
     - ``<trait-non-temperature-section>``
     - Temperature activation energy parameters.
   * - ``energyDecay``
     - ``<trait-non-temperature-section>``
     - Temperature energy decay parameters.
   * - ``temperatureOptimal``
     - ``<trait-non-temperature-section>``
     - Temperature optimal parameters.
   * - ``temperatureRef``
     - ``<trait-non-temperature-section>``
     - Reference temperature parameters.


----------------------------------------------------------
Hierarchical Summary
----------------------------------------------------------

.. code-block:: text

    genetics
    ├── numberOfLociPerTrait: integer ≥ 0
    ├── numberOfAllelesPerLocus: integer ≥ 0
    ├── numberOfChiasmasPerChromosome: integer ≥ 0
    ├── modules:
    │   ├── traitsPerModule: integer ≥ 0
    │   └── correlationCoefficientRhoPerModule: array[number]
    ├── modifyAlleles:
    │   ├── affectedAlleles: number
    │   ├── timesAlleles: number ≥ 0.0
    │   ├── affectedLocus: array[number 0.0–1.0]
    │   └── affectedTraits:
    │       ├── trait: string
    │       └── element: enum [value, activationEnergy, energyDecay, temperatureOptimal, temperatureRef]
    └── traits:
        ├── individualLevelTraitsOrder:
        │   ├── trait: string
        │   └── element: enum [value, activationEnergy, energyDecay, temperatureOptimal, temperatureRef]
        └── definition:
            ├── base
            │   ├── <enum-base-nonTemperatureTraits>
            │   │   └── <trait-non-temperature-section>
            │   ├── <enum-base-temperatureSizeRuleTraits>
            │   │   └── <trait-temperature-size-rule-section>
            │   └── <enum-base-dellsTraits>
            │       └── <trait-dells-section>
            ├── decisions
            │   ├── escapeProbabilityWeight
            │   │   └── <enum-escapeProbabilityWeight-dellsTraits>
            │   │       └── <trait-dells-section>
            │   ├── predationProbabilityWeight
            │   │   └── <enum-predationProbabilityWeight-dellsTraits>
            │   │       └── <trait-dells-section>
            │   ├── edibilityValueWeight
            │   │   └── <enum-edibilityValueWeight-dellsTraits>
            │   │       └── <trait-dells-section>
            │   └── preferences
            │       └── <enum-preferences-dellsTraits>
            │           └── <trait-dells-section>
            ├── probabilityDensityFunction
            │   └── <enum-probabilityDensityFunction-dellsTraits>
            │       └── <trait-dells-section>
            └── cellValue
                └── <enum-cellValue-dellsTraits>
                    └── <trait-dells-section>

    <trait-non-temperature-section>
    ├── definitionType: enum [SpeciesLevel, IndividualLevel]
    ├── speciesLevelParams
    │   └── value: number
    └── individualLevelParams
        ├── limits
        │   ├── min: number  ≥ 0.0
        │   └── max: number  ≥ 0.0
        ├── ranges
        │   ├── min: number
        │   └── max: number
        └── restrictValue: number [0.0 - 1.0]

    <trait-temperature-size-rule-section>
    ├── definitionType: enum [SpeciesLevel, IndividualLevel]
    ├── speciesLevelParams
    │   └── value: number
    ├── individualLevelParams
    │   ├── limits
    │   │   ├── min: number  ≥ 0.0
    │   │   └── max: number  ≥ 0.0
    │   ├── ranges
    │   │   ├── min: number
    │   │   └── max: number
    │   └── restrictValue: number [0.0 - 1.0]
    └── temperature
        ├── dependent: boolean
        └── tempSizeRuleVector: array[object]
            ├── temperature: number > 0.0
            └── dryMass: number > 0.0

    <trait-dells-section>
    ├── definitionType: enum [SpeciesLevel, IndividualLevel]
    ├── speciesLevelParams
    │   └── value: number
    ├── individualLevelParams
    │   ├── limits
    │   │   ├── min: number  ≥ 0.0
    │   │   └── max: number  ≥ 0.0
    │   ├── ranges
    │   │   ├── min: number
    │   │   └── max: number
    │   └── restrictValue: number [0.0 - 1.0]
    └── temperature
        ├── dependent: boolean
        ├── activationEnergy
        │   └── <trait-non-temperature-section>
        ├── energyDecay
        │   └── <trait-non-temperature-section>
        ├── temperatureOptimal
        │   └── <trait-non-temperature-section>
        └── temperatureRef
            └── <trait-non-temperature-section>
