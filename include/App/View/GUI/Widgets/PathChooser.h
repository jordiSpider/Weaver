/**
 * @file PathChooser.h
 * @brief Defines the PathChooser widget for selecting files or directories.
 *
 * This file contains the declaration of the PathChooser class, a GUI widget
 * built using the Nana library. It allows the user to choose a file or directory
 * path via a read-only textbox and a browse button. The selected path is displayed
 * in the textbox.
 */

#ifndef PATH_CHOOSER_H_
#define PATH_CHOOSER_H_



#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>

#include <filesystem>



/**
 * @class PathChooser
 * @brief A widget that allows the user to choose a file or directory path.
 *
 * Provides a label, a read-only textbox, and a browse button. Clicking the button
 * opens a file or folder dialog, and the selected path is displayed in the textbox.
 */
class PathChooser : public nana::panel<true> {
public:
    /**
     * @brief Constructs a PathChooser widget.
     * @param parent The parent window.
     * @param labelText Text to display as the label.
     * @param isDirectory True to select directories, false to select files.
     */
    PathChooser(nana::window parent, const std::string& labelText, const bool isDirectory);

    /**
     * @brief Destructor for PathChooser.
     */
    virtual ~PathChooser();

    // Disable copy constructor and assignment
    PathChooser(const PathChooser& other) = delete;
	PathChooser& operator=(const PathChooser& other) = delete;

    /**
     * @brief Returns the current path in the textbox.
     * @return Selected path.
     */
    std::filesystem::path getPath() const;

    /**
     * @brief Sets the path displayed in the textbox.
     * @param newPath Path to set.
     */
    void setPath(const std::filesystem::path& newPath);

protected:
    nana::place layout{*this};    /**< Layout manager for arranging the widgets */
    nana::label label{*this};     /**< Label widget */
    nana::textbox textbox{*this}; /**< Textbox displaying the selected path */
    nana::button button{*this};   /**< Button to open file/folder dialog */
    
    bool isDirectory;             /**< Flag indicating if selection is directory or file */

    /**
     * @brief Opens the file or folder dialog to select a path.
     *
     * Updates the textbox with the selected path.
     */
    void browsePath();
};


#endif // PATH_CHOOSER_H_