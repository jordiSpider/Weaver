library(future)
library(shinyWidgets)
library(shinyjqui)
library(codetools)
library(shinyjs)


# Repository of predefined objectives
predefined_objectives <- list(
  "totalTime" = "Total simulation time tracking from breakConditionsInfo",
  "minAbundance" = "Minimum global abundance calculation from extendedDailySummary",
  "coefficientOfVariation" = "Coefficient of variation evaluation from breakConditionsInfo"
)

# Reactive control structure
inserted_objectives <- reactiveValues(
  ids = c(), 
  names_map = list(),
  code_errors = list()
)

# Helper function to render an independent objective sub-card
render_single_objective_ui <- function(id) {
  tags$div(
    id = paste0("objective_card_container_", id),
    class = "mb-3 border secondary-subtle shadow-sm card",
    style = "padding: 12px;",
    
    div(class = "d-flex justify-content-between align-items-center mb-2 pb-2 border-bottom",
        tags$span(
          id = paste0("objective_title_display_", id), 
          class = "fw-bold text-primary", 
          tags$code(paste0("objective_", id))
        ),
        # The button starts visible by default; the observer will control its state
        actionButton(paste0("remove_obj_", id)
    ),
    div(class = "row g-2 mb-2",
        div(class = "col-md-6",
            textInput(paste0("obj_name_", id), label = "Objective Name:", value = paste0("objective_", id))
        ),
        div(class = "col-md-6",
            selectInput(paste0("obj_criteria_", id), label = "Criteria:", choices = c("Maximize" = "MAX", "Minimize" = "MIN"))
        )
    ),
    selectInput(paste0("obj_source_", id), label = "Function Source:", choices = c("Predefined System Objective" = "predefined", "Custom R Code Block" = "custom")),
    uiOutput(paste0("obj_body_conditional_ui_", id)),
    
    uiOutput(paste0("obj_code_error_display_", id)),
    
    # Listener scripts for the Blur and Input events
    tags$script(HTML(paste0("
      // Instant visual update of the header
      $('#obj_name_", id, "').on('input', function() {
        var currentText = $.trim($(this).val());
        var displayPlaceholder = currentText === '' ? '(Blank Name)' : currentText;
        $('#objective_title_display_", id, "').html('<code>' + displayPlaceholder + '</code>');
      });

      // Notify the name change on blur
      $('#obj_name_", id, "').on('blur', function() {
        var cleanName = $.trim($(this).val());
        Shiny.setInputValue('blur_obj_name_change', {id: ", id, ", name: cleanName}, {priority: 'event'});
      });

      // Notify the code edit on blur
      $(document).on('blur', '#obj_custom_code_", id, "', function() {
        var rawCode = $(this).val();
        Shiny.setInputValue('blur_obj_code_change', {id: ", id, ", code: rawCode}, {priority: 'event'});
      });
    ")))
  )
}

# Initialize the first objective
observe({
  if(length(inserted_objectives$ids) == 0) {
    id <- 1
    inserted_objectives$ids <- c(id)
    inserted_objectives$names_map[[as.character(id)]] <- paste0("objective_", id)
    inserted_objectives$code_errors[[as.character(id)]] <- NULL
    
    insertUI(
      selector = "#objectives_anchor",
      where = "beforeEnd",
      ui = render_single_objective_ui(id),
      immediate = TRUE
    )
  }
})

# Event: add an isolated objective
observeEvent(input$add_objective_btn, {
  next_id <- if(length(inserted_objectives$ids) == 0) 1 else max(inserted_objectives$ids) + 1
  inserted_objectives$ids <- c(inserted_objectives$ids, next_id)
  inserted_objectives$names_map[[as.character(next_id)]] <- paste0("objective_", next_id)
  inserted_objectives$code_errors[[as.character(next_id)]] <- NULL
  
  insertUI(
    selector = "#objectives_anchor",
    where = "beforeEnd",
    ui = render_single_objective_ui(next_id),
    immediate = TRUE
  )
})

# Event: remove a single objective
observe({
  lapply(inserted_objectives$ids, function(id) {
    observeEvent(input[[paste0("remove_obj_", id)]], {
      removeUI(selector = paste0("#objective_card_container_", id), immediate = TRUE)
      inserted_objectives$ids <- setdiff(inserted_objectives$ids, id)
      inserted_objectives$names_map[[as.character(id)]] <- NULL
      inserted_objectives$code_errors[[as.character(id)]] <- NULL
    }, once = TRUE)
  })
})

# =========================================================================
# HARDWARE SAFETY CONTROL: RESERVE ONE CORE FOR THE UI THREAD (-1 CORE)
# =========================================================================
observe({
  # Get the total physical cores of the machine
  total_physical_cores <- as.integer(availableCores())

  # The safe maximum limit for the optimiser is the available CPU minus 1
  safe_max_cores <- max(1L, total_physical_cores - 1L)

  # If the user has Auto-detect checked, update the real value in the background
  if (isTRUE(input$total_cores_auto)) {
    updateNumericInput(session, "total_cores", value = safe_max_cores)
  }

  # Dynamically update the 'max' limit attribute of the manual input in the browser
  shinyjs::runjs(sprintf("$('#total_cores').attr('max', %d);", safe_max_cores))
})

# =========================================================================
# DYNAMIC VISIBILITY CONTROL FOR THE REMOVE BUTTONS
# =========================================================================
observe({
  current_ids <- inserted_objectives$ids

  if (length(current_ids) <= 1) {
    # If only 1 objective (or none) remains, force-hide the remaining item's button
    for (id in current_ids) {
      shinyjs::hide(id = paste0("remove_obj_", id))
    }
  } else {
    # If there are 2 or more objectives, make sure every button is enabled and shown
    for (id in current_ids) {
      shinyjs::show(id = paste0("remove_obj_", id))
    }
  }
})

# Conditional inner bodies (Predefined vs Custom)
observe({
  lapply(inserted_objectives$ids, function(id) {
    output[[paste0("obj_body_conditional_ui_", id)]] <- renderUI({
      source_type <- input[[paste0("obj_source_", id)]]
      if (is.null(source_type)) return(NULL)
      
      if (source_type == "predefined") {
        selectInput(paste0("obj_predefined_select_", id), label = "Select Predefined Logic:", choices = names(predefined_objectives))
      } else {
        tagList(
          div(class = "mt-2", tags$span(tags$b("R Function Body Code:"))),
          div(class = "code-header", "fitness_function <- function(result_folder) {"),
          div(class = "code-body",
              textAreaInput(paste0("obj_custom_code_", id), label = NULL, value = "    # Write your R body code here\n    return(1)", rows = 4, width = "100%")
          ),
          div(class = "code-footer", "}")
        )
      }
    })
  })
})

# Capture the name change on blur
observeEvent(input$blur_obj_name_change, {
  evt <- input$blur_obj_name_change
  req(!is.null(evt$id), !is.null(evt$name))
  if (inserted_objectives$names_map[[as.character(evt$id)]] != evt$name) {
    inserted_objectives$names_map[[as.character(evt$id)]] <- evt$name
  }
})

# Syntax and return-type validator (AST on blur)
observeEvent(input$blur_obj_code_change, {
  evt <- input$blur_obj_code_change
  req(!is.null(evt$id), !is.null(evt$code))
  
  complete_function_string <- paste0(
    "fitness_function <- function(result_folder) {\n", 
    evt$code, 
    "\n}"
  )
  
  tryCatch({
    parsed_expression <- parse(text = complete_function_string)
    
    if (!grepl("return\\s*\\(", evt$code)) {
      stop("The function code must explicitly contain a 'return()' statement, just like predefined system metrics.")
    }
    
    func_body <- body(eval(parsed_expression))
    last_expr <- if (as.character(func_body[]) == "{") {
      func_body[[length(func_body)]]
    } else {
      func_body
    }
    
    if (is.call(last_expr) && as.character(last_expr[]) == "return") {
      return_val_expr <- last_expr[]
      
      if (is.atomic(return_val_expr) && !is.symbol(return_val_expr)) {
        if (is.character(return_val_expr)) {
          stop("Type mismatch error: The return statement passes a 'character' string. It must return a numeric value.")
        }
        if (is.logical(return_val_expr)) {
          stop("Type mismatch error: The return statement passes a 'logical' boolean. It must return a numeric value.")
        }
      }
      
      if (is.call(return_val_expr)) {
        forbidden_functions <- c("paste", "paste0", "sprintf", "character", "logical", "c", "cat")
        func_called <- as.character(return_val_expr[])
        if (func_called %in% forbidden_functions && is.character(return_val_expr[])) {
          stop(paste0("Type mismatch error: The return expression utilizes '", func_called, 
                      "', which generates character strings. It must return a numeric value."))
        }
      }
    }
    
    inserted_objectives$code_errors[[as.character(evt$id)]] <- NULL
    shinyjs::removeClass(id = paste0("obj_custom_code_", evt$id), class = "is-invalid")
    
  }, error = function(e) {
    error_msg <- e$message
    inserted_objectives$code_errors[[as.character(evt$id)]] <- error_msg
    shinyjs::addClass(id = paste0("obj_custom_code_", evt$id), class = "is-invalid")
  })
})

# Render individual code errors
observe({
  lapply(inserted_objectives$ids, function(id) {
    output[[paste0("obj_code_error_display_", id)]] <- renderUI({
      err <- inserted_objectives$code_errors[[as.character(id)]]
      source_type <- input[[paste0("obj_source_", id)]]
      if (is.null(err) || is.null(source_type) || source_type != "custom") return(NULL)
      return(p(icon("triangle-exclamation"), paste(" Code Error:", err), class = "text-danger small fw-bold mt-1 mb-0 text-break"))
    })
  })
})


# =========================================================================
# GLOBAL REACTIVE FORM VALIDATION
# =========================================================================
fitness_validation_status <- reactive({
  ids <- inserted_objectives$ids
  if(length(ids) == 0) return(list(valid = TRUE, message = NULL))

  # Collect consolidated names after blur
  obj_names <- unlist(lapply(ids, function(id) inserted_objectives$names_map[[as.character(id)]]))

  # 1. Check for blank fields
  if(any(obj_names == "")) {
    return(list(valid = FALSE, message = "Error: Objective names cannot be left blank."))
  }

  # 2. Check for duplicate names (blocks both the Ranking and the Weights)
  if(any(duplicated(obj_names))) {
    return(list(valid = FALSE, message = "Error: There are duplicate objective names. Each objective must have a unique identifier."))
  }

  # 3. Block if any syntax error from Custom blocks is still active
  active_syntax_errors <- unlist(inserted_objectives$code_errors)
  if (length(active_syntax_errors) > 0) {
    return(list(valid = FALSE, message = "Error: One or more custom functions contain R syntax or logic errors."))
  }

  return(list(valid = TRUE, message = NULL))
})

# Render the unified error message in the interface
output$display_fitness_errors <- renderUI({
  status <- fitness_validation_status()
  if (!status$valid) {
    return(p(icon("circle-xmark"), status$message, class = "text-danger small fw-bold text-break mt-2"))
  }
  return(NULL)
})


# =========================================================================
# RENDERING OF THE LOWER STRATEGY PANEL (COMMITTED ON BLUR)
# =========================================================================
output$fitness_strategy_specific_ui <- renderUI({
  req(input$fitness_class)
  status <- fitness_validation_status()

  ids <- inserted_objectives$ids
  if(length(ids) == 0) return(NULL)

  if(!status$valid) {
    return(p("Please resolve validation errors above to configure strategy parameters.", class = "text-muted small italic"))
  }

  # Extract the list of names from the map consolidated on blur
  obj_names <- unlist(lapply(ids, function(id) inserted_objectives$names_map[[as.character(id)]]))

  if (input$fitness_class == "MultiObjectiveFitness") {
    tagList(
      # .ui-sortable rules are provided by the shared stylesheet (optimiser_shared_css()).
      tags$b("MultiObjective Priority Ranking:"),
      p("Drag and drop items to sort them by order of importance (Top item represents the highest priority hierarchy):", class = "text-muted small"),

      orderInput(
        inputId = "multiobjective_order",

        items = obj_names,
        item_class = "primary", 
        placeholder = "Drag items here..."
      )
    )
  } else {
    # 1. Reactively compute the current weight sum for the local visual state
    obj_names <- unlist(lapply(ids, function(id) inserted_objectives$names_map[[as.character(id)]]))
    current_weights <- numeric(length(obj_names))
    is_any_weight_empty <- FALSE
    
    for (i in seq_along(obj_names)) {
      w_input_id <- paste0("weight_for_", obj_names[i])
      w_val <- input[[w_input_id]]
      if (is.null(w_val) || is.na(w_val)) {
        is_any_weight_empty <- TRUE
      } else {
        current_weights[i] <- w_val
      }
    }
    weights_sum <- sum(current_weights)
    
    # Validate that the sum satisfies the exact 1.0 constraint (floating point tolerance)
    is_weights_sum_valid <- !is_any_weight_empty && (abs(weights_sum - 1.0) < 1e-5)
    badge_weights_class <- if (is_weights_sum_valid) "bg-success" else "bg-warning text-dark"
    has_weights_error <- is_any_weight_empty || !is_weights_sum_valid
    
    weights_error_msg <- if (is_any_weight_empty) {
      "Error: All weighted strategy coefficients must be filled with a valid numeric value."
    } else {
      paste0("Error: The sum of the weighted strategy coefficients is ", round(weights_sum, 4), ". It must equal exactly 1.0. Click the 'Normalize to 1.0' button to fix this automatically.")
    }
    
    # 2. Register the Panel 1 weights normalization button observer
    local({
      o_names <- obj_names
      w_count <- length(obj_names)

      observeEvent(input$normalize_weights_btn, {
        vals <- numeric(w_count)
        for (k in seq_len(w_count)) {
          vals[k] <- input[[paste0("weight_for_", o_names[k])]] %||% 0.0
        }

        total <- sum(vals)
        norm_vals <- if (total == 0) rep(1 / w_count, w_count) else vals / total

        # Round and apply residual correction on the last element (forced to 2 decimals)
        norm_vals <- round(norm_vals, 2)
        diff_residual <- 1.0 - sum(norm_vals)
        norm_vals[w_count] <- norm_vals[w_count] + diff_residual

        for (k in seq_len(w_count)) {
          updateNumericInput(session, paste0("weight_for_", o_names[k]), value = norm_vals[k])
        }
      }, ignoreInit = TRUE, once = FALSE)
    })

    # 3. Render the weights visual container with the normalization bar on top
    tagList(
      tags$script(HTML("
        // 1. Block keypresses that are not digits or the decimal point
        $(document).on('keypress', '.weight-decimal-input input', function(e) {
          var charCode = e.which;
          if (charCode !== 46 && (charCode < 48 || charCode > 57)) return false;
          if (charCode === 46 && $(this).val().indexOf('.') !== -1) return false;
        });

        // 2. Sanitize invalid text on paste
        $(document).on('input', '.weight-decimal-input input', function() {
          var $el = $(this);
          var sanitized = $el.val().replace(/[^0-9.]/g, '');
          var parts = sanitized.split('.');
          if (parts.length > 2) sanitized = parts[0] + '.' + parts.slice(1).join('');
          if ($el.val() !== sanitized) $el.val(sanitized);
        });

        // 3. On blur, only trigger the change without altering the typed value
        $(document).on('blur', '.weight-decimal-input input', function() {
          $(this).trigger('change');
        });
      ")),

      # Weights grid structure with structural red border and local alert
      tags$div(
        class = paste("p-3 border rounded bg-light weights-coefficients-container", if (has_weights_error) "is-invalid" else ""),
        style = if (has_weights_error) "border-color: #df4759 !important;" else "",
        
        div(class = "d-flex justify-content-between align-items-center mb-2",
            tags$b("Weighted Strategy Coefficients (Weights):"),
            div(
              tags$span(class = paste("badge me-2 p-2", badge_weights_class), 
                        paste("Current Sum:", round(weights_sum, 4))),
              actionButton("normalize_weights_btn", "Normalize to 1.0", class = "btn-outline-secondary btn-sm", icon = icon("wand-magic-sparkles"))
            )
        ),
        p("Assign relative importance weights for the linear combination summation:", class = "text-muted small mb-3"),
        
        lapply(obj_names, function(name) {
          w_input_id <- paste0("weight_for_", name)
          
          # INITIALIZE-TO-0.0 SOLUTION:
          # If the input already exists in the current Shiny session, keep what the user typed.
          # If the objective is NEW (rendered for the first time), it will strictly start at 0.0.
          current_input_value <- if (w_input_id %in% names(input)) {
            input[[w_input_id]]
          } else {
            0.0
          }
          
          div(class = "d-flex align-items-center gap-3 mb-2",
              div(style = "width: 200px;", tags$span(icon("scale-balanced"), tags$code(name))),
              div(style = "width: 120px;", class = "weight-decimal-input",
                  numericInput(w_input_id, label = NULL, value = current_input_value, min = 0, max = 1, step = 0.01)
              )
          )
        }),
        
        # Dynamic error text block below the field
        tags$div(
          class = "weights-error-msg",
          style = if (has_weights_error) "display: block;" else "display: none;",
          if (has_weights_error) p(icon("circle-xmark"), weights_error_msg, class = "text-danger small fw-bold text-break mt-3 mb-1")
        )
      )
    )
  }
})


# =========================================================================
# CASCADING COMPUTE CAPACITY REACTIVE VALIDATION (UI PROTECTED)
# =========================================================================
cores_validation_status <- reactive({
  if (is.null(input$program_cores) || is.null(input$total_cores) || is.null(input$total_cores_auto)) {
    return(list(valid = TRUE, message = NULL, err_type = NULL))
  }

  cores_per_instance <- input$program_cores
  total_system_cores <- input$total_cores

  # Read the hardware CPU count cleanly
  total_physical_cores <- as.integer(availableCores())
  # Optimiser's max limit, reserving 1 core for the Shiny UI
  safe_max_cores <- max(1L, total_physical_cores - 1L)

  # Avoid evaluating while fields are empty due to temporary user deletion
  if (is.na(cores_per_instance) || is.na(total_system_cores)) {
    return(list(valid = TRUE, message = NULL, err_type = NULL))
  }

  # ERROR 1: if manual, validate it does not break the hardware's safe limit
  if (input$total_cores_auto == FALSE && total_system_cores > safe_max_cores) {
    return(list(
      valid = FALSE, 
      err_type = "system_overflow",
      message = paste0("Error: Total allocated cores (", total_system_cores, 
                       ") cannot be greater than the maximum safe cores allowed for this system (", safe_max_cores, 
                       "). Exactly 1 core must remain free to handle interface updates synchronously.")
    ))
  }

  # ERROR 2: validate that cores per instance do not exceed the total configured cores
  if (cores_per_instance > total_system_cores) {
    return(list(
      valid = FALSE, 
      err_type = "instance_overflow",
      message = "Error: Cores per simulation instance cannot be greater than the total allocated system cores."
    ))
  }

  return(list(valid = TRUE, message = NULL, err_type = NULL))
})

# Render the active error message in the UI
output$display_cores_validation <- renderUI({
status <- cores_validation_status()
if (!status$valid) {
return(p(icon("circle-xmark"), status$message, class = "text-danger small fw-bold text-break mt-2"))
}
return(NULL)
})

# Dynamic error border control with shinyjs
observe({
  status <- cores_validation_status()

  # If the field is empty, reset the error classes
  if (is.na(input$program_cores) || is.na(input$total_cores)) {
shinyjs::toggleClass(id = "program_cores", class = "is-invalid", condition = FALSE)
shinyjs::toggleClass(id = "total_cores", class = "is-invalid", condition = FALSE)
} else {
if (!status$valid) {
# If the error is caused by exceeding the global CPU limit, highlight only the total cores field
if (status$err_type == "system_overflow") {
shinyjs::toggleClass(id = "total_cores", class = "is-invalid", condition = TRUE)
shinyjs::toggleClass(id = "program_cores", class = "is-invalid", condition = FALSE)
}
# If the error is caused by the per-instance allocation, highlight both involved fields
else if (status$err_type == "instance_overflow") {
shinyjs::toggleClass(id = "program_cores", class = "is-invalid", condition = TRUE)
shinyjs::toggleClass(id = "total_cores", class = "is-invalid", condition = TRUE)
}
} else {
# If everything is correct, clear the red borders
shinyjs::toggleClass(id = "program_cores", class = "is-invalid", condition = FALSE)
shinyjs::toggleClass(id = "total_cores", class = "is-invalid", condition = FALSE)
}
}
})