#ifndef EMBEDDED_CONFIGURATION_SCHEMAS_H_
#define EMBEDDED_CONFIGURATION_SCHEMAS_H_

#include "validator_schemas/configuration_structure_json.h"
#include "validator_schemas/constraints_json.h"
#include "validator_schemas/species_schema_json.h"
#include "validator_schemas/resource_schema_json.h"
#include "validator_schemas/simulation_params_schema_json.h"
#include "validator_schemas/landscape_params_schema_json.h"
#include "validator_schemas/habitat_domain_patch_schema_json.h"
#include "validator_schemas/moisture_patch_schema_json.h"
#include "validator_schemas/obstacle_patch_schema_json.h"
#include "validator_schemas/resource_patch_schema_json.h"
#include "validator_schemas/configuration_structure_schema_json.h"
#include "validator_schemas/constraints_schema_json.h"

#include <string>
#include <string_view>
#include <unordered_map>

namespace WeaverEmbeddedSchemas {
inline constexpr std::string_view configurationStructure = EmbeddedResources::configuration_structure_json;
inline constexpr std::string_view configurationConstraints = EmbeddedResources::constraints_json;

inline const std::unordered_map<std::string, std::string_view>& jsonSchemas() {
    static const std::unordered_map<std::string, std::string_view> schemas = {
        {"json/species_schema.json", EmbeddedResources::species_schema_json},
        {"json/resource_schema.json", EmbeddedResources::resource_schema_json},
        {"json/simulation_params_schema.json", EmbeddedResources::simulation_params_schema_json},
        {"json/landscape_params_schema.json", EmbeddedResources::landscape_params_schema_json},
        {"json/habitat_domain_patch_schema.json", EmbeddedResources::habitat_domain_patch_schema_json},
        {"json/moisture_patch_schema.json", EmbeddedResources::moisture_patch_schema_json},
        {"json/obstacle_patch_schema.json", EmbeddedResources::obstacle_patch_schema_json},
        {"json/resource_patch_schema.json", EmbeddedResources::resource_patch_schema_json},
        {"configuration_structure.schema.json", EmbeddedResources::configuration_structure_schema_json},
        {"constraints.schema.json", EmbeddedResources::constraints_schema_json}
    };
    return schemas;
}
}

#endif
