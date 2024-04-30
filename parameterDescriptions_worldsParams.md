# World 

## World parameters

  This section contains input arguments for specific world set up parameters.
  The world, as it is now (June 2023) is an array (2D or 3D - yet 3D has been temporarily abandoned)
  
### World Type

*Too Technical detail* : 2D World is a 1D array of pointers to 1D arrays - Not sure whether that is common in C++ as oppossite to 2D C arrays (int arr [n][m] ;)
A new type of world, probably named pyramid or octree/quadtree or simila is now expected (June 2023) to account for a) Floating point animal coordinates (*TODO* *ASK ABOUT SOFTWARE STRUCTURE REGARDING ANIMAL BELONGING TO CELLS*)

> ***world.worldType***
> 
>> OneOf ["Old"]  
>> Description: *By the time this guide is being written* there´s two types of Worlds, the old one is a 2D world grid-based with the supposed availability of having 3D coordinates one.
>> In addition, a new brave new world is being coded based on multiple levels according to size/search area - informally naded octree.
>
>> Suggestions : This need to be a more clear approach but it is important to have several type of worlds and within world structure. I would say OneOF [ (2D, 3D) X (Flat, Pyramid)]
>> *Yet, having multiple types of world evaluation, i.e. flat vs pyramid may induce several parameter changes that have to be explained concisely*
  >> Will have all the possible worlds (e.g. Single Cell World, bag of animal worlds, River/Aire/Soil the same type of paramaeters?
                                          
> ***world.simulationType***
> 
> OneOf ["dinosaurs", "arthropods]
> 
>> Description: The simulation may run in different scales, small insects in the soil live in cells on the centimeter unit size, although it would be more like centimeter squared or centimeters cubid. Dinosaurs live on top of 2D cells of a kilometer unit size, kilometer squared.
>> Sugestion : Dinosaurs and arthropods are not really descriptives of the type of world. It would be better to set up units and whatever other options - yet, keeping Dinos and Arthropods as "template" sounds be good. I just mean to have proper parameters for the world.

### Space Configuration

This is a section within the world (so as if it were a list I will write them as world.spaceconfiguration.* )

**NOTE:** *Along time, this section has become quite disorganized and need revision*

#### Dimensions 

This section is just about the dimension of the world as a matrix (notice above it is really an array of arrays of arrays)

**NOTE: The world is a 3D that becomes 2D just by setting up depth to 1**

**NOTE:** *the dimensions would be much simpler to put in a tuple/array as (W, L, D, cellSize)*

> ***world.spaceConfiguration.dimensions.width***
>
> ***world.spaceConfiguration.dimensions.length***
>
> ***world.spaceConfiguration.dimensions.depth***
>
>> Nat+ - World dimensions in units defined by World type - e.g. (10,10,1) for a 10 by 10 flat world
>
>> Description -  A world described as a grid starting in (0,0,0) to (w,l,d) as in images ( (0,0,0) at toplefmost point, (w,l,d) at bottomrightmost coordinates) with coordinates as a matrix or chess board, indicating the interior of the grid and not the intersections.
>> Doubt: *Can be an axis be equal to 0?*
*Can be depth > width or depth > length? or depth >1 but length == 1? Can I have 1D worlds*
*I have the feeling that every cell is a volume 1x1x1 with many individuals within it (so in fact 3D microworlds)*
>>  *What will happen or is happening when the world becomes continuous coordinates in the pyramids?*


>***world.spaceConfiguration.dimensions.cellSize***
>
>> Nat+ - *UNKNOWN* - e.g. 1
> 
>> Doubt : *What happens with values larger than 1? and 0 or negative?*
>> This should be some sort of units, 1 meter or 25 cm - otherwise is some sort of multiplier

### Obstacles

An obstacle is meant to be a contiguous (connected or adjacent, 4 or 8 neighborhood) group of cells where animals or resources cannot be. They serve as an element to create caves, stones or any kind of situation where live beings cannot trespass

**NOTE:** *So far, obstacles have been inactivated and there´s no guarantee these parameters are reliable*

**NOTE:** *Obstacle seems that they were designed having in mind the possibility of make random patches, of the same kind of other ones we will say later*

**NOTE** *It is not clear to me if all the other helper code to generate patches are available for obstacles. Anyway, either for obstacles and for other type of patches the routines to generate them could be expanded (input images?, matrices?, Beta-skeletions?? )*

>***world.spaceConfiguration.obstacleFolder***
>
>> String - Folder name for the obstacle files (patches) - e.g. "obstacle",
>
>> Description : The folder address where all the obstacles are being saved
>> **UNSURE** As with other types of patches we will see, the obstacle folder will contain a list of files, each one corresponding to a patch or some type of file. We will see them later.


>***world.spaceConfiguration.initRandomObstaclePatches.enabled***
>
>> Bool - A flag to activate or inactivate the obstacles; or just its random positioning - e.g. false
> 
>> Doubt: *What happens if I activate it now?*
>> *Do we create them, given the next set of parameters, does weaver generate them?*
>> *Does this option also exist for other types of patches*


>***world.spaceConfiguration.initRandomObstaclePatches.obstacleDensity***":
>> Prop - *UNKNOWN* - e.g. 0.7
>
>> Doubt : *It seems this parameter would guide to make a proportion of cells inactive as obstacles. It may be, but I do not know it, that the next 3 parameters dictate the size of obstacles - The algorithms to get that done is unknown to me*

>***world.spaceConfiguration.initRandomObstaclePatches.patchesWidth***
>
>***world.spaceConfiguration.initRandomObstaclePatches.patchesLength***
>
>***world.spaceConfiguration.initRandomObstaclePatches.patchesDepth***
>
>> Nat+  - Number of cells in a patch (quite likely squared) - e.g. 5, 1, 1
>> Explanation : As with other type of patches, and as with the world an obstacle patch will have width, length and depths. The configuration here is that the obstables being created and placed randomly must have these dimensions (why  not a range with probabilities?- a distribution)



### Moisture patches

**NOTE:** *In here there´s a sort of explanation of what is a patch that really should go somewhere else*

The world is represented by a matrix of (w X l X d) so the terrain consist of cells as discrete contiguous elements. Each cell in the grid is linked to a type of terrain (*is that true? I believe that there´s only one type and easily could be several, since at the moment there´s only terrestrial terrains, but it could be  water, air, or different type of soils (with different mobility?)*.
Each "Terrain Cell"" can be empty or associated with a **patch**, a multi-cell element with some given properties. In the case of moisture patches, these fill the terrain with values that allow to describe the temperature and relative humidity in the cells plus the way to change their values along time. In this World parameters section, the general configuration for all patches are described and later every patch can have their own local parameter overwritten.

**NOTE** *This seems quite strange here when, as I´ve understood so far, the world is not automatically filled with these values but still requires a global patch to be defined. Then, I would say that it would be better to have these parameters in the Moisture section*

**NOTE** *It is possible to decouple many concepts here - we can have many patch linking to a single value set and another group of patches to a different value sets*

>***world.spaceConfiguration.moistureFolder***
>
>> String - Folder name containing moisture patches configuration - e.g. "moisture"
>
>> Explanation : Each moisture patch has a configuration file with its position and other data. All moisture patches are written as single files in this folders

#### Moisture Patches variables

>***world.spaceConfiguration.moistureBasePatch.totalMaximumResourceCapacity***
>
>> Real **ASSUMED BUT UNKNOWN** - The amount of resouces that a cell can contain in some unknown units -e.g. 0.0
>
>> **Doubts:** *Originally, this parameter was there to allow for competition among the different resources species. They can be a network themselves with types that compete and other than don´t. It may be possible to write guilds of competence and their totalMaximumresourceCapacity to allow some compartments*

>***world.spaceConfiguration.moistureBasePatch.inEnemyFreeSpace***
>
>> Bool - Flag to define whether predation may happen in these patches  - e.g. false
> 
>> Doubts : *It sounds to me that this only take advantage of the idea of patches (that could be more generalizable with few changes) to make an obscure way of setting up refuges*
>> *How is enemy defined? Which species are considered enemies? Do they move in or just predation does not happen?*
 
>***world.spaceConfiguration.moistureBasePatch.inCompetitorFreeSpace***
>
>> Bool - Flag to define whether or not competitions happens in these patches - e.g. false
> 
>> Doubts : *See above*

>***world.spaceConfiguration.moistureBasePatch.temperatureCycle***
>
>>Range Integer - A vector representing a time series of temperatures -e.g. [20, 20]
>
>> Explanation : Since Weaver timesteps are organized in two levels, e.g. days and hours, we can describe the temperature as a vector of 24 temperatures. The vector values is recicled.
>
>> Doubts : *What happen if the vector is longer than hours, gets to start or carries on - how the vector is consumed* *Could it be a function? A * Sen B x hour + C?*

>***world.spaceConfiguration.moistureBasePatch.useRelativeHumidityCycle***
>
>> Bool - Flag to determine whether or not use relative humidity - e.g. true

>***world.spaceConfiguration.moistureBasePatch.relativeHumidityCycle***
>
>>Range - A vector (reciclable, see temperatureCycle) dictating the values on a time series for relative humidity - e.g. [25, 25]

>***world.spaceConfiguration.moistureBasePatch.useRelativeHumidityDecayOverTime***
>
>> Bool - Flag to indicate whether using or not a decay function on the relative humidity to simulate drought - e.g. false
>
>> Doubts : *What is the decay function?* *how this relate with the cycling vector? - Likely to be a drop in all values, like - 5 or 20% *

>***world.spaceConfiguration.moistureBasePatch.relativeHumidityDecayOverTime***
>
>>Prop or Real **Unsure** - A factor for time decay on RH - e.g. 0.2
>
>>Doubt : *Most likely than the decay function is an scalar multiplication of a factor by a vector - in this case either 0.2xRH or 0.8xRH*

>***world.spaceConfiguration.moistureBasePatch.timeStepsBetweenRainEvents***
>>Nat - How ofter in timesteps a rain event happens - e.g. 5
>
>>Doubts : *Does it happen at day/year level or within day/year level?*

>***world.spaceConfiguration.moistureBasePatch.relativeHumidityOnRainEvent***
>>Range Real [0, 100], - Amount of water fall during a raining event- e.g. 87.5
>>Doubt : *Unsure of how rains work, is that a 1.875 raise in the RH vector values?*

>***world.spaceConfiguration.moistureBasePatch.standardDeviationForRainEvent***
>> Prop - A standard deviation for rain events - e.g. 1.0
>> Doubts : *Is that for the relativeHumidityonRainEvent or for the timestepsbetweenrainevents?*

**NOTE** ***I have no clue how does the rain events work***

### Nutrients
In earlier versions of Weaver there were a way to assign the transference of nutrients to any consumer. This part seemed to not be working at the moment, if ever.
*NOTE:* *Why this is not set up in patches as well?*

>***world.spaceConfiguration.nutrients.minC*** 0.0
>
>***world.spaceConfiguration.nutrients.maxC*** 23.0
>
>***world.spaceConfiguration.nutrients.minN*** 0.0
>
>***world.spaceConfiguration.nutrients.maxN*** 23.0
>
>***world.spaceConfiguration.nutrients.minP*** 0.0
>
>***world.spaceConfiguration.nutrients.maxP*** 23.0

>> Float+ - Minimum and maximum amounts (**mass?**) of each macro elements NPC- 0.0 - 23.0

### Mass or Energy transference among trophic levels.

The amount of mass or energy (*Unsure which one*) that a consumer (or predator) takes from their consumed species (or prey) is set universally as 10%. *To me (so likely to be completely wrong)* that means that, from the total amount of matter/energy accumulate in a trophic level, let´s say plants and fungi, or "rabbits", just 10% ends in the next level and the rest ends as carrion or nutrients for saprophytics (*isn´t those a trophic level itself?*) or energy lost as heat. *My confusion comes from the idea that this is the amount of matter/energy transfer among levels - group based- but I am not so sure that this is the individual exploitation of a prey. If that where true, a cow eating n grams of grass would take the same amount of energy/nutrients than a lynx eating n grams of rabbits. There´s a concentration in nutrients and energy that allows to eat less amount for the same amount of energy/mass * **Note: is that the idea of edibility?** .


>***world.spaceConfiguration.resourceToPreysCapacityTransference***
>                                                  >***world.spaceConfiguration.preysToPredatorsCapacityTransference***
>
>> Prop - Percentage of mass/energy transfer to the next trophic level - e.g. 0.1
>
>> Doubt : *What happens when there are many levels, like in a trophic chain - all are set at the same value? In trophic "trees", an animal that eat two preys, is the same amount for each? How this interfere with edibility and other values*

## LIFE

In this section, the biotic interaction parameters are found, split in sections on the type of interaction.


### Resources and Species Folders

Resources are organisms like plants, fungi, bacteria or similar that are autotrophic or saprophytic species that extract nutrients and energy from non-living sources. They are considered species that are sessile or with little dispersion ability and that their feeding is not considered predation. They are modeled as biomass units in the terrain cells using many parameters we will see in due time. Yet, they are not widespread around the world but distributed in patches, and a patch can extend as large as the whole world. Resources growth according to curves called as recurrent ones 8 (*I need to  see them*).

>***world.life.resourceFolder***
>
>> String - As - e.g."resource",
>
>> Explanation : As in other cases the address of a folder where files with the configuration of several species of resources. Inside there will be a single json file defining species parameters
and another folder <resource>Patches defining patches of every resource, e.g ferns.json and a folder ferns_Patches with as many ferns_patch_0.json as patches there will be in the simulation.

>***world.life.competitionAmongResourceSpecies***
>
>> Bool - *UNSURE* Flat to determine wether Resources species sum up to an ammount set up in  *likely* world.spaceConfiguration.moistureBasePatch.totalMaximumResourceCapacity - e.g. false,

>***world.life.speciesFolder***
>
>> String - As in other cases, the address of an species Folder - e.g. "species",
>
>> Explanation : See resourceFolder and change the world species for resources. The parameters inside every json changes.

### Populations related parameters or initialization parameters ?


>***world.life.initIndividualsPerDensities.enabled***
>
>> Bool - *UNKNOW* - e.g. false

>***world.life.initIndividualsPerDensities.initialEcosystemSize***
>
>> Nat - *UNKNOW* - e.g. 20000 

### **UNKNOWN PARAMETERS**
>***world.life.exitTimeThreshold***
>
>> Nat+ -*UNKNOWN* - e.g. 120000

>***world.life.minLogMassRatio***
>
>> Int - *UNKNOWN* - e.g. -6.0

>***world.life.maxLogMassRatio***
>
>> Int - *UNKNOWN* - e.g. 6.0

#### Parameters for a Normal Distribution - Generation?

>***world.life.sigmaForPDF***
>
>> Float - Standard Deviation of a Normal Distribution ... For what? *UNKNOWN* - e.g. 2.54
***world.life.muForPDF***

>> Float - Mean of a Normal Distribution ... For what? *UNKNOWN* - e.g. 8.65,

#### Parameters related with --- ¿Automatic Network Generation?

**Note** *I´ve heard that Gabi did some code to set up automatically trophic networks given some inputs and this could be them.* The description of the code are putative based on some ideas I may have. We need to reimplement this or delete it.

>***world.life.initFromRandomSpecies.enabled***
>
>> bool -  *UNKNOWN* - Some Flag possibly to just activate/deactivate the random network generation - e.g. true

>***world.life.initFromRandomSpecies.numberOfPredatorSpecies***
>> Nat or Nat+  - *UNKNOWN* - The graph could be bipartite or allow a hierarchy of predators (this, i don´t know) but anyway the number of animals able to eat other animals in the network - e.g. 1

>***world.life.initFromRandomSpecies.numberOfPreySpecies***
>
>> Nat or Nat+ - *UNKNOWN* - The number of animals able to eat resources - e.g. 1

>***world.life.initFromRandomSpecies.numberOfResourceSpecies***
>
>> Nat or Nat+  - *UNKNOWN* - The number of Especies of Resource to be generate - e.g. 1

>***world.life.initFromRandomLinks.enabled***
>
>> Bool - *UNKNOWN* - I guess this allow to make a random distribution of links (which distribution? what happen when it is disabled - all with all or read a file?) - e.g. true

>***world.life.initFromRandomLinks.eraseCurrentExistingLinks***
>
>> Bool - *UNKNOWN* - Flag to restart the links rather than use existing ones - e.g. true,
 
>***world.life.initFromRandomLinks.predator-preyLinksDensity***
>
>> Prop - *UNKNOWN* - It seems it generates a random graph Erdos-Renyi? between bipartite sets but I do not know the details- e.g. 0.2

>***world.life.initFromRandomLinks.intraguildPredation.enabled***
>
>> Bool  - *UNKNOWN* - In principle Intraguild refers that those that eat the same prey can eat themselves (does it add atacks but not eating?) - e.g. true

>***world.life.initFromRandomLinks.intraguildPredation.allowCannibalism***
>
>> Bool - *UNKNOWN* - In the case of predating those that eat the same that you ... can I eat my own kind? - e.g. false

>***world.life.initFromRandomLinks.resourcevore-resourceLinksDensity***
>
>Prop - *UNKNOWN* - It seems they added not only competence but some sort of attack¿ - e.g. 0.1

#### Parameters related withPredation

*NOTE:* **why this parameters are globals and not species related?**

I have not been able to learn yet what all this parameters mean deeply.
At the moment we can say that there is an hierachy of parameters
- SAW | AH
  - Encounter | Predation
    - Hunter | Hunted | ProbabilityDensityFunction | VoracitiesProduct | SpeedRatio (Predation) 
      - Voracity | Size (for Hunte(r|d))
        
  

>***world.life.encounterHuntedVoracitySAW": 0.5,
>***world.life.encounterHunterVoracitySAW": 5.0,
>***world.life.encounterVoracitiesProductSAW": 0.5,
>***world.life.encounterHunterSizeSAW": 5.0,
>***world.life.encounterHuntedSizeSAW": 0.5,
>***world.life.encounterProbabilityDensityFunctionSAW": 0.0,
>***world.life.encounterHuntedVoracityAH": 1.0,
>***world.life.encounterHunterVoracityAH": 1.0,
>***world.life.encounterVoracitiesProductAH": 1.0,
>***world.life.encounterHunterSizeAH": 1.0,
>***world.life.encounterHuntedSizeAH": 1.0,
>***world.life.encounterProbabilityDensityFunctionAH": 1.0,
>***world.life.predationSpeedRatioSAW": 0.0,
>***world.life.predationHunterVoracitySAW": 5.0,
>***world.life.predationProbabilityDensityFunctionSAW": 0.1,
>***world.life.predationSpeedRatioAH": 1.0,
>***world.life.predationHunterVoracityAH": 1.0,
>***world.life.predationProbabilityDensityFunctionAH": 1.0
