
set(EXTERNAL_LIBRARIES
    magic_enum
    cxxopts
    nlohmann_json
)

# ===========================
# magic_enum

set(LIBRARY_magic_enum magic_enum::magic_enum)

set(CLASS_FILES_magic_enum 
    magic_enum.hpp
)

# ===========================
# cxxopts

set(LIBRARY_cxxopts cxxopts::cxxopts)

set(CLASS_FILES_cxxopts 
    cxxopts.hpp
)

# ===========================
# nlohmann_json

set(LIBRARY_nlohmann_json nlohmann_json::nlohmann_json)

set(CLASS_FILES_nlohmann_json 
    nlohmann/json.hpp
)

# ===========================
# fmt

set(LIBRARY_fmt fmt::fmt-header-only)

set(CLASS_FILES_fmt 
    fmt/core.h
)



set(EXTERNAL_CLASSES)

foreach(library ${EXTERNAL_LIBRARIES})
    find_package(${library} REQUIRED)

    foreach(classFile ${CLASS_FILES_${library}})
        string(REGEX REPLACE "\\.(h|hpp)$" "" class ${classFile})

        list(APPEND EXTERNAL_CLASSES ${class})

        string(REPLACE "/" ${PATH_SEP} refactorClass ${class})

        set(MAP_DEPEN_${refactorClass} ${LIBRARY_${library}})
    endforeach()
endforeach()
