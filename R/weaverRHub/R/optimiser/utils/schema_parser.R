library(jsonlite)

#' Recursive JSON Schema Analyzer for Weaver (Terminal Leaves)
extract_parameters_from_schema <- function(schema_list, current_path = character(0), file_origin = "", include_basic_strings = TRUE) {
  extracted_df <- data.frame(
    id = character(0), json_path = character(0), type = character(0),
    min = numeric(0), max = numeric(0), enum_values = character(0),
    schema_file = character(0), is_pattern = logical(0), stringsAsFactors = FALSE
  )
  if (!is.list(schema_list)) return(extracted_df)
  
  if ("properties" %in% names(schema_list) && is.list(schema_list$properties)) {
    props <- schema_list$properties
    for (prop_name in names(props)) {
      sub_schema <- props[[prop_name]]
      new_path <- c(current_path, prop_name)
      path_str <- paste(new_path, collapse = "$")
      if (length(new_path) == 1 && prop_name == "version") next
      
      node_type <- if ("type" %in% names(sub_schema)) sub_schema$type else "object"
      has_enum <- "enum" %in% names(sub_schema) && length(unlist(sub_schema$enum, use.names = FALSE)) > 0

      if (node_type %in% c("number", "integer", "boolean", "string", "array")) {
        min_val <- if ("minimum" %in% names(sub_schema)) as.numeric(sub_schema$minimum) else NA
        max_val <- if ("maximum" %in% names(sub_schema)) as.numeric(sub_schema$maximum) else NA
        enum_values <- if (has_enum) paste(as.character(unlist(sub_schema$enum, use.names = FALSE)), collapse = "|") else ""
        include_row <- !identical(node_type, "string") || has_enum || isTRUE(include_basic_strings)
        
        if (include_row) {
          row_df <- data.frame(
            id = "", json_path = path_str, type = node_type, min = min_val, max = max_val,
            enum_values = enum_values, schema_file = file_origin, is_pattern = FALSE, stringsAsFactors = FALSE
          )
          extracted_df <- rbind(extracted_df, row_df)
        }
      } else {
        if (is.list(sub_schema)) {
          sub_extracted <- extract_parameters_from_schema(sub_schema, new_path, file_origin, include_basic_strings)
          extracted_df <- rbind(extracted_df, sub_extracted)
        }
      }
    }
  }
  
  if ("patternProperties" %in% names(schema_list) && is.list(schema_list$patternProperties)) {
    patterns <- schema_list$patternProperties
    for (regex_str in names(patterns)) {
      sub_schema <- patterns[[regex_str]]
      enum_values <- if ("enum" %in% names(sub_schema) && length(unlist(sub_schema$enum, use.names = FALSE)) > 0) {
        as.character(unlist(sub_schema$enum, use.names = FALSE))
      } else { character(0) }
      
      if (length(enum_values) > 0) {
        for (enum_value in enum_values) {
          new_path <- c(current_path, enum_value)
          path_str <- paste(new_path, collapse = "$")
          param_type <- if ("type" %in% names(sub_schema)) sub_schema$type else "number"
          if (param_type != "object") {
            min_val <- if ("minimum" %in% names(sub_schema)) as.numeric(sub_schema$minimum) else NA
            max_val <- if ("maximum" %in% names(sub_schema)) as.numeric(sub_schema$maximum) else NA
            row_df <- data.frame(
              id = "", json_path = path_str, type = param_type, min = min_val, max = max_val,
              enum_values = paste(enum_values, collapse = "|"), schema_file = file_origin, is_pattern = TRUE, stringsAsFactors = FALSE
            )
            extracted_df <- rbind(extracted_df, row_df)
          }
        }
      }
      if (is.list(sub_schema)) {
        pattern_path <- c(current_path, paste0("[", regex_str, "]"))
        sub_extracted <- extract_parameters_from_schema(sub_schema, pattern_path, file_origin, include_basic_strings)
        if (nrow(sub_extracted) > 0) {
          sub_extracted$is_pattern <- TRUE
          extracted_df <- rbind(extracted_df, sub_extracted)
        }
      }
    }
  }
  
  if ("conditionals" %in% names(schema_list) && is.list(schema_list$conditionals)) {
    for (cond_block in schema_list$conditionals) {
      if ("then" %in% names(cond_block) && is.list(cond_block$then)) {
        for (then_action in cond_block$then) {
          if (is.list(then_action)) {
            sub_extracted <- extract_parameters_from_schema(then_action, current_path, file_origin, include_basic_strings)
            extracted_df <- rbind(extracted_df, sub_extracted)
          }
        }
      }
    }
  }
  
  if (nrow(extracted_df) > 0) {
    unique_key <- paste0(extracted_df$schema_file, "::", extracted_df$json_path)
    extracted_df <- extracted_df[!duplicated(unique_key), ]
    extracted_df$id <- paste0("sch_param_local_", seq_len(nrow(extracted_df)))
  }
  return(extracted_df)
}

#' Embedded Package Schema Loader Redirection Matrix
load_all_version_schemas <- function(version_str, include_basic_strings = FALSE) {
  # EMBEDDED SYSTEM REDIRECTION: Maps internally to the deployed R package assets structure
  schema_dir <- system.file("schema", version_str, package = "weaverRHub")
  
  if (!dir.exists(schema_dir) || version_str == "") {
    schema_dir <- system.file("schema", package = "weaverRHub")
    if (!dir.exists(schema_dir)) return(data.frame())
  }
  
  json_files <- list.files(schema_dir, pattern = "\\.json$", full.names = TRUE)
  if (length(json_files) == 0) return(data.frame())
  
  unified_catalog <- data.frame()
  for (file_path in json_files) {
    tryCatch({
      schema_data <- jsonlite::fromJSON(file_path, simplifyVector = FALSE)
      file_param_df <- extract_parameters_from_schema(
        schema_data, current_path = character(0), file_origin = basename(file_path),
        include_basic_strings = include_basic_strings
      )
      unified_catalog <- rbind(unified_catalog, file_param_df)
    }, error = function(e) {
      message("[Schema Parser Error] Failed to read: ", basename(file_path), " - ", e$message)
    })
  }
  if (nrow(unified_catalog) == 0) return(unified_catalog)
  
  stable_key <- paste0(unified_catalog$schema_file, "::", unified_catalog$json_path)
  unified_catalog <- unified_catalog[!duplicated(stable_key), , drop = FALSE]
  stable_id <- tolower(stable_key)
  stable_id <- gsub("[^a-z0-9]+", "_", stable_id)
  stable_id <- gsub("^_+|_+$", "", stable_id)
  unified_catalog$id <- make.unique(paste0("sch_param_", stable_id), sep = "_")
  
  return(unified_catalog)
}

resolve_config_destinations_by_schema <- function(schema_file, base_config_path) {
  schema_to_target <- list(
    simulation_params_schema.json = list(mode = "file", path = "simulation_params.json"),
    landscape_params_schema.json = list(mode = "file", path = "landscape_params.json"),
    species_schema.json = list(mode = "dir", path = "species"),
    resource_schema.json = list(mode = "dir", path = "resource"),
    resource_patch_schema.json = list(mode = "dir", path = "resource/patches"),
    obstacle_patch_schema.json = list(mode = "dir", path = "obstacle"),
    moisture_patch_schema.json = list(mode = "dir", path = "moisture"),
    habitat_domain_patch_schema.json = list(mode = "dir", path = "habitat_domain")
  )
  rule <- schema_to_target[[schema_file]]
  if (is.null(rule)) return(character(0))
  
  target_path <- file.path(base_config_path, rule$path)
  if (identical(rule$mode, "file")) {
    if (file.exists(target_path)) return(rule$path)
    return(character(0))
  }
  if (!dir.exists(target_path)) return(character(0))
  found <- list.files(target_path, pattern = "\\.json$", full.names = FALSE)
  if (length(found) == 0) return(character(0))
  file.path(rule$path, found)
}

filter_catalog_by_available_config_json <- function(df, base_config_path) {
  if (nrow(df) == 0) return(df)
  if (is.null(base_config_path) || base_config_path == "" || !dir.exists(base_config_path)) return(df[0, , drop = FALSE])
  
  available_by_schema <- lapply(unique(df$schema_file), function(schema_name) {
    resolve_config_destinations_by_schema(schema_name, base_config_path)
  })
  names(available_by_schema) <- unique(df$schema_file)
  
  keep_mask <- vapply(df$schema_file, function(schema_name) {
    length(available_by_schema[[schema_name]]) > 0
  }, logical(1))
  
  return(df[keep_mask, , drop = FALSE])
}