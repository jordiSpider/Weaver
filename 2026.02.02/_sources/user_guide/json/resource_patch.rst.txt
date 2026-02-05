
==============
resource_patch
==============

It describes the resource patch configuration, including resource species, growth dynamics, and geometric shape in the environment.

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
     - Patch version.
   * - ``patch``
     - ``object``
     - Object defining the resource patch properties.

----------------------------------------------------------
Object ``patch``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``resourceSpecies``
     - ``string``
     - Identifier of the resource species affected by the patch.
   * - ``priority``
     - ``integer``
     - Patch priority. Minimum value: ``1``.
   * - ``source``
     - ``object``
     - Defines resource source dynamics and parameters.
   * - ``shape``
     - ``object``
     - Defines the geometric shape of the patch area.

----------------------------------------------------------
Object ``source``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``resourceMaximumCapacityDensity``
     - ``number``
     - Maximum carrying capacity density for the resource.
   * - ``edibleFractionOfMaxCarryingCapacity``
     - ``number``
     - Fraction of the maximum carrying capacity that is edible.
   * - ``patchSpread``
     - ``boolean``
     - Indicates whether the patch spreads across neighboring regions.
   * - ``growthDynamics``
     - ``object``
     - Describes the temporal evolution of the resource availability.

----------------------------------------------------------
Object ``growthDynamics``
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
     - Type of resource dynamic. Possible values:
       - ``Cycle``  
       - ``Pulse``  
       - ``GrowthRate``
   * - ``cycleParams``
     - ``object``
     - Parameters for cyclic resource dynamics.
   * - ``pulseParams``
     - ``object``
     - Parameters for pulsed resource dynamics.
   * - ``growthRateParams``
     - ``object``
     - Parameters for continuous growth rate dynamics.

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
     - List of cycle values between ``0.0`` (exclusive) and ``1.0`` (inclusive). Minimum one element.

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
     - Range of days between pulses.
   * - ``pulsePeakValueRange``
     - ``object``
     - Range of pulse peak values (0.0–1.0).
   * - ``decay``
     - ``object``
     - Defines the decay behavior of each pulse.

-----------------------------------------------------------
Object ``daysBetweenPulsesRange`` / ``pulsePeakValueRange``
-----------------------------------------------------------

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
     - Minimum value (> 0.0).
   * - ``max``
     - ``number``
     - Maximum value (> 0.0).

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
     - Decay type. Possible values:
       - ``NonDecay``  
       - ``Lineal``
   * - ``linealDecayParams``
     - ``object``
     - Parameters for linear decay behavior.

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
     - Decay intensity over time, between (0.0, 1.0].

----------------------------------------------------------
Object ``growthRateParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``initialValue``
     - ``number``
     - Initial value of the resource (0.0–1.0).
   * - ``rMaxScale``
     - ``number``
     - Scaling factor for the maximum growth rate.
   * - ``variableIntrinsicRateOfIncrease``
     - ``boolean``
     - Whether the intrinsic rate of increase depends on temperature.
   * - ``temperature``
     - ``object``
     - Parameters that describe the temperature dependency (used only if ``variableIntrinsicRateOfIncrease`` is true).

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
   * - ``activationEnergy``
     - ``number``
     - Activation energy of growth.
   * - ``energyDecay``
     - ``number``
     - Energy decay rate.
   * - ``temperatureOptimal``
     - ``number``
     - Optimal temperature for growth.
   * - ``temperatureRef``
     - ``number``
     - Reference temperature.

----------------------------------------------------------
Object ``shape``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``type``
     - ``string (enum)`` 
     - Geometric shape type. Possible values:
       - ``Homogeneous``  
       - ``Gaussian``  
       - ``Spherical``  
       - ``Cubic``  
       - ``RandomGaussian``
   * - ``cubicParams``
     - ``object``
     - Parameters for cubic shapes.
   * - ``sphericalParams``
     - ``object``
     - Parameters for spherical shapes.
   * - ``homogeneousParams``
     - ``object``
     - Parameters for homogeneous shapes.
   * - ``gaussianParams``
     - ``object``
     - Parameters for Gaussian shapes.
   * - ``randomGaussianParams``
     - ``object``
     - Parameters for random Gaussian shapes.

----------------------------------------------------------
Object ``cubicParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``bottomLeftCorner``
     - ``array[number]``
     - Coordinates of the bottom-left corner.
   * - ``topRightCorner``
     - ``array[number]``
     - Coordinates of the top-right corner.

----------------------------------------------------------
Object ``sphericalParams``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``center``
     - ``array[number]``
     - Center coordinates of the sphere.
   * - ``radius``
     - ``number``
     - Radius of the sphere. Minimum value: ``0.0``.

----------------------------------------------------------
Object ``homogeneousParams``
----------------------------------------------------------
 
Currently does not contain any specific properties.

----------------------------------------------------------
Object ``gaussianParams``
----------------------------------------------------------
 
Currently does not contain any specific properties.

----------------------------------------------------------
Object ``randomGaussianParams``
----------------------------------------------------------
 
Currently does not contain any specific properties.

----------------------------------------------------------
Hierarchical Summary
----------------------------------------------------------

.. code-block:: text

    resource_patch
    ├── version: string
    └── patch: object
        ├── resourceSpecies: string
        ├── priority: integer ≥ 1
        ├── source:
        │   ├── resourceMaximumCapacityDensity: number
        │   ├── edibleFractionOfMaxCarryingCapacity: number
        │   ├── patchSpread: boolean
        │   └── growthDynamics:
        │       ├── dynamicsType: enum [Cycle, Pulse, GrowthRate]
        │       ├── cycleParams:
        │       │   └── cycle: array[number] (0.0–1.0)
        │       ├── pulseParams:
        │       │   ├── daysBetweenPulsesRange:
        │       │   │   ├── min: number > 0.0
        │       │   │   └── max: number > 0.0
        │       │   ├── pulsePeakValueRange:
        │       │   │   ├── min: number (0.0–1.0)
        │       │   │   └── max: number (0.0–1.0)
        │       │   └── decay:
        │       │       ├── decayType: enum [NonDecay, Lineal]
        │       │       └── linealDecayParams:
        │       │           └── decayOverTime: number (0.0–1.0)
        │       └── growthRateParams:
        │           ├── initialValue: number (0.0–1.0)
        │           ├── rMaxScale: number
        │           ├── variableIntrinsicRateOfIncrease: boolean
        │           └── temperature:
        │               ├── activationEnergy: number
        │               ├── energyDecay: number
        │               ├── temperatureOptimal: number
        │               └── temperatureRef: number
        └── shape:
            ├── type: enum [Homogeneous, Gaussian, Spherical, Cubic, RandomGaussian]
            ├── cubicParams:
            │   ├── bottomLeftCorner: array[number]
            │   └── topRightCorner: array[number]
            ├── sphericalParams:
            │   ├── center: array[number]
            │   └── radius: number ≥ 0.0
            ├── homogeneousParams: {}
            ├── gaussianParams: {}
            └── randomGaussianParams: {}
