ui_genetic_algorithm_card <- card(
    card_header(tags$b("Genetic Algorithm Parameters")),
    card_body(
    # Script optimized to freeze validation until the strict blur event
    tags$script(HTML("
    // 1. Block keypresses that are not digits or the decimal point
    $(document).on('keypress', '.decimal-input input', function(e) {
    var charCode = e.which;
    if (charCode !== 46 && (charCode < 48 || charCode > 57)) {
        return false;
    }
    if (charCode === 46 && $(this).val().indexOf('.') !== -1) {
        return false;
    }
    });

    // 2. Sanitize invalid characters on paste
    $(document).on('input', '.decimal-input input', function() {
    var $el = $(this);
    var sanitized = $el.val().replace(/[^0-9.]/g, '');
    var parts = sanitized.split('.');
    if (parts.length > 2) {
        sanitized = parts[0] + '.' + parts.slice(1).join('');
    }
    if ($el.val() !== sanitized) {
        $el.val(sanitized);
    }
    });

    // 3. Sync from the numeric box to the slider: ONLY ON BLUR
    $(document).on('blur', '.decimal-input input', function() {
    var $el = $(this);
    var id_num = $el.attr('id');
    var id_slider = id_num.replace('_num', ''); // Get the id of the matching slider

    var val = parseFloat($el.val());
    var min = parseFloat($el.attr('min'));
    var max = parseFloat($el.attr('max'));
    var step = parseFloat($el.attr('step')) || 0.01;

    if (isNaN(val)) val = min;
    if (val < min) val = min;
    if (val > max) val = max;

    // Snap to the step and strictly limit to 2 decimals
    var quantized = Math.round(val / step) * step;
    var fixedVal = parseFloat(quantized.toFixed(2));

    // Visually update the box
    $el.val(fixedVal);

    // Force the Shiny slider to update once focus is lost
    var $slider = $('#' + id_slider);
    if ($slider.length) {
        var ionSlider = $slider.data('ionRangeSlider');
        if (ionSlider) {
        ionSlider.update({ from: fixedVal });
        // Notify Shiny of both element changes simultaneously
        $slider.trigger('change');
        $el.trigger('change');
        }
    }
    });

    // 4. Sync from the slider to the numeric box: ONLY WHEN THE SLIDER IS RELEASED
    $(document).on('change', '.js-range-slider', function() {
    var $slider = $(this);
    var id_slider = $slider.attr('id');
    var id_num = id_slider + '_num';
    var $el = $('#' + id_num);

    if ($el.length) {
        var current_slider_val = parseFloat($slider.val());
        // Only update the numeric box if it differs, without interrupting the user
        if (parseFloat($el.val()) !== current_slider_val) {
        $el.val(current_slider_val).trigger('change');
        }
    }
    });


    // 1. Block non-numeric keystrokes in real time
    $(document).on('keypress', '.integer-input input', function(e) {
    if (e.which < 48 || e.which > 57) {
        return false;
    }
    });

    // 2. Strip non-numeric text on paste
    $(document).on('input', '.integer-input input', function() {
    var $el = $(this);
    var sanitized = $el.val().replace(/[^0-9]/g, '');
    if ($el.val() !== sanitized) {
        $el.val(sanitized);
    }
    });

    // 3. Evaluate and correct EXCLUSIVELY on 'blur' using the current min attribute
    $(document).on('blur', '.integer-input input', function() {
    var $el = $(this);
    var val = parseInt($el.val(), 10);
    var min = parseInt($el.attr('min'), 10);

    // If empty (isNaN) or below the dynamic minimum, force the correction
    if (isNaN(val) || (!isNaN(min) && val < min)) {
        $el.val(min);
        $el.trigger('change');
    }
    });
    ")),
    
    div(class = "integer-input",
    numericInput("population_size", tags$b("Population Size:"), value = 2, min = 2, step = 1)
    ),
    div(class = "integer-input",
    numericInput("max_generations", tags$b("Max Generations:"), value = 1, min = 1, step = 1)
    ),
    div(class = "integer-input",
    numericInput("tournament_size", tags$b("Tournament Size:"), value = 2, min = 2, step = 1)
    ),
    
    div(class = "my-3",
    tags$b("Crossover Probability:"),
    div(class = "d-flex align-items-center gap-3 mt-1",
        div(style = "flex-grow: 1;",
            sliderInput("crossover_prob", label = NULL, min = 0, max = 1, value = 0.8, step = 0.01, width = "100%")
        ),
        div(style = "width: 90px;", class = "decimal-input",
            numericInput("crossover_prob_num", label = NULL, value = 0.8, min = 0, max = 1, step = 0.01)
        )
    )
    ),

    div(class = "my-3",
    tags$b("Mutation Rate:"),
    div(class = "d-flex align-items-center gap-3 mt-1",
        div(style = "flex-grow: 1;",
            sliderInput("mutation_rate", label = NULL, min = 0, max = 1, value = 0.05, step = 0.01, width = "100%")
        ),
        div(style = "width: 90px;", class = "decimal-input",
            numericInput("mutation_rate_num", label = NULL, value = 0.05, min = 0, max = 1, step = 0.01)
        )
    )
    ),
    
    div(class = "mt-2",
    switchInput("keepBest", tags$b("Keep Best Individual"), value = FALSE, inline = TRUE)
    )
    )
    )