
# =========================================================
# 💻 Command Line Interface (CLI) Target Configuration
# =========================================================

# ---------------------------------------------------------
# 🎯 Executable Target Declaration
# ---------------------------------------------------------
if(WIN32)
    # Windows environments require the .rc resource file to embed application metadata (e.g., icons)
    # add_executable(${CLI_EXE_NAME} mainCLI.cpp app_icon.rc)
    add_executable(${CLI_EXE_NAME} mainCLI.cpp)
else()
    # Linux/Unix environments utilize standard source inputs without PE resource files
    add_executable(${CLI_EXE_NAME} mainCLI.cpp)
endif()

# Append a 'd' suffix to the binary name during Debug builds to avoid overwriting Release assets
set_target_properties(${CLI_EXE_NAME} PROPERTIES DEBUG_POSTFIX "_d")

# ---------------------------------------------------------
# 🔗 Linkage & Dependency Propagation
# ---------------------------------------------------------
target_link_libraries(${CLI_EXE_NAME}
    PRIVATE
        weaver_options
        weaver_app
)

# ---------------------------------------------------------
# 📦 Deployment & Installation Rules
# ---------------------------------------------------------
# Professional component-based deployment rules
install(TARGETS ${CLI_EXE_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT CLI # Explicitly isolates the CLI binary for granular CPack distribution
)
