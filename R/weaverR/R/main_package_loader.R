# =========================================================================
# CENTRAL PACKAGE COMPONENT LOADER (MULTITOOL LOADER)
# =========================================================================

.load_all_tool_submodules <- function(target_env = parent.frame()) {
  package_root <- system.file(package = "weaverR")

  if (package_root == "") {
    package_root <- "."
  }

  r_base_path <- file.path(package_root, "R")

  if (dir.exists(r_base_path)) {
    # Collect only the top-level tool directories
    tool_directories <- list.dirs(r_base_path, full.names = TRUE, recursive = FALSE)

    for (tool_dir in tool_directories) {
      tool_files <- list.files(
        path = tool_dir, 
        pattern = "\\.[rR]$", 
        full.names = TRUE, 
        recursive = TRUE
      )

      for (f in tool_files) {
        # Evaluate explicitly into the environment injected by the caller (e.g. the HUB)
        source(f, local = target_env)
      }
    }
  }
}

.onLoad <- function(libname, pkgname) {
  # Default preventive initialization of the root namespace
  .load_all_tool_submodules(topenv())
}