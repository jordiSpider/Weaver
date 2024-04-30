
# Import optimisation tools
library(weaverWSLOptimiser)

weaverWSLOptimiser::setBaseConfigPath("/mnt/c/Users/mcs20/Desktop/EEZA/Weaver/config/base_arthropods_temp_updated")

weaverWSLOptimiser::setOutputFolder("/mnt/c/Users/mcs20/Desktop/EEZA/Weaver/pruebaOpti")

weaverWSLOptimiser::setNumberOfCores(2)

weaverWSLOptimiser::setMaxNumberOfGenerations(2)

weaverWSLOptimiser::setEvaluationFunctionSelected("Amplitude")

weaverWSLOptimiser::setSelectionOperatorSelected("Roulette")

weaverWSLOptimiser::setCrossoverOperatorSelected("Chiasms")

weaverWSLOptimiser::setMutationOperatorSelected("Simple")

weaverWSLOptimiser::setReplacementOperatorSelected("Elitism")

weaverWSLOptimiser::setFitnessObjective(0.0005)

weaverWSLOptimiser::setMaximiseFitness(FALSE)

weaverWSLOptimiser::setBurn_in(1)

weaverWSLOptimiser::addFixedParameter(
    name = "simulation_params.json:simulation/runDays",
    initialValues = c(5),
    range = c(1, 2)
)

weaverWSLOptimiser::addFixedParameter(
    name = "simulation_params.json:simulation/encountersMatrixFilename",
    initialValues = c("prueba1", "prueba2"),
    range = c("prueba1", "prueba2")
)

weaverWSLOptimiser::addRandomParameter(
    name = "species/Consumer_fast_asexual.json:ontogeneticLinks/resourceLinks/item_0/links/item_0/profitability",
    numberOfInitialValues = 3,
    range = c(0.1, 0.9)
)



weaverWSLOptimiser::run(overwrite = TRUE)
