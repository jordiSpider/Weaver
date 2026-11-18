#' Shared Hub Styles
#'
#' @description Provides shared visual styles for the Hub and its tools.
#' @import shiny
get_shared_hub_styles <- function() {
  tags$head(
    tags$style(HTML("
      :root:not([data-bs-theme='dark']),
      [data-bs-theme='light'] {
        --bs-body-bg: #f3f6f1;
        --bs-body-bg-rgb: 243, 246, 241;
        --bs-body-color: #1c2b21;
        --bs-body-color-rgb: 28, 43, 33;
        --bs-secondary-color: #526052;
        --bs-secondary-color-rgb: 82, 96, 82;
        --bs-tertiary-bg: #e5ece4;
        --bs-border-color: #aab8aa;
        --bs-card-bg: #ffffff;
        --bs-dropdown-bg: #ffffff;
        --bs-dropdown-border-color: #aab8aa;
        --weaver-editor-bg: #ffffff;
        --weaver-editor-color: #1c2b21;
        --weaver-editor-header-bg: #e5ece4;
        --weaver-editor-header-color: #35463a;
        --weaver-editor-border: #aab8aa;
        --weaver-terminal-bg: #f7faf7;
        --weaver-terminal-color: #25352a;
        --weaver-log-info: #185c91;
        --weaver-log-success: #176344;
        --weaver-log-warning: #8f4817;
        --weaver-log-danger: #b42318;
        --weaver-hover-bg: #eaf2e9;
        --weaver-selected-bg: #dbe9da;
        --weaver-selected-border: #2c6338;
        --weaver-focus-ring: rgba(30, 63, 32, 0.3);
        --weaver-success-color: #176334;
      }

      :root[data-bs-theme='dark'],
      [data-bs-theme='dark'] {
        --weaver-editor-bg: #1a202c;
        --weaver-editor-color: #e2e8f0;
        --weaver-editor-header-bg: #2d3748;
        --weaver-editor-header-color: #a0aec0;
        --weaver-editor-border: #4a5568;
        --weaver-terminal-bg: #1e1e1e;
        --weaver-terminal-color: #a9b7c6;
        --weaver-log-info: #569cd6;
        --weaver-log-success: #4ec9b2;
        --weaver-log-warning: #ce9178;
        --weaver-log-danger: #f44747;
        --weaver-hover-bg: rgba(122, 179, 126, 0.18);
        --weaver-selected-bg: rgba(122, 179, 126, 0.28);
        --weaver-selected-border: #79a77d;
        --weaver-focus-ring: rgba(122, 179, 126, 0.38);
        --weaver-success-color: #4ec9b2;
      }

      .card {
        border-color: var(--bs-border-color);
        box-shadow: 0 0.2rem 0.7rem rgba(28, 43, 33, 0.08);
      }
      .card-header,
      .card-footer {
        background-color: var(--bs-tertiary-bg);
        color: var(--bs-body-color);
      }
      .optimiser-input-card {
        border-left: 4px solid var(--bs-primary) !important;
        margin-bottom: 15px;
        padding-left: 15px;
      }
      .simulation-field-label {
        font-weight: 600;
        color: var(--bs-body-color);
      }
      .is-invalid-border {
        border-color: var(--bs-danger) !important;
        box-shadow: 0 0 0 0.25rem rgba(220, 53, 69, 0.25) !important;
      }
      .weaver-validation-error {
        color: var(--bs-danger-text-emphasis, var(--bs-danger));
        font-size: 0.875rem;
        font-weight: 600;
        overflow-wrap: anywhere;
      }
      .weaver-validation-error ul {
        margin-top: 0.3rem;
        margin-bottom: 0.3rem;
        padding-left: 1.4rem;
      }
      .weaver-validation-status {
        font-size: 0.875rem;
        margin-top: 0.35rem;
      }
      .weaver-hub-modal .form-control {
        width: 100%;
      }
      #shiny-tab-optimiser_tab,
      #shiny-tab-input_manager_tab {
        padding-top: 0 !important;
      }
      #shiny-tab-optimiser_tab > .container-fluid,
      #shiny-tab-input_manager_tab > .container-fluid {
        width: 100%;
        max-width: none;
        padding-top: 0 !important;
        padding-right: 0;
        padding-left: 0;
      }
      #render_optimiser_module,
      #render_input_module,
      #render_optimiser_module > .bslib-page-sidebar,
      #render_input_module > .bslib-page-sidebar,
      #render_optimiser_module .bslib-sidebar-layout,
      #render_input_module .bslib-sidebar-layout {
        width: 100%;
        max-width: none;
        min-height: calc(100vh - 4rem);
      }
    "))
  )
}
