#' Optimiser-Specific Styles
#'
#' @description Returns styles and input sanitizers used by Optimiser phases.
#' @import shiny
#' @export
get_shared_optimiser_styles <- function() {
  tags$head(
    # 1. Structural CSS layout rules
    tags$style("
        /* --- Tipografía Monospace Compartida --- */
        .code-editor-header-wrapper,
        .code-editor-footer-wrapper,
        .vertical-order-container .ui-sortable-handle,
        .vertical-order-container .jstree-anchor,
        .weight-row-item .decimal-input input {
          font-family: 'Fira Code', 'Courier New', Courier, monospace !important;
          font-size: 0.85rem !important;
        }

        .code-editor-textarea-container textarea {
          font-family: 'Fira Code', 'Courier New', Courier, monospace !important;
          font-size: 0.9rem !important;
          background-color: var(--weaver-editor-bg) !important;
          color: var(--weaver-editor-color) !important;
          border: 1px solid var(--weaver-editor-border) !important;
          border-radius: 0 !important;
          resize: vertical;
          padding: 10px !important;
        }

        /* --- Componentes del Editor de Código (Cabecera y Pie) --- */
        .code-editor-header-wrapper,
        .code-editor-footer-wrapper {
          background-color: var(--weaver-editor-header-bg) !important;
          color: var(--weaver-editor-header-color) !important;
          padding: 6px 12px;
          border: 1px solid var(--weaver-editor-border);
        }

        .code-editor-header-wrapper {
          border-bottom: none;
          border-radius: 6px 6px 0 0;
        }

        .code-editor-footer-wrapper {
          border-top: none;
          border-radius: 0 0 6px 6px;
          margin-bottom: 12px;
        }

        /* --- Contenedores Flex Verticales --- */
        .vertical-order-container .ui-sortable,
        .vertical-weights-container {
          display: flex !important;
          flex-direction: column !important;
          gap: 6px !important;
          width: 100% !important;
        }

        /* --- Elementos de Lista y Filas --- */
        .vertical-order-container .ui-sortable-handle,
        .weight-row-item {
          width: 100% !important;
          padding: 8px 12px !important;
          border-radius: 4px !important;
        }

        .vertical-order-container .ui-sortable-handle {
          display: block !important;
          margin: 0 !important;
          text-align: left !important;
        }

        .weight-row-item {
          display: flex !important;
          align-items: center !important;
          justify-content: space-between !important;
          background-color: var(--bs-body-bg) !important;
          border: 1px solid var(--bs-border-color) !important;
        }

        /* --- Inputs Numéricos Generales y Específicos --- */
        .decimal-input input, 
        .integer-input input {
          font-family: 'Fira Code', font-monospace, monospace !important;
          font-size: 0.85rem !important;
          background-color: var(--bs-body-bg) !important;
          color: var(--bs-body-color) !important;
          border: 1px solid var(--bs-border-color) !important;
        }

        /* Nota: Hereda los estilos generales anteriores y añade sus particulares */
        .weight-row-item .decimal-input input {
          text-align: right;
          padding: 4px 8px !important;
          border-radius: 4px !important;
        }

        .decimal-input input:focus, 
        .integer-input input:focus {
          border-color: var(--bs-primary) !important;
          box-shadow: 0 0 0 0.25rem var(--weaver-focus-ring) !important;
        }

        /* --- JSTree (Temas adaptables) --- */
        .vertical-order-container .jstree-default {
          background: transparent !important;
        }

        .vertical-order-container .jstree-default,
        .vertical-order-container .jstree-anchor {
          color: var(--bs-body-color) !important;
        }

        .vertical-order-container .jstree-hovered,
        .vertical-order-container .jstree-clicked {
          border-radius: 4px;
        }

        .vertical-order-container .jstree-hovered {
          background: var(--weaver-hover-bg) !important;
        }

        .vertical-order-container .jstree-clicked {
          background: var(--weaver-selected-bg) !important;
          border: 1px solid var(--weaver-selected-border);
        }

        .vertical-order-container .jstree-search {
          color: var(--weaver-success-color) !important;
          font-weight: bold !important;
          font-style: italic;
        }

        /* --- Bootstrap / Componentes del Hub --- */
        .preference-cell-container, 
        .abundances-vector-container,
        .preference-cell-container h6, 
        .preference-cell-container label {
          color: var(--bs-body-color) !important;
        }

        .preference-cell-container, 
        .abundances-vector-container {
          background-color: var(--bs-tertiary-bg) !important;
          border-color: var(--bs-border-color) !important;
        }

        .preference-cell-container .bg-white {
          background-color: var(--bs-card-bg) !important;
          border-color: var(--bs-border-color) !important;
        }

        .bootstrap-select .dropdown-menu {
          background-color: var(--bs-dropdown-bg) !important;
          border: 1px solid var(--bs-dropdown-border-color) !important;
        }

        .bootstrap-select .dropdown-menu span.text {
          color: var(--bs-dropdown-link-color) !important;
        }

        .bootstrap-select .dropdown-menu .active span.text {
          color: var(--bs-dropdown-link-active-color) !important;
        }

        /* --- Estilos Independientes (Sin redundancias) --- */
        .phase-status-badge {
          font-size: 0.85rem;
          padding: 5px 10px;
          border-radius: 4px;
        }

        /* forces the wrapper buttons container to unroll flawlessly */
        .permutation-buttons-container .btn-group,
        .permutation-buttons-container .btn-group-toggle {
          display: flex !important;
          flex-wrap: wrap !important;
          gap: 6px !important;
          width: 100% !important;
          box-shadow: none !important;
        }

        /* Transforms the checkbox buttons into premium, standalone responsive pads */
        .permutation-buttons-container .btn-check + .btn-primary {
          flex: 1 1 auto !important;
          display: inline-flex !important;
          align-items: center !important;
          justify-content: center !important;
          min-height: 40px !important; /* Incrementa la superficie vertical para facilitar el clic */
          padding: 8px 16px !important;
          margin: 0 !important;
          cursor: pointer !important;
          text-align: center !important;
          border-radius: 4px !important;
          font-family: 'Fira Code', font-monospace, monospace !important;
          font-size: 0.85rem !important;
          background-color: var(--bs-body-bg) !important;
          color: var(--bs-body-color) !important;
          border: 1px solid var(--bs-border-color) !important;
          transition: all 0.2s ease-in-out;
        }

        /* SURFACE CLIC LIEN: Enforces the label pseudo-element to spread out across 100% space */
        .permutation-buttons-container .btn-check + .btn-primary * {
          pointer-events: none !important; /* Evita que los textos internos bloqueen el clic del padre */
        }

        /* Active / Checked State layout matching the global web theme */
        .permutation-buttons-container .btn-check:checked + .btn-primary {
          background-color: var(--bs-primary) !important;
          color: var(--bs-primary-contrast, #ffffff) !important;
          border-color: var(--bs-primary) !important;
          font-weight: bold !important;
          box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.1) !important;
        }

        /* Hover aesthetics feedback protection */
        .permutation-buttons-container .btn-check + .btn-primary:hover {
          border-color: var(--bs-primary) !important;
          background-color: var(--weaver-hover-bg) !important;
        }

        .terminal-logs-monitor {
          background-color: var(--weaver-terminal-bg) !important;
          color: var(--weaver-terminal-color) !important;
          font-family: 'Fira Code', 'Courier New', font-monospace, monospace !important;
          font-size: 0.85rem !important;
          padding: 15px;
          border-radius: 6px;
          border: 1px solid var(--bs-border-color);
          max-height: 400px;
          overflow-y: auto;
          white-space: pre-wrap;
          box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.35);
        }

        .terminal-logs-monitor .log-system-info { color: var(--weaver-log-info); }
        .terminal-logs-monitor .log-success { color: var(--weaver-log-success); }
        .terminal-logs-monitor .log-warning { color: var(--weaver-log-warning); }
        .terminal-logs-monitor .log-danger { color: var(--weaver-log-danger); font-weight: bold; }

        /* --- Contenedor Global de Ligadura de Parámetros Adaptativo --- */
        .tied-destinations-container {
          background-color: var(--bs-tertiary-bg) !important;
          border-color: var(--bs-border-color) !important;
          border-left: 4px solid var(--bs-warning) !important;
        }

        /* --- Filas Internas de Coordenadas de Pares (Sub-grilla) --- */
        .pairs-grid-subcontainer .row.bg-white {
          /* Sobreescribe el fondo rígido blanco por el color de tarjeta adaptativo */
          background-color: var(--bs-card-bg) !important;
          border-color: var(--bs-border-color) !important;
          color: var(--bs-body-color) !important;
        }

        .pairs-grid-subcontainer .text-muted {
          color: var(--bs-secondary-color) !important;
        }
    "),
    
    # 2. Global Client-Side JavaScript event sanitizers for standard types
    tags$script(HTML("
      $(document).ready(function() {
        // --- INTEGER INPUTS HANDLERS ---
        $(document).on('keypress', '.integer-input input', function(e) {
          var charCode = e.which;
          if (charCode < 48 || charCode > 57) return false;
        });

        $(document).on('input', '.integer-input input', function() {
          var $inputField = $(this);
          var sanitizedText = $inputField.val().replace(/[^0-9]/g, '');
          if ($inputField.val() !== sanitizedText) $inputField.val(sanitizedText);
        });

        $(document).on('blur', '.integer-input input', function() {
          var $inputField = $(this);
          var parsedValue = parseInt($inputField.val(), 10);
          var minimumAttribute = parseInt($inputField.attr('min'), 10);
          if (isNaN(parsedValue)) {
            $inputField.val(!isNaN(minimumAttribute) ? minimumAttribute : 0).trigger('change');
          } else if (!isNaN(minimumAttribute) && parsedValue < minimumAttribute) {
            $inputField.val(minimumAttribute).trigger('change');
          }
        });

        // --- DECIMAL INPUTS HANDLERS ---
        $(document).on('keypress', '.decimal-input input', function(e) {
          var charCode = e.which;
          if (charCode !== 46 && (charCode < 48 || charCode > 57)) return false;
          if (charCode === 46 && $(this).val().indexOf('.') !== -1) return false;
        });

        $(document).on('input', '.decimal-input input', function() {
          var $inputField = $(this);
          var sanitizedText = $inputField.val().replace(/[^0-9.]/g, '');
          var textParts = sanitizedText.split('.');
          if (textParts.length > 2) {
            sanitizedText = textParts[0] + '.' + textParts.slice(1).join('');
          }
          if ($inputField.val() !== sanitizedText) $inputField.val(sanitizedText);
        });

        $(document).on('blur', '.decimal-input input', function() {
          var $inputField = $(this);
          var numericalValue = parseFloat($inputField.val());
          var minimumLimit = parseFloat($inputField.attr('min'));
          var maximumLimit = parseFloat($inputField.attr('max'));
          var stepAttribute = $inputField.attr('step');
          var stepInterval = parseFloat(stepAttribute) || 0.01;

          if (isNaN(numericalValue)) numericalValue = !isNaN(minimumLimit) ? minimumLimit : 0.0;
          if (!isNaN(minimumLimit) && numericalValue < minimumLimit) numericalValue = minimumLimit;
          if (!isNaN(maximumLimit) && numericalValue > maximumLimit) numericalValue = maximumLimit;

          if (stepAttribute && stepAttribute.toLowerCase() === 'any') {
            $inputField.val(numericalValue).trigger('change');
            return;
          }

          var quantizedValue = Math.round(numericalValue / stepInterval) * stepInterval;
          var decimalPlaces = (stepInterval.toString().split('.')[1] || '').length;
          var fixedPrecisionValue = parseFloat(quantizedValue.toFixed(decimalPlaces));
          $inputField.val(fixedPrecisionValue).trigger('change');
        });
      });
    "))
  )
}
