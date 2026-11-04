#' Validate a Weaver configuration directory with the shared C++ validator
#'
#' @param config_directory Directory containing a Weaver configuration.
#' @param expected_schema_version Expected configuration version, or NULL when no executable version is available yet.
#' @return A list with valid, errors, and warnings fields.
#' @export
validate_configuration <- function(config_directory, expected_schema_version = NULL) {
  config_directory <- normalizePath(config_directory, mustWork = TRUE)
  result_file <- tempfile(fileext = ".json")
  on.exit(unlink(result_file), add = TRUE)
  native_directory <- system.file("native", package = "weaverR")
  if (!nzchar(native_directory)) native_directory <- file.path(getOption("weaverR.package_root", ""), "inst", "native")
  ext <- if (.Platform$OS.type == "windows") ".dll" else if (Sys.info()[["sysname"]] == "Darwin") ".dylib" else ".so"
  core_dll <- file.path(native_directory, paste0("weaver_config_validator_core", ext))
  dll <- file.path(native_directory, paste0("weaver_config_validator", ext))
  if (!file.exists(core_dll)) stop("The shared Weaver validator core library is missing.", call. = FALSE)
  if (!file.exists(dll)) stop("The shared Weaver validation library is missing.", call. = FALSE)
  # The R bridge depends on this second shared library. Load it by absolute path
  # first because Windows may not search the package's native directory when
  # resolving transitive DLL dependencies of dyn.load().
  if (!"weaver_config_validator_core" %in% names(getLoadedDLLs())) dyn.load(core_dll, local = FALSE)
  if (!"weaver_config_validator" %in% names(getLoadedDLLs())) dyn.load(dll, local = FALSE)
  expected_schema_version <- if (is.null(expected_schema_version) || !length(expected_schema_version) || is.na(expected_schema_version[[1]])) "" else as.character(expected_schema_version[[1]])
  call <- .C("weaver_validate_configuration", as.character(config_directory), expected_schema_version, as.character(result_file),
             status = integer(1), PACKAGE = "weaver_config_validator")
  if (call$status != 0L && !file.exists(result_file)) stop("The shared Weaver validator could not write its result.", call. = FALSE)
  jsonlite::fromJSON(result_file, simplifyVector = FALSE)
}
