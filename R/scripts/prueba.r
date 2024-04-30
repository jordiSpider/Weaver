
library(weaverParallel)

WeaverRootPath <- "/home/macarse/Escritorio/EEZA/Weaver"

baseConfigPath <- file.path(WeaverRootPath, "config/base_arthropods_temp")
modifications <- Modifications(list(
        "simulation_params.json:simulation/runDays" = IntegerVector(c(3, 4, 5, 6)),
        "simulation_params.json:simulation/recordEach" = IntegerVector(c(1, 2))#,
        #"simulation_params.json:simulation/growthAndReproTest" = LogicalVector(c(TRUE, FALSE)),
        #"simulation_params.json:simulation/encountersMatrixFilename" = CharacterVector(c("elem1", "elem2"))
    )
)
outputFolderPath = file.path(WeaverRootPath, "poblacion")


runWeaverParallel(WeaverRootPath=WeaverRootPath, numCores=4, baseConfigPath=baseConfigPath, modifications=modifications, outputFolderPath=outputFolderPath)


