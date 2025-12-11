/**
 * @file LogTextbox.h
 * @brief Defines the LogTextbox widget for displaying log messages.
 *
 * This file contains the declaration of the LogTextbox class, a GUI widget
 * built using the Nana library. It provides a read-only, multiline textbox
 * for logging messages with thread-safe access.
 */

#ifndef LOG_TEXTBOX_H_
#define LOG_TEXTBOX_H_



#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/place.hpp>
#include <mutex>
#include <queue>
#include <string>



/**
 * @class LogTextbox
 * @brief A read-only multiline textbox widget for logging messages.
 *
 * This class provides a simple logging panel with a non-editable,
 * line-wrapped, and multi-line textbox. It allows appending messages
 * and clearing the log content.
 */
class LogTextbox : public nana::panel<true> {
public:
    /**
     * @brief Constructs the LogTextbox widget.
     * @param parent The parent window.
     */
    LogTextbox(nana::window parent);

    /**
     * @brief Destructor for LogTextbox.
     */
    virtual ~LogTextbox();

    // Disable copy constructor and assignment
    LogTextbox(const LogTextbox& other) = delete;
	LogTextbox& operator=(const LogTextbox& other) = delete;

    /**
     * @brief Appends a new line of text to the log.
     * @param text The text to append.
     */
    void append(const std::string& text);

    /**
     * @brief Clears all the text in the log.
     */
    void clean();

protected:
    nana::place layout{*this}; /**< Layout manager for arranging the textbox */
    nana::textbox log{*this};  /**< Textbox widget displaying log messages */
};


#endif // LOG_TEXTBOX_H_