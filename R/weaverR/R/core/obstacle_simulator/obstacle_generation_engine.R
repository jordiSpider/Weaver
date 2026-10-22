#' Generate a porous obstacle mask using the shared native engine
#'
#' @param map_width_cells Number of map cells along X.
#' @param map_height_cells Number of map cells along Y.
#' @param cell_size Grid cell size in physical units.
#' @param bottom_left_x X coordinate of the patch bottom-left corner.
#' @param bottom_left_y Y coordinate of the patch bottom-left corner.
#' @param top_right_x X coordinate of the patch top-right corner.
#' @param top_right_y Y coordinate of the patch top-right corner.
#' @param fraction Target porous fraction between zero and one.
#' @param minimum_pore_width Minimum pore width in physical units.
#' @param feature_size Perlin feature size in physical units.
#' @param octaves Number of Perlin octaves.
#' @param seed Noise seed.
#' @return An integer matrix: -1 outside the patch, 0 obstacle, and 1 pore.
#' @export
generate_obstacle_porosity <- function(map_width_cells, map_height_cells, cell_size, bottom_left_x, bottom_left_y, top_right_x, top_right_y, fraction, minimum_pore_width, feature_size, octaves, seed = 0L) {
  scalar_values <- list(cell_size = cell_size, minimum_pore_width = minimum_pore_width, feature_size = feature_size)
  invalid_scalar <- vapply(scalar_values, function(value) length(value) != 1L || !is.numeric(value) || is.na(value) || !is.finite(value) || value <= 0, logical(1))
  if (any(invalid_scalar)) {
	stop(sprintf("These values must be finite positive numbers: %s.", paste(names(scalar_values)[invalid_scalar], collapse = ", ")), call. = FALSE)
  }
	map_dimensions <- list(map_width_cells, map_height_cells)
	if (any(vapply(map_dimensions, function(value) length(value) != 1L || !is.numeric(value) || is.na(value) || !is.finite(value) || value < 2 || value != floor(value) || value > .Machine$integer.max, logical(1)))) {
	stop("Map width and height must be whole numbers of at least two cells within the supported grid size.", call. = FALSE)
  }
	map_dimensions <- unlist(map_dimensions, use.names = FALSE)
	if (prod(map_dimensions) > .Machine$integer.max) {
	stop("The requested map exceeds the supported grid size.", call. = FALSE)
	}
	patch_corners <- list(bottom_left_x, bottom_left_y, top_right_x, top_right_y)
	if (any(vapply(patch_corners, function(value) length(value) != 1L || !is.numeric(value) || is.na(value) || !is.finite(value), logical(1))) || top_right_x <= bottom_left_x || top_right_y <= bottom_left_y) {
	stop("Patch corners must be finite and top-right must be greater than bottom-left on both axes.", call. = FALSE)
  }
  if (length(fraction) != 1L || !is.numeric(fraction) || is.na(fraction) || !is.finite(fraction) || fraction < 0 || fraction > 1) {
	stop("fraction must be between zero and one.", call. = FALSE)
  }
  if (length(octaves) != 1L || !is.numeric(octaves) || is.na(octaves) || !is.finite(octaves) || octaves < 1 || octaves != floor(octaves) || octaves > .Machine$integer.max) {
	stop("octaves must be a positive integer.", call. = FALSE)
  }
	abi_integer_max <- .Machine$integer.max
  if (length(seed) != 1L || !is.numeric(seed) || is.na(seed) || !is.finite(seed) || seed < 0 || seed != floor(seed) || seed > abi_integer_max) {
	stop(sprintf("seed must be a non-negative integer within the ABI-safe range [0, %d].", abi_integer_max), call. = FALSE)
  }
  seed <- as.integer(seed)

	grid_width <- as.integer(map_width_cells)
  grid_height <- as.integer(map_height_cells)
  cell_count <- grid_width * grid_height

  native_library <- system.file("native", package = "weaverR")
  if (!nzchar(native_library)) {
	package_root <- getOption("weaverR.package_root", "")
	if (!nzchar(package_root)) stop("The obstacle engine library is missing from the weaverR installation.", call. = FALSE)
	native_library <- file.path(package_root, "inst", "native")
  }
  extension <- if (.Platform$OS.type == "windows") ".dll" else if (Sys.info()[["sysname"]] == "Darwin") ".dylib" else ".so"
  native_name <- paste0("weaver_obstacle_engine", extension)
  native_path <- file.path(native_library, native_name)
  if (!file.exists(native_path)) stop("The obstacle engine library is missing for this platform.", call. = FALSE)

	library_name <- "weaver_obstacle_engine"
  if (!library_name %in% names(getLoadedDLLs())) dyn.load(native_path, local = FALSE)

  native_call <- tryCatch(
	.C(
	  "weaver_generate_obstacle_porosity",
		  eligibleCells = rep(1L, cell_count),
		  mapWidth = grid_width,
		  mapHeight = grid_height,
	  cellWidth = as.double(cell_size),
	  cellHeight = as.double(cell_size),
		  bottomLeftX = as.double(bottom_left_x),
		  bottomLeftY = as.double(bottom_left_y),
		  topRightX = as.double(top_right_x),
		  topRightY = as.double(top_right_y),
	  fraction = as.double(fraction),
	  minimumPoreWidth = as.double(minimum_pore_width),
	  featureSize = as.double(feature_size),
	  octaves = as.integer(octaves),
	  seed = seed,
		  patchCells = integer(cell_count),
	  status = integer(1),
		PACKAGE = library_name
	),
	error = function(error) stop("Unable to call the native obstacle engine: ", conditionMessage(error), call. = FALSE)
  )
  if (native_call$status != 0L) stop("The native obstacle engine failed with status ", native_call$status, ".", call. = FALSE)
	matrix(native_call$patchCells, nrow = grid_height, ncol = grid_width, byrow = TRUE)
}
