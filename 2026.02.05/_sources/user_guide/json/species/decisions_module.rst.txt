
================
Decisions Module
================

It defines the decision-making models that govern animal behavior, including how species assess predation opportunities, resource quality, and spatial movement between patches.

----------------------------------------------------------
Main Properties
----------------------------------------------------------

Properties (JSON name: ``decisions``):

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``predationProbability``
     - ``object``
     - Defines the probability model governing predation attempts and escape behavior.
   * - ``edibilityValue``
     - ``object``
     - Determines how resource quality affects edibility and consumption decisions.
   * - ``decisionWithinPatch``
     - ``object``
     - Controls decision-making for interactions within a single patch (e.g., kill probability).
   * - ``decisionAmongPatches``
     - ``object``
     - Defines movement and preference between different patches based on individual and global assessment weights.
   * - ``sensoryModel``
     - ``object``
     - Configures the sensory response model of the species to environmental stimuli.

----------------------------------------------------------
Object ``predationProbability``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``additiveMechanism``
     - ``boolean``
     - If ``true``, uses additive combination of probability components instead of multiplicative.
   * - ``isRemainingWeightNull``
     - ``boolean``
     - If ``true``, residual weight contributions are ignored.
   * - ``escapeProbability``
     - ``object``
     - Defines parameters related to prey escape behavior.
   * - ``useGlobalMaximumPreyVoracity``
     - ``boolean``
     - If ``true``, normalizes predation probability based on the globally maximum prey voracity.

----------------------------------------------------------
Object ``escapeProbability``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``additiveMechanism``
     - ``boolean``
     - If ``true``, combines escape components additively.
   * - ``isRemainingWeightNull``
     - ``boolean``
     - If ``true``, ignores remaining weighting factors.
   * - ``cVelocity``
     - ``number`` (0.0 ≤ value ≤ 1.0)
     - Scaling coefficient representing the prey’s relative velocity or agility.

----------------------------------------------------------
Object ``edibilityValue``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``additiveMechanism``
     - ``boolean``
     - If ``true``, edibility factors are combined additively.
   * - ``isRemainingWeightNull``
     - ``boolean``
     - If ``true``, ignores leftover weighting in the edibility model.
   * - ``qualityResourceAssessment``
     - ``boolean``
     - Enables evaluation of resource quality when making feeding decisions.

----------------------------------------------------------
Object ``decisionWithinPatch``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``killProbability``
     - ``number`` (0.0 ≤ value ≤ 1.0)
     - Probability of successfully killing prey within the same patch.

----------------------------------------------------------
Object ``decisionAmongPatches``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``weightIndividualToGlobalAssessment``
     - ``number`` (0.0 ≤ value ≤ 1.0)
     - Balance factor between local (individual) and global (environmental) assessment when deciding patch transitions.

----------------------------------------------------------
Object ``sensoryModel``
----------------------------------------------------------

Properties:

.. list-table::
   :header-rows: 1
   :widths: 25 15 60
   :align: left

   * - **Name**
     - **Type**
     - **Description**
   * - ``beta``
     - ``number``
     - Sensitivity parameter controlling the steepness of the sensory response curve.

----------------------------------------------------------
Hierarchical Summary
----------------------------------------------------------

.. code-block:: text

    decisions
    ├── predationProbability: object
    │   ├── additiveMechanism: boolean
    │   ├── isRemainingWeightNull: boolean
    │   ├── escapeProbability: object
    │   │   ├── additiveMechanism: boolean
    │   │   ├── isRemainingWeightNull: boolean
    │   │   └── cVelocity: number [0.0–1.0]
    │   └── useGlobalMaximumPreyVoracity: boolean
    ├── edibilityValue: object
    │   ├── additiveMechanism: boolean
    │   ├── isRemainingWeightNull: boolean
    │   └── qualityResourceAssessment: boolean
    ├── decisionWithinPatch: object
    │   └── killProbability: number [0.0–1.0]
    ├── decisionAmongPatches: object
    │   └── weightIndividualToGlobalAssessment: number [0.0–1.0]
    └── sensoryModel: object
        └── beta: number
