# update_lock.R
#
# Keeps renv.lock up to date: the exact versions of the dependencies of
# weaverR (and of their dependencies) that the installer downloads from CRAN
# on the target machine with renv::restore().
#
# Usage: Rscript --vanilla update_lock.R <mode> <DESCRIPTION> <renv.lock> <repo>
#
#   mode "ensure"  Generates the lockfile if it is missing, or if a dependency
#                  declared in DESCRIPTION is not in it. Otherwise it does
#                  nothing (fast). This is what every build runs.
#   mode "force"   Always regenerates it (use it to pick up newer versions).
#
# Commit the resulting renv.lock to keep builds reproducible.

args <- commandArgs(trailingOnly = TRUE)
if (length(args) != 4L || !args[1] %in% c("ensure", "force")) {
  stop("Usage: Rscript update_lock.R <ensure|force> <DESCRIPTION> <renv.lock> <repo>",
       call. = FALSE)
}

mode      <- args[1]
desc_path <- normalizePath(args[2], winslash = "/", mustWork = TRUE)
lock_path <- normalizePath(args[3], winslash = "/", mustWork = FALSE)
repo      <- args[4]

# --- Direct dependencies declared in DESCRIPTION -------------------------------
direct_deps <- function(path) {
  dcf  <- read.dcf(path, fields = c("Depends", "Imports", "LinkingTo"))
  deps <- unlist(strsplit(paste(stats::na.omit(as.character(dcf)), collapse = ","), ","))
  deps <- trimws(sub("\\s*\\(.*\\)", "", deps))
  deps <- deps[nzchar(deps) & deps != "R"]
  setdiff(deps, rownames(installed.packages(priority = "base")))
}

# --- Dependencies missing from the lockfile -------------------------------------
missing_in_lock <- function(deps, lock) {
  if (!file.exists(lock)) return(deps)
  txt <- paste(readLines(lock, warn = FALSE, encoding = "UTF-8"), collapse = "\n")
  found <- vapply(deps, function(pkg) {
    pattern <- sprintf('"%s"\\s*:\\s*\\{', gsub(".", "\\.", pkg, fixed = TRUE))
    grepl(pattern, txt)
  }, logical(1))
  deps[!found]
}

deps <- direct_deps(desc_path)

if (mode == "ensure") {
  if (length(missing_in_lock(deps, lock_path)) == 0L) {
    message("renv.lock is up to date (", length(deps), " direct dependencies).")
    Sys.setFileTime(lock_path, Sys.time())   # so the build does not re-run this step
    quit(save = "no", status = 0L)
  }
  message("renv.lock is missing or incomplete: generating it (takes a few minutes)...")
} else {
  message("Regenerating renv.lock (takes a few minutes)...")
}

# --- Scratch project and library -------------------------------------------------
options(repos = c(CRAN = repo))
if (.Platform$OS.type == "windows") {
  options(install.packages.compile.from.source = "never")
}

tmp <- tempfile("weaver-lock-")
dir.create(tmp)
file.copy(desc_path, file.path(tmp, "DESCRIPTION"))

lib <- file.path(tmp, "library")
dir.create(lib)
.libPaths(lib)

install.packages("renv", lib = lib)
install.packages(deps, lib = lib, dependencies = NA)

# --- Snapshot: only what DESCRIPTION needs, recursively ---------------------------
tmp_lock <- file.path(tmp, "renv.lock")
renv::snapshot(
  project  = tmp,
  library  = lib,
  lockfile = tmp_lock,
  type     = "explicit",
  repos    = c(CRAN = repo),
  prompt   = FALSE,
  force    = TRUE
)

# Copy only after a successful snapshot, so a failure never leaves a broken lockfile
dir.create(dirname(lock_path), recursive = TRUE, showWarnings = FALSE)
file.copy(tmp_lock, lock_path, overwrite = TRUE)

still_missing <- missing_in_lock(deps, lock_path)
if (length(still_missing) > 0L) {
  stop("renv.lock does not contain: ", paste(still_missing, collapse = ", "), call. = FALSE)
}
message("Lockfile written to ", lock_path)