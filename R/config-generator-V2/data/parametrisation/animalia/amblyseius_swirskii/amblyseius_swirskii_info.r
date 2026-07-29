# @@@@ species: amblyseius_swirskii

if (!"app" %in% ls()) {
	rm(list=ls(all=TRUE))

	script_path <- dirname(sys.frame(1)$ofile)
}


set.seed(123)


shared_environment <- list2env(mget(ls()))

source(file.path(script_path, "amblyseius_swirskii_baseline.r"), local = shared_environment)

source(file.path(script_path, "amblyseius_swirskii_derived.r"), local = shared_environment)
