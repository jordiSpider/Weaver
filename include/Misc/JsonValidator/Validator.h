/**
 * @file Validator.h
 * @brief Provides functionality for validating JSON configuration files against a schema.
 */

#ifndef JSON_VALIDATOR_H_
#define JSON_VALIDATOR_H_


#include <iostream>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Object.h"
#include "Misc/JsonValidator/Exceptions.h"


/**
 * @class JsonValidator
 * @brief Validates JSON objects against a predefined JSON Schema.
 *
 * This class loads a JSON schema from a file and provides a method to validate
 * JSON configuration files. It is used to ensure that configuration files conform
 * to the expected structure and data types before being used in the simulation.
 */
class JsonValidator
{
protected:
    /// @brief Root object of the loaded JSON schema.
    Object* root;

public:
    /**
     * @brief Constructs the validator by loading a JSON schema.
     *
     * @param schemaPath Path to the JSON schema file.
     *
     * Loads the schema from the specified path and prepares the validator
     * to validate configuration files against it.
     */
    JsonValidator(const std::filesystem::path& schemaPath);

    /**
     * @brief Virtual destructor.
     *
     * Ensures proper cleanup of resources associated with the JSON schema.
     */
    virtual ~JsonValidator();

    /**
     * @brief Validates a JSON configuration object against the schema.
     *
     * @param configFilename Name of the configuration file (used for reporting errors).
     * @param config JSON object to validate.
     *
     * Throws an exception or reports errors if the JSON object does not conform
     * to the loaded schema.
     */
    void validate(const std::string& configFilename, const nlohmann::json& config) const;
};

#endif /* JSON_VALIDATOR_H_ */
