
==============
moisture_patch
==============

It describes the patch version, priority, environmental source dynamics (temperature and humidity),  
and geometric shape configuration.

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
     - Object defining the patch properties and environmental parameters.

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
   * - ``priority``
     - ``integer``
     - Patch priority. Minimum value: ``1``.
   * - ``source``
     - ``object``
     - Defines the environmental sources affected by the patch.
   * - ``shape``
     - ``object``
     - Defines the geometric shape of the patch.

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
   * - ``temperatureDynamics``
     - ``object``
     - Defines temporal temperature variation.
   * - ``relativeHumidityDynamics``
     - ``object``
     - Defines the temporal evolution of relative humidity.
   * - ``maximumResourceCapacityDensity``
     - ``number``
     - Maximum resource capacity density. Minimum value: ``0.0``.
   * - ``inEnemyFreeSpace``
     - ``boolean``
     - Whether the patch exists in an enemy-free space.
   * - ``inCompetitorFreeSpace``
     - ``boolean``
     - Whether the patch exists in a competitor-free space.

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
     - Temperature cycle values over time.  
       Must contain at least one value.

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
     - Type of humidity dynamics. Possible values:
       - ``Cycle``  
       - ``Pulse``
   * - ``cycleParams``
     - ``object``
     - Parameters for the cyclic humidity dynamics.
   * - ``pulseParams``
     - ``object``
     - Parameters for pulse-based humidity dynamics.

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
     - Sequence of humidity values representing one full cycle.  
       Each value must be between ``0.0`` and ``100.0``.  
       Must contain at least one element.

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
     - Range of days between consecutive pulses.
   * - ``pulsePeakValueRange``
     - ``object``
     - Range of peak humidity values for each pulse.
   * - ``decay``
     - ``object``
     - Defines how humidity decays after a pulse.

----------------------------------------------------------
Object ``daysBetweenPulsesRange``
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
     - Minimum days between pulses. Must be greater than 0.0.
   * - ``max``
     - ``number``
     - Maximum days between pulses. Must be greater than 0.0.

----------------------------------------------------------
Object ``pulsePeakValueRange``
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
     - Minimum peak humidity percentage. Must be between ``0.0`` and ``100.0``.
   * - ``max``
     - ``number``
     - Maximum peak humidity percentage. Must be between ``0.0`` and ``100.0``.

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
     - Type of decay. Possible values:
       - ``NonDecay``  
       - ``Lineal``
   * - ``linealDecayParams``
     - ``object``
     - Parameters for linear decay over time.

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

    moisture_patch
    ├── version: string
    └── patch: object
        ├── priority: integer ≥ 1
        ├── source:
        │   ├── temperatureDynamics:
        │   │   └── cycle[]: number
        │   ├── relativeHumidityDynamics:
        │   │   ├── dynamicsType: enum [Cycle, Pulse]
        │   │   ├── cycleParams:
        │   │   │   └── cycle[]: number 0–100
        │   │   ├── pulseParams:
        │   │   │   ├── daysBetweenPulsesRange:
        │   │   │   │   ├── min: number > 0.0
        │   │   │   │   └── max: number > 0.0
        │   │   │   ├── pulsePeakValueRange:
        │   │   │   │   ├── min: number 0–100
        │   │   │   │   └── max: number 0–100
        │   │   │   └── decay:
        │   │   │       ├── decayType: enum [NonDecay, Lineal]
        │   │   │       └── linealDecayParams:
        │   │   │           └── decayOverTime: number ≥ 0.0
        │   ├── maximumResourceCapacityDensity: number ≥ 0.0
        │   ├── inEnemyFreeSpace: boolean
        │   └── inCompetitorFreeSpace: boolean
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
