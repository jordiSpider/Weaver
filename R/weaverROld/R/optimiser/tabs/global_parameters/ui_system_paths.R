ui_system_paths_card <- card(
  card_header(tags$b("System Environment Paths")),
  card_body(
    # 1. Selector for the Simulator Executable (fixed input in the UI so focus is not lost)
    span(tags$b("Simulator Executable Path:")),
    div(class = "my-2",
        # The text input spans the full width and never misaligns
        textInput(
          inputId = "program_text_path", 
          label = NULL, 
          value = "", 
          placeholder = "Enter absolute or relative path to the executable...", 
          width = "100%"
        ),
        # The button sits right below, cleanly spanning 100% of the width
        div(class = "mt-2",
            shinyFilesButton("program_file", "Browse...", 
                             title = "Select Simulator Executable (WeaverCLI.exe)", 
                             multiple = FALSE, class = "btn-outline-primary w-100")
        )
    ),
    
    # Schema version display
    htmlOutput("schema_version_status"),

    hr(),
    
    # 2. Selector for the Base Configuration Directory
    span(tags$b("Base Configuration Directory:")),
    div(class = "my-2",
        textInput(
          inputId = "base_config_text_path", 
          label = NULL,
          value = "",
          placeholder = "Enter absolute or relative path to base configuration...",
          width = "100%"
        ),
        div(class = "mt-2",
            shinyDirButton("base_config_dir", "Browse Directory...", 
                           title = "Select Base Configuration Directory", 
                           class = "btn-outline-primary w-100")
        )
    ),

    hr(),
    
    # 3. Selector for the Optimization Output Directory
    span(tags$b("Optimization Output Directory:")),
    div(class = "my-2",
        textInput(
          inputId = "output_text_path", 
          label = NULL,
          value = "",
          placeholder = "Enter absolute or relative path to output directory...",
          width = "100%"
        ),
        div(class = "mt-2",
            shinyDirButton("output_dir", "Browse Directory...", 
                           title = "Select Output Directory", 
                           class = "btn-outline-primary w-100")
        )
    )
  )
)