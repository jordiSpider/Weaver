# =========================================================================
# BÚSQUEDA AUTOMATIZADA DEL INTÉRPRETE DE R (WINDOWS COMPATIBLE)
# =========================================================================

# 1. Definir los nombres de los ejecutables válidos
set(R_NAMES R Rscript Rscript.exe R.exe)

# 2. Definir las rutas de instalación estándar de R en Windows de forma dinámica
set(R_PROG_FILES_PATHS
    "C:/Program Files/R"
    "C:/Program Files (x86)/R"
)

# 3. Escanear las subcarpetas "bin" de cualquier versión instalada (R-4.2, R-4.4, etc.)
set(R_HINTS "")
foreach(BASE_PATH ${R_PROG_FILES_PATHS})
    if(EXISTS ${BASE_PATH})
        # Obtener la lista de subcarpetas (ej: R-4.4.1)
        file(GLOB R_VERSIONS RELATIVE ${BASE_PATH} "${BASE_PATH}/R-*")
        foreach(VERSION ${R_VERSIONS})
            # Añadir las variantes de arquitectura de 64 y 32 bits a la lista de búsqueda
            list(APPEND R_HINTS "${BASE_PATH}/${VERSION}/bin")
            list(APPEND R_HINTS "${BASE_PATH}/${VERSION}/bin/x64")
            list(APPEND R_HINTS "${BASE_PATH}/${VERSION}/bin/i386")
        endforeach()
    endif()
endforeach()

# 4. Lanzar la búsqueda con prioridad en los directorios encontrados
find_program(R_EXECUTABLE 
    NAMES ${R_NAMES}
    HINTS ${R_HINTS}
    PATHS ${R_HINTS}
    COMMENT "Buscando binarios de R en el sistema..."
)

# 5. Si CMake sigue sin encontrarlo, mostrar un mensaje de ayuda descriptivo en lugar de colapsar
if(NOT R_EXECUTABLE)
    message(FATAL_ERROR 
        "CMake Error: El intérprete de R no ha sido detectado en el sistema.\n"
        "Asegúrate de tener R instalado de forma independiente a RStudio.\n"
        "Si ya lo tienes, puedes forzar la ruta en la consola de CMake ejecutando:\n"
        "cmake .. -DR_EXECUTABLE=\"C:/Ruta/A/Tu/R/bin/x64/Rscript.exe\""
    )
endif()

message(STATUS "Intérprete de R detectado con éxito en: ${R_EXECUTABLE}")

add_subdirectory(weaverRHub)