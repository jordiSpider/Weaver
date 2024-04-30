
set(EXTERNAL_LIBRARIES
    magic_enum
    cxxopts
    nlohmann_json
    boost
)

# ===========================
# magic_enum

set(LIBRARY_magic_enum magic_enum::magic_enum)
set(LIBRARY_COMPONENTS_magic_enum)

# ===========================
# cxxopts

set(LIBRARY_cxxopts cxxopts::cxxopts)
set(LIBRARY_COMPONENTS_cxxopts)

# ===========================
# nlohmann_json

set(LIBRARY_nlohmann_json nlohmann_json::nlohmann_json)
set(LIBRARY_COMPONENTS_nlohmann_json)

# ===========================
# fmt

set(LIBRARY_fmt fmt::fmt-header-only)
set(LIBRARY_COMPONENTS_fmt)

# ===========================
# boost

set(LIBRARY_boost Boost)
set(LIBRARY_COMPONENTS_boost)



set(EXTERNAL_CLASSES)

foreach(library ${EXTERNAL_LIBRARIES})
    # Comprobar si la lista está vacía
    list(LENGTH LIBRARY_COMPONENTS_${library} numbreOfComponents)

    if(numbreOfComponents EQUAL 0)
        # List all library files

        string(REPLACE "_" "-" refactorLibrary ${library})
        set(libraryInclude "$ENV{VCPKG_ROOT}/packages/${refactorLibrary}_${VCPKG_HOST_TRIPLET}/${INCLUDE_DIRECTORY}")
        file(GLOB_RECURSE libraryFiles "${libraryInclude}/*.h" "${libraryInclude}/*.hpp")

        foreach(file ${libraryFiles})
            string(REGEX MATCH "${INCLUDE_DIRECTORY}/([A-Za-z0-9_/]+)\.(h|hpp)" matchedPart "${file}")
            set(class ${CMAKE_MATCH_1})

            list(APPEND EXTERNAL_CLASSES ${class})

            string(REPLACE "/" ${PATH_SEP} refactorClass ${class})

            set(MAP_DEPEN_${refactorClass} ${LIBRARY_${library}})
        endforeach()

        find_package(${library})
    else()
        foreach(component ${LIBRARY_COMPONENTS_${library}})
            # List all library files

            string(REPLACE "_" "-" refactorLibrary ${library})
            set(libraryInclude "$ENV{VCPKG_ROOT}/packages/${refactorLibrary}-${component}_${VCPKG_HOST_TRIPLET}/${INCLUDE_DIRECTORY}")
            file(GLOB_RECURSE libraryFiles "${libraryInclude}/*.h" "${libraryInclude}/*.hpp")

            foreach(file ${libraryFiles})
                string(REGEX MATCH "${INCLUDE_DIRECTORY}/([A-Za-z0-9_/]+)\.(h|hpp)" matchedPart "${file}")
                set(class ${CMAKE_MATCH_1})

                list(APPEND EXTERNAL_CLASSES ${class})

                string(REPLACE "/" ${PATH_SEP} refactorClass ${class})

                set(MAP_DEPEN_${refactorClass} "${LIBRARY_${library}}::${component}")
            endforeach()

        endforeach()

        find_package(${library} COMPONENTS ${LIBRARY_COMPONENTS_${library}})
    endif()
endforeach()
