#ifndef CONFIGURATION_VALIDATOR_H_
#define CONFIGURATION_VALIDATOR_H_

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include "Misc/JsonValidator/ConfigurationValidatorExport.h"

struct ValidationDiagnostic {
    std::string code;
    std::string message;
    std::string file;
    std::string instancePath;
    std::string schemaPath;
    std::string severity = "error";
    WEAVER_CONFIG_VALIDATOR_API nlohmann::json toJson() const;
};

struct ValidationResult {
    std::vector<ValidationDiagnostic> errors;
    std::vector<ValidationDiagnostic> warnings;
    bool valid() const noexcept { return errors.empty(); }
    bool hasWarnings() const noexcept { return !warnings.empty(); }
    WEAVER_CONFIG_VALIDATOR_API nlohmann::json toJson() const;
    WEAVER_CONFIG_VALIDATOR_API std::string toPrettyString(int indent = 2) const;
};

// Owns the embedded validation assets and validates the complete configuration
// (directory structure, each JSON document, and cross-file constraints).
class ConfigurationValidator {
public:
    WEAVER_CONFIG_VALIDATOR_API ConfigurationValidator(
        std::string_view configurationStructure,
        std::string_view constraints,
        std::unordered_map<std::string, std::string_view> jsonSchemas);

    WEAVER_CONFIG_VALIDATOR_API ValidationResult validate(
        const std::filesystem::path& configDirectory,
        std::string_view expectedSchemaVersion) const;

private:
    std::string configurationStructure_;
    std::string constraints_;
    std::unordered_map<std::string, std::string> jsonSchemas_;
};

#endif
