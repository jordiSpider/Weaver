
===================================
ontogeneticLinksProfitability
===================================

Overview
========

The file ``ontogeneticLinksProfitability.csv`` defines the **energy gain**, **nutritional benefit**,  
or more generally the **profitability** that a species obtains when interacting with another species
(e.g., feeding on it).

This file specifies *how valuable* each prey or resource is for each predator or consumer.

Typical uses within the ecosystem model include:

- Energy intake during feeding events.
- Cost–benefit evaluation when selecting resources.
- Ontogenetic changes in dietary profitability across life stages.
- Differences in nutritional quality among prey or food types.

File Format
===========

The file is a **comma-separated values (CSV)** matrix.

- **Rows (first column):** represent the *prey* or *resource species*.  
- **Columns (first row):** represent the *predators* or *consumers*.  
- **Cells:** contain numerical values indicating the **profitability** a predator obtains from
  consuming or interacting with that prey/resource.

A higher value indicates a higher energetic or functional benefit.

Example Structure
=================

Structure example (values are only illustrative):

::

    , PredatorA, PredatorB, PredatorC
    Prey1, 0.9, 0.4, 0.0
    Prey2, 0.7, 0.98, 0.31
    Prey3, 0.0, 0.64, 0.52

Interpretation Example
======================

From the example:

- ``PredatorA`` obtains a high benefit (0.9) from ``Prey1``.
- ``PredatorB`` gains more energy from ``Prey2`` and ``Prey3``.
- ``PredatorC`` gains no benefit from ``Prey1`` (0.0), indicating either incompatibility or lack of consumption.
