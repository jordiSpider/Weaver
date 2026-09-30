
# =========================================================
# 🖥️ Graphical User Interface (GUI) Target Configuration
# =========================================================

# Dependency Resolution for GUI Tooling
find_package(unofficial-nana CONFIG REQUIRED)

set(GUI_EXE_NAME "${PROJECT_NAME}GUI")

# ---------------------------------------------------------
# 🎯 Executable Target Declaration
# ---------------------------------------------------------
if(WIN32)
    # Windows environments require the .rc resource file to embed application metadata (e.g., icons)
    # add_executable(${GUI_EXE_NAME} mainGUI.cpp app_icon.rc)
    add_executable(${GUI_EXE_NAME} mainGUI.cpp)
else()
    # Linux/Unix environments utilize standard source inputs without PE resource files
    add_executable(${GUI_EXE_NAME} mainGUI.cpp)
endif()

# Append a suffix to the binary name during Debug builds to avoid overwriting Release assets
if(ENABLE_ASAN)
    set_target_properties(${GUI_EXE_NAME} PROPERTIES DEBUG_POSTFIX "_d_asan")
else()
    set_target_properties(${GUI_EXE_NAME} PROPERTIES DEBUG_POSTFIX "_d")
endif()

# ---------------------------------------------------------
# Windows-Specific Subsystem & Linker Overrides
# ---------------------------------------------------------
if(WIN32)
    # 1. Define the executable subsystem as GUI (suppresses the background terminal window)
    set_target_properties(${GUI_EXE_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

    # 2. Override the default MSVC linker entry point
    # Forces the execution to route through standard 'main' rather than 'WinMain', 
    # effectively resolving LNK2001 unresolved external symbol errors within the WINDOWS subsystem.
    target_link_options(${GUI_EXE_NAME} PRIVATE "/ENTRY:mainCRTStartup")
endif()

# ---------------------------------------------------------
# 🔗 Linkage & Include Directives
# ---------------------------------------------------------
target_link_libraries(${GUI_EXE_NAME}
    PRIVATE
        weaver_options
        weaver_app_view
        unofficial::nana::nana
)

# ---------------------------------------------------------
# 📦 Deployment & Installation Rules
# ---------------------------------------------------------
# Deployment rules specific to the Graphical User Interface component
install(TARGETS ${GUI_EXE_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT GUI # Explicitly segregates this binary asset from the CLI layer in CPack distributions
)
