#' Load or Install an R Package
#'
#' Checks if a package is installed, and if not, prompts the user to install it.
#' After installation (or if already installed), the package is loaded.
#'
#' @param package_name Character. Name of the package to load or install.
#'
#' @return None. The function loads the package into the current R session.
#'
#' @details
#' This function first checks if the package is available using `requireNamespace`.
#' If the package is not installed, the user is prompted to install it.
#' If the user agrees, the package is installed from CRAN using `install.packages()`.
#' After installation, or if the package was already installed, the package is loaded
#' with `library()` so that its functions are available in the current session.
#'
#' @examples
#' \dontrun{
#' load_or_install_package("dplyr")
#' }
#'
#' @export
load_or_install_package <- function(package_name) {
    if (!requireNamespace(package_name, quietly = TRUE)) {
        answer <- readline(
            paste0("The package '", package_name, "' is not installed. Do you want to install it? (y/n): ")
        )

        if (tolower(answer) == "y") {
            message(paste0("Installing '", package_name, "'..."))
            install.packages(package_name, repos = "https://cloud.r-project.org")
            message(paste0("Installation of '", package_name, "' completed."))
        } else {
            message("The package will not be installed.")
        }
    } 

    library(package_name, character.only = TRUE)
}
