
# Import optimisation tools
library(weaverOptimiser)

weaverOptimiser::setBaseConfigPath("/home/macarse/Escritorio/EEZA/Weaver/config/base_arthropods_temp_updated")

weaverOptimiser::setOutputFolder("/home/macarse/Escritorio/EEZA/Weaver/pruebaOpti")

weaverOptimiser::setNumberOfCores(2)

weaverOptimiser::setMaxNumberOfGenerations(2)

weaverOptimiser::setEvaluationFunctionSelected("Amplitude")

weaverOptimiser::setSelectionOperatorSelected("Roulette")

weaverOptimiser::setCrossoverOperatorSelected("Chiasms")

weaverOptimiser::setMutationOperatorSelected("Simple")

weaverOptimiser::setReplacementOperatorSelected("Elitism")

weaverOptimiser::setFitnessObjective(0.0005)

weaverOptimiser::setMaximiseFitness(FALSE)

weaverOptimiser::setBurn_in(1)

weaverOptimiser::addFixedParameter(
    name = "simulation_params.json:simulation/runDays",
    initialValues = c(5),
    range = c(1, 2)
)

weaverOptimiser::addFixedParameter(
    name = "simulation_params.json:simulation/encountersMatrixFilename",
    initialValues = c("prueba1", "prueba2"),
    range = c("prueba1", "prueba2")
)

weaverOptimiser::addRandomParameter(
    name = "species/Consumer_fast_asexual.json:ontogeneticLinks/resourceLinks/item_0/links/item_0/profitability",
    numberOfInitialValues = 3,
    range = c(0.1, 0.9)
)



weaverOptimiser::run(overwrite = TRUE)
