#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_

#include <boost/filesystem.hpp>


#define SCHEMA_FOLDER (filesystem::path("schema") / filesystem::path(PROJECT_VERSION)).string()
#define RESOURCE_SCHEMA "resource.schema.json"
#define RESOURCE_PATCH_SCHEMA "resource_patch.schema.json"
#define MOISTURE_PATCH_SCHEMA "moisture_patch.schema.json"
#define OBSTACLE_PATCH_SCHEMA "obstacle_patch.schema.json"
#define SPECIES_SCHEMA "species.schema.json"
#define SIMULATION_PARAMS_SCHEMA "simulation_params.schema.json"
#define WORLD_PARAMS_SCHEMA "world_params.schema.json"

#define RESOURCE_PATCH_FOLDER "patches"
#define RESULT_SIMULATION_FOLDER "result_simulation"

#define LOG_FILENAME "log"
#define LOG_EXTENSION "txt"

#define TAB "    "

#endif // GLOBAL_VARIABLES_H_
