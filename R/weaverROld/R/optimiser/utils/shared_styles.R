# =========================================================================
# UNIFIED OPTIMISER STYLESHEET
# =========================================================================
# Single source of truth for all CSS used across the optimiser tool so that
# validation states, color palette and loading indicators look consistent
# across every tab. Call `optimiser_shared_css()` once per page (it is
# already wired into the page header by the orchestrator / main.R).

#' Shared CSS/variables for the optimiser UI
#'
#' @return a `tags$style` element to be placed inside a `tags$head()`.
optimiser_shared_css <- function() {
  tags$style(HTML("
	:root {
	  --optimiser-error-color: #df4759;
	  --optimiser-error-bg: rgba(223, 71, 89, 0.15);
	  --optimiser-error-bg-strong: rgba(223, 71, 89, 0.25);
	  --optimiser-code-bg: #f4f6f9;
	  --optimiser-loading-bg: #ffffff;
	  --optimiser-loading-fg: #2c3e50;
	}

	/* ---------------------------------------------------------------
	   Loading overlay (shown while the optimiser environment boots)
	   --------------------------------------------------------------- */
	#loading-overlay {
	  position: fixed; top: 0; left: 0; width: 100%; height: 100%;
	  background-color: var(--optimiser-loading-bg); z-index: 999999;
	  display: flex; flex-direction: column; justify-content: center; align-items: center;
	  transition: opacity 0.4s ease-out;
	}
	.spinner-loader {
	  width: 50px; height: 50px; border: 5px solid #f3f3f3;
	  border-top: 5px solid var(--optimiser-loading-fg); border-radius: 50%;
	  animation: spin-anim 1s linear infinite;
	}
	@keyframes spin-anim { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
	.loading-text { margin-top: 20px; font-family: 'Lato', Arial, sans-serif; color: var(--optimiser-loading-fg); font-weight: bold; }

	/* ---------------------------------------------------------------
	   Code-style blocks (objective function editors)
	   --------------------------------------------------------------- */
	.code-header, .code-footer {
	  background-color: var(--optimiser-code-bg);
	  font-family: monospace;
	  padding: 6px 12px;
	  border: 1px solid #ced4da;
	  color: #212529;
	}
	.code-header { border-bottom: none; border-radius: 4px 4px 0 0; }
	.code-footer { border-top: none; border-radius: 0 0 4px 4px; }
	.code-body textarea {
	  border-radius: 0 !important;
	  font-family: monospace;
	  resize: vertical;
	}

	/* ---------------------------------------------------------------
	   Sortable priority lists (fitness hierarchy)
	   --------------------------------------------------------------- */
	.ui-sortable {
	  display: block !important;
	  clear: both !important;
	  margin-bottom: 15px !important;
	  overflow: hidden !important;
	}
	.ui-sortable-handle {
	  text-align: left !important;
	  font-weight: bold !important;
	  margin-top: 6px !important;
	  margin-bottom: 6px !important;
	  width: 100% !important;
	  display: block !important;
	  float: none !important;
	}

	/* ---------------------------------------------------------------
	   Unified "invalid" state for every field/container type
	   --------------------------------------------------------------- */
	.weights-coefficients-container.is-invalid .weights-error-msg,
	.numeric-range-container.is-invalid .range-error-msg,
	.numeric-range-container.is-invalid-input .range-error-msg,
	.abundances-vector-container.is-invalid .abundance-error-msg,
	.pref-error-msg {
	  display: block !important;
	}

	div.is-invalid .selectize-input,
	div.is-invalid .selectize-control .selectize-input,
	div.is-invalid .selectize-control.multi .selectize-input,
	div.is-invalid .selectize-control.single .selectize-input,
	div.is-invalid .selectize-control.multi .selectize-input.has-items,
	.weights-coefficients-container.is-invalid .weight-decimal-input input,
	.numeric-range-container.is-invalid .decimal-input input,
	.numeric-range-container.is-invalid .integer-input input,
	.numeric-range-container.is-invalid-input .decimal-input input,
	.numeric-range-container.is-invalid-input .integer-input input,
	div.is-invalid-input .decimal-input input,
	div.is-invalid-input .integer-input input,
	.abundances-vector-container.is-invalid .decimal-input input {
	  border: 1px solid var(--optimiser-error-color) !important;
	  border-color: var(--optimiser-error-color) !important;
	  background-image: none !important;
	  box-shadow: 0 0 0 0.25rem var(--optimiser-error-bg-strong) !important;
	}

	.weights-coefficients-container.is-invalid,
	.abundances-vector-container.is-invalid {
	  border: 1px solid var(--optimiser-error-color) !important;
	  border-color: var(--optimiser-error-color) !important;
	  box-shadow: 0 0 0 0.25rem var(--optimiser-error-bg) !important;
	}

	div.is-invalid .btn-group-container .btn-group,
	.permutation-buttons-container.is-invalid .btn-group {
	  border: 2px solid var(--optimiser-error-color) !important;
	  border-radius: 6px !important;
	  padding: 2px;
	  box-shadow: 0 0 0 0.25rem var(--optimiser-error-bg-strong) !important;
	}

	.picker-invalid-container .bootstrap-select > .dropdown-toggle {
	  border: 1px solid var(--optimiser-error-color) !important;
	  border-color: var(--optimiser-error-color) !important;
	  box-shadow: 0 0 0 0.25rem var(--optimiser-error-bg-strong) !important;
	  background-image: none !important;
	}
  "))
}
