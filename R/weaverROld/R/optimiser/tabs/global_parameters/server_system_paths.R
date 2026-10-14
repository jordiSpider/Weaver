# --- shinyFiles configuration (access to the system disks) ---
volumes <- getVolumes()()

# Apply the selective filter using regular expressions in the 'pattern' argument
shinyFileChoose(
  input, 
  "program_file", 
  roots = volumes, 
  session = session,
  pattern = "^.*\\.([eE][xX][eE])$" # This restricts the view and selection to .exe files only
)

shinyDirChoose(input, "base_config_dir", roots = volumes, session = session)
shinyDirChoose(input, "output_dir", roots = volumes, session = session)

# [PATH 1] Reactive capture of the user's manual typing (supports fluid handwriting)
observeEvent(input$program_text_path, {
  clean_path <- normalizePath(input$program_text_path, mustWork = FALSE)
  if (paths$program != clean_path) {
    paths$program <- clean_path
  }
}, ignoreInit = TRUE)

# [PATH 2] Reactive capture from the shinyFiles file browser
observeEvent(input$program_file, {
  req(!is.integer(input$program_file))
  file_info <- parseFilePaths(volumes, input$program_file)
  
  if (nrow(file_info) > 0) {
    selected_path <- normalizePath(file_info$datapath, mustWork = FALSE)
    paths$program <- selected_path
    
    # Sync the text box by updating its on-screen value
    updateTextInput(session, "program_text_path", value = selected_path)
  }
})


# --- SECTOR 2: Base Configuration ---
# Manual typing
observeEvent(input$base_config_text_path, {
  clean_path <- normalizePath(input$base_config_text_path, mustWork = FALSE)
  if (paths$base_config_path != clean_path) {
    paths$base_config_path <- clean_path
  }
}, ignoreInit = TRUE)

# File browser
observeEvent(input$base_config_dir, {
  req(!is.integer(input$base_config_dir))
  dir_path <- parseDirPath(volumes, input$base_config_dir)
  if (length(dir_path) > 0) {
    selected_path <- normalizePath(dir_path, mustWork = FALSE)
    paths$base_config_path <- selected_path
    updateTextInput(session, "base_config_text_path", value = selected_path)
  }
})


# --- SECTOR 3: Optimization Output ---
# Manual typing
observeEvent(input$output_text_path, {
  clean_path <- normalizePath(input$output_text_path, mustWork = FALSE)
  if (paths$output_path != clean_path) {
    paths$output_path <- clean_path
  }
}, ignoreInit = TRUE)

# File browser
observeEvent(input$output_dir, {
  req(!is.integer(input$output_dir))
  dir_path <- parseDirPath(volumes, input$output_dir)
  if (length(dir_path) > 0) {
    selected_path <- normalizePath(dir_path, mustWork = FALSE)
    paths$output_path <- selected_path
    updateTextInput(session, "output_text_path", value = selected_path)
  }
})


# =========================================================================
# REACTIVE EVALUATION OF THE WEAVER STRUCTURE
# =========================================================================
weaver_structure_status <- reactive({
  target_dir <- paths$base_config_path

  # If empty or the directory does not physically exist, skip internal structure validation
  if (target_dir == "" || !dir.exists(target_dir)) {
    return(list(valid = FALSE, missing = NULL))
  }

  # Elements required relative to the chosen root
  required_dirs <- c(
    "habitat_domain", 
    "moisture", 
    "obstacle", 
    "resource", 
    "resource/patches", 
    "species"
  )
  required_files <- c(
    "simulation_params.json", 
    "landscape_params.json"
  )
  
  required_species_csv <- c(
    "ontogeneticLinksPreference.csv",
    "ontogeneticLinksProfitability.csv"
  )
  
  missing_elements <- c()
  
  # 1. Validate subdirectories
  for (d in required_dirs) {
    full_path <- file.path(target_dir, d)
    if (!dir.exists(full_path)) {
      missing_elements <- c(missing_elements, paste0("Folder: ", d))
    }
  }

  # 2. Validate JSON files at the root
  for (f in required_files) {
    full_path <- file.path(target_dir, f)
    if (!file.exists(full_path)) {
      missing_elements <- c(missing_elements, paste0("File: ", f))
    }
  }

  # 3. Validate CSV files inside the 'species' subfolder (only if the species folder exists)
  if (!"Folder: species" %in% missing_elements) {
    for (csv_f in required_species_csv) {
      full_path <- file.path(target_dir, "species", csv_f)
      if (!file.exists(full_path)) {
        missing_elements <- c(missing_elements, paste0("File: species/", csv_f))
      }
    }
  }
  
  if (length(missing_elements) > 0) {
    return(list(valid = FALSE, missing = missing_elements))
  }
  
  return(list(valid = TRUE, missing = NULL))
})


# =========================================================================
# VISUAL RENDERING OF PATHS AND ERROR MESSAGES
# =========================================================================

# 1. Executable output (kept as-is for reference)
output$display_program_path <- renderUI({
  current_path <- paths$program
  if (current_path == "") {
    return(p(icon("triangle-exclamation"), " No file selected.", class = "text-danger small italic fw-bold"))
  }
  if (!grepl("\\.[eE][xX][eE]$", current_path)) {
    return(p(icon("circle-xmark"), " Error: The specified path does not point to a valid executable (.exe) file.", class = "text-danger small fw-bold text-break"))
  }
  return(p(tags$code(current_path), class = "text-muted small text-break"))
})

# 2. Base Configuration output (with existence and Weaver structure validation)
output$display_base_config_path <- renderUI({
  current_path <- paths$base_config_path

  if (current_path == "") {
    return(p(icon("triangle-exclamation"), " No directory selected.", class = "text-danger small italic fw-bold"))
  }

  # Check whether the directory exists
  if (!dir.exists(current_path)) {
    return(p(icon("circle-xmark"), " Error: The specified path does not point to an existing directory.", class = "text-danger small fw-bold text-break"))
  }

  # Check the internal Weaver structure
  struct <- weaver_structure_status()
  if (!struct$valid) {
    # Build a clean HTML list with the missing elements
    missing_list <- tags$ul(
      style = "margin-top: 5px; padding-left: 20px;",
      lapply(struct$missing, function(item) tags$li(item))
    )
    
    return(div(
      class = "text-danger small fw-bold text-break",
      p(icon("circle-xmark"), " Error: The directory does not follow the required Weaver configuration format. Missing elements:"),
      missing_list
    ))
  }
  
  return(p(tags$code(current_path), class = "text-muted small text-break"))
})

# 3. Optimization Output path (with existence validation)
output$display_output_path <- renderUI({
  current_path <- paths$output_path

  if (current_path == "") {
    return(p(icon("triangle-exclamation"), " No directory selected.", class = "text-danger small italic fw-bold"))
  }

  # Check whether the directory exists
  if (!dir.exists(current_path)) {
    return(p(icon("circle-xmark"), " Error: The specified path does not point to an existing directory.", class = "text-danger small fw-bold text-break"))
  }
  
  return(p(tags$code(current_path), class = "text-muted small text-break"))
})


# =========================================================================
# RED BORDER LOGIC (shinyjs)
# =========================================================================
observe({
  # Executable is invalid if empty or not a .exe
  shinyjs::toggleClass(
    id = "program_text_path",
    class = "is-invalid",
    condition = (paths$program == "" || !grepl("\\.[eE][xX][eE]$", paths$program))
  )

  # Configuration is invalid if empty, missing, or lacking the Weaver structure
  struct <- weaver_structure_status()
  shinyjs::toggleClass(
    id = "base_config_text_path",
    class = "is-invalid",
    condition = (paths$base_config_path == "" || !dir.exists(paths$base_config_path) || !struct$valid)
  )

  # Output is invalid if empty or the directory does not exist
  shinyjs::toggleClass(
    id = "output_text_path",
    class = "is-invalid",
    condition = (paths$output_path == "" || !dir.exists(paths$output_path))
  )
})


# --- Real Schema Version Extraction Logic ---
schema_version <- reactive({
  current_path <- paths$program

  # [VALIDATION FILTER] If empty or NOT ending in .exe, abort immediately
  if (current_path == "" || !grepl("\\.[eE][xX][eE]$", current_path)) {
    return(NULL)
  }

  # If it passes the filter but the file does not physically exist on disk, also abort
  if (!file.exists(current_path)) {
    return("Error: Simulator executable not found at specified path.")
  }
  
  tryCatch({
    res <- system2(
      command = current_path, 
      args = c("--schema_version"), 
      stdout = TRUE, 
      stderr = TRUE,
      timeout = 5
    )
    
    version_str <- paste(res, collapse = " ")
    
    if (grepl("Schema version:", version_str)) {
      clean_version <- sub(".*Schema version:\\s*", "", version_str)
      return(clean_version)
    }
    
    if(version_str == "") version_str <- "Unknown Format Output"
    return(version_str)
    
  }, error = function(e) {
    return("Error executing --schema_version command.")
  })
})

# Render the Schema status block with HTML formatting
output$schema_version_status <- renderUI({
  ver <- schema_version()
  if (is.null(ver)) return(NULL)
  
  if (grepl("Error", ver)) {
    p(icon("triangle-exclamation"), paste(" Schema:", ver), class = "text-danger small mt-1")
  } else {
    p(icon("circle-check"), HTML(paste(" <b>Schema Version:</b>", tags$span(class="badge bg-success", ver))), class = "text-success small mt-1")
  }
})

# --- Sidebar Execution State Mockup ---
output$txt_status <- renderPrint({
  cat("System Status: Ready to configure.\n\n")
  cat("Current Executable Path:\n", ifelse(paths$program == "", "[Empty]", paths$program))
})