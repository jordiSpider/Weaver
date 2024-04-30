#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file WindowManagerUnix.py
# @brief WindowManagerUnix class definition
#
# @section description Description
# WindowManagerUnix class definition
#
# @section libraries Libraries/Modules
# - [sys](https://docs.python.org/3.7/library/sys.html) standard library
#   - Access to [stdin](https://docs.python.org/3.7/library/sys.html#sys.stdin) file object
# - [termios](https://docs.python.org/3.7/library/termios.html) standard library
#   - Access to [tcgetattr](https://docs.python.org/es/3.7/library/termios.html#termios.tcgetattr) function
#   - Access to [tcsetattr](https://docs.python.org/es/3.7/library/termios.html#termios.tcsetattr) function
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.



# Imports
import sys
from common.toolsScript.Tools import Tools
from common.toolsScript.WindowManager import WindowManager
from common.Types import FileType


## @brief Defines a window manager on Unix. Inherits from the WindowManager class
class WindowManagerUnix(WindowManager):
    ## @brief WindowManagerUnix class initializer
    #
    # @param tools Project tools manager
    def __init__(self, tools: Tools):
        global termios
        import termios

        super().__init__(tools)

    ## @brief Close the script window
    # @note The contents of the window will be printed on the script terminal.
    def close_window(self):
        super().close_window()
        
        # If the executable file is running and there is help text to display, it waits for a keystroke to terminate the script
        if(self._tools.file_type == FileType.EXECUTABLE):
            print("> Press ANY key to finish script: ", end='', flush=True)
            fd = sys.stdin.fileno()
            oldterm = termios.tcgetattr(fd)
            newattr = termios.tcgetattr(fd)
            # Temporarily disables the ECHO and ICANON option of the terminal
            newattr[3] = newattr[3] & ~termios.ICANON & ~termios.ECHO
            termios.tcsetattr(fd, termios.TCSANOW, newattr)
            try:
                sys.stdin.read(1)
            finally:
                termios.tcsetattr(fd, termios.TCSAFLUSH, oldterm)
            # To move the cursor to the next line
            print("")

