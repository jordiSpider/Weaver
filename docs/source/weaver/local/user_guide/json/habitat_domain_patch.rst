
====================
habitat_domain_patch
====================

It includes information about versioning, patch priority, affected animal species,  
and the geometric shape of the patch.

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
     - Object defining the content and parameters of the domain patch.

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
     - Defines the animal species affected by the patch.
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
   * - ``affectedAnimalSpecies``
     - ``array``
     - List of animal species affected by the patch.

Each element of this array is an object with:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``name``
     - ``string``
     - Name of the affected species.
   * - ``affectedInstars``
     - ``array[integer]``
     - List of affected instars (developmental stages). Each integer must be ≥ 1.

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
     - Parameters specific to cubic-shaped patches.
   * - ``sphericalParams``
     - ``object``
     - Parameters specific to spherical patches.
   * - ``homogeneousParams``
     - ``object``
     - Parameters specific to homogeneous patches.
   * - ``gaussianParams``
     - ``object``
     - Parameters specific to Gaussian-shaped patches.
   * - ``randomGaussianParams``
     - ``object``
     - Parameters specific to random Gaussian patches.

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

    habitat_domain_patch
    ├── version: string
    └── patch: object
        ├── priority: integer ≥ 1
        ├── source:
        │   └── affectedAnimalSpecies[]:
        │       ├── name: string
        │       └── affectedInstars[]: integer ≥ 1
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
