library(shiny)
library(DT)
library(shinyTree)

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
  if (base_config_path == "" || !dir.exists(base_config_path)) return(df[0, , drop = FALSE])

  available_by_schema <- lapply(unique(df$schema_file), function(schema_name) {
    resolve_config_destinations_by_schema(schema_name, base_config_path)
  })
  names(available_by_schema) <- unique(df$schema_file)

  keep_mask <- vapply(df$schema_file, function(schema_name) {
    length(available_by_schema[[schema_name]]) > 0
  }, logical(1))

  df[keep_mask, , drop = FALSE]
}

# --- Reactive Object to Store the Parameter Catalog ---
CURRENT_SCHEMA_CATALOG <- reactive({
  req(schema_version())
  ver <- schema_version()
  df <- load_all_version_schemas(
    ver,
    include_basic_strings = FALSE
  )

  filter_catalog_by_available_config_json(df, paths$base_config_path)
})

# --- Helper to Build the Nested R Tree ---
build_nested_list_tree <- function(df) {
  tree <- list()

  # 1. NEW: Explicitly inject the required CSV files at the root of the tree
  # Assign native 'shinyTree' attributes so they are selectable checkboxes
  tree[["ontogeneticLinksPreference.csv"]] <- structure(
    "CSV Link File", 
    stselected = FALSE, 
    stid = "csv_param_ontogenetic_links_preference"
  )
  tree[["ontogeneticLinksProfitability.csv"]] <- structure(
    "CSV Link File", 
    stselected = FALSE, 
    stid = "csv_param_ontogenetic_links_profitability"
  )
  
  # 2. Iteratively map all JSON parameters coming from the corporate Schema
  for (i in 1:nrow(df)) {
    parts <- unlist(strsplit(df$json_path[i], "\\$"))
    parts <- c(df$schema_file[i], parts)
    
    eval_str <- "tree"
    for (p in parts) {
      p_clean <- gsub("'", "\\'", p, fixed = TRUE)
      eval_str <- paste0(eval_str, "[['", p_clean, "']]")
    }
    eval(parse(text = paste0(eval_str, " <- structure('', stselected = FALSE, stid = '", df$id[i], "')")))
  }
  
  return(tree)
}

# --- Hierarchical Tree Rendering ---
output$tree_parameters <- renderTree({
  selection_reset_nonce()
  catalog <- CURRENT_SCHEMA_CATALOG()
  req(nrow(catalog) > 0)
  build_nested_list_tree(catalog)
})

observe({
  catalog <- CURRENT_SCHEMA_CATALOG()
  
  # NEW: Add the CSV IDs to the list of valid identifiers so they persist
  valid_ids <- c(
    catalog$id, 
    "csv_param_ontogenetic_links_preference", 
    "csv_param_ontogenetic_links_profitability"
  )
  
  current_ids <- active_selected_ids()

  if (is.null(current_ids) || length(current_ids) == 0) return()

  # Preserve the current selection order and remove invalid IDs
  active_selected_ids(current_ids[current_ids %in% valid_ids])
})

# --- Helper to Extract Selected IDs while Preserving Tree Attributes ---
collect_selected_ids <- function(node) {
  ids <- character(0)

  if (!is.null(attr(node, "stselected")) && isTRUE(attr(node, "stselected"))) {
    node_id <- attr(node, "stid")
    if (!is.null(node_id) && nzchar(node_id)) {
      ids <- c(ids, node_id)
    }
  }

  if (is.list(node) && length(node) > 0) {
    for (child in node) {
      ids <- c(ids, collect_selected_ids(child))
    }
  }

  ids
}

# --- Reactive and Instant Tree Synchronization Logic ---
observe({
  # Direct reactive dependency on the UI tree
  tree_input <- input$tree_parameters

  # If the tree is empty or nothing has been selected, clear the state immediately
  if (is.null(tree_input) || length(tree_input) == 0) {
    active_selected_ids(character(0))
    return()
  }

  extracted_ids <- unique(collect_selected_ids(tree_input))

  if (length(extracted_ids) == 0) {
    active_selected_ids(character(0))
    return()
  }

  # Update the vector with unique values (added or removed in real time)
  active_selected_ids(extracted_ids)
})