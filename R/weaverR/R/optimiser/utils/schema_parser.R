library(jsonlite)

#' Recursive JSON Schema Analyzer for Weaver (Terminal Leaves)
#' 
#' @param schema_list Nested list coming from jsonlite::fromJSON.
#' @param current_path Character vector with the history of visited nodes.
#' @param file_origin Name of the JSON file being processed.
#' @param include_basic_strings If FALSE, excludes strings without an enum from the catalog.
#' @return A data.frame with the optimizable leaves of the system (excludes branch objects).
extract_parameters_from_schema <- function(schema_list, current_path = character(0), file_origin = "", include_basic_strings = TRUE) {
  
  # Structure of the return container
  extracted_df <- data.frame(
    id = character(0),
    json_path = character(0),
    type = character(0),
    min = numeric(0),
    max = numeric(0),
    enum_values = character(0),
    schema_file = character(0),
    is_pattern = logical(0),
    stringsAsFactors = FALSE
  )
  
  if (!is.list(schema_list)) return(extracted_df)
  
  # -------------------------------------------------------------------------
  # 1. PROCESS STANDARD PROPERTIES (properties)
  # -------------------------------------------------------------------------
  if ("properties" %in% names(schema_list) && is.list(schema_list$properties)) {
    props <- schema_list$properties
    for (prop_name in names(props)) {
      sub_schema <- props[[prop_name]]
      new_path <- c(current_path, prop_name)
      path_str <- paste(new_path, collapse = "$")
      
      # Skip the global version informational field
      if (length(new_path) == 1 && prop_name == "version") next
      
      # Determine the data type assigned by the Schema (defaults to object if not declared)
      node_type <- if ("type" %in% names(sub_schema)) sub_schema$type else "object"
      has_enum <- "enum" %in% names(sub_schema) && length(unlist(sub_schema$enum, use.names = FALSE)) > 0

      # FILTERING CRITERIA: If it is a primitive leaf type, it is saved to the list.
      # "object" is explicitly excluded since it is considered a structural branch node.
      if (node_type %in% c("number", "integer", "boolean", "string", "array")) {

        min_val <- if ("minimum" %in% names(sub_schema)) as.numeric(sub_schema$minimum) else NA
        max_val <- if ("maximum" %in% names(sub_schema)) as.numeric(sub_schema$maximum) else NA
        enum_values <- if (has_enum) paste(as.character(unlist(sub_schema$enum, use.names = FALSE)), collapse = "|") else ""

        include_row <- !identical(node_type, "string") || has_enum || isTRUE(include_basic_strings)
        if (include_row) {
          row_df <- data.frame(
            id = "",
            json_path = path_str,
            type = node_type,
            min = min_val,
            max = max_val,
            enum_values = enum_values,
            schema_file = file_origin,
            is_pattern = FALSE,
            stringsAsFactors = FALSE
          )
          extracted_df <- rbind(extracted_df, row_df)
        }

      } else {
        # If it is an intermediate (branch) object, keep descending recursively through its children
        if (is.list(sub_schema)) {
          sub_extracted <- extract_parameters_from_schema(sub_schema, new_path, file_origin, include_basic_strings)
          extracted_df <- rbind(extracted_df, sub_extracted)
        }
      }
    }
  }
  
  # -------------------------------------------------------------------------
  # 2. PROCESS DYNAMIC PROPERTIES (patternProperties)
  # -------------------------------------------------------------------------
  if ("patternProperties" %in% names(schema_list) && is.list(schema_list$patternProperties)) {
    patterns <- schema_list$patternProperties
    for (regex_str in names(patterns)) {
      sub_schema <- patterns[[regex_str]]
      
      # Scenario A: the pattern directly defines a list of enums (e.g.: dellsTraits or nonTemperatureTraits)
      enum_values <- if ("enum" %in% names(sub_schema) && length(unlist(sub_schema$enum, use.names = FALSE)) > 0) {
        as.character(unlist(sub_schema$enum, use.names = FALSE))
      } else {
        character(0)
      }
      if (length(enum_values) > 0) {
        for (enum_value in enum_values) {
          new_path <- c(current_path, enum_value)
          path_str <- paste(new_path, collapse = "$")

          param_type <- if ("type" %in% names(sub_schema)) sub_schema$type else "number"

          # Protection: only save if the resulting enum leaf is not declared as a branch object
          if (param_type != "object") {
            min_val <- if ("minimum" %in% names(sub_schema)) as.numeric(sub_schema$minimum) else NA
            max_val <- if ("maximum" %in% names(sub_schema)) as.numeric(sub_schema$maximum) else NA

            row_df <- data.frame(
              id = "",
              json_path = path_str,
              type = param_type,
              min = min_val,
              max = max_val,
              enum_values = paste(enum_values, collapse = "|"),
              schema_file = file_origin,
              is_pattern = TRUE,
              stringsAsFactors = FALSE
            )
            extracted_df <- rbind(extracted_df, row_df)
          }
        }
      }
      
      # Scenario B: the pattern hosts a large/complex nested sub-object.
      # We descend through the tree simulating the pattern token in the visual navigation path.
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
  
  # -------------------------------------------------------------------------
  # 3. PROCESS INTERNAL CONDITIONAL BLOCKS (conditionals)
  # -------------------------------------------------------------------------
  if ("conditionals" %in% names(schema_list) && is.list(schema_list$conditionals)) {
    for (cond_block in schema_list$conditionals) {
      if ("then" %in% names(cond_block) && is.list(cond_block$then)) {
        for (then_action in cond_block$then) {
          if (is.list(then_action)) {
            # Embedded structures within the "then" actions are analyzed
            sub_extracted <- extract_parameters_from_schema(then_action, current_path, file_origin, include_basic_strings)
            extracted_df <- rbind(extracted_df, sub_extracted)
          }
        }
      }
    }
  }
  
  # -------------------------------------------------------------------------
  # 4. CONSOLIDATION, CLEANUP AND UNIQUE ID ASSIGNMENT
  # -------------------------------------------------------------------------
  if (nrow(extracted_df) > 0) {
    # Remove duplicates within the schema by pair (schema_file + json_path)
    unique_key <- paste0(extracted_df$schema_file, "::", extracted_df$json_path)
    extracted_df <- extracted_df[!duplicated(unique_key), ]

    # Temporary local IDs (rewritten globally when combining all schemas)
    extracted_df$id <- paste0("sch_param_local_", seq_len(nrow(extracted_df)))
  }

  return(extracted_df)
}

#' Bulk loading of JSON schemas parameterized by environment folders
#' 
#' @param version_str Identifier of the version subfolder (e.g.: "2026.08.05").
#' @param include_basic_strings If FALSE, excludes strings without an enum from the catalog.
#' @return An integrated data.frame ready to feed the Tab 2 tables.
load_all_version_schemas <- function(version_str, include_basic_strings = TRUE) {
  base_schema_path <- Sys.getenv("SCHEMA_DIR")
  
  if (base_schema_path == "") {
    base_schema_path <- file.path(getwd(), "schema")
  }

  schema_dir <- file.path(base_schema_path, version_str)
  
  # Automatic redirection if the exact version subfolder is not found
  if (!dir.exists(schema_dir)) {
    schema_dir <- base_schema_path
    if (!dir.exists(schema_dir)) return(data.frame())
  }
  
  json_files <- list.files(schema_dir, pattern = "\\.json$", full.names = TRUE)
  if (length(json_files) == 0) return(data.frame())
  
  unified_catalog <- data.frame()

  for (file_path in json_files) {
    tryCatch({
      schema_data <- fromJSON(file_path, simplifyVector = FALSE)
      file_param_df <- extract_parameters_from_schema(
        schema_data,
        current_path = character(0),
        file_origin = basename(file_path),
        include_basic_strings = include_basic_strings
      )
      unified_catalog <- rbind(unified_catalog, file_param_df)
    }, error = function(e) {
      cat("Error parsing schema file:", file_path, "\nMessage:", e$message, "\n")
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