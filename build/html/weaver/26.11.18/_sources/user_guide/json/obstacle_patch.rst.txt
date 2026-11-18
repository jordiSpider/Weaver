
==============
obstacle_patch
==============

It describes the patch version, priority, and geometric shape configuration for defining obstacle regions in the environment.

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
     - Object defining the obstacle patch properties.

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
   * - ``shape``
     - ``object``
     - Defines the geometric shape of the obstacle patch.

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

    obstacle_patch
    ├── version: string
    └── patch: object
        ├── priority: integer ≥ 1
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
