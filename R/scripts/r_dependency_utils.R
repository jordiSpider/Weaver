# Shared dependency parsing and version checks for local and installer workflows.

weaver_parse_requirement <- function(item) {
  match <- regexec(
    "^([A-Za-z][A-Za-z0-9.]*)\\s*(?:\\(\\s*(>=|<=|==|>|<)\\s*([^)]*)\\s*\\))?$",
    item, perl = TRUE
  )
  parts <- regmatches(item, match)[[1]]
  if (length(parts) == 0L) {
    stop("Cannot parse DESCRIPTION dependency: ", item, call. = FALSE)
  }
  list(
    package = parts[2],
    operator = if (nzchar(parts[3])) parts[3] else "",
    version = if (nzchar(parts[4])) trimws(parts[4]) else ""
  )
}

weaver_read_requirements <- function(description) {
  fields <- c("Depends", "Imports", "LinkingTo")
  dcf <- read.dcf(description, fields = fields)
  raw <- paste(stats::na.omit(as.character(dcf[1, ])), collapse = ",")
  items <- trimws(strsplit(raw, ",", fixed = TRUE)[[1]])
  requirements <- lapply(items[nzchar(items)], weaver_parse_requirement)
  list(
    requirements = Filter(function(x) x$package != "R", requirements),
    raw = raw
  )
}

weaver_installed_packages <- function(libraries = .libPaths()) {
  libraries <- unique(libraries[dir.exists(libraries)])
  packages <- lapply(libraries, function(library) {
    utils::installed.packages(lib.loc = library)
  })
  packages <- Filter(function(installed) nrow(installed) > 0L, packages)
  if (!length(packages)) return(matrix(character(), nrow = 0L, ncol = 0L))
  do.call(rbind, packages)
}

weaver_package_copies <- function(package, installed) {
  if (is.null(dim(installed))) {
    return(matrix(character(), nrow = 0L, ncol = 0L))
  }
  if (!"Package" %in% colnames(installed)) return(installed[integer(), , drop = FALSE])
  installed[installed[, "Package"] == package, , drop = FALSE]
}

weaver_visible_package <- function(package, installed) {
  rows <- weaver_package_copies(package, installed)
  if (nrow(rows) == 0L) return(NULL)
  rows[1L, , drop = FALSE]
}

weaver_format_package_copies <- function(package, installed) {
  rows <- weaver_package_copies(package, installed)
  if (nrow(rows) == 0L) return("no installed copies")
  paste(sprintf("%s in %s", rows[, "Version"], rows[, "LibPath"]), collapse = "; ")
}

weaver_requirement_diagnostic <- function(requirement, installed) {
  requirement_text <- if (nzchar(requirement$operator)) {
    paste0(requirement$operator, " ", requirement$version)
  } else {
    "installed"
  }
  visible <- weaver_visible_package(requirement$package, installed)
  if (is.null(visible)) {
    return(paste0(requirement$package, " requires ", requirement_text,
                  "; no installed copy was found"))
  }

  visible_version <- visible[1L, "Version"]
  visible_library <- visible[1L, "LibPath"]
  status <- if (weaver_requirement_satisfied(requirement, installed)) {
    "satisfies the requirement"
  } else {
    "does not satisfy the requirement"
  }
  paste0(requirement$package, " requires ", requirement_text,
         "; R resolves version ", visible_version, " from ", visible_library,
         " (", status, "). Installed copies: ",
         weaver_format_package_copies(requirement$package, installed))
}

weaver_requirement_satisfied <- function(requirement, installed) {
  visible <- weaver_visible_package(requirement$package, installed)
  if (is.null(visible)) return(FALSE)
  if (!nzchar(requirement$operator)) return(TRUE)

  comparison <- utils::compareVersion(visible[1L, "Version"], requirement$version)
  switch(requirement$operator,
         ">=" = comparison >= 0L,
         ">"  = comparison > 0L,
         "<=" = comparison <= 0L,
         "<"  = comparison < 0L,
         "==" = comparison == 0L,
         FALSE)
}

weaver_check_r_requirement <- function(raw) {
  match <- regexec(
    "(^|,)\\s*R\\s*\\(\\s*(>=|<=|==|>|<)\\s*([^)]*)\\s*\\)",
    raw, perl = TRUE
  )
  parts <- regmatches(raw, match)[[1]]
  if (length(parts) == 0L) return(invisible(TRUE))

  comparison <- utils::compareVersion(as.character(getRversion()), trimws(parts[4]))
  satisfied <- switch(parts[3],
                      ">=" = comparison >= 0L,
                      ">"  = comparison > 0L,
                      "<=" = comparison <= 0L,
                      "<"  = comparison < 0L,
                      "==" = comparison == 0L,
                      FALSE)
  if (!satisfied) {
    stop("Weaver requires R ", parts[3], " ", trimws(parts[4]),
         "; this installation is R ", getRversion(), call. = FALSE)
  }
  invisible(TRUE)
}
