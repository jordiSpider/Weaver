library(shiny)
library(future)
library(promises)

# Configuramos R para que acepte procesos en segundo plano (multisession)
plan(multisession)

# 1. Tu función de R (Se ejecutará en segundo plano)
# En lugar de una función callback de R (que no puede cruzar hilos),
# le pasamos una función que escribe en un archivo temporal.
proceso_largo <- function(callback_escribir_archivo) {
  callback_escribir_archivo("Iniciando el proceso pesado en segundo plano...")
  Sys.sleep(1.5)
  
  for (i in 1:5) {
    Sys.sleep(1.5) # Simula una tarea real
    callback_escribir_archivo(paste("Etapa", i, "completada con éxito."))
  }
  
  Sys.sleep(1)
  callback_escribir_archivo("Proceso finalizado correctamente.")
  return("¡Éxito!")
}

# 2. Interfaz de Usuario (UI)
ui <- fluidPage(
  titlePanel("Log en Tiempo Real (Asíncrono y No Bloqueante)"),
  
  sidebarLayout(
    sidebarPanel(
      actionButton("btn_iniciar", "Iniciar Proceso Asíncrono", class = "btn-primary"),
      hr(),
      # Botón de prueba para demostrar que la UI NO se congela
      actionButton("btn_test", "UI Interactiva (Hazme clic)", class = "btn-info"),
      textOutput("txt_test")
    ),
    
    mainPanel(
      tags$h4("Consola de Log (Actualizándose en vivo):"),
      verbatimTextOutput("txt_log")
    )
  )
)

# 3. Lógica del Servidor (Server)
server <- function(input, output, session) {
  
  # Creamos un archivo temporal exclusivo para esta sesión de usuario
  archivo_log <- tempfile(fileext = ".txt")
  writeLines("Esperando inicio...", archivo_log)
  
  # Invalidación reactiva: Shiny leerá el archivo cada 500ms automáticamente
  log_contenido <- reactive({
    invalidateLater(500, session) # Revisa el archivo dos veces por segundo
    if (file.exists(archivo_log)) {
      paste(readLines(archivo_log), collapse = "\n")
    } else {
      "Archivo de log no disponible."
    }
  })
  
  # Renderizamos el log en la UI de forma reactiva continua
  output$txt_log <- renderText({
    log_contenido()
  })
  
  # Definimos la Tarea Extendida (Asíncrona)
  tarea_pesada <- ExtendedTask$new(function(ruta_archivo) {
    future({
      # Esta función interna actúa como la callback dentro del hilo de fondo
      mi_callback_hilo <- function(mensaje) {
        cat(mensaje, file = ruta_archivo, sep = "\n", append = TRUE)
      }
      
      # Ejecutamos la función pasando el puente de escritura
      proceso_largo(mi_callback_hilo)
    })
  })
  
  # Al hacer clic, iniciamos la tarea sin bloquear el servidor
  observeEvent(input$btn_iniciar, {
    # Limpiamos el archivo para el nuevo proceso
    writeLines("Iniciando entorno asíncrono...", archivo_log)
    tarea_pesada$invoke(archivo_log)
  })
  
  # Lógica del botón de prueba (demuestra que la UI sigue respondiendo)
  observeEvent(input$btn_test, {
    output$txt_test <- renderText({
      paste("¡La UI funciona! Clic detectado a las:", Sys.time())
    })
  })
  
  # Buena práctica: Limpiar el archivo temporal cuando el usuario cierre la pestaña
  onSessionEnded(function() {
    if (file.exists(archivo_log)) unlink(archivo_log)
  })
}

shinyApp(ui = ui, server = server)