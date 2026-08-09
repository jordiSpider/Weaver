
# Function to embed a text file (JSON) into a C++ Header
function(embed_json_to_header INPUT_JSON GENERATED_DIR)
    # Extract the base name of the file (e.g., from "path/config.json" extract "config")
    get_filename_component(BASE_NAME "${INPUT_JSON}" NAME_WE)

    # Construct the variable name and the header file name by appending "_json"
    set(VAR_NAME "${BASE_NAME}_json")
    set(OUTPUT_HEADER "${GENERATED_DIR}/${VAR_NAME}.h")

    # 1. We tell CMake that if the JSON changes, it should reconfigure the project
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${INPUT_JSON}")

    # 2. We read the content of the JSON
    file(READ "${INPUT_JSON}" FILE_HEX HEX)

    # 3. Join the hex string into pairs of characters (representing bytes) and store them in a list
    string(REGEX MATCHALL ".." HEX_LIST "${FILE_HEX}")

    # 4. Format the list (separated by ;) into a C++ style array: 0xXX, 0xYY
    string(REPLACE ";" ", 0x" HEX_ARRAY "${HEX_LIST}")

    # 5. Add the first "0x" and the null terminator ('\0') at the end
    if(HEX_ARRAY)
        set(HEX_ARRAY "0x${HEX_ARRAY}, 0x00")
    else()
        set(HEX_ARRAY "0x00") # In case the JSON is empty
    endif()

    # 6. Generate the C++ code
    # The string_view packs the array, so the usage interface DOES NOT change in your code
    set(HEADER_CONTENT
"#pragma once
#include <string_view>

namespace EmbeddedResources {
    inline constexpr char ${VAR_NAME}_data[] = { ${HEX_ARRAY} };
    inline constexpr std::string_view ${VAR_NAME}(${VAR_NAME}_data, sizeof(${VAR_NAME}_data) - 1);
}
"
    )

    file(WRITE "${OUTPUT_HEADER}" "${HEADER_CONTENT}")
endfunction()
