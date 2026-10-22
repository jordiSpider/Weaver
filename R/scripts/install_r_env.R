# install_r_env.R
#
# Install weaverR and any missing or outdated dependencies in a standard R
# library selected for the current installation scope.
# The package DESCRIPTION defines minimum versions; CRAN snapshot URL is passed
# as the third argument so installation is repeatable without pinning exact
# package versions in the user's or site's standard library.
#
# Usage: Rscript install_r_env.R <user|site> <tarball> <repo-file>

args <- commandArgs(trailingOnly = TRUE)
if (length(args) != 3L || !args[1] %in% c("user", "site")) {
  stop("Usage: Rscript install_r_env.R <user|site> <tarball> <repo-file>", call. = FALSE)
}

scope    <- args[1]
tarball  <- normalizePath(args[2], winslash = "/", mustWork = TRUE)
repoFile <- normalizePath(args[3], winslash = "/", mustWork = TRUE)

expand_library_path <- function(path) {
  version_parts <- strsplit(as.character(getRversion()), ".", fixed = TRUE)[[1]]
  minor_version <- paste(version_parts[1:2], collapse = ".")
  path <- gsub("%V", as.character(getRversion()), path, fixed = TRUE)
  path <- gsub("%v", minor_version, path, fixed = TRUE)
  path <- gsub("%p", R.version$platform, path, fixed = TRUE)
  path <- gsub("%o", R.version$os, path, fixed = TRUE)
  path <- gsub("%a", R.version$arch, path, fixed = TRUE)
  path.expand(path)
}

if (scope == "user") {
  configured_libraries <- strsplit(Sys.getenv("R_LIBS_USER"), .Platform$path.sep,
                                   fixed = TRUE)[[1]]
  configured_libraries <- configured_libraries[nzchar(configured_libraries)]
  if (length(configured_libraries)) {
    lib <- expand_library_path(configured_libraries[1])
  } else {
    version_parts <- strsplit(as.character(getRversion()), ".", fixed = TRUE)[[1]]
    minor_version <- paste(version_parts[1:2], collapse = ".")
    library_folder <- if (.Platform$OS.type == "windows") {
      file.path("R", "win-library", minor_version)
    } else {
      file.path("R", paste0(R.version$platform, "-library"), minor_version)
    }
    lib <- file.path(path.expand("~"), library_folder)
  }
} else {
  site_libraries <- .Library.site[!is.na(.Library.site) & nzchar(.Library.site)]
  if (!length(site_libraries)) {
    stop("This R installation has no standard site library configured.", call. = FALSE)
  }
  lib <- site_libraries[1]
}
lib <- normalizePath(lib, winslash = "/", mustWork = FALSE)
repo_lines <- readLines(repoFile, warn = FALSE, encoding = "UTF-8")
repo <- if (length(repo_lines)) trimws(repo_lines[1]) else ""
if (!nzchar(repo)) stop("CRAN repository file is empty: ", repoFile, call. = FALSE)
options(repos = c(CRAN = repo))
script_arg <- grep("^--file=", commandArgs(), value = TRUE)[1]
script_dir <- dirname(normalizePath(sub("^--file=", "", script_arg),
                                   winslash = "/", mustWork = TRUE))
source(file.path(script_dir, "r_dependency_utils.R"), local = TRUE)

message("R ", getRversion(), " at ", R.home())
message("CRAN snapshot: ", repo)
message("Standard R library target: ", lib, " (", scope, ")")

available_cores <- parallel::detectCores(logical = TRUE)
if (is.na(available_cores) || available_cores < 1L) available_cores <- 1L
default_ncpus <- max(1L, min(4L, available_cores - 1L))
requested_ncpus <- suppressWarnings(as.integer(Sys.getenv("WEAVER_R_INSTALL_NCPUS", "")))
ncpus <- if (!is.na(requested_ncpus) && requested_ncpus > 0L) requested_ncpus else default_ncpus
options(Ncpus = ncpus)
message("Package installation parallelism: ", ncpus,
        " (set WEAVER_R_INSTALL_NCPUS to override)")

dir.create(lib, recursive = TRUE, showWarnings = FALSE)
if (!dir.exists(lib) || file.access(lib, 2L) != 0L) {
  stop("The selected standard R library is not writable: ", lib, call. = FALSE)
}
inherited_libraries <- .libPaths()
Sys.setenv(R_LIBS = paste(unique(c(lib, inherited_libraries)), collapse = .Platform$path.sep))
.libPaths(c(lib, inherited_libraries))
message("R library search order (first matching package is selected):")
for (index in seq_along(.libPaths())) {
  message("  [", index, "] ", .libPaths()[index])
}

# Read DESCRIPTION from the package tarball so the same declared minimum
# versions drive local builds and package installation.
members <- utils::untar(tarball, list = TRUE)
description_member <- members[grepl("(^|/)DESCRIPTION$", members)]
if (length(description_member) != 1L) {
  stop("Expected one DESCRIPTION in package tarball; found ",
       length(description_member), call. = FALSE)
}
desc_dir <- tempfile("weaver-description-")
dir.create(desc_dir)
utils::untar(tarball, files = description_member, exdir = desc_dir)
description <- file.path(desc_dir, description_member)
requirements <- weaver_read_requirements(description)
dependencies <- requirements$requirements
weaver_check_r_requirement(requirements$raw)

packages <- weaver_installed_packages()
message("Installed dependency copies and versions visible to R:")
for (dependency in dependencies) {
  message("  ", weaver_requirement_diagnostic(dependency, packages))
}
unmet <- Filter(function(dependency) !weaver_requirement_satisfied(dependency, packages), dependencies)
if (length(unmet) == 0L) {
  message("All declared dependencies meet their minimum versions in the active R libraries.")
} else {
  for (index in seq_along(unmet)) {
    dependency <- unmet[[index]]
    requirement <- if (nzchar(dependency$operator)) {
      paste0(" (", dependency$operator, " ", dependency$version, ")")
    } else ""
    message("[", index, "/", length(unmet), "] Installing required dependency: ",
            dependency$package, requirement, " into ", lib)
    install_warnings <- character()
    install_error <- NULL
    tryCatch(
      withCallingHandlers(
        utils::install.packages(dependency$package, lib = lib, repos = getOption("repos"),
                                dependencies = NA),
        warning = function(warning) {
          install_warnings <<- c(install_warnings, conditionMessage(warning))
          invokeRestart("muffleWarning")
        }
      ),
      error = function(error) install_error <<- conditionMessage(error)
    )
    if (length(install_warnings)) {
      message("Warnings while installing ", dependency$package, ": ",
              paste(unique(install_warnings), collapse = " | "))
    }
    packages <- weaver_installed_packages()
    if (!is.null(install_error) || !weaver_requirement_satisfied(dependency, packages)) {
      details <- c(
        paste0("ERROR: required dependency could not be installed: ", dependency$package,
               if (nzchar(dependency$operator)) paste0(" (", dependency$operator, " ", dependency$version, ")") else ""),
        paste0("Resolved dependency: ", weaver_requirement_diagnostic(dependency, packages)),
        paste0("Standard R library target: ", lib),
        paste0("CRAN repository: ", repo)
      )
      if (!is.null(install_error)) details <- c(details, paste0("install.packages error: ", install_error))
      if (length(install_warnings)) {
        details <- c(details, paste0("install.packages warnings: ", paste(unique(install_warnings), collapse = " | ")))
      }
      stop(paste(details, collapse = "\n"), call. = FALSE)
    }
    message("Installed dependency is now visible at the required version: ",
            weaver_requirement_diagnostic(dependency, packages))
  }
}

message("Installing Weaver package weaverR from ", tarball, " into ", lib)
package_warnings <- character()
package_error <- NULL
tryCatch(
  withCallingHandlers(
    utils::install.packages(
      tarball,
      repos = NULL,
      type = "source",
      lib = lib,
      dependencies = FALSE,
      INSTALL_opts = c("--no-docs", "--no-multiarch")
    ),
    warning = function(warning) {
      package_warnings <<- c(package_warnings, conditionMessage(warning))
      invokeRestart("muffleWarning")
    }
  ),
  error = function(error) package_error <<- conditionMessage(error)
)
if (length(package_warnings)) {
  message("Warnings while installing weaverR: ", paste(unique(package_warnings), collapse = " | "))
}
if (!is.null(package_error) || !nrow(weaver_package_copies("weaverR", weaver_installed_packages(lib)))) {
  details <- c(
    paste0("ERROR: weaverR could not be installed into ", lib),
    paste0("Required dependency state: ",
           paste(vapply(dependencies, weaver_requirement_diagnostic, character(1), installed = packages),
                 collapse = "\n  "))
  )
  if (!is.null(package_error)) details <- c(details, paste0("install.packages error: ", package_error))
  if (length(package_warnings)) {
    details <- c(details, paste0("install.packages warnings: ", paste(unique(package_warnings), collapse = " | ")))
  }
  stop(paste(details, collapse = "\n"), call. = FALSE)
}

message("Verifying Weaver R environment")
namespace_error <- NULL
tryCatch(
  invisible(loadNamespace("weaverR", lib.loc = .libPaths())),
  error = function(error) namespace_error <<- conditionMessage(error)
)
if (!is.null(namespace_error)) {
  packages <- weaver_installed_packages()
  details <- vapply(dependencies, weaver_requirement_diagnostic, character(1), installed = packages)
  message("ERROR: weaverR namespace loading failed: ", namespace_error)
  message("Dependency resolution at failure:\n  ", paste(details, collapse = "\n  "))
  stop("weaverR was installed but could not be loaded: ", namespace_error, call. = FALSE)
}
message("weaverR ", as.character(utils::packageVersion("weaverR", lib.loc = lib)),
        " ready in the standard R library ", lib)
