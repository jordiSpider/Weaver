
set(EXTERNAL_LIBRARIES
    magic_enum
    cxxopts
    nlohmann_json
    boost
    fmt
    tbb
)

# ===========================
# magic_enum

set(LIBRARY_PACKAGE_NAME_magic_enum magic_enum)
set(LIBRARY_COMPONENTS_BASE_magic_enum magic_enum)
set(LIBRARY_COMPONENTS_magic_enum )
set(LIBRARY_VCPKG_FOLDER_NAME_magic_enum magic-enum)

# ===========================
# cxxopts

set(LIBRARY_PACKAGE_NAME_cxxopts cxxopts)
set(LIBRARY_COMPONENTS_BASE_cxxopts cxxopts)
set(LIBRARY_COMPONENTS_cxxopts )
set(LIBRARY_VCPKG_FOLDER_NAME_cxxopts cxxopts)

# ===========================
# nlohmann_json

set(LIBRARY_PACKAGE_NAME_nlohmann_json nlohmann_json)
set(LIBRARY_COMPONENTS_BASE_nlohmann_json nlohmann_json)
set(LIBRARY_COMPONENTS_nlohmann_json )
set(LIBRARY_VCPKG_FOLDER_NAME_nlohmann_json nlohmann-json)

# ===========================
# fmt

set(LIBRARY_PACKAGE_NAME_fmt fmt)
set(LIBRARY_COMPONENTS_BASE_fmt fmt-header-only)
set(LIBRARY_COMPONENTS_fmt )
set(LIBRARY_VCPKG_FOLDER_NAME_fmt fmt)

# ===========================
# Boost

set(LIBRARY_PACKAGE_NAME_boost Boost)
set(LIBRARY_COMPONENTS_BASE_boost boost)
set(LIBRARY_COMPONENTS_HEADER_ONLY_boost 
    geometry
)
set(LIBRARY_COMPONENTS_boost 
    serialization
    filesystem
)
set(LIBRARY_COMPONENT_CLASSES_boost_geometry )
set(LIBRARY_VCPKG_FOLDER_NAME_Boost boost)

# ===========================
# TBB

set(LIBRARY_PACKAGE_NAME_tbb TBB)
set(LIBRARY_COMPONENTS_BASE_tbb tbb tbbmalloc)
set(LIBRARY_COMPONENTS_tbb )
set(LIBRARY_VCPKG_FOLDER_NAME_tbb tbb)



set(EXTERNAL_CLASSES)

foreach(library ${EXTERNAL_LIBRARIES})
    foreach(component_base ${LIBRARY_COMPONENTS_BASE_${library}})
        list(APPEND ${library}_components_base ${LIBRARY_PACKAGE_NAME_${library}}::${component_base})
    endforeach()
    
    if(LIBRARY_COMPONENTS_HEADER_ONLY_${library} OR LIBRARY_COMPONENTS_${library})
        foreach(component ${LIBRARY_COMPONENTS_HEADER_ONLY_${library}})
            if(LIBRARY_COMPONENT_CLASSES_${library}_${component})
                set(${library}_${component}_classes ${LIBRARY_COMPONENT_CLASSES_${library}_${component}})
            else()
                set(libraryInclude "$ENV{VCPKG_ROOT}/packages/${library}-${component}_${VCPKG_HOST_TRIPLET}/${INCLUDE_DIRECTORY}")
                file(GLOB_RECURSE libraryFiles "${libraryInclude}/*.h" "${libraryInclude}/*.hpp")

                set(${library}_${component}_classes "")

                foreach(file ${libraryFiles})
                    string(REGEX MATCH "${INCLUDE_DIRECTORY}/([A-Za-z0-9_/\\-]+)\.(h|hpp)" matchedPart "${file}")
                    set(class ${CMAKE_MATCH_1})

                    list(APPEND ${library}_${component}_classes ${class})
                endforeach()
            endif()

            foreach(class ${${library}_${component}_classes})
                list(APPEND EXTERNAL_CLASSES ${class})

                string(REPLACE "/" ${PATH_SEP} refactorClass ${class})

                set(MAP_DEPEN_${refactorClass} ${${library}_components_base})
            endforeach()
        endforeach()

        foreach(component ${LIBRARY_COMPONENTS_${library}})
            if(LIBRARY_COMPONENT_CLASSES_${library}_${component})
                set(${library}_${component}_classes ${LIBRARY_COMPONENT_CLASSES_${library}_${component}})
            else()
                set(libraryInclude "$ENV{VCPKG_ROOT}/packages/${library}-${component}_${VCPKG_HOST_TRIPLET}/${INCLUDE_DIRECTORY}")
                file(GLOB_RECURSE libraryFiles "${libraryInclude}/*.h" "${libraryInclude}/*.hpp")

                set(${library}_${component}_classes "")

                foreach(file ${libraryFiles})
                    string(REGEX MATCH "${INCLUDE_DIRECTORY}/([A-Za-z0-9_/\\-]+)\.(h|hpp)" matchedPart "${file}")
                    set(class ${CMAKE_MATCH_1})

                    list(APPEND ${library}_${component}_classes ${class})
                endforeach()
            endif()

            foreach(class ${${library}_${component}_classes})
                list(APPEND EXTERNAL_CLASSES ${class})

                string(REPLACE "/" ${PATH_SEP} refactorClass ${class})

                set(MAP_DEPEN_${refactorClass} ${${library}_components_base} ${LIBRARY_PACKAGE_NAME_${library}}::${component})
            endforeach()
        endforeach()

        find_package(${LIBRARY_PACKAGE_NAME_${library}} REQUIRED COMPONENTS ${LIBRARY_COMPONENTS_${library}})
    else()
        if(LIBRARY_CLASSES_${library})
            set(${library}_classes ${LIBRARY_CLASSES_${library}})
        else()
            set(libraryInclude "$ENV{VCPKG_ROOT}/packages/${LIBRARY_VCPKG_FOLDER_NAME_${library}}_${VCPKG_HOST_TRIPLET}/${INCLUDE_DIRECTORY}")
            file(GLOB_RECURSE libraryFiles "${libraryInclude}/*.h" "${libraryInclude}/*.hpp")

            foreach(file ${libraryFiles})
                string(REGEX MATCH "${INCLUDE_DIRECTORY}/([A-Za-z0-9_/\\-]+)\.(h|hpp)" matchedPart "${file}")
                set(class ${CMAKE_MATCH_1})

                list(APPEND ${library}_classes ${class})
            endforeach()
        endif()

        foreach(class ${${library}_classes})
            list(APPEND EXTERNAL_CLASSES ${class})

            string(REPLACE "/" ${PATH_SEP} refactorClass ${class})

            set(MAP_DEPEN_${refactorClass} ${${library}_components_base})
        endforeach()

        find_package(${LIBRARY_PACKAGE_NAME_${library}} REQUIRED)
    endif()
endforeach()
