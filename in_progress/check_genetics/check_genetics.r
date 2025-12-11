
rm(list=ls(all=TRUE))

if (interactive()) {
    script_path <- sys.frame(1)$ofile
    script_path <- dirname(script_path)
} else {
    args <- commandArgs(trailingOnly = FALSE)
    script_path <- sub("--file=", "", args[grep("--file=", args)])
    script_path <- dirname(script_path)
}


library(plotly)




output_path = "C:\\Users\\mcs20\\Desktop\\EEZA\\Weaver\\output\\config_genetics__2025-6-23_10-48-58"

animal_species = "Cricotopus_spx"






traits_to_plot <- tools::file_path_sans_ext(list.files(file.path(output_path, "genetics", animal_species)))


data <- setNames(
    lapply(traits_to_plot, function(trait_name) {
        trait_data <- read.table(file.path(output_path, "genetics", animal_species, paste0(trait_name, ".txt")), header = TRUE, sep = "\t", stringsAsFactors = FALSE)
    
        if(any(duplicated(trait_data$id)))
        {
            stop("Error: Duplicate IDs found. Each ID must be unique.")
        }

        trait_data_by_generation <- split(trait_data, trait_data$g_numb_prt1)

        numberOfLociPerTrait <- as.integer(sub("^second_locus_(\\d+)_order$", "\\1", colnames(trait_data)[[length(colnames(trait_data))]]))

        info <- sapply(names(trait_data_by_generation), function(gen) {
            total_alleles = sum(sapply(seq_len(numberOfLociPerTrait), function(index) {
                alleles <- unique(c(
                    trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]][trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]] >= trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]]],
                    trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]][trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]] < trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]]]
                ))

                return(length(alleles))
            }))



            min_pseudovalue <- sum(sapply(seq_len(numberOfLociPerTrait), function(index) {

                return(min(c(
                    min(trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_value")]][trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]] >= trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]]]),
                    min(trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_value")]][trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]] < trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]]])
                )))
            }))


            max_pseudovalue <- sum(sapply(seq_len(numberOfLociPerTrait), function(index) {

                return(max(c(
                    max(trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_value")]][trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]] >= trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]]]),
                    max(trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_value")]][trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]] < trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]]])
                )))
            }))


            mean_pseudovalue <- sum(sapply(seq_len(numberOfLociPerTrait), function(index) {

                return(mean(c(
                    mean(trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_value")]][trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]] >= trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]]]),
                    mean(trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_value")]][trait_data_by_generation[[gen]][[paste0("first_locus_", index, "_order")]] < trait_data_by_generation[[gen]][[paste0("second_locus_", index, "_order")]]])
                )))
            }))



            return(list(
                individuals = nrow(trait_data_by_generation[[gen]]),
                total_alleles = total_alleles,
                min_pseudovalue = min_pseudovalue,
                max_pseudovalue = max_pseudovalue,
                mean_pseudovalue = mean_pseudovalue
            ))
        })

        return(info)
    }),
    traits_to_plot
)




colors <- rainbow(length(data)) 



allele_count_plot <- plot_ly()

for(i in seq_len(length(data))) {
    trait_name <- names(data)[[i]]
    
    allele_count_plot <- allele_count_plot %>% add_trace(
        x = as.integer(colnames(data[[trait_name]])),
        y = unlist(data[[trait_name]]["total_alleles", ]),
        type = 'scatter',
        mode = 'lines+markers',
        name = trait_name,
        line = list(color = colors[i]),
        marker = list(symbol = "o", color = colors[i])
    )
}

allele_count_plot <- allele_count_plot %>% layout(
    title = "Allele Count Over Generations",
    xaxis = list(
        title = "Generation",
        range = c(0, max(sapply(data, function(x) max(as.integer(colnames(x)))) + 1))
    ),
    yaxis = list(
        title = "Total Alleles",
        range = c(0, max(sapply(data, function(x) unlist(x["total_alleles", ]))) * 1.1)
    )
)

print(allele_count_plot)







pseudovalue_plot <- plot_ly()

for(i in seq_len(length(data))) {
    trait_name <- names(data)[[i]]
    
    pseudovalue_plot <- pseudovalue_plot %>% add_trace(
        x = as.integer(colnames(data[[trait_name]])),
        y = unlist(data[[trait_name]]["min_pseudovalue", ]),
        type = 'scatter',
        mode = 'lines+markers',
        name = paste0("Min ", trait_name),
        legendgroup = trait_name,
        line = list(color = colors[i]),
        marker = list(symbol = "o", color = colors[i])
    )

    pseudovalue_plot <- pseudovalue_plot %>% add_trace(
        x = as.integer(colnames(data[[trait_name]])),
        y = unlist(data[[trait_name]]["max_pseudovalue", ]),
        type = 'scatter',
        mode = 'lines+markers',
        name = paste0("Max ", trait_name),
        legendgroup = trait_name,
        showlegend = TRUE,
        line = list(color = colors[i]),
        marker = list(symbol = "o", color = colors[i])
    )

    pseudovalue_plot <- pseudovalue_plot %>% add_trace(
        x = as.integer(colnames(data[[trait_name]])),
        y = unlist(data[[trait_name]]["mean_pseudovalue", ]),
        type = 'scatter',
        mode = 'lines+markers',
        name = paste0("Mean ", trait_name),
        legendgroup = trait_name,
        showlegend = TRUE,
        line = list(color = colors[i]),
        marker = list(symbol = "o", color = colors[i])
    )
}

pseudovalue_plot <- pseudovalue_plot %>% layout(
    title = "Pseudovalue Over Generations",
    xaxis = list(
        title = "Generation",
        range = c(0, max(sapply(data, function(x) max(as.integer(colnames(x)))) + 1))
    ),
    yaxis = list(
        title = "Pseudovalue"
    )
)

print(pseudovalue_plot)
