# =========================================================================
# INSTALACIÓN NATIVA DEL PAQUETE R (SIN DEPENDENCIAS DE DEVTOOLS)
# =========================================================================

# Usamos barras normales '/' para que el parseador de cadenas de R no rompa la sintaxis
set(CLEAN_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
string(REPLACE "\\" "/" CLEAN_SOURCE_DIR "${CLEAN_SOURCE_DIR}")

add_custom_target(install_weaver_package ALL
    # 1. Asegurar dependencias usando la función match() en lugar del operador %in% (Evita el conflicto del % en MSBuild)
    # También corregimos la URL al repositorio en la nube oficial y estable (https://cloud.r-project.org)
    COMMAND ${R_EXECUTABLE} -e "options(repos = c(CRAN = 'https://cloud.r-project.org')); deps <- c('R6', 'shiny', 'bslib', 'shinyjs', 'shinyFiles', 'ggplot2', 'dplyr', 'limSolve', 'future', 'future.apply', 'progressr', 'callr'); missing_logical <- is.na(match(deps, installed.packages()[, 'Package'])); new_deps <- deps[missing_logical]; if(length(new_deps)) install.packages(new_deps)"
    
    # 2. Instalar tu paquete 'weaver' utilizando el comando nativo de R
    COMMAND ${R_EXECUTABLE} -e "utils::install.packages('${CLEAN_SOURCE_DIR}', repos = NULL, type = 'source', force = TRUE)"
    
    COMMENT "Instalando paquete R 'weaver' y verificando dependencias en el sistema..."
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)