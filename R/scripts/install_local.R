# install_local.R
#
# Installs the package tarball in the developer's own R library, installing
# first any missing dependency. Used by the "windows-R-packages" workflow.
#
# Usage: Rscript install_local.R <tarball> <DESCRIPTION> <repo>

args <- commandArgs(trailingOnly = TRUE)
if (length(args) != 3L) {
  stop("Usage: Rscript install_local.R <tarball> <DESCRIPTION> <repo>", call. = FALSE)
}

tarball   <- normalizePath(args[1], winslash = "/", mustWork = TRUE)
desc_path <- normalizePath(args[2], winslash = "/", mustWork = TRUE)
repo      <- args[3]
script_arg <- grep("^--file=", commandArgs(), value = TRUE)[1]
script_dir <- dirname(normalizePath(sub("^--file=", "", script_arg),
                                   winslash = "/", mustWork = TRUE))
source(file.path(script_dir, "r_dependency_utils.R"), local = TRUE)

options(repos = c(CRAN = repo))
available_cores <- parallel::detectCores(logical = TRUE)
if (is.na(available_cores) || available_cores < 1L) available_cores <- 1L
default_ncpus <- max(1L, min(4L, available_cores - 1L))
requested_ncpus <- suppressWarnings(as.integer(Sys.getenv("WEAVER_R_INSTALL_NCPUS", "")))
ncpus <- if (!is.na(requested_ncpus) && requested_ncpus > 0L) requested_ncpus else default_ncpus
options(Ncpus = ncpus)
message("Package installation parallelism: ", ncpus,
        " (set WEAVER_R_INSTALL_NCPUS to override)")
if (.Platform$OS.type == "windows") {
  options(install.packages.compile.from.source = "never")
}

# --- Target library: first writable one, else the personal library ------------
lib <- .libPaths()[1]
if (file.access(lib, 2L) != 0L) {
  lib <- Sys.getenv("R_LIBS_USER")
  if (!nzchar(lib)) stop("No writable R library found.", call. = FALSE)
  dir.create(lib, recursive = TRUE, showWarnings = FALSE)
  .libPaths(c(lib, .libPaths()))
}
message("Target R library for missing/outdated packages: ", lib)
if (!identical(normalizePath(lib, winslash = "/", mustWork = FALSE),
               normalizePath(.libPaths()[1], winslash = "/", mustWork = FALSE))) {
  .libPaths(c(lib, .libPaths()))
}

# --- Missing or outdated dependencies -----------------------------------------
requirements <- weaver_read_requirements(desc_path)
weaver_check_r_requirement(requirements$raw)
installed <- weaver_installed_packages()
missing <- Filter(function(dep) !weaver_requirement_satisfied(dep, installed),
                  requirements$requirements)

if (length(missing) > 0L) {
  for (index in seq_along(missing)) {
    dependency <- missing[[index]]
    requirement <- if (nzchar(dependency$operator)) {
      paste0(" (", dependency$operator, " ", dependency$version, ")")
    } else ""
    message("[", index, "/", length(missing), "] Installing required dependency: ",
            dependency$package, requirement)
    install.packages(dependency$package, lib = lib, repos = getOption("repos"),
                     dependencies = NA)
    installed <- weaver_installed_packages()
    if (!weaver_requirement_satisfied(dependency, installed)) {
      stop("Could not satisfy dependency ", dependency$package, requirement,
           " from ", repo, call. = FALSE)
    }
  }
} else {
  message("All declared dependencies meet their minimum versions in active R libraries; reusing them.")
}

# --- The package itself --------------------------------------------------------
message("Installing Weaver package weaverR")
install.packages(tarball, repos = NULL, type = "source", lib = lib, dependencies = FALSE)
message("Installed ", basename(tarball), " in ", lib)
