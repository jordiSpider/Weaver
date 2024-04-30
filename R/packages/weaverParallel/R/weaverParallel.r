
#######################
# IntegerVectorClass

setClass("IntegerVectorClass", representation(values="vector"))

setMethod("toString", signature(x = "IntegerVectorClass"), function(x) {
        values <- as.integer(x@values)

        string <- paste("[", values[[1]], sep="")

        if(length(values) > 1) {
            for(index in 2:length(values)) {
                string <- paste(string, values[[index]], sep=", ")
            }
        }

        string <- paste(string, "]", sep="")

        return(string)
    }
)

IntegerVector <- function(values) {
    return(new("IntegerVectorClass", values = values))
}


######################
# DoubleVectorClass

setClass("DoubleVectorClass", representation(values="vector"))

setMethod("toString", signature(x = "DoubleVectorClass"), function(x) {
        values <- x@values
        
        string <- paste("[", format(values[[1]], nsmall = 1), sep="")

        if(length(values) > 1) {
            for(index in 2:length(values)) {
                string <- paste(string, format(values[[index]], nsmall = 1), sep=", ")
            }
        }

        string <- paste(string, "]", sep="")

        return(string)
    }
)

DoubleVector <- function(values) {
    return(new("DoubleVectorClass", values = values))
}


######################
# LogicalVectorClass

setClass("LogicalVectorClass", representation(values="vector"))

setMethod("toString", signature(x = "LogicalVectorClass"), function(x) {
        values <- x@values

        if(values[[1]] == TRUE) {
            string <- paste("[", "True", sep="")
        }
        else {
            string <- paste("[", "False", sep="")
        }

        if(length(values) > 1) {
            for(index in 2:length(values)) {
                if(values[[index]] == TRUE) {
                    string <- paste(string, "True", sep=", ")
                }
                else {
                    string <- paste(string, "False", sep=", ")
                }
            }
        }

        string <- paste(string, "]", sep="")

        return(string)
    }
)

LogicalVector <- function(values) {
    return(new("LogicalVectorClass", values = values))
}


#######################
# CharacterVectorClass

setClass("CharacterVectorClass", representation(values="vector"))

setMethod("toString", signature(x = "CharacterVectorClass"), function(x) {
        values <- x@values

        string <- paste("[", "'", values[[1]], "'", sep="")

        if(length(values) > 1) {
            for(index in 2:length(values)) {
                elem <- paste("'", values[[index]], "'", sep="")
                string <- paste(string, elem, sep=", ")
            }
        }

        string <- paste(string, "]", sep="")

        return(string)
    }
)

CharacterVector <- function(values) {
    return(new("CharacterVectorClass", values = values))
}


#######################
# ModificationsClass

setClass("ModificationsClass", representation(values="vector"))

setMethod("toString", signature(x = "ModificationsClass"), function(x) {
        values <- x@values

        keysList <- names(values)

        string <- paste("\"{'", keysList[[1]], "': ", toString(values[[keysList[[1]]]]), sep="")

        if(length(keysList) > 1) {
            for(index in 2:length(keysList)) {
                elem <- paste("'", keysList[[index]], "': ", toString(values[[keysList[[index]]]]), sep="")
                string <- paste(string, elem, sep=", ")
            }
        }

        string <- paste(string, "}\"", sep="")

        return(string)
    }
)

Modifications <- function(values) {
    return(new("ModificationsClass", values = values))
}






createConfigCombinations<-function(WeaverRootPath, baseConfigPath, modifications, configCombinationsOutputFolderPath) {
    pythonScript <- file.path(WeaverRootPath, 'scripts/configs/createConfigCombinations.py')

    command<-paste('python', pythonScript, baseConfigPath, toString(modifications), configCombinationsOutputFolderPath)
    system(command,wait=TRUE)
}

callWeaver<-function(configPatch) {
    if(Sys.info()["sysname"] == "Windows") {
        executable <- file.path(WeaverRootPath, 'Weaver.exe')
    }
    else {
        executable <- file.path(WeaverRootPath, 'Weaver.out')
    }

    command<-paste(executable, '--silent', '-I', configPatch, '-O', outputFolderPath)
    system(command,wait=TRUE)
}


runWeaverParallel <- function(WeaverRootPath, numCores, baseConfigPath, modifications, outputFolderPath) {
    library(parallel)

    if(!file.exists(outputFolderPath)) {
        dir.create(outputFolderPath, recursive = TRUE)
    }

    configCombinationsOutputFolderPath <- file.path(outputFolderPath, ".tmp_config_combinations")

    cl <- makeCluster(numCores)

    clusterExport(cl, c("WeaverRootPath", "outputFolderPath"))

    createConfigCombinations(WeaverRootPath, baseConfigPath=baseConfigPath, modifications=modifications, configCombinationsOutputFolderPath=configCombinationsOutputFolderPath)

    configPathsVector <- list.dirs(path=configCombinationsOutputFolderPath, full.names=TRUE, recursive=FALSE)

    r <- clusterApply(cl, configPathsVector, callWeaver)

    unlink(configCombinationsOutputFolderPath, recursive=TRUE)

    stopCluster(cl)
}
