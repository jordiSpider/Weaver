#' Recursive JSON Schema Analyzer for Weaver (Terminal Leaves)
format_schema_json_path <- function(path_segments) {
  if (length(path_segments) == 0) return("")

  path <- path_segments[[1]]
  if (length(path_segments) > 1) {
    for (segment in path_segments[-1]) {
      if (startsWith(segment, "[[")) {
        path <- paste0(path, segment)
      } else {
        path <- paste0(path, "$", segment)
      }
    }
  }
  path
}

extract_parameters_from_schema <- function(schema_list, current_path = character(0), file_origin = "", include_basic_strings = TRUE, is_array_item = FALSE) {
  extracted_df <- data.frame(
    id = character(0), json_path = character(0), type = character(0),
    min = numeric(0), max = numeric(0), enum_values = character(0),
    schema_file = character(0), is_pattern = logical(0), stringsAsFactors = FALSE
  )
  if (!is.list(schema_list)) return(extracted_df)

  if (isTRUE(is_array_item) && length(current_path) > 0) {
    item_type <- if ("type" %in% names(schema_list)) schema_list$type else "object"
    if (item_type %in% c("number", "integer", "boolean", "string", "array")) {
      item_enum <- "enum" %in% names(schema_list) && length(unlist(schema_list$enum, use.names = FALSE)) > 0
      include_item <- !identical(item_type, "string") || item_enum || isTRUE(include_basic_strings)
      if (include_item) {
        item_row <- data.frame(
          id = "", json_path = format_schema_json_path(current_path), type = item_type,
          min = if ("minimum" %in% names(schema_list)) as.numeric(schema_list$minimum) else NA,
          max = if ("maximum" %in% names(schema_list)) as.numeric(schema_list$maximum) else NA,
          enum_values = if (item_enum) paste(as.character(unlist(schema_list$enum, use.names = FALSE)), collapse = "|") else "",
          schema_file = file_origin, is_pattern = FALSE, stringsAsFactors = FALSE
        )
        extracted_df <- rbind(extracted_df, item_row)
      }

      if (identical(item_type, "array") && is.list(schema_list$items)) {
        nested_items <- extract_parameters_from_schema(
          schema_list$items,
          current_path = c(current_path, "[[*]]"),
          file_origin = file_origin,
          include_basic_strings = include_basic_strings,
          is_array_item = TRUE
        )
        extracted_df <- rbind(extracted_df, nested_items)
      }
    }
  }
  
  if ("properties" %in% names(schema_list) && is.list(schema_list$properties)) {
    props <- schema_list$properties
    for (prop_name in names(props)) {
      sub_schema <- props[[prop_name]]
      new_path <- c(current_path, prop_name)
      path_str <- format_schema_json_path(new_path)
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

        # Keep array-level destinations available and also expose each item
        # schema beneath an R-style one-based index placeholder.
        if (identical(node_type, "array") && is.list(sub_schema$items)) {
          item_extracted <- extract_parameters_from_schema(
            sub_schema$items,
            current_path = c(new_path, "[[*]]"),
            file_origin = file_origin,
            include_basic_strings = include_basic_strings,
            is_array_item = TRUE
          )
          extracted_df <- rbind(extracted_df, item_extracted)
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
        # An enum inside patternProperties defines the concrete property names
        # represented by this pattern group. Expand each name before traversing
        # the group's schema so descendants receive concrete JSON paths.
        for (enum_value in enum_values) {
          new_path <- c(current_path, enum_value)
          path_str <- format_schema_json_path(new_path)
          param_type <- if ("type" %in% names(sub_schema)) sub_schema$type else "object"

          if (param_type %in% c("number", "integer", "boolean", "string", "array")) {
            min_val <- if ("minimum" %in% names(sub_schema)) as.numeric(sub_schema$minimum) else NA
            max_val <- if ("maximum" %in% names(sub_schema)) as.numeric(sub_schema$maximum) else NA
            has_value_enum <- "enum" %in% names(sub_schema) && length(unlist(sub_schema$enum, use.names = FALSE)) > 0
            include_row <- !identical(param_type, "string") || has_value_enum || isTRUE(include_basic_strings)

            if (!include_row) next

            row_df <- data.frame(
              id = "", json_path = path_str, type = param_type, min = min_val, max = max_val,
              enum_values = if (has_value_enum) paste(as.character(unlist(sub_schema$enum, use.names = FALSE)), collapse = "|") else "",
              schema_file = file_origin, is_pattern = TRUE, stringsAsFactors = FALSE
            )
            extracted_df <- rbind(extracted_df, row_df)
          } else if (is.list(sub_schema)) {
            sub_extracted <- extract_parameters_from_schema(
              sub_schema,
              current_path = new_path,
              file_origin = file_origin,
              include_basic_strings = include_basic_strings
            )
            if (nrow(sub_extracted) > 0) {
              sub_extracted$is_pattern <- TRUE
              extracted_df <- rbind(extracted_df, sub_extracted)
            }
          }
        }
      } else if (is.list(sub_schema)) {
        # Preserve a generic path for pattern groups that do not declare their
        # concrete property names through an enum.
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

#' Map optimiser parameter classes to compatible schema leaf types
schema_type_mapping_for_parameter_class <- function(parameter_class) {
  mapping <- list(
    RealValued = list(types = "number", requires_enum = FALSE),
    Integer = list(types = "integer", requires_enum = FALSE),
    Boolean = list(types = "boolean", requires_enum = FALSE),
    Permutation = list(types = "string", requires_enum = TRUE),
    Abundances = list(types = "array", requires_enum = FALSE)
  )
  mapping[[as.character(parameter_class)]]
}

#' Keep only schema destinations compatible with an optimiser parameter class
filter_schema_catalog_by_parameter_class <- function(catalog, parameter_class) {
  mapping <- schema_type_mapping_for_parameter_class(parameter_class)
  if (is.null(catalog)) return(data.frame())
  if (is.null(mapping) || nrow(catalog) == 0L) {
    return(catalog[0, , drop = FALSE])
  }

  keep <- catalog$type %in% mapping$types
  if (isTRUE(mapping$requires_enum)) {
    keep <- keep & !is.na(catalog$enum_values) & nzchar(catalog$enum_values)
  }
  catalog[keep, , drop = FALSE]
}

#' Expand array-item placeholders into concrete, one-based R JSON paths
expand_schema_paths_for_config <- function(json_paths, config_file) {
  json_paths <- unique(as.character(json_paths))
  paths_requiring_expansion <- grepl("\\[\\[\\*\\]\\]", json_paths)
  if (!any(paths_requiring_expansion)) return(json_paths)

  config_data <- tryCatch(
    jsonlite::fromJSON(config_file, simplifyVector = FALSE),
    error = function(e) NULL
  )
  if (is.null(config_data)) return(json_paths[!paths_requiring_expansion])

  expand_one_path <- function(json_path) {
    segments <- strsplit(json_path, "$", fixed = TRUE)[[1]]
    states <- list(list(value = config_data, path = ""))

    for (segment in segments) {
      wildcard_matches <- gregexpr("\\[\\[\\*\\]\\]", segment, perl = TRUE)[[1]]
      wildcard_count <- if (wildcard_matches[[1]] < 0) 0L else length(wildcard_matches)
      property_name <- sub("\\[\\[\\*\\]\\].*$", "", segment)
      next_states <- list()

      for (state in states) {
        current_value <- state$value
        current_path <- state$path

        if (nzchar(property_name)) {
          if (!is.list(current_value) || is.null(names(current_value)) || !(property_name %in% names(current_value))) next
          current_value <- current_value[[property_name]]
          current_path <- if (nzchar(current_path)) paste0(current_path, "$", property_name) else property_name
        }

        if (wildcard_count == 0L) {
          next_states[[length(next_states) + 1L]] <- list(value = current_value, path = current_path)
          next
        }

        branch_states <- list(list(value = current_value, path = current_path))
        for (wildcard_index in seq_len(wildcard_count)) {
          expanded_branches <- list()
          for (branch in branch_states) {
            if (is.null(branch$value) || length(branch$value) == 0L) next
            for (index in seq_along(branch$value)) {
              expanded_branches[[length(expanded_branches) + 1L]] <- list(
                value = branch$value[[index]],
                path = paste0(branch$path, "[[", index, "]]")
              )
            }
          }
          branch_states <- expanded_branches
          if (length(branch_states) == 0L) break
        }
        next_states <- c(next_states, branch_states)
      }

      states <- next_states
      if (length(states) == 0L) return(character(0))
    }

    vapply(states, `[[`, character(1), "path")
  }

  expanded_paths <- unlist(lapply(json_paths, function(path) {
    if (grepl("\\[\\[\\*\\]\\]", path)) expand_one_path(path) else path
  }), use.names = FALSE)
  unique(expanded_paths)
}

#' Embedded Package Schema Loader Redirection Matrix
load_all_version_schemas <- function(version_str, include_basic_strings = FALSE) {
  # EMBEDDED SYSTEM REDIRECTION: Maps internally to the deployed R package assets structure
  schema_dir <- system.file("schema", version_str, package = "weaverR")
  
  if (!dir.exists(schema_dir) || version_str == "") {
    schema_dir <- system.file("schema", package = "weaverR")
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

normalize_config_relative_path <- function(path) {
  gsub("\\\\", "/", as.character(path))
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
    if (file.exists(target_path)) return(normalize_config_relative_path(rule$path))
    return(character(0))
  }
  if (!dir.exists(target_path)) return(character(0))
  found <- list.files(target_path, pattern = "\\.json$", full.names = FALSE)
  if (length(found) == 0) return(character(0))
  normalize_config_relative_path(file.path(rule$path, found))
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
