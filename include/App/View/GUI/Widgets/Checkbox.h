/**
 * @file Checkbox.h
 * @brief Defines a custom Checkbox widget combining a label and a checkbox.
 *
 * This file contains the declaration of the Checkbox class, a GUI widget
 * built using the Nana library. It provides a labeled checkbox for user
 * interaction in forms or panels.
 */

#ifndef CHECKBOX_H_
#define CHECKBOX_H_



#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/place.hpp>
#include <vector>
#include <string>



/**
 * @class Checkbox
 * @brief A custom Checkbox widget combining a label and a checkbox in a panel.
 *
 * This class provides a simple checkbox with an associated label.
 * It uses a Nana panel to arrange the label and checkbox horizontally.
 */
class Checkbox : public nana::panel<true> {
public:
    /**
     * @brief Constructs a Checkbox widget.
     * @param parent The parent window.
     * @param labelText The text to display next to the checkbox.
     * @param defaultValue Initial state of the checkbox (checked or not).
     */
    Checkbox(nana::window parent, const std::string& labelText, const bool defaultValue);

    /**
     * @brief Destructor for Checkbox.
     */
    virtual ~Checkbox();

    // Disable copy constructor and assignment
    Checkbox(const Checkbox& other) = delete;
	Checkbox& operator=(const Checkbox& other) = delete;

    /**
     * @brief Returns the current state of the checkbox.
     * @return true if checked, false otherwise.
     */
    bool checked() const;

protected:
    nana::place layout{*this};   /**< Layout manager for arranging the label and checkbox */
    nana::label label{*this};    /**< Label widget displayed next to the checkbox */
    nana::checkbox checkbox{*this}; /**< Checkbox widget */
};


#endif // CHECKBOX_H_