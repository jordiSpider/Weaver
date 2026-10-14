#!/usr/bin/env Rscript

# ============================================================================
# SLURM DEPLOYMENT EXECUTION SCRIPT FOR WEAVER OPTIMISER ENGINE
# ============================================================================
# Este script inicializa el bucle evolutivo de forma aislada en un nodo de cómputo.
# Consume un JSON blueprint unificado y restaura las trazas detalladas del sistema.

# Limpieza estricta de memoria inicial en el nodo
rm(list = ls(all = TRUE))

# Carga de dependencias del paquete consolidado
library(weaverRHub)
library(jsonlite)
library(future)

# --- CONFIGURACIÓN DE PARÁMETROS DE RUTAS EN EL NODO DE CÓMPUTO ---
# Modifica estas variables de acuerdo a la asignación de discos de tu clúster
JSON_BLUEPRINT_PATH <- "C:\\Devs\\Github\\Weaver\\R\\optimization_run_blueprint.json"
TARGET_OUTPUT_PATH  <- "C:\\Devs\\Github\\Weaver\\output"
WEAVER_CLI_BINARY   <- "C:\\Devs\\Github\\Weaver\\bin\\Windows\\Release\\WeaverCLI.exe" # Cambiar por ruta local o de modulo en Slurm
BASE_CONFIG_PATH <- "C:\\Devs\\Github\\Weaver\\config\\config_Alba_test"

# Validar existencia de ficheros críticos antes de reservar memoria
if (!file.exists(JSON_BLUEPRINT_PATH)) stop("Error: JSON Blueprint file not found: ", JSON_BLUEPRINT_PATH)

# ============================================================================
# DEFINICIÓN DEL CALLBACK RECONSTRUCTOR DE TRAZAS (Estilo Optimizador Antiguo)
# ============================================================================
# Esta función intercepta cada evento del algoritmo genético y replica exactamente
# las marcas visuales (*, #, ||) en el archivo de salida de Slurm (slurm-XXXX.out)
slurm_metrics_callback <- function(generation, current_best, current_mean, log_line, log_type) {
  
  # Caso A: Inicialización de la población o logs genéricos de sistema
  if (identical(log_type, "system") || identical(log_type, "success")) {
    cat(paste0("[", Sys.time(), "]\t[", toupper(log_type), "]\t", log_line, "\n"))
    return()
  }
  
  # Caso B: Bloques informativos complejos o advertencias de colisión
  if (identical(log_type, "warning") || identical(log_type, "danger")) {
    cat(paste0("######################################################################\n"))
    cat(paste0("##  ", toupper(log_type), " DIAGNOSTIC: ", log_line, "  ##\n"))
    cat(paste0("######################################################################\n"))
    return()
  }

  # Caso C: Trazas e índices analíticos de los individuos en ejecución (Estilo Antiguo)
  # Si la línea contiene los descriptores clave de depuración, formateamos sus sangrías
  if (grepl("Individual", log_line) || grepl("Parameter", log_line) || grepl("Preference", log_line) || grepl("Profitability", log_line)) {
    cat(paste0(log_line, "\n"))
    return()
  }
  
  # Caso D: Resumen del cierre de la generación (Aptitudes y convergencias de fitness)
  if (generation > 0 && !is.null(current_best)) {
    # Cuando el motor envía el cierre de una generación, pintamos la tabla formateada
    cat(paste0("------------------------------------------------\n"))
    cat(paste0("## Generation ", generation, " Summary Statistics ##\n"))
    cat(paste0(" - Best Individual Fitness Metrics: ", jsonlite::toJSON(current_best, auto_unbox = TRUE), "\n"))
    if (!is.null(current_mean)) {
      cat(paste0(" - Mean Population Fitness Metrics: ", jsonlite::toJSON(current_mean, auto_unbox = TRUE), "\n"))
    }
    cat(paste0("------------------------------------------------\n"))
    return()
  }
  
  # Fallback: Volcado directo de texto plano para conservar cualquier mensaje residual
  cat(paste0(log_line, "\n"))
  flush.console() # Fuerza el vaciado inmediato del búfer hacia el log de Slurm
}

# ============================================================================
# LECTURA DEL BLUEPRINT Y LANZAMIENTO DEL MOTOR PIPELINE
# ============================================================================
cat(paste0("[", Sys.time(), "]\t[SYSTEM]\tLoading portable optimization blueprint payload...\n"))
blueprint_data <- jsonlite::fromJSON(JSON_BLUEPRINT_PATH, simplifyVector = FALSE)

# Creamos la carpeta contenedora si no existe de forma segura
dir.create(TARGET_OUTPUT_PATH, recursive = TRUE, showWarnings = FALSE)

cat(paste0("[", Sys.time(), "]\t[SYSTEM]\tInvoking R6 encapsulated genetic loop compiler...\n"))

# Invocamos la Función Envoltorio de la Opción A pasándole el callback tradicional de consola
weaverRHub::run_optimiser(
  blueprint          = blueprint_data,
  output_path        = TARGET_OUTPUT_PATH,
  program = WEAVER_CLI_BINARY,
  base_config_path   = BASE_CONFIG_PATH,
  seed = 0,
  total_cores = future::availableCores(),
  program_cores = 4,
  metrics_callback   = slurm_metrics_callback
)

cat(paste0("[", Sys.time(), "]\t[SYSTEM]\tSlurm job tasks execution finished successfully.\n"))