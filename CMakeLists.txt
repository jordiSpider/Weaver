cmake_minimum_required(VERSION 3.25)
project(Weaver VERSION 23.12.04)


include(${PROJECT_SOURCE_DIR}/cmake/SelectBuildType.cmake)

include(${PROJECT_SOURCE_DIR}/cmake/Compiler.cmake)








set(PATH_SEP "_pathSep_")
set(UNION "_")




add_custom_target(main_target)


# ========================================================
# Executables
add_subdirectory(src)



add_subdirectory(R/packages)


# ========================================================
# Build documentation

add_subdirectory(docs)
