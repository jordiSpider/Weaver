library(shiny)
library(bslib)
library(shinyjs)
library(future)
library(weaverR) # Verified and installed package

# Plan de ejecución paralela por defecto de 'future' para TODA la app.
# IMPORTANTE: debe fijarse aquí, en el arranque del script (fuera de
# server_hub y de cualquier moduleServer/observeEvent), para que se ejecute
# UNA sola vez en el proceso principal de R, antes de que exista ninguna
# sesión de Shiny. Si se mueve dentro de server_hub o de un observeEvent se
# reevalúa en cada sesión/evento sin necesidad, y si se coloca dentro de un
# future::future(...) (p.ej. en phase_3_server.R) dejaría de tener efecto
# sobre el hilo principal, volviendo a bloquear invalidateLater() y perdiendo
# la actualización del log en tiempo real.
#
# Los módulos que anidan futures (phase_3_server.R lanza un future interno
# para el optimizador, y éste a su vez paraleliza con otro future::plan())
# sustituyen este plan por uno ANIDADO (lista de 2 niveles) justo antes de
# invocar su ExtendedTask, para evitar que la protección anti-sobresuscripción
# de 'future' limite availableCores() a 1 dentro del future anidado.
future::plan(future::multisession)

# =========================================================================
# CENTRAL HUB INTERFACE (HIDDEN-CONTAINER ARCHITECTURE)
# =========================================================================
ui_hub <- page_fluid(
  useShinyjs(),
  theme = bs_theme(version = 5, bootswatch = "flatly"),

  navset_hidden(
    id = "central_switcher",

    # View 1: Main HUB dashboard
    nav_panel_hidden(
      value = "view_hub",
      tagList(
        div(class = "mt-3 mb-2 d-flex justify-content-between align-items-center bg-primary p-2 text-white rounded shadow-sm",
            tags$span(class = "fw-bold ms-2", icon("cubes"), " Ecological Toolbox - HUB Management View"),
            actionButton("exit_hub_btn", " Close HUB Ecosystem", class = "btn-danger btn-sm", icon = icon("power-off"))
        ),
        div(class = "p-5 mb-4 bg-light rounded-3 mt-2 border shadow-sm",
            div(class = "container-fluid py-3",
                tags$h1(class = "display-5 fw-bold text-primary", icon("cubes"), " Ecological Toolbox HUB"),
                p(class = "col-md-8 fs-4 text-muted", "Centralized platform for calibration, analytical simulation and parameter optimization.")
            )
        ),
        layout_columns(
          col_widths = c(6, 6), fill = FALSE,
          card(
            card_header(tags$b("Weaver Genetic Algorithm Optimiser")),
            card_body(
              p("Automated calibration of feeding preferences, instar parameterization and matrix immutability."),
              actionButton("launch_optimiser", " Launch Optimiser", class = "btn-success btn-lg w-100 shadow-sm", icon = icon("sliders"))
            )
          ),
          card(
            card_header(tags$b("System Information")),
            card_body(
              tags$ul(
                tags$li(tags$b("HUB Status: "), tags$span(class="badge bg-success", "Online")),
                tags$li(tags$b("Core Package 'weaverR': "), ifelse("weaverR" %in% rownames(installed.packages()), "Installed \u2714", "Not detected \u2718"))
              )
            )
          )
        )
      )
    ),

    # View 2: The full Optimiser sub-environment
    nav_panel_hidden(
      value = "view_optimiser",
      tagList(
        div(class = "mt-3 mb-2 d-flex justify-content-between align-items-center bg-dark p-2 text-white rounded shadow-sm",
            tags$span(class = "fw-bold ms-2", icon("gear"), " Weaver Engine Sub-Environment Active"),
            actionButton("return_hub_btn", " Back to Central HUB", class = "btn-outline-light btn-sm", icon = icon("arrow-left"))
        ),
        uiOutput("optimiser_ui_container")
      )
    )
  )
)

# =========================================================================
# CENTRAL HUB SERVER
# =========================================================================
server_hub <- function(input, output, session) {

  # Isolated environment where the optimiser tool's submodules get compiled
  tool_env <- new.env(parent = globalenv())
  weaverR:::.load_all_tool_submodules(tool_env)

  # Render the optimiser's navbar UI, without its own exit button since the
  # HUB already provides a "Back to Central HUB" affordance for this view.
  output$optimiser_ui_container <- renderUI({
    optimiser$build_ui(tool_env, show_exit_button = FALSE)
  })

  # Attach the optimiser's full reactive orchestration (validations, tab
  # gating, loading overlay, exit handling) to this very session. This is the
  # same implementation used by the standalone optimiser app, so switching
  # between "standalone tool" and "HUB-embedded tool" never loses behaviour.
  optimiser$attach_server(input, output, session, tool_env, on_exit = function() {
    nav_select("central_switcher", "view_hub")
    current_view("hub")
  })

  # --- SCREEN NAVIGATION CONTROL ---
  current_view <- reactiveVal("hub")

  observeEvent(input$launch_optimiser, {
    showNotification("Opening optimiser panel...", type = "message")
    nav_select("central_switcher", "view_optimiser")
    current_view("optimiser")
  })

  observeEvent(input$return_hub_btn, {
    nav_select("central_switcher", "view_hub")
    current_view("hub")
  })

  # --- SECURE SHUTDOWN ---
  .execute_secure_shutdown <- function() {
    cat("\n[", as.character(Sys.time()), "]\tShutting down the Weaver ecosystem and releasing sockets...\n")
    shinyjs::runjs("window.open('', '_self'); window.close(); setTimeout(function() { window.location.href = 'about:blank'; }, 100);")
    stopApp()
  }

  observeEvent(input$exit_hub_btn, { .execute_secure_shutdown() })
}

shinyApp(ui = ui_hub, server = server_hub, options = list(port = 8000))
