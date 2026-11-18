=============
Growth Module
=============

It defines the biological growth parameters and reproductive strategies of the species.  
This module determines how individuals grow, molt, and reproduce according to both intrinsic parameters and environmental factors such as temperature and habitat shifts.

----------------------------------------------------------
Main Properties
----------------------------------------------------------

Properties (JSON name: ``growthModule``):

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``betaScaleTank``
     - ``number``
     - Scaling coefficient affecting metabolic or growth rate within controlled (tank-like) environments.
   * - ``moltingAgeThreshold``
     - ``number``
     - Age (in time units) at which molting occurs.
   * - ``capitalBreeding``
     - ``object``
     - Parameters related to capital breeding (reproduction based on stored energy reserves).
   * - ``coefficientForMassA``
     - ``number``
     - Growth coefficient (A) for early developmental stages.
   * - ``coefficientForMassAforMature``
     - ``number``
     - Growth coefficient (A) for mature individuals.
   * - ``assignedForMolt``
     - ``number``
     - Fraction of resources allocated to molting.
   * - ``ageVector``
     - ``array[number]``
     - Vector of age checkpoints (strictly increasing). Minimum value: ``0.0``.
   * - ``eggsPerBatch``
     - ``object``
     - Parameters defining egg production per reproductive batch.
   * - ``femaleWetMass``
     - ``number``
     - Wet mass of an adult female (in grams or arbitrary units).
   * - ``habitatShiftFactor``
     - ``number`` *(nullable)*
     - Scaling factor applied when the species transitions between habitats.
   * - ``scaleForMassB``
     - ``number``
     - Growth scaling coefficient (B) for immature stages.
   * - ``scaleForMassBforMature``
     - ``number``
     - Growth scaling coefficient (B) for mature individuals.
   * - ``maxPlasticityKVonBertalanffy``
     - ``number``
     - Upper limit of plasticity for the Von Bertalanffy growth constant ``k``.
   * - ``minPlasticityKVonBertalanffy``
     - ``number``
     - Lower limit of plasticity for the Von Bertalanffy growth constant ``k``.
   * - ``habitatShift``
     - ``array[integer]``
     - Age or instar indices where habitat shifts occur. Minimum value: ``2``.
   * - ``excessInvestInSize``
     - ``number``
     - Additional investment fraction in body size beyond standard growth.
   * - ``eggDryMass``
     - ``object``
     - Parameters defining the dry mass of eggs.
   * - ``growthModel``
     - ``object``
     - Definition of the mathematical model used for growth, possibly temperature-dependent.
   * - ``instarFirstReproduction``
     - ``integer``
     - Instar (developmental stage) at which reproduction begins. Minimum: ``2``.

----------------------------------------------------------
Object ``capitalBreeding``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``enabled``
     - ``boolean``
     - Whether capital breeding is active.
   * - ``capitalBreedingParams``
     - ``object``
     - Parameters defining the duration and number of capital breeding events.

----------------------------------------------------------
Object ``capitalBreedingParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``timeOfReproEventDuringCapitalBreeding``
     - ``number`` *(nullable, ≥ 0.0)*
     - Timing of the reproductive event during the capital breeding cycle.
   * - ``numberOfCapitalBreeds``
     - ``integer`` *(nullable, ≥ 0)*
     - Number of reproductive events using stored energy.

----------------------------------------------------------
Object ``eggsPerBatch``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``eggsPerBatchFromEquation``
     - ``boolean``
     - Whether the egg count is calculated via a formula.
   * - ``value``
     - ``integer`` *(nullable, ≥ 0)*
     - Fixed number of eggs per batch (used when equation-based model is disabled).
   * - ``equationParameters``
     - ``object``
     - Parameters for the egg production formula.

----------------------------------------------------------
Object ``eggsPerBatch - equationParameters``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``interceptForEggBatchFromEquation``
     - ``number`` *(nullable)*
     - Intercept term of the regression defining egg production.
   * - ``slopeForEggBatchFromEquation``
     - ``number`` *(nullable)*
     - Slope term of the regression defining egg production.

----------------------------------------------------------
Object ``eggDryMass``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``eggDryMassFromEquation``
     - ``boolean``
     - Whether the egg dry mass is calculated from a regression equation.
   * - ``value``
     - ``number`` *(nullable)*
     - Fixed dry mass value when not using the equation.
   * - ``equationParameters``
     - ``object``
     - Parameters defining the regression model for egg dry mass.

----------------------------------------------------------
Object ``eggDryMass - equationParameters``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``coefficient``
     - ``number`` *(nullable)*
     - Regression coefficient controlling the scale of dry mass.
   * - ``scale``
     - ``number`` *(nullable)*
     - Scale factor for the regression model.

----------------------------------------------------------
Object ``growthModel``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``defaultAtTempFromLab``
     - ``object``
     - Growth model determined at laboratory temperature conditions.
   * - ``temperature``
     - ``object``
     - Defines thermal dependency and potential temperature-specific growth models.

----------------------------------------------------------
Object ``defaultAtTempFromLab``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``model``
     - ``string (enum)``
     - Model used for growth. Possible values:
       - ``Linear``  
       - ``VonBertalanffy``  
       - ``Logistic``  
       - ``Exponential``  
       - ``Logistic4P_FixedA``
   * - ``A``
     - ``number``
     - Growth coefficient of the model.

----------------------------------------------------------
Object ``temperature``
----------------------------------------------------------

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
     - Whether the growth rate depends on temperature.
   * - ``growthModelThermalChanges``
     - ``array[object]`` *(nullable)*
     - Defines temperature-specific model changes.

----------------------------------------------------------
Object ``growthModelThermalChanges`` (elements)
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``temperature``
     - ``number`` ( > 0.0 )
     - Temperature at which the change applies.
   * - ``model``
     - ``string (enum)``
     - Growth model used at this temperature. Possible values:
       - ``Linear``  
       - ``VonBertalanffy``  
       - ``Logistic``  
       - ``Exponential``  
       - ``Logistic4P_FixedA``
   * - ``A``
     - ``number``
     - Model coefficient at this temperature.

----------------------------------------------------------
Hierarchical Summary
----------------------------------------------------------

.. code-block:: text

    growthModule
    ├── betaScaleTank: number
    ├── moltingAgeThreshold: number
    ├── capitalBreeding:
    │   ├── enabled: boolean
    │   └── capitalBreedingParams:
    │       ├── timeOfReproEventDuringCapitalBreeding: number ≥ 0.0 (nullable)
    │       └── numberOfCapitalBreeds: integer ≥ 0 (nullable)
    ├── coefficientForMassA: number
    ├── coefficientForMassAforMature: number
    ├── assignedForMolt: number
    ├── ageVector: array[number ≥ 0.0] (strictly increasing)
    ├── eggsPerBatch:
    │   ├── eggsPerBatchFromEquation: boolean
    │   ├── value: integer ≥ 0 (nullable)
    │   └── equationParameters:
    │       ├── interceptForEggBatchFromEquation: number (nullable)
    │       └── slopeForEggBatchFromEquation: number (nullable)
    ├── femaleWetMass: number
    ├── habitatShiftFactor: number (nullable)
    ├── scaleForMassB: number
    ├── scaleForMassBforMature: number
    ├── maxPlasticityKVonBertalanffy: number
    ├── minPlasticityKVonBertalanffy: number
    ├── habitatShift: array[integer ≥ 2]
    ├── excessInvestInSize: number
    ├── eggDryMass:
    │   ├── eggDryMassFromEquation: boolean
    │   ├── value: number (nullable)
    │   └── equationParameters:
    │       ├── coefficient: number (nullable)
    │       └── scale: number (nullable)
    ├── growthModel:
    │   ├── defaultAtTempFromLab:
    │   │   ├── model: enum [Linear, VonBertalanffy, Logistic, Exponential, Logistic4P_FixedA]
    │   │   └── A: number
    │   └── temperature:
    │       ├── dependent: boolean
    │       └── growthModelThermalChanges: array[object]
    │           ├── temperature: number > 0.0
    │           ├── model: enum [Linear, VonBertalanffy, Logistic, Exponential, Logistic4P_FixedA]
    │           └── A: number
    └── instarFirstReproduction: integer ≥ 2
