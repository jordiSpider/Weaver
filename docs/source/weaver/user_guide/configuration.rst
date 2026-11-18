
.. _configuration:

=============
Configuration
=============

.. note::

   During installation, two main configuration folders are created:

   - **config/** — located in the program’s installation directory.  
     This folder contains **default configuration examples** provided with the program.

   - **user_config/** — Created automatically in the user’s *Documents* folder (or the system-specific equivalent).  
     This folder is intended for user-defined configurations.  
     While configurations can be stored anywhere, it is recommended to place them here for convenience.

Program configurations define **all the parameters of the simulation**.  
Each configuration is composed of several **JSON** and **CSV** files organized in a hierarchical directory structure.  
Together, these files describe the environment, species, interactions, and runtime behavior of the simulation.

Types of Configurations
=======================

There are two main configuration modes, depending on how the landscape state is initialized:

- **Initial Landscape Configuration (Mode: FromConfig)**  
   The simulation starts from an empty landscape.  
   All parameters and entities are defined through JSON files and all species interactions are defined through CSV files.  
   This mode provides full control over every aspect of the simulation setup.

- **Checkpoint-based Configuration (Mode: FromTextCheckpoint or FromBinaryCheckpoint)**  
   The simulation starts from a previously saved landscape state (a *checkpoint*).  
   In this case, the configuration directory contains:
   
   - A **checkpoint file** (either text or binary, depending on the mode).
   - Additional **JSON** files defining modifications or updates to apply over the loaded landscape.
   - The same **CSV** files that describe species interactions.

Folder Structure
================

**Initial Landscape configuration:**

.. code-block:: text

   my_landscape_config/
   ├── simulation_params.json
   ├── landscape_params.json
   ├── habitat_domain/
   │   ├── habitat_domain_patch_1.json
   │   ├── habitat_domain_patch_2.json
   │   └── .....
   ├── moisture/
   │   ├── moisture_patch_1.json
   │   ├── moisture_patch_2.json
   │   └── .....
   ├── obstacle/
   │   ├── obstacle_patch_1.json
   │   ├── obstacle_patch_2.json
   │   └── .....
   ├── resource/
   │   ├── resource_1.json
   │   ├── resource_2.json
   │   ├── .....
   │   └── patches/
   │       ├── resource_patch_1.json
   │       ├── resource_patch_2.json
   │       └── .....
   └── species/
       ├── species_1.json
       ├── species_2.json
       ├── .....
       ├── ontogeneticLinksPreference.csv
       └── ontogeneticLinksProfitability.csv

**Checkpoint-based configuration:**

.. code-block:: text

   my_checkpoint_config/
   ├── simulation_params.json
   ├── landscape_params.json
   ├── checkpoint/
   │   └── checkpoint.bin / checkpoint.txt
   ├── habitat_domain/
   │   ├── habitat_domain_patch_1.json
   │   ├── habitat_domain_patch_2.json
   │   └── .....
   ├── moisture/
   │   ├── moisture_patch_1.json
   │   ├── moisture_patch_2.json
   │   └── .....
   ├── obstacle/
   │   ├── obstacle_patch_1.json
   │   ├── obstacle_patch_2.json
   │   └── .....
   ├── resource/
   │   ├── resource_1.json
   │   ├── resource_2.json
   │   ├── .....
   │   └── patches/
   │       ├── resource_patch_1.json
   │       ├── resource_patch_2.json
   │       └── .....
   └── species/
       ├── species_1.json
       ├── species_2.json
       ├── .....
       ├── ontogeneticLinksPreference.csv
       └── ontogeneticLinksProfitability.csv

File Descriptions
=================

simulation_params.json  
----------------------

.. important::
   The file name **must be exactly** ``simulation_params.json``.  
   The program explicitly looks for this file to load global simulation control parameters.

Defines high-level control aspects of the simulation:
- Enabling or disabling data outputs.
- Total simulation time.
- Time step between iterations.
- Stop and checkpoint conditions.
- Control of output frequency and verbosity.

landscape_params.json
---------------------

.. important::
   The file name **must be exactly** ``landscape_params.json``.  
   This file establishes the global physical and environmental parameters for the simulation.

Defines:
- Landscape dimensions and resolution.
- Base humidity and temperature.
- Global baseline for resource capacity.

habitat_domain_patch.json  
-------------------------

.. note::
   The file name **can be arbitrary**, provided it is stored within the appropriate
   subfolder ``habitat_domain/``.  
   You can define **multiple habitat domain patch** files to compose complex environmental maps.

Defines habitat domains for animal species.

moisture_patch.json  
-------------------

.. note::
   The file name **can be arbitrary**, provided it is stored within the appropriate
   subfolder ``moisture/``.  
   You can define **multiple moisture patch** files to compose complex environmental maps.

Defines prioritized regions that locally modify:
- Humidity.
- Temperature.
- Resource capacity.

obstacle_patch.json  
-------------------

.. note::
   The file name **can be arbitrary**, provided it is stored within the appropriate
   subfolder ``obstacle/``.  
   You can define **multiple obstacle patch** files to compose complex environmental maps.

Defines prioritized zones that act as **impassable obstacles** for all species.

resource.json  
-------------

.. important::
   The file name **can be arbitrary**, provided it is stored within the appropriate
   subfolder ``resource/``.

Defines all the parameters of a resource species.

resource_patch.json  
-------------------

.. note::
   The file name **can be arbitrary**, provided it is stored within the appropriate
   subfolder ``resource/patches/``.  
   You can define **multiple resource patch** files to compose complex environmental maps.

Defines prioritized zones that promote resource growth.

species.json  
------------

.. important::
   The file name **can be arbitrary**, provided it is stored within the appropriate
   subfolder ``species/``.

Defines all the parameters of an animal species.

ontogeneticLinksPreference.csv 
------------------------------

.. important::
   The file name **must be exactly** ``ontogeneticLinksPreference.csv``.  
   The program explicitly loads this file to read inter-species preference data.

Defines the **preference relationships** between species (animals and resources).  
Each entry specifies how much a given species prefers interacting (e.g., feeding) with another.

Typical contents include:
- Predator–prey or consumer–resource preference strength.
- Symbiotic or competitive interactions between animal species.

ontogeneticLinksProfitability.csv 
---------------------------------

.. important::
   The file name **must be exactly** ``ontogeneticLinksProfitability.csv``.  
   This file provides the **profitability relationships** between species.

Defines the **energy or benefit gain** that a species obtains when interacting with another.

checkpoint.bin / checkpoint.txt  
-------------------------------

.. note::
   The file name **can be arbitrary**, provided it is stored within the appropriate
   subfolder ``checkpoint/``. 

Represents a saved state of the simulation (binary or text format).  
Used in checkpoint-based configurations to resume or modify a simulation.

Configuration Parameters
========================

Every configuration file follows a **strict JSON schema** that defines its expected structure
and data types.  
When a simulation starts, all JSON files are automatically **validated against these schemas**
before execution.  

For more information on how this validation is implemented and enforced, see :ref:`JSON validator <json-validator>`.

Refer to the dedicated sections below for details on each file type:

.. toctree::
   :maxdepth: 1

   json/simulation_params
   json/landscape_params
   json/habitat_domain_patch
   json/moisture_patch
   json/obstacle_patch
   json/resource
   json/resource_patch
   json/species/species
   json/ontogeneticLinksPreference
   json/ontogeneticLinksProfitability
