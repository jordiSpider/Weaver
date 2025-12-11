/**
 * @file OptionSelector.h
 * @brief Defines the OptionSelector widget for selecting options from a dropdown.
 *
 * This file contains the declaration of the OptionSelector class, a GUI widget
 * built using the Nana library. It provides a labeled combo box (dropdown) to
 * select one of multiple options.
 */

#ifndef OPTION_SELECTOR_H_
#define OPTION_SELECTOR_H_



#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/place.hpp>
#include <vector>
#include <string>



/**
 * @class OptionSelector
 * @brief A widget combining a label and a dropdown (combo box) for selecting options.
 *
 * This class provides a labeled dropdown using nana::panel and nana::combox.
 * The label and dropdown are arranged horizontally using nana::place.
 */
class OptionSelector : public nana::panel<true> {
public:
    /**
     * @brief Constructs an OptionSelector widget.
     * @param parent The parent window.
     * @param labelText Text to display as the label.
     * @param options List of string options to populate the dropdown.
     */
    OptionSelector(nana::window parent, const std::string& labelText, const std::vector<std::string>& options);

    /**
     * @brief Destructor for OptionSelector.
     */
    virtual ~OptionSelector();

    // Disable copy constructor and assignment
    OptionSelector(const OptionSelector& other) = delete;
	OptionSelector& operator=(const OptionSelector& other) = delete;

    /**
     * @brief Sets the current selected option by its index.
     * @param pos Index of the option to select.
     */
    void option(const size_t pos);

    /**
     * @brief Returns the index of the currently selected option.
     * @return Index of selected option.
     */
    size_t option() const;

protected:
    nana::place layout{*this};           /**< Layout manager for arranging label and dropdown */
    nana::label label{*this};            /**< Label widget */
    nana::combox optionsDropdown{*this}; /**< Combo box widget for selecting options */
};


#endif // OPTION_SELECTOR_H_