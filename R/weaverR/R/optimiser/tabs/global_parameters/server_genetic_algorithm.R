# =========================================================================
# DYNAMIC MINIMUM CONTROL BASED ON THE SWITCH (KEEP BEST)
# =========================================================================

observeEvent(input$keepBest, {
  req(!is.null(input$keepBest))

  # Determine the new minimum according to the given condition
  new_minimum <- if (input$keepBest) 3 else 2

  # If the current value in R is already below the newly required minimum,
  # force an immediate update of its value together with the new limit.
  # Otherwise, just passively update the "min" attribute in the HTML.
  if (!is.na(input$tournament_size) && input$tournament_size < new_minimum) {
    updateNumericInput(session, "tournament_size", min = new_minimum, value = new_minimum)
  } else {
    updateNumericInput(session, "tournament_size", min = new_minimum)
  }
})


# =========================================================================
# REACTIVE VALIDATION OF GENETIC ALGORITHM PARAMETERS
# =========================================================================

ga_validation_status <- reactive({
  if (is.null(input$population_size) || is.null(input$tournament_size) || is.null(input$keepBest)) {
    return(list(valid = TRUE, message = NULL))
  }

  pop <- input$population_size
  tour <- input$tournament_size
  keep_best <- input$keepBest

  # While the user is actively clearing/editing the field and it is empty,
  # treat the state as temporarily valid to avoid premature alerts or crashes
  if (is.na(pop) || is.na(tour)) {
    return(list(valid = TRUE, message = NULL))
  }

  if (keep_best) {
    if (pop <= tour) {
      return(list(valid = FALSE, message = "Error: Population size must be greater than the tournament selection size."))
    }
  } else {
    if (pop < tour) {
      return(list(valid = FALSE, message = "Error: Population size must be greater or equal than the tournament selection size."))
    }
  }

  return(list(valid = TRUE, message = NULL))
})


# =========================================================================
# RENDERING AND INVALID-STATE BORDER CONTROL
# =========================================================================

output$display_ga_validation <- renderUI({
  status <- ga_validation_status()
  if (!status$valid) {
    return(p(icon("circle-xmark"), status$message, class = "text-danger small fw-bold text-break mt-2"))
  }
  return(NULL)
})

observe({
  status <- ga_validation_status()
  
  if (is.na(input$population_size) || is.na(input$tournament_size)) {
    shinyjs::toggleClass(id = "population_size", class = "is-invalid", condition = FALSE)
    shinyjs::toggleClass(id = "tournament_size", class = "is-invalid", condition = FALSE)
  } else {
    shinyjs::toggleClass(id = "population_size", class = "is-invalid", condition = !status$valid)
    shinyjs::toggleClass(id = "tournament_size", class = "is-invalid", condition = !status$valid)
  }
})