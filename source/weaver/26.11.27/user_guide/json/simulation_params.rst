
=================
simulation_params
=================

It defines the version and main simulation parameters, including runtime configuration, output options, checkpointing behavior, and termination conditions.

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
     - Simulation parameters version.
   * - ``simulation``
     - ``object``
     - Object defining the simulation configuration and operational parameters.

----------------------------------------------------------
Object ``simulation``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``checkpoints``
     - ``object``
     - Defines checkpoint saving behavior during the simulation.
   * - ``numberOfCombinations``
     - ``integer``
     - Number of simulation combinations to run. Minimum value: ``1``.
   * - ``breakConditions``
     - ``object``
     - Defines conditions for early termination of the simulation.
   * - ``growthAndReproTest``
     - ``boolean``
     - Enables a growth and reproduction test mode.
   * - ``runDays``
     - ``integer``
     - Number of simulation days. Minimum value: ``1``.
   * - ``timeStepsPerDay``
     - ``number``
     - Number of time steps per day. Must be greater than ``0.0``.
   * - ``initFromFixedSeed``
     - ``object``
     - Defines whether the simulation initializes with a fixed random seed.
   * - ``outputs``
     - ``object``
     - Defines which data and files will be generated as output.

----------------------------------------------------------
Object ``checkpoints``
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
     - Enables or disables checkpoint recording.
   * - ``recordEach``
     - ``integer``
     - Interval between checkpoints. Minimum value: ``0``.
   * - ``binaryEnabled``
     - ``boolean``
     - Enables binary checkpoint format.

----------------------------------------------------------
Object ``breakConditions``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``exitAtFirstExtinction``
     - ``boolean``
     - Stops the simulation when the first species extinction occurs.
   * - ``stability``
     - ``object``
     - Stability-based termination configuration.

----------------------------------------------------------
Object ``stability``
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
     - Enables stability-based termination condition.
   * - ``coefficientOfVariationThreshold``
     - ``number``
     - Coefficient of variation threshold for stability detection.  
       Minimum value: ``0.0``.
   * - ``stabilityCalculationInterval``
     - ``number``
     - Interval used for calculating stability.  
       Must be strictly greater than ``0.0``.

----------------------------------------------------------
Object ``initFromFixedSeed``
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
     - Enables initialization with a fixed seed.
   * - ``fixedSeedValue``
     - ``integer``
     - Specific fixed seed value used when enabled.

----------------------------------------------------------
Object ``outputs``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``saveAnimalConstitutiveTraits``
     - ``boolean``
     - Saves constitutive traits of each animal.
   * - ``saveGenetics``
     - ``boolean``
     - Saves genetic information.
   * - ``saveDailySummary``
     - ``boolean``
     - Saves a daily summary file.
   * - ``saveExtendedDailySummary``
     - ``boolean``
     - Saves extended daily statistics.
   * - ``saveMovements``
     - ``boolean``
     - Saves animal movement data.
   * - ``saveEdibilitiesFile``
     - ``boolean``
     - Saves edibility matrices between species.
   * - ``savePredationEventsOnOtherSpecies``
     - ``boolean``
     - Records inter-species predation events.
   * - ``recordEach``
     - ``integer``
     - Interval for recording outputs. Minimum value: ``0``.
   * - ``saveIntermidiateVolumes``
     - ``boolean``
     - Enables saving of intermediate simulation volumes.
   * - ``saveIntermidiateVolumesPeriodicity``
     - ``integer``
     - Periodicity of intermediate volume saves. Minimum value: ``0``.
   * - ``saveAnimalsEachDayStart``
     - ``boolean``
     - Saves animal states at the start of each day.
   * - ``saveAnimalsEachDayEnd``
     - ``boolean``
     - Saves animal states at the end of each day.
   * - ``saveCellsEachDay``
     - ``boolean``
     - Saves cell data each day.
   * - ``saveAnimalsEachDayVoracities``
     - ``boolean``
     - Saves voracity data for each animal daily.
   * - ``saveAnimalsEachDayPredationProbabilities``
     - ``boolean``
     - Saves predation probability data per animal daily.
   * - ``saveActivity``
     - ``boolean``
     - Saves animal activity logs.
   * - ``saveMassInfo``
     - ``boolean``
     - Saves information about mass changes throughout the simulation.

----------------------------------------------------------
Warnings
----------------------------------------------------------

- **Condition:** ``recordEach > runDays``  
  **Warning:** Item ``recordEach`` is greater than item ``runDays``.

----------------------------------------------------------
Hierarchical Summary
----------------------------------------------------------

.. code-block:: text

    simulation_params
    ├── version: string
    └── simulation: object
        ├── checkpoints:
        │   ├── enabled: boolean
        │   ├── recordEach: integer ≥ 0
        │   └── binaryEnabled: boolean
        ├── numberOfCombinations: integer ≥ 1
        ├── breakConditions:
        │   ├── exitAtFirstExtinction: boolean
        │   └── stability:
        │       ├── enabled: boolean
        │       ├── coefficientOfVariationThreshold: number ≥ 0.0
        │       └── stabilityCalculationInterval: number > 0.0
        ├── growthAndReproTest: boolean
        ├── runDays: integer ≥ 1
        ├── timeStepsPerDay: number > 0.0
        ├── initFromFixedSeed:
        │   ├── enabled: boolean
        │   └── fixedSeedValue: integer
        └── outputs:
            ├── saveAnimalConstitutiveTraits: boolean
            ├── saveGenetics: boolean
            ├── saveDailySummary: boolean
            ├── saveExtendedDailySummary: boolean
            ├── saveMovements: boolean
            ├── saveEdibilitiesFile: boolean
            ├── savePredationEventsOnOtherSpecies: boolean
            ├── recordEach: integer ≥ 0
            ├── saveIntermidiateVolumes: boolean
            ├── saveIntermidiateVolumesPeriodicity: integer ≥ 0
            ├── saveAnimalsEachDayStart: boolean
            ├── saveAnimalsEachDayEnd: boolean
            ├── saveCellsEachDay: boolean
            ├── saveAnimalsEachDayVoracities: boolean
            ├── saveAnimalsEachDayPredationProbabilities: boolean
            ├── saveActivity: boolean
            └── saveMassInfo: boolean
