# check_lock.R
#
# Fails if a dependency declared in DESCRIPTION is missing from renv.lock.
# Without this check the installer would be built with a lockfile that cannot
# satisfy the package, and the installation would fail on the target machine.
#
# Usage: Rscript --vanilla check_lock.R <DESCRIPTION> <renv.lock>

args <- commandArgs(trailingOnly = TRUE)
if (length(args) != 2L) stop("Usage: Rscript check_lock.R <DESCRIPTION> <renv.lock>", call. = FALSE)

desc_path <- args[1]
lock_path <- args[2]

fields <- c("Depends", "Imports", "LinkingTo")
dcf    <- read.dcf(desc_path, fields = fields)
deps   <- unlist(strsplit(paste(stats::na.omit(as.character(dcf)), collapse = ","), ","))
deps   <- trimws(sub("\\s*\\(.*\\)", "", deps))
deps   <- deps[nzchar(deps) & deps != "R"]
deps   <- setdiff(deps, rownames(installed.packages(priority = "base")))

lock <- paste(readLines(lock_path, warn = FALSE, encoding = "UTF-8"), collapse = "\n")
in_lock <- vapply(deps, function(pkg) {
  pattern <- sprintf('"%s"\\s*:\\s*\\{', gsub(".", "\\.", pkg, fixed = TRUE))
  grepl(pattern, lock)
}, logical(1))

if (!all(in_lock)) {
  stop(
    "renv.lock does not contain: ", paste(deps[!in_lock], collapse = ", "),
    "\nRegenerate it with the CMake target 'weaverR_update_lock'.",
    call. = FALSE
  )
}
message("renv.lock covers all ", length(deps), " direct dependencies of the package.")