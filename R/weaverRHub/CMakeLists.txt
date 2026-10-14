# ==============================================================================
# 1. PATHS DEFINITIONS AND WORKSPACE CONFIGURATION
# ==============================================================================
set(PACKAGE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(STAGE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/staged_package_build")
set(FLATTEN_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/flatten_package.ps1")
set(EMBED_SCHEMAS_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/embed_schemas.ps1")

set(SOURCE_SCHEMA_DIR "${CMAKE_SOURCE_DIR}/schema")

message(STATUS "[weaverRHub CMake] Package Source Directory: ${PACKAGE_SRC_DIR}")
message(STATUS "[weaverRHub CMake] Temporary Staging Build Target: ${STAGE_BUILD_DIR}")
message(STATUS "[weaverRHub CMake] Source External Schema Directory: ${SOURCE_SCHEMA_DIR}")

# ==============================================================================
# 2. CUSTOM TARGET: CONTEXT FLATTENING & STATIC ASSETS EMBEDDING
# ==============================================================================
# Executes our separated scripts sequentially passing safe workspace variables arguments
add_custom_target(stage_flatten_package
    COMMAND powershell.exe -NoProfile -ExecutionPolicy Bypass -File "${FLATTEN_SCRIPT}" "${PACKAGE_SRC_DIR}" "${STAGE_BUILD_DIR}"
    
    COMMAND powershell.exe -NoProfile -ExecutionPolicy Bypass -File "${EMBED_SCHEMAS_SCRIPT}" "${SOURCE_SCHEMA_DIR}" "${STAGE_BUILD_DIR}"
    
    COMMENT "Executing transactional staging data synchronization, multi-folder flattening, and schema asset embedding via separate scripts..."
)

# ==============================================================================
# 3. COMPILATION TARGET: NATIVE R SYSTEM INSTALLATION
# ==============================================================================
# Triggers R CMD INSTALL pointing directly to our dynamically generated staging layer.
add_custom_target(install_package ALL
    DEPENDS stage_flatten_package
    COMMAND ${R_EXECUTABLE} CMD INSTALL "${STAGE_BUILD_DIR}"
    COMMENT "Compiling and registering flattened weaverRHub package architecture into local R ecosystem library..."
)