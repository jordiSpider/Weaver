#!/usr/bin/env Rscript

# ============================================================================
# SLURM DEPLOYMENT EXECUTION SCRIPT FOR WEAVER OPTIMISER ENGINE
# ============================================================================
# Este script inicializa el bucle evolutivo de forma aislada en un nodo de cómputo.
# Consume un optimization template unificado y restaura las trazas detalladas del sistema.

# Limpieza estricta de memoria inicial en el nodo
rm(list = ls(all = TRUE))

# Carga de dependencias del paquete consolidado
library(weaverR)
library(jsonlite)
library(future)

# --- CONFIGURACIÓN DE PARÁMETROS DE RUTAS EN EL NODO DE CÓMPUTO ---
# Modifica estas variables de acuerdo a la asignación de discos de tu clúster
OPTIMIZATION_TEMPLATE_PATH <- "C:\\Devs\\Github\\Weaver\\R\\optimization_run_opti_template.json"
TARGET_OUTPUT_PATH  <- "C:\\Devs\\Github\\Weaver\\output"
WEAVER_CLI_BINARY   <- "C:\\Devs\\Github\\Weaver\\bin\\Windows\\Release\\WeaverCLI.exe" # Cambiar por ruta local o de modulo en Slurm
BASE_CONFIG_PATH <- "C:\\Devs\\Github\\Weaver\\config\\config_Alba_optimiser"

# Validar existencia de ficheros críticos antes de reservar memoria
if (!file.exists(OPTIMIZATION_TEMPLATE_PATH)) stop("Error: Optimization template file not found: ", OPTIMIZATION_TEMPLATE_PATH)

# ============================================================================
# LECTURA DEL OPTIMIZATION TEMPLATE Y LANZAMIENTO DEL MOTOR PIPELINE
# ============================================================================
cat(paste0("[", Sys.time(), "]\t[SYSTEM]\tLoading portable optimization template...\n"))
optimization_config <- jsonlite::fromJSON(OPTIMIZATION_TEMPLATE_PATH, simplifyVector = FALSE)

# Creamos la carpeta contenedora si no existe de forma segura
dir.create(TARGET_OUTPUT_PATH, recursive = TRUE, showWarnings = FALSE)

cat(paste0("[", Sys.time(), "]\t[SYSTEM]\tInvoking R6 encapsulated genetic loop compiler...\n"))

weaverR::run_optimiser(
	optimization_config = optimization_config,
  output_path      = TARGET_OUTPUT_PATH,
  program          = WEAVER_CLI_BINARY,
  base_config_path = BASE_CONFIG_PATH,
  seed             = 0,
  total_cores      = future::availableCores(),
  program_cores    = 4
)
cat(paste0("[", Sys.time(), "]\t[SYSTEM]\tSlurm job tasks execution finished successfully.\n"))
