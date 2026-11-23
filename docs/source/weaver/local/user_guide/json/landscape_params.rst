
================
landscape_params
================

It defines the version and configuration of the simulation landscape, including spatial structure, environmental dynamics, and life parameters for the selected ecosystem type.

.. warning::

   When the world is **initialized from a checkpoint**, only the parameters inside the :ref:`life <landscape_params_life>` object are used.
   All other configuration values in ``landscape_params`` are **ignored** during checkpoint loading.

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
     - Landscape configuration version.
   * - ``landscape``
     - ``object``
     - Defines the simulation landscape structure and biological parameters.

----------------------------------------------------------
Object ``landscape``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``simulationType``
     - ``string (enum)``
     - Defines the ecosystem type. Possible values:
       - ``Arthropods``  
       - ``Dinosaurs``  
       - ``Aquatic``
   * - ``mapConfig``
     - ``object``
     - Contains the map configuration and environmental parameters.
   * - ``life``
     - ``object``
     - Defines biological and ecological settings.

----------------------------------------------------------
Object ``mapConfig``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``mapType``
     - ``string (enum)``
     - Type of map used in the simulation. Possible value:
       - ``SpatialTree``
   * - ``landscapeWideParams``
     - ``object``
     - Global landscape parameters defining grid resolution.
   * - ``spatialTreeParams``
     - ``object``
     - Parameters specific to ``SpatialTree`` maps.
   * - ``moistureBasePatch``
     - ``object``
     - Base configuration for temperature and humidity dynamics.
   * - ``nutrients``
     - ``object``
     - Defines nutrient composition ranges.

----------------------------------------------------------
Object ``landscapeWideParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``minCellSize``
     - ``number``
     - Minimum cell size in spatial grid. Must be greater than ``0.0``.
   * - ``numberOfCellsAxisX``
     - ``integer``
     - Number of cells along the X axis. Minimum value: ``2``.
   * - ``numberOfCellsAxisY``
     - ``integer``
     - Number of cells along the Y axis. Minimum value: ``2``.

----------------------------------------------------------
Object ``spatialTreeParams``
----------------------------------------------------------
 
Currently does not contain any specific properties.

----------------------------------------------------------
Object ``moistureBasePatch``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``temperatureDynamics``
     - ``object``
     - Defines temperature cycle through the simulation.
   * - ``relativeHumidityDynamics``
     - ``object``
     - Defines relative humidity dynamics.
   * - ``maximumResourceCapacityDensity``
     - ``number``
     - Maximum density of available resources. Minimum value: ``0.0``.
   * - ``inEnemyFreeSpace``
     - ``boolean``
     - Indicates whether the patch is free of enemies.
   * - ``inCompetitorFreeSpace``
     - ``boolean``
     - Indicates whether the patch is free of competitors.

----------------------------------------------------------
Object ``temperatureDynamics``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``cycle``
     - ``array[number]``
     - Temperature cycle pattern. Must contain at least one value.

----------------------------------------------------------
Object ``relativeHumidityDynamics``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``dynamicsType``
     - ``string (enum)``
     - Type of humidity variation. Possible values:
       - ``Cycle``  
       - ``Pulse``
   * - ``cycleParams``
     - ``object``
     - Parameters for cyclic humidity variation.
   * - ``pulseParams``
     - ``object``
     - Parameters for pulse-type humidity variation.

----------------------------------------------------------
Object ``cycleParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``cycle``
     - ``array[number]``
     - Sequence of humidity values (%). Each element must be between ``0.0`` and ``100.0`` and the array must contain at least one element.

----------------------------------------------------------
Object ``pulseParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``daysBetweenPulsesRange``
     - ``object``
     - Range of days between humidity pulses.
   * - ``pulsePeakValueRange``
     - ``object``
     - Range of humidity peak values for each pulse.
   * - ``decay``
     - ``object``
     - Defines the humidity decay behavior after a pulse.

-------------------------------------------------------------
Object ``daysBetweenPulsesRange`` and ``pulsePeakValueRange``
-------------------------------------------------------------

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
     - Minimum value. Must be greater than ``0.0``.
   * - ``max``
     - ``number``
     - Maximum value. Must be greater than ``0.0``.

----------------------------------------------------------
Object ``decay``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``decayType``
     - ``string (enum)``
     - Defines decay behavior type. Possible values:
       - ``NonDecay``  
       - ``Lineal``
   * - ``linealDecayParams``
     - ``object``
     - Parameters for lineal decay configuration.

----------------------------------------------------------
Object ``linealDecayParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``decayOverTime``
     - ``number``
     - Linear decay rate over time. Minimum value: ``0.0``.

----------------------------------------------------------
Object ``nutrients``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``minC`` / ``maxC``
     - ``number``
     - Minimum and maximum carbon content.
   * - ``minN`` / ``maxN``
     - ``number``
     - Minimum and maximum nitrogen content.
   * - ``minP`` / ``maxP``
     - ``number``
     - Minimum and maximum phosphorus content.

----------------------------------------------------------
Object ``life``
----------------------------------------------------------
.. _landscape_params_life:

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``competitionAmongResourceSpecies``
     - ``boolean``
     - Enables or disables competition among resource species.
   * - ``exitTimeThreshold``
     - ``integer``
     - Time threshold (in steps) for entities to exit. Minimum value: ``0``.
   * - ``pdfThreshold``
     - ``number``
     - Probability density function threshold. Range: ``0.0``–``1.0``.
   * - ``multiplierForFieldMetabolicRate``
     - ``number``
     - Multiplier applied to the field metabolic rate.

----------------------------------------------------------
Hierarchical Summary
----------------------------------------------------------

.. code-block:: text

    landscape_params
    ├── version: string
    └── landscape:
        ├── simulationType: enum [Arthropods, Dinosaurs, Aquatic]
        ├── mapConfig:
        │   ├── mapType: enum [SpatialTree]
        │   ├── landscapeWideParams:
        │   │   ├── minCellSize: number > 0.0
        │   │   ├── numberOfCellsAxisX: integer ≥ 2
        │   │   └── numberOfCellsAxisY: integer ≥ 2
        │   ├── spatialTreeParams: {}
        │   ├── moistureBasePatch:
        │   │   ├── temperatureDynamics:
        │   │   │   └── cycle: array[number]
        │   │   ├── relativeHumidityDynamics:
        │   │   │   ├── dynamicsType: enum [Cycle, Pulse]
        │   │   │   ├── cycleParams:
        │   │   │   │   └── cycle: array[number 0.0–100.0]
        │   │   │   └── pulseParams:
        │   │   │       ├── daysBetweenPulsesRange: {min > 0.0, max > 0.0}
        │   │   │       ├── pulsePeakValueRange: {min 0.0–100.0, max 0.0–100.0}
        │   │   │       └── decay:
        │   │   │           ├── decayType: enum [NonDecay, Lineal]
        │   │   │           └── linealDecayParams:
        │   │   │               └── decayOverTime: number ≥ 0.0
        │   │   ├── maximumResourceCapacityDensity: number ≥ 0.0
        │   │   ├── inEnemyFreeSpace: boolean
        │   │   └── inCompetitorFreeSpace: boolean
        │   └── nutrients:
        │       ├── minC / maxC: number
        │       ├── minN / maxN: number
        │       └── minP / maxP: number
        └── life:
            ├── competitionAmongResourceSpecies: boolean
            ├── exitTimeThreshold: integer ≥ 0
            ├── pdfThreshold: number 0.0–1.0
            └── multiplierForFieldMetabolicRate: number
