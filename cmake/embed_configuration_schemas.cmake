include(${CMAKE_CURRENT_LIST_DIR}/embed_json_to_header.cmake)

function(weaver_embed_configuration_schemas GENERATED_ROOT)
    set(SCHEMA_VERSION_DIR "${CMAKE_SOURCE_DIR}/schema/${SCHEMA_VERSION}")
    set(GENERATED_DIR "${GENERATED_ROOT}/validator_schemas")
    file(MAKE_DIRECTORY "${GENERATED_DIR}")
    file(GLOB VERSIONED_SCHEMAS CONFIGURE_DEPENDS
        "${SCHEMA_VERSION_DIR}/configuration_structure.json"
        "${SCHEMA_VERSION_DIR}/constraints.json"
        "${SCHEMA_VERSION_DIR}/json/*.json"
    )
    set(META_SCHEMAS
        "${CMAKE_SOURCE_DIR}/schema/configuration_structure.schema.json"
        "${CMAKE_SOURCE_DIR}/schema/constraints.schema.json"
    )
    foreach(SCHEMA_FILE IN LISTS VERSIONED_SCHEMAS META_SCHEMAS)
        embed_json_to_header("${SCHEMA_FILE}" "${GENERATED_DIR}")
    endforeach()
endfunction()
