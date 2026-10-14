# Definir la ruta de instalación/distribución final
set(DIST_DIR "${CMAKE_BINARY_DIR}/dist")
file(MAKE_DIRECTORY ${DIST_DIR})

add_custom_target(deploy_hub ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_CURRENT_SOURCE_DIR}" 
            "${DIST_DIR}/hub"
    COMMENT "Desplegando HUB Central en la carpeta de distribución analítica..."
)

# Asegurar que el HUB se despliegue solo después de que el paquete R esté instalado correctamente
add_dependencies(deploy_hub install_weaver_package)

# =========================================================================
# GENERACIÓN AUTOMATIZADA DEL LAUNCHER .BAT PORTÁTIL (PORT SHIELD SECURE)
# =========================================================================
set(BAT_LAUNCHER_PATH "${DIST_DIR}/run_hub.bat")

get_filename_component(R_BIN_DIR "${R_EXECUTABLE}" DIRECTORY)
set(R_EXE_LAUNCHER "${R_BIN_DIR}/R.exe")

file(WRITE "${BAT_LAUNCHER_PATH}" 
    "@echo off\n"
    "title Weaver Ecological Toolbox - Central HUB\n"
    "echo =========================================================================\n"
    "echo        Liberando puertos del sistema y limpiando procesos zombies...     \n"
    "echo =========================================================================\n"
    "echo.\n"
    
    # CORRECCIÓN DE ESTADO: Cambiamos -State Listening por -State Listen para cumplir el estándar de PowerShell
    "powershell -NoProfile -Command \"$conn = Get-NetTCPConnection -LocalPort 8000 -State Listen -ErrorAction SilentlyContinue; if ($conn) { Get-Process -Id $conn.OwningProcess -ErrorAction SilentlyContinue | Stop-Process -Force }\"\n"
    "echo Puerto 8000 verificado y listo para su uso.\n"
    "echo.\n"
    
    "echo =========================================================================\n"
    "echo        Inicializando entorno grafico e hilos de computo Weaver...         \n"
    "echo =========================================================================\n"
    "cd /d \"%~dp0hub\"\n"
    "\"${R_EXECUTABLE}\" -e \"shiny::runApp('app.R', launch.browser=TRUE)\"\n"
    "if %errorlevel% neq 0 (\n"
    "    echo.\n"
    "    echo [ERROR] Ocurrio un fallo critico al ejecutar el HUB Central.\n"
    "    pause\n"
    ")\n"
)

message(STATUS "Launcher por lotes Shiny corregido generado en: ${BAT_LAUNCHER_PATH}")