#' Run the Configuration Generator App
#'
#' Determines the path of the currently running script, sets the working directory
#' to the script’s location, and launches the Shiny application
#' `"config-generator-app"` on the local host (127.0.0.1) and port 8080.
#'
#' @details
#' The script can be executed in two different ways:
#' - **Interactive mode:** when running from RStudio or an R console, it retrieves
#'   the current script’s path using the call stack.
#' - **Non-interactive mode:** when executed via `Rscript`, it obtains the file path
#'   from the command-line arguments passed to R.
#'
#' After determining the path, the working directory is set to the directory
#' containing this script. Finally, the Shiny app is started from the folder
#' `"config-generator-app"`.
#'
#' @import shiny
#' @export
#'
#' @examples
#' \dontrun{
#' # Run the configuration generator app
#' source("run_config_generator.R")
#' }
#'

if (interactive()) {
    # If running in an interactive session (e.g., RStudio, R console),
    # get the file path of the current script from the active frame.
    script_path <- sys.frame(1)$ofile
} else {
    # If running non-interactively (e.g., via Rscript),
    # extract the script path from the command-line arguments.
    args <- commandArgs(trailingOnly = FALSE)
    script_path <- sub("--file=", "", args[grep("--file=", args)])
}

# Set the working directory to the folder where this script is located.
setwd(dirname(script_path))

####################################################################

# Load the shiny library to run the application.
library(shiny)

# Launch the Shiny app located in the "config-generator-app" directory
# on the local host 127.0.0.1, listening on port 8080.
shiny::runApp("config-generator-app", host = "127.0.0.1", port = 8080)
