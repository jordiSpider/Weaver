
===============================
ontogeneticLinksPreference
===============================

Overview
========

The file ``ontogeneticLinksPreference.csv`` defines the **preference relationships** between species
(both animals and resources) within the ecosystem model.  

Each entry specifies **how strongly a given species prefers to interact with another**, typically in
the context of feeding, resource consumption, or other ecological interactions.

These preference values are used throughout the simulation to determine:

- Predator–prey preference intensity.
- Consumer–resource affinity.
- Symbiotic, competitive, or facilitative interactions between animal species.
- Ontogenetic changes in feeding preferences across life stages (if applicable in the model).

File Format
===========

The file is a **comma‐separated values (CSV)** table.

- **Rows (first column):** represent the *prey* or *resource species*.  
- **Columns (first row):** represent the *predators* or *consumers*.  
- **Cells:** contain numerical values indicating the **preference strength** of the predator (column)
  for the prey/resource (row).

A higher value indicates a stronger preference.  
Values may be zero when no ecological interaction exists.

Example Structure
=================

Although the actual file content depends on the ecosystem configuration, the structure is always:

::

    , PredatorA, PredatorB, PredatorC
    Prey1, 0.8, 0.1, 0.0
    Prey2, 0.3, 0.5, 0.2
    Prey3, 0.0, 0.7, 0.4

Interpretation Example
======================

In the snippet above:

- ``PredatorA`` has a high preference (0.8) for ``Prey1``.
- ``PredatorB`` prefers ``Prey3`` the most (0.7).
- ``PredatorC`` ignores ``Prey1`` (0.0).
