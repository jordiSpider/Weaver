#' Launch the Modular Ecosystem Simulation Hub with Adaptive Outward Safeguards
#'
#' @description
#' Initializes the central application hub. Enforces navigation protection exclusively 
#' when exiting active tool workspaces. Inward navigation from the main directory remains seamless.
#'
#' @param ... Additional arguments passed to shiny::runApp.
#' @import shiny
#' @importFrom bslib page_navbar bs_theme card card_header card_footer nav_panel nav_spacer nav_item nav_select page_sidebar sidebar input_dark_mode
#' @export
run_hub <- function(...) {
  
  # ============================================================================
  # 1. USER INTERFACE (UI) REGISTRATION FRAMEWORK
  # ============================================================================
  hub_ui <- bslib::page_navbar(
    title = "Weaver EcoHub",
    id = "main_hub_nav",
    theme = bslib::bs_theme(
      version = 5,
      bootswatch = "minty",
      primary = "#1E3F20"
    ),
    
    # Injection of JavaScript browser tab destruction callback hook
    header = tags$head(
      tags$script(
        "Shiny.addCustomMessageHandler('close_browser_window', function(message) {
          window.close();
        });"
      ),
      # Hide standard navbar tabs to enforce strict programmatic navigation routing
      tags$style(".navbar-nav .nav-link { display: none !important; }")
    ),
    
    # Anchor Tab 1: Central Directory (Home)
    bslib::nav_panel(
      title = "Home",
      value = "home_tab",
      icon = icon("leaf"),
      uiOutput("render_home_view")
    ),
    
    # Anchor Tab 2: Simulation Optimiser Compute Stack
    bslib::nav_panel(
      title = "Optimiser Workspace",
      value = "optimiser_tab",
      icon = icon("cogs"),
      fluidPage(
        div(class = "d-flex justify-content-between align-items-center my-3",
          h3("Simulation Optimiser Engine Context"),
          actionButton("btn_back_from_optimiser", "Return to Directory", icon = icon("arrow-left"), class = "btn-outline-secondary btn-sm")
        ),
        uiOutput("render_optimiser_module")
      )
    ),
    
    # Anchor Tab 3: Ecosystem Input Parameter Matrix Builder
    bslib::nav_panel(
      title = "Input Manager Workspace",
      value = "input_manager_tab",
      icon = icon("sliders-h"),
      fluidPage(
        div(class = "d-flex justify-content-between align-items-center my-3",
          h3("Input Configuration Manager Context"),
          actionButton("btn_back_from_inputs", "Return to Directory", icon = icon("arrow-left"), class = "btn-outline-secondary btn-sm")
        ),
        uiOutput("render_input_module")
      )
    ),
    
    # Right-aligned globally inherited theme and system lifecycle controls
    bslib::nav_spacer(),
    bslib::nav_item(bslib::input_dark_mode(id = "hub_theme_toggle")),
    bslib::nav_item(actionButton("btn_terminate_hub", "Exit Hub", icon = icon("power-off"), class = "btn-danger btn-sm my-1 ms-2 font-weight-bold"))
  )

  # ============================================================================
  # 2. SERVER ROUTING INTERCEPTORS & LIFECYCLE ENGINE
  # ============================================================================
  hub_server <- function(input, output, session) {
    message("[weaverRHub] Core ecosystem state infrastructure framework listening.")
    
    # Core state machine vectors mapping active panels and tracking target shifts
    active_view <- reactiveVal("home_tab")
    pending_destination <- reactiveVal(NULL)
    
    # --- RENDER ENGINE ROUTERS (Forces clean instantiation / absolute state destruction) ---
    output$render_home_view <- renderUI({
      req(active_view() == "home_tab")
      fluidPage(
        div(class = "my-4",
          h2("Ecosystem Simulation Tools Directory"),
          p(class = "text-muted", "Select a specialized module below. Warning: Navigating away from active sessions purges calculations data state.")
        ),
        fluidRow(
          column(width = 4,
            bslib::card(
              bslib::card_header(span(icon("cogs"), class = "me-2"), "Simulation Optimiser Engine"),
              p("Calibrate ecological parameters and optimize ecosystem stability models natively using heuristic GA loops."),
              bslib::card_footer(actionButton("btn_goto_optimiser", "Open Workspace", class = "btn-primary w-100"))
            )
          ),
          column(width = 4,
            bslib::card(
              bslib::card_header(span(icon("sliders-h"), class = "me-2"), "Input Configuration Manager"),
              p("Construct environmental baseline constraints, initial population matrices, and abiotical factor files dynamically."),
              bslib::card_footer(actionButton("btn_goto_inputs", "Open Workspace", class = "btn-primary w-100"))
            )
          )
        )
      )
    })
    
    output$render_optimiser_module <- renderUI({
      req(active_view() == "optimiser_tab")
      mod_simulation_optimiser_ui("optimiser_core_session")
    })
    
    output$render_input_module <- renderUI({
      req(active_view() == "input_manager_tab")
      mod_input_configuration_ui("input_manager_core_session")
    })
    
    # --- ISOLATED MODULE LIFECYCLE SERVERS INVOCATION ---
    observe({
      if (active_view() == "optimiser_tab") {
        mod_simulation_optimiser_server("optimiser_core_session")
      } else if (active_view() == "input_manager_tab") {
        mod_input_configuration_server("input_manager_core_session")
      }
    })
    
    # --- INTERCEPT NAVIGATION ATTEMPTS (Enforces conditional confirmation) ---
    route_to_target <- function(target) {
      # If current view is Home, allow seamless transition into any tool without warning
      if (active_view() == "home_tab") {
        message(paste("[Navigation Router] Seamless entry authorized into:", target))
        active_view(target)
        bslib::nav_select(id = "main_hub_nav", selected = target)
      } else {
        # Trigger modal warning only when exiting an active tool workspace
        pending_destination(target)
        showModal(modalDialog(
          title = "Confirm Environment Tab Navigation Swap",
          span(icon("exclamation-triangle"), class = "text-warning me-2"),
          "Warning: Moving to another environment immediately destroys your current workspace. Unsaved configuration metrics or running models parameters will be completely lost.",
          footer = tagList(
            modalButton("Cancel Operation"),
            actionButton("btn_confirm_navigation", "Discard Progress & Move", class = "btn-danger")
          ),
          easyClose = FALSE
        ))
      }
    }
    
    # Navigation Action Directives Mapping
    observeEvent(input$btn_goto_optimiser, { route_to_target("optimiser_tab") })
    observeEvent(input$btn_goto_inputs, { route_to_target("input_manager_tab") })
    observeEvent(input$btn_back_from_optimiser, { route_to_target("home_tab") })
    observeEvent(input$btn_back_from_inputs, { route_to_target("home_tab") })
    
    # Transactional execution commit upon explicit user consent to discard progress
    observeEvent(input$btn_confirm_navigation, {
      req(pending_destination())
      removeModal()
      
      target <- pending_destination()
      message(paste("[Navigation Router] State destruction confirmed. Purging memory space. Swapping to:", target))
      
      active_view(target)
      bslib::nav_select(id = "main_hub_nav", selected = target)
      pending_destination(NULL)
    })
    
    # --- APP SHUTDOWN ORCHESTRATION ---
    observeEvent(input$btn_terminate_hub, {
      message("[Hub Lifecycle] Termination sequence initialized by user trigger.")
      session$sendCustomMessage(type = 'close_browser_window', message = list())
      stopApp()
    })
  }

  # ============================================================================
  # 3. CONTAINER RUN ENGINE INITIALIZATION
  # ============================================================================
  shiny::runApp(appDir = shiny::shinyApp(ui = hub_ui, server = hub_server), ...)
}