#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file WindowManager.py
# @brief WindowManager class definition
#
# @section description Description
# WindowManager class definition
#
# @section libraries Libraries/Modules
# - [time](https://docs.python.org/3.7/library/time.html) standard library
#   - Access to [sleep](https://docs.python.org/3.7/library/time.html#time.sleep) function
# - [curses](https://docs.python.org/3.7/library/curses.html) standard library
#   - Access to [init_pair](https://docs.python.org/es/3.7/library/curses.html#curses.init_pair) function
#   - Access to [initscr](https://docs.python.org/es/3.7/library/curses.html#curses.initscr) function
#   - Access to [cbreak](https://docs.python.org/es/3.7/library/curses.html#curses.cbreak) function
#   - Access to [curs_set](https://docs.python.org/es/3.7/library/curses.html#curses.curs_set) function
#   - Access to [start_color](https://docs.python.org/es/3.7/library/curses.html#curses.start_color) function
#   - Access to [use_default_colors](https://docs.python.org/es/3.7/library/curses.html#curses.use_default_colors) function
#   - Access to [endwin](https://docs.python.org/es/3.7/library/curses.html#curses.endwin) function
#   - Access to [color_pair](https://docs.python.org/es/3.7/library/curses.html#curses.color_pair) function
#   - Access to COLOR_RED predefined colors
#   - Access to COLOR_YELLOW predefined colors
#   - Access to COLOR_GREEN predefined colors
#   - Access to A_BOLD predefined style
#   - Access to KEY_UP predefined key
#   - Access to KEY_DOWN predefined key
#   - Access to KEY_ENTER predefined key
#   - Access to [Window](https://docs.python.org/es/3.7/library/curses.html#window-objects) object
#     - Access to [getyx](https://docs.python.org/es/3.7/library/curses.html#curses.window.getyx) function
#     - Access to [addstr](https://docs.python.org/es/3.7/library/curses.html#curses.window.addstr) function
#     - Access to [refresh](https://docs.python.org/es/3.7/library/curses.html#curses.window.refresh) function
#     - Access to [move](https://docs.python.org/es/3.7/library/curses.html#curses.window.move) function
#     - Access to [clrtoeol](https://docs.python.org/es/3.7/library/curses.html#curses.window.clrtoeol) function
#     - Access to [getch](https://docs.python.org/es/3.7/library/curses.html#curses.window.getch) function
#     - Access to [keypad](https://docs.python.org/es/3.7/library/curses.html#curses.window.keypad) function
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.




# Imports
import time
from common.Utilities import tab_4
from typing import List, Tuple


## @brief Defines all color configurations available for the terminal 
class Colors:
    ## @brief Colors class initializer
    def __init__(self):
        # Assign an identifier to each color configuration
        ## Default configuration
        self.NORMAL: int = 1
        ## Error configuration
        self.ERROR: int = 2
        ## Warning configuration
        self.WARN: int = 3
        ## Info configuration
        self.INFO: int = 4
        ## Green background configuration
        self.BACK_INFO: int = 5

        """
        Define all color configurations:
        - The first parameter is the identifier
        - The second parameter is the foreground color
        - The third parameter is the background color

        @note The value "-1" indicates the default color
        """
        curses.init_pair(self.NORMAL, -1, -1)
        curses.init_pair(self.ERROR, curses.COLOR_RED, -1)
        curses.init_pair(self.WARN, curses.COLOR_YELLOW, -1)
        curses.init_pair(self.INFO, curses.COLOR_GREEN, -1)
        curses.init_pair(self.BACK_INFO, -1, curses.COLOR_GREEN)


## @brief Defines a window manager
class WindowManager:
    ## @brief WindowManager class initializer
    #
    # @param tools Project tools manager
    def __init__(self, tools: "ProjectTools"):
        global curses
        import curses

        # Initializes the window
        ## Script window
        self._stdscr = curses.initscr()
        # Set window to cbreak mode
        curses.cbreak()
        # Set the course as invisible
        curses.curs_set(0)
        # Enable the use of colors
        curses.start_color()
        # Enable the use of default colors
        curses.use_default_colors()

        # Generate color configurations
        ## Color configurations
        self._colors = Colors()

        ## Loading process frames
        self._frames_loading: Tuple[str] = ("/", "|", "\\", "-")

        ## Project tools manager
        self._tools: "ProjectTools" = tools

    ## @brief Close the script window
    # @note The contents of the window will be printed on the script terminal.
    def close_window(self):
        # Get all the current contents of the window
        content = ''.join([self._stdscr.instr(line, 0).decode("utf-8") for line in range(curses.LINES)])
        # Remove the blank spaces at the end 
        content = content.rstrip()
        # Set the course as visible
        curses.curs_set(1)
        # Close the window and return the terminal to the normal state
        curses.endwin()

        # Print the content next to the help text
        if(content):
            print(f"{content}\n{self._tools.help_text}")
        else:
            print(self._tools.help_text)  

    ## @brief Displays a loading process
    #
    # @param text Text displayed in the loading process
    def loading(self, text: str):
        # Saves the current line of the window
        actual_line = self._stdscr.getyx()

        # Shows the loading process until the process running in the background has finished
        i = 0
        while(not self._tools.future.done()):
            # Displays the loading text
            self._stdscr.addstr(f"> {text}... [{self._frames_loading[i]}]", curses.color_pair(self._colors.NORMAL))
            self._stdscr.refresh()
            time.sleep(0.1)
            # Advances to the next loading symbol
            i = (i + 1) % len(self._frames_loading)
            # Returns to the initial position in the window
            self._stdscr.move(*actual_line)
            # Cleans the window from the current position to the end
            self._stdscr.clrtoeol()

        # Checks the completion status of the background process
        try:
            result = self._tools.future.result()
            future_successed = True
        except SystemExit as e:
            future_successed = False

        # Shows the completion status
        if(future_successed):
            self.show_done_job(text)
        else:
            self.show_error_job(text)

        # If the background process fails, the script is terminated
        if(not future_successed):
            # At the end of the script, as the background process will have 
            # included a help text, the reason for the failure will be shown
            self._tools.exit_script(1)

    ## @brief Displays the progress of a series of commands by means of a progress bar
    #
    # @param commands List of commands to execute
    # @param commands_text List of text to be displayed during command execution
    def progress_bar(self, commands: List[List[str]], commands_text: List[str]):
        ## @brief Displays a bar with the current progress
        #
        # @param current_step Current step
        # @param total_steps Total number of steps
        def show_bar(current_step: int, total_steps: int):
            # Calculate the percentage completed
            percentage = (100 * current_step) // total_steps

            # Calculate the number of progress bar characters to display
            num_progress_chars = percentage // 2

            # Displays the progress bar
            self._stdscr.addstr("> ", curses.color_pair(self._colors.NORMAL))
            self._stdscr.addstr(f"Progress: [{percentage:3}%]", curses.color_pair(self._colors.BACK_INFO))
            self._stdscr.addstr(f" [{progress_char*num_progress_chars}{remaining_progress_char*(50-num_progress_chars)}]\n", curses.color_pair(self._colors.NORMAL))
            self._stdscr.refresh()

        # Set the character to use for the progress bar
        progress_char = '#'
        # Set the character to be used for the remaining progress
        remaining_progress_char = '.'

        # Saves the current line of the window
        init_line = self._stdscr.getyx()

        # Total number of steps
        total_steps = len(commands)

        for current_step in range(total_steps):
            show_bar(current_step, total_steps)

            # Launchs the command in the background
            self._tools.set_future(self._tools.executor.submit(*commands[current_step]))
            self.loading(commands_text[current_step])

            time.sleep(0.5)

            # Returns to the initial position in the window
            self._stdscr.move(*init_line)
            # Cleans the window from the current position to the end
            self._stdscr.clrtobot()

        show_bar(total_steps, total_steps)

        time.sleep(0.5)

        # Returns to the initial position in the window
        self._stdscr.move(*init_line)
        # Cleans the window from the current position to the end
        self._stdscr.clrtobot()
    
    ## @brief Displays a decision question
    #
    # @param text Pre-question text
    # @param question Decision question
    # @return Acceptance or rejection of the decision
    def yes_no_question(self, text: str, question: str) -> bool:
        # Displays the pre-question text
        self._stdscr.addstr(f"{text}", curses.color_pair(self._colors.NORMAL))

        # Saves the current line of the window
        init_line = self._stdscr.getyx()

        reply = False

        finish = False
        while(not finish):
            # Set the course as visible
            curses.curs_set(1)
            self._stdscr.addstr(f"{question} [Y/n] ", curses.color_pair(self._colors.NORMAL))
            self._stdscr.refresh()

            # Capture the key pressed
            key = self._stdscr.getch()

            # Performs the function of the key pressed
            if(key == ord('Y') or key == ord('y')):
                reply = True
                finish = True
            elif(key == ord('N') or key == ord('n')):
                finish = True

            # Set the course as invisible
            curses.curs_set(0)

        # Returns to the initial position in the window
        self._stdscr.move(*init_line)
        # Cleans the window from the current position to the end
        self._stdscr.clrtobot()

        return reply

    ## @brief Displays an interactive selection menu
    #
    # @param main_text Menu main text
    # @param options_text List of options to select
    # @param add_exit Flag of the presence or absence of the Exit option
    # @return Index of the selected option
    def selection_menu(self, main_text: str, options_text: List[str], add_exit: bool = False) -> int:
        # Activates the interpretation of the escape sequences
        self._stdscr.keypad(True)

        # Saves the current line of the window
        init_line = self._stdscr.getyx()

        if(add_exit):
            # Adds the Exit option
            options_text.append("Exit")

        # Selected option
        selected_option = 0
        # Flag for displaying the warning message
        print_warning = False

        # Instructions for use of the menu
        use_instructions = "Use arrow keys to MOVE | Use enter key to SELECT"

        finish = False
        while(not finish):
            # Displays the menu
            self._stdscr.addstr(f"> {main_text} ({use_instructions}):\n", curses.color_pair(self._colors.NORMAL))
            for i in range(len(options_text)):
                if(selected_option == i):
                    self._stdscr.addstr(f"{tab_4}[*] {options_text[i]}\n", curses.A_BOLD)
                else:
                    self._stdscr.addstr(f"{tab_4}[ ] {options_text[i]}\n", curses.color_pair(self._colors.NORMAL))

            if(print_warning):
                # Displays the warning message
                self._stdscr.addstr(f"Warning: {use_instructions}\n", curses.color_pair(self._colors.WARN))
                print_warning = False
            
            self._stdscr.refresh()

            # Capture the key pressed
            key = self._stdscr.getch()

            # Performs the function of the key pressed
            if(key == curses.KEY_UP):
                if(selected_option > 0):
                    selected_option -= 1
            elif(key == curses.KEY_DOWN):
                if(selected_option < len(options_text)-1):
                    selected_option += 1
            elif(key in [curses.KEY_ENTER, 10, 13]):
                # @note The curses constant "KEY_ENTER" corresponds to the numeric 
                # value 10 on most terminals, but on some terminals it may correspond to the numeric value 13
                finish = True
            else:
                # If the pressed key has no defined function
                print_warning = True
            
            # Returns to the initial position in the window
            self._stdscr.move(*init_line)
            # Cleans the window from the current position to the end
            self._stdscr.clrtobot()

        # Deactivates the interpretation of the escape sequences
        self._stdscr.keypad(False)

        if(add_exit and (selected_option == len(options_text)-1)):
            self._tools.exit_script(0)
        else:
            return selected_option

    ## @brief Displays an interactive multiselection menu
    #
    # @param main_text Menu main text
    # @param options_text List of options to select
    # @param add_all Flag of the presence or absence of the "All options" option
    # @return Index list of selected options
    def multiselection_menu(self, main_text, options_text, add_all=False):
        # Activates the interpretation of the escape sequences
        self._stdscr.keypad(True)

        # Saves the current line of the window
        init_line = self._stdscr.getyx()

        if(add_all):
            # Adds the "All options" option
            options_text.append("All options")

        # Index list of selected options
        selected_options = []
        # Current selected option
        actual_option = 0
        # Flag for displaying the warning message
        print_warning = False

        # Instructions for use of the menu
        use_instructions = "Use arrow keys to MOVE | Use space key to CHECK | Use enter key to CONTINUE"

        finish = False
        while(not finish):
            # Displays the menu
            self._stdscr.addstr(f"> {main_text} ({use_instructions}):\n", curses.color_pair(self._colors.NORMAL))
            for i in range(len(options_text)):
                if(actual_option == i):
                    color_mode = curses.A_BOLD
                    self._stdscr.addstr(f"{tab_4}[*] ", color_mode)
                else:
                    color_mode = curses.color_pair(self._colors.NORMAL)
                    self._stdscr.addstr(f"{tab_4}[ ] ", color_mode)
                
                if(i in selected_options):
                    self._stdscr.addstr("(X) ", color_mode)
                else:
                    self._stdscr.addstr("( ) ", color_mode)

                self._stdscr.addstr(f"{options_text[i]}\n", color_mode)
            
            if(print_warning):
                # Displays the warning message
                self._stdscr.addstr(f"Warning: {use_instructions}\n", curses.color_pair(self._colors.WARN))
                print_warning = False
            
            self._stdscr.refresh()

            # Capture the key pressed
            key = self._stdscr.getch()

            # Performs the function of the key pressed
            if(key == curses.KEY_UP):
                if(actual_option > 0):
                    actual_option -= 1
            elif(key == curses.KEY_DOWN):
                if(actual_option < len(options_text)-1):
                    actual_option += 1
            elif(key == ord(' ')):
                if(add_all and actual_option == len(options_text)-1):
                    if(actual_option in selected_options):
                        selected_options = []
                    else:
                        selected_options = list(range(len(options_text)))
                else:
                    if(actual_option in selected_options):
                        selected_options.remove(actual_option)
                    else:
                        selected_options.append(actual_option)
            elif(key in [curses.KEY_ENTER, 10, 13]):
                # @note The curses constant "KEY_ENTER" corresponds to the numeric 
                # value 10 on most terminals, but on some terminals it may correspond to the numeric value 13
                finish = True
                if(len(selected_options) == 0):
                    if(not self.yes_no_question("> You haven't checked any option, ", "do you want to continue?")):
                        finish = False
            else:
                # If the pressed key has no defined function
                print_warning = True

            # Returns to the initial position in the window
            self._stdscr.move(*init_line)
            # Cleans the window from the current position to the end
            self._stdscr.clrtobot()

        # Deactivates the interpretation of the escape sequences
        self._stdscr.keypad(False)

        if(add_all and len(options_text)-1 in selected_options):
            selected_options.remove(len(options_text)-1)

        return selected_options

    def print(self, text, color):
        self._stdscr.addstr(f"{text}", curses.color_pair(color))
        self._stdscr.refresh()

    def user_input(self, text, input_masking=False):
        if(input_masking):
            curses.noecho()
        else:
            curses.echo()
        curses.curs_set(1)

        self._stdscr.addstr(f"> {text}: ", curses.color_pair(self._colors.NORMAL))
        self._stdscr.refresh()

        input = self._stdscr.getstr()
        input = input.decode('utf-8')

        curses.curs_set(0)
        curses.noecho()

        return input

    def show_option_selected(self, text, selected_option):
        self._stdscr.addstr(f"> {text}: ", curses.color_pair(self._colors.NORMAL))
        self._stdscr.addstr(f'"{selected_option}"', curses.A_BOLD)
        self._stdscr.addstr(" [", curses.color_pair(self._colors.NORMAL))
        self._stdscr.addstr("DONE", curses.color_pair(self._colors.INFO))
        self._stdscr.addstr("]\n", curses.color_pair(self._colors.NORMAL))
        self._stdscr.refresh()

    ## @brief Displays successful completion of a job
    #
    # @param text Text to describe the work performed
    def show_done_job(self, text: str):
        self._stdscr.addstr(f"> {text}... [", curses.color_pair(self._colors.NORMAL))
        self._stdscr.addstr("DONE", curses.color_pair(self._colors.INFO))
        self._stdscr.addstr("]\n", curses.color_pair(self._colors.NORMAL))
        self._stdscr.refresh()
    
    ## @brief Displays failed completion of a job
    #
    # @param text Text to describe the work performed
    def show_error_job(self, text: str):
        self._stdscr.addstr(f"> {text}... [", curses.color_pair(self._colors.NORMAL))
        self._stdscr.addstr("ERROR", curses.color_pair(self._colors.ERROR))
        self._stdscr.addstr("]\n", curses.color_pair(self._colors.NORMAL))
        self._stdscr.refresh()

    def show_warning_message(self, text: str):
        self._stdscr.addstr(f"> ", curses.color_pair(self._colors.NORMAL))
        self._stdscr.addstr(text, curses.color_pair(self._colors.WARN))
        self._stdscr.addstr("\n", curses.color_pair(self._colors.NORMAL))
