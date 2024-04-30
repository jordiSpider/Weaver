# Parameter Description.
**NOTE**: Our configuration files are saved as JSON (Javascript Object Notation - sometimes JavaScript Serialization Object Notation). Yet, for this guide I will use a notation more readable and related with future implementations of JSONPath and/or Parameter Change Files (that need to be explained somewhere else). Since JSON files allows for nested objects, we will write in this document the hierarchies separated by a dot (.). For instance, a file containing inputs for the simulation may have a base level called simulation, followed by many child levels like runDays and timeStepsPerDay, having a format:  

simulation.runDays : 15 
simulation.timeStepsPerDay : 10 

**NOTE 2** : I will use short alias for the type of output in each parameter 
Nat = Naturales {0,1,...},
Z = Enteros {...,-1,0,1,...},
R = Reales {..., x.yz, ...},
Prop = decimal numbers in the interval [0, 1]

Bool = {true, false},
String - a character array or string,

OneOf - a set of possibilities but just one value,
ManyOf - a set of values among several posibilities.
Range - [from, to] or ]from, to[

## Simulation specific parameters

Mandatory File with filename (variable) : *runparams.json*

### Unknown parameter function

> ***simulation.numberOfCombinations***:
>
>> Nat - *UNKNOWN* - e.g. 1000

> ***simulation.growthAndReproTest***
>
>> Bool - *UNKNOWN* -e.g. false
        
### Control of simulation length

>***simulation.runDays***
>
>> Nat - Number of cycles per simulation - e.g. 15

>***simulation.timeStepsPerDay***
>
>> Nat+ - Number of time units per cycle  - e.g. 10
>        
>> Explanation : **Deactivated -  must be set to 1** Meant to be a subdivision of subsets within days/years but it has been deactivated.
>        
>> Doubts : Do days and time steps per day  have units?. What happen when I set up (in past - so I did it by mistake) it to 10?
   
>***simulation.exitAtFirstExtinction***
>
>> Bool - Flag to force exit when 1 specie is exinct - e.g. false
>
>> Explanation : Our simulations last for as many days as dictated by runDays parameter. To save time and computational power, a flag to stop the simulation and leave the program after **any** species has become extinct.
>
>> Suggestion : Two types of values, either boolean or an array of species to account for extinctions - similar parameter type for transient/stable states like any excess or lack of individuals.
>> Doubts : Does it work with Animals and Resources?


### Flags to record summaries

>***simulation.saveAnimalConstitutiveTraits***
>
>> Bool - Flag to indicate whether include Traits in the output - e.g. false

>***simulation.saveGeneticsSummaries***
>
>> Bool - Flag to indicate whether include Genetic Values in the output -  e.g. false
>
>>  Explanation : Weaver Agents may have a Genetics modules consisting in a collection of genes and their alleles located in chromosomes (called correlosomes) that determine traits. This output would serve for population genetics studies, yet they are responsible for a large hardrive compsumption and tend to be set as off.

>****simulation.saveEdibilitiesFile***
>
>> Bool - Flag to indicate whether or not save edibilities - e.g. false
>> Doubts : This flag comes with an option to indicate the name of the file but other options don't - better keep a common interface.
>>  *Also, edibilities are not clear to me at the moment.*

>***simulation.saveIntermidiateVolumes***
>
>> Bool - *UNKNOWN* - e.g. false
>
>> *is that Intermidiate or Intermediate?* *What are the Volumes?*

>***simulation.saveIntermidiateVolumesPeriodicity***
>
>> Nat or Nat+ - *UNKNOWN* - e.g. 100000,
>
>> *Unclear what are the Volumes and their periodicity* 

### Files and Folder Management

>***simulation.configName***
>
>> String - Simulation Configuration folder- e.g. "base_dinos_temp"
>
>> Explanation : Configuration files are located in a folder usually config/base_x_temp. I would remove the *config* ones
>
>> Suggestion : Remove the hardcoded dependency of a local "config" folder and allow for a relative or absolute route. Do this for all folders so they can be reusable

>***simulation.encountersMatrixFilename***
>
>> String - *UNKNOWN* - e.g. "encountersMatrix"

>***simulation.predationsMatrixFilename***
>
>> String - *UNKNOWN* - e.g. "predationsMatrix"

>***simulation.nodesMatrixFilename***
>
>> String - *UNKNOWN* - e.g. "nodesMatrix"

>***simulation.predationEventsOnOtherSpeciesFilename***
>
>> String - File name for an output file where each predation event, successful or not is recorded - e.g. "predationOnSpecies",

>***simulation.edibilitiesFilename***
>
>> String - File name for edibilities - e.g. "edibilities"

### Control of simulation output

>****simulation.recordEach***
>
>> Nat or Nat+ - How often Weaver saves the output -  e.g. 1
>> Doubts : What may happen when the timeStepsPerDay is set to a Nat number?
    
> ***simulation.outputStream.enabled***
>
>> Bool - Determine whether output recorded each given number of timesteps are being written in some device  - e.g. false

> ***simulation.outputStream.selectedChannel***
>
>> OneOf : ["Terminal"]
>
>>  Doubt : Which else? Are we really interested on puttting databases and others? If I say Terminal, am I preventing Weaver to also record it on the HDD? Can't I select two ways, like Terminal, JSON file, TOML file, SQL database, MongoDB ...

### Random Number generation seeds

>***simulation.initFromFixedSeed.enabled***
>
>> Bool - using an externally obtained random number - e.g.  true

>***simulation.initFromFixedSeed.fixedSeedValue***
>
>> Nat+ - The value stablished for the RNG seed- e.g. 3

**END OF RUN PARAMS**
