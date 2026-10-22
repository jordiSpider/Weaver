#!/usr/bin/env Rscript

rm(list = ls(all = TRUE))

library(weaverR)
library(jsonlite)
library(future)

WEAVER_CLI_BINARY   <- "C:\\Program Files\\Weaver\\bin\\WeaverCLI.exe"

OPTIMIZATION_TEMPLATE_PATH <- "C:\\Devs\\Github\\Weaver\\R\\Alba_opti_template.json"
TARGET_OUTPUT_PATH  <- "C:\\Devs\\Github\\Weaver\\output"

BASE_CONFIG_PATH <- "C:\\Devs\\Github\\Weaver\\config\\config_Alba_optimiser"

optimization_config <- jsonlite::fromJSON(OPTIMIZATION_TEMPLATE_PATH, simplifyVector = FALSE)

dir.create(TARGET_OUTPUT_PATH, recursive = TRUE, showWarnings = FALSE)

weaverR::run_optimiser(
	optimization_config = optimization_config,
  output_path      = TARGET_OUTPUT_PATH,
  program          = WEAVER_CLI_BINARY,
  base_config_path = BASE_CONFIG_PATH,
  seed             = 0,
  total_cores      = future::availableCores(),
  program_cores    = 4
)
