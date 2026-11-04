#include "Misc/JsonValidator/ConfigurationValidator.h"
#include "Misc/JsonValidator/EmbeddedConfigurationSchemas.h"

#include <fstream>

#if defined(_WIN32)
#define WEAVER_R_API extern "C" __declspec(dllexport)
#else
#define WEAVER_R_API extern "C" __attribute__((visibility("default")))
#endif

// .C-compatible ABI; schema content is compiled into this dedicated library.
WEAVER_R_API void weaver_validate_configuration(char **configDirectory,
                                                 char **expectedSchemaVersion,
                                                 char **resultFile,
                                                 int *status) {
    *status = 1;
    try {
        ConfigurationValidator validator(WeaverEmbeddedSchemas::configurationStructure,
            WeaverEmbeddedSchemas::configurationConstraints, WeaverEmbeddedSchemas::jsonSchemas());
        auto result = validator.validate(*configDirectory, *expectedSchemaVersion);
        std::ofstream out(*resultFile, std::ios::binary);
        if (!out) return;
        out << result.toPrettyString(2);
        *status = 0;
    } catch (...) {
        try {
            std::ofstream out(*resultFile, std::ios::binary);
            if (out) out << R"({"valid":false,"errors":[{"code":"native_failure","message":"Native validation could not complete","file":"","instancePath":"","schemaPath":"","severity":"error"}],"warnings":[]})";
        } catch (...) {}
    }
}
