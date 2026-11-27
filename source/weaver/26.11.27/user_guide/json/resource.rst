
========
resource
========

It defines the version and configuration of a resource entity, including its biological growth parameters and environmental constraints.

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
     - Resource configuration version.
   * - ``resource``
     - ``object``
     - Object defining the resource properties and growth parameters.

----------------------------------------------------------
Object ``resource``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``growthModule``
     - ``object``
     - Defines the biological growth parameters of the resource.
   * - ``name``
     - ``string``
     - Resource name or identifier.
   * - ``zeroResource``
     - ``integer``
     - Minimum allowed value for the resource.  
       Minimum value: ``0``.

----------------------------------------------------------
Object ``growthModule``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``cellMass``
     - ``number``
     - Mass per individual cell in arbitrary units.
   * - ``conversionToWetMass``
     - ``number``
     - Conversion factor from dry mass to wet mass.
   * - ``minRH``
     - ``number``
     - Minimum relative humidity (%) required for growth.  
       Range: ``0.0`` – ``100.0``.
   * - ``maxRH``
     - ``number``
     - Maximum relative humidity (%) allowed for growth.  
       Range: ``0.0`` – ``100.0``.

----------------------------------------------------------
Hierarchical Summary
----------------------------------------------------------

.. code-block:: text

    resource
    ├── version: string
    └── resource: object
        ├── name: string
        ├── zeroResource: integer ≥ 0
        └── growthModule: object
            ├── cellMass: number
            ├── conversionToWetMass: number
            ├── minRH: number [0.0–100.0]
            └── maxRH: number [0.0–100.0]
