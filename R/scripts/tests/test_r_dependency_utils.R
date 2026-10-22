script_path <- grep("^--file=", commandArgs(), value = TRUE)[1]
script_path <- normalizePath(sub("^--file=", "", script_path), winslash = "/", mustWork = TRUE)
source(file.path(dirname(script_path), "..", "r_dependency_utils.R"), local = TRUE)
invisible(parse(file.path(dirname(script_path), "..", "install_r_env.R")))
invisible(parse(file.path(dirname(script_path), "..", "install_local.R")))

run_tests <- function() {
	old_library <- "R-library-old"
  new_library <- "R-library-new"
  installed <- rbind(
	c(Package = "shiny", Version = "1.13.0", LibPath = old_library),
	c(Package = "shiny", Version = "1.14.0", LibPath = new_library)
  )

  requirement <- list(package = "shiny", operator = ">=", version = "1.14.0")
  copies <- weaver_package_copies("shiny", installed)

  stopifnot(nrow(copies) == 2L)
  stopifnot(copies[1L, "Version"] == "1.13.0")
  stopifnot(!weaver_requirement_satisfied(requirement, installed))

  diagnostic <- weaver_requirement_diagnostic(requirement, installed)
  stopifnot(grepl("R resolves version 1.13.0", diagnostic, fixed = TRUE))
  stopifnot(grepl(old_library, diagnostic, fixed = TRUE))
  stopifnot(grepl("1.14.0", diagnostic, fixed = TRUE))

	reversed <- installed[2:1, , drop = FALSE]
  stopifnot(weaver_requirement_satisfied(requirement, reversed))
  stopifnot(weaver_visible_package("shiny", reversed)[1L, "Version"] == "1.14.0")

  missing <- list(package = "missingPackage", operator = ">=", version = "1.0.0")
  stopifnot(!weaver_requirement_satisfied(missing, installed))
  stopifnot(grepl("no installed copy was found",
				  weaver_requirement_diagnostic(missing, installed), fixed = TRUE))
  empty <- matrix(character(), nrow = 0L, ncol = 0L)
  stopifnot(nrow(weaver_package_copies("shiny", empty)) == 0L)
}

run_tests()
message("R dependency utility tests passed.")
