
# =========================================================
# 💻 Command Line Interface (CLI) Target Configuration
# =========================================================

set(CLI_EXE_NAME "${PROJECT_NAME}CLI")

# ---------------------------------------------------------
# 🎯 Executable Target Declaration
# ---------------------------------------------------------
add_executable(${CLI_EXE_NAME} mainCLI.cpp)
if(WIN32)
    target_sources(${CLI_EXE_NAME} PRIVATE "${WEAVER_APP_ICON_RC}")
endif()

# Append a suffix to the binary name during Debug builds to avoid overwriting Release assets
if(ENABLE_ASAN)
    set_target_properties(${CLI_EXE_NAME} PROPERTIES DEBUG_POSTFIX "_d_asan")
else()
    set_target_properties(${CLI_EXE_NAME} PROPERTIES DEBUG_POSTFIX "_d")
endif()

# ---------------------------------------------------------
# 🔗 Linkage & Dependency Propagation
# ---------------------------------------------------------
target_link_libraries(${CLI_EXE_NAME}
    PRIVATE
        weaver_options
        weaver_app_view
        weaver_core_misc
)

# ---------------------------------------------------------
# 📦 Deployment & Installation Rules
# ---------------------------------------------------------
# Professional component-based deployment rules
install(TARGETS ${CLI_EXE_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT CLI # Explicitly isolates the CLI binary for granular CPack distribution
)
