parameter_specifics_ui <- nav_panel(
  title = "3. Parameter Specifics",
  icon = icon("sliders"),
  value = "tab_specifics",
  
  tags$head(
    # Shared CSS is injected once by the orchestrator / main.R header.
    tags$script(HTML("
      /* 1. Handle the blur event for the Destinations selectors */
      $(document).on('blur', '.selectize-input input', function() {
        var $select = $(this).closest('.selectize-control').prev('select');
        if ($select.length) {
          var inputId = $select.attr('id');
          if (inputId && inputId.indexOf('destinations_') === 0) {
            var selectizeInstance = $select.selectize;
            if (selectizeInstance) {
              var currentVal = selectizeInstance.getValue();
              Shiny.setInputValue(inputId, currentVal, {priority: 'event'});
            }
          }
        }
      });

      /* 2. Block keypresses that are not digits or the decimal point */
      $(document).on('keypress', '.decimal-input input, .integer-input input', function(e) {
        var charCode = e.which;
        var isInteger = $(this).closest('.integer-input').length > 0;

        // If it is an integer field, fully block the decimal point (code 46)
        if (isInteger && charCode === 46) {
          return false;
        }

        // Only allow digits (48 to 57) and the decimal point for decimal fields
        if (charCode !== 46 && (charCode < 48 || charCode > 57)) {
          return false;
        }

        // Prevent multiple decimal points in decimal fields
        if (charCode === 46 && $(this).val().indexOf('.') !== -1) {
          return false;
        }
      });

      /* 3. Sanitize invalid characters on paste */
      $(document).on('input', '.decimal-input input, .integer-input input', function() {
        var $el = $(this);
        var isInteger = $el.closest('.integer-input').length > 0;
        var val = $el.val();

        if (isInteger) {
          // Remove any character that is not an integer digit
          var sanitized = val.replace(/[^0-9]/g, '');
          if (val !== sanitized) {
            $el.val(sanitized);
          }
        } else {
          // Existing logic for real values (RealValued)
          var sanitized = val.replace(/[^0-9.]/g, '');
          var parts = sanitized.split('.');
          if (parts.length > 2) {
            sanitized = parts[0] + '.' + parts.slice(1).join('');
          }
          if (val !== sanitized) {
            $el.val(sanitized);
          }
        }
      });

      /* 4. Clean normalization on blur */
      $(document).on('blur', '.decimal-input input, .integer-input input', function() {
        var $el = $(this);
        var isInteger = $el.closest('.integer-input').length > 0;
        var minAttrStr = $el.attr('min');
        var hasMinLimit = (typeof minAttrStr !== 'undefined' && minAttrStr !== false);
        var minLimitValue = hasMinLimit ? parseInt(minAttrStr, 10) : 0;
        
        if (isInteger) {
          var val = parseInt($el.val(), 10);
          if (isNaN(val)) {
            $el.val(hasMinLimit ? minLimitValue : 0);
          } else {
            // Specific rule for Abundances: do not allow numbers below the min attribute (0)
            if (hasMinLimit && val < minLimitValue) {
              val = minLimitValue;
            }
            $el.val(val);
          }
        } else {
          var val = parseFloat($el.val());
          if (isNaN(val)) {
            var minAttr = parseFloat($el.attr('min'));
            $el.val(isNaN(minAttr) ? 0 : minAttr);
          }
        }
        $el.trigger('change');
      });

      /* 5. Visual min/max range validation when leaving the numeric block */
      $(document).on('focusout', '.numeric-range-container', function(e) {
        var $container = $(this);

        setTimeout(function() {
          if ($container.has(document.activeElement).length === 0) {
            var paramId = $container.data('param-id');
            var adjId = $container.data('adj-id');
            var minInputId = 'min_' + paramId + '_' + adjId;
            var maxInputId = 'max_' + paramId + '_' + adjId;
            var $minInput = $container.find('[id=\"' + minInputId + '\"]');
            var $maxInput = $container.find('[id=\"' + maxInputId + '\"]');
            var minVal = parseFloat($minInput.val());
            var maxVal = parseFloat($maxInput.val());
            var hasRangeError = !isNaN(minVal) && !isNaN(maxVal) && minVal > maxVal;
            var $errorText = $container.find('.range-error-msg');
            var $minWrapper = $container.find('[id=\"wrapper_min_' + paramId + '_' + adjId + '\"]');
            var $maxWrapper = $container.find('[id=\"wrapper_max_' + paramId + '_' + adjId + '\"]');

            if (hasRangeError) {
              $container.addClass('is-invalid');
              $minWrapper.addClass('is-invalid-input');
              $maxWrapper.addClass('is-invalid-input');
              $errorText.show(); // Simply show the container R already prepared
            } else {
              $container.removeClass('is-invalid');
              $minWrapper.removeClass('is-invalid-input');
              $maxWrapper.removeClass('is-invalid-input');
              $errorText.hide();
            }

            // Notify R immediately to keep the top alert box in sync
            Shiny.setInputValue('range_status_' + paramId + '_' + adjId, {valid: !hasRangeError}, {priority: 'event'});
          }
        }, 50);
      });

      $(document).on('focusout', '.permutation-buttons-container', function(e) {
        var $container = $(this);

        // Avoid false positives when focus moves to another button within the SAME group
        setTimeout(function() {
          if ($container.has(document.activeElement).length === 0) {
            var paramId = $container.data('param-id');
            var adjId = $container.data('adj-id');

            // Count how many checkbox-style buttons are actually checked
            var selectedCount = $container.find('input[type=\"checkbox\"]:checked').length;
            var $errorText = $container.find('.permutation-error-msg');

            if (selectedCount === 0) {
              $container.addClass('is-invalid');
              $errorText.html('<p class=\"text-danger small fw-bold text-break mt-2 mb-1\"><i class=\"fa fa-circle-xmark\"></i> Error: You must select at least one element from the list.</p>').show();

              // Notify R about the invalid state in isolation
              Shiny.setInputValue('permutation_status_' + paramId + '_' + adjId, false, {priority: 'event'});
            } else {
              $container.removeClass('is-invalid');
              $errorText.empty().hide();

              // Notify R about the valid state in isolation
              Shiny.setInputValue('permutation_status_' + paramId + '_' + adjId, true, {priority: 'event'});
            }
          }
        }, 50);
      });

      /* 6. Replace whitespace with underscores live, while typing or pasting */
      $(document).on('input', '[id^=\"name_\"]', function() {
        var $el = $(this);
        // Capture spaces, tabs and line breaks and replace them with a single underscore
        var sanitized = $el.val().replace(/[ \t\r\n]+/g, '_');
        if ($el.val() !== sanitized) {
          $el.val(sanitized);
        }
      });

      /* 7. Automatically strip whitespace live (e.g. on paste) */
      $(document).on('input', '[id^=\"name_\"]', function() {
        var $el = $(this);
        // Use a character class [ ] in the regex instead of the \\s shorthand
        // so ordinary spaces, tabs and line breaks are removed without breaking R's string parser.
        var sanitized = $el.val().replace(/[ \t\r\n]+/g, '');
        if ($el.val() !== sanitized) {
          $el.val(sanitized);
        }
      });

      /* 8. Validate on blur (hide.bs.select) for the Preference instar pickers */
      $(document).on('hide.bs.select', '[id^=\"pref_predator_instars_\"], [id^=\"pref_prey_instars_\"]', function() {
        var $el = $(this);
        var inputId = $el.attr('id');
        var selectedCount = $el.val() ? $el.val().length : 0;
        var hasError = (selectedCount === 0);

        // Notify R in isolation about this component's validation state
        Shiny.setInputValue(inputId + '_status', { valid: !hasError }, { priority: 'event' });
      });
    "))
  ),

  # Dynamic Bootstrap 5 alert container in the header
  uiOutput("specifics_errors_container"),
  
  card(
    card_header("Boundaries, Destinations, & Structures"),
    card_body(
      p("Configure the distinct optimization boundaries, target data fields, and nested structures for active entities."),
      hr(),
      uiOutput("dynamic_param_editors")
    )
  )
)