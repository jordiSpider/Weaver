#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file WindowManagerWindows.py
# @brief WindowManagerWindows class definition
#
# @section description Description
# WindowManagerWindows class definition
#
# @section libraries Libraries/Modules
# - [msvcrt](https://docs.python.org/3.7/library/msvcrt.html) standard library
#   - Access to [getwch](https://docs.python.org/es/3.7/library/msvcrt.html#msvcrt.getwch) function
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.



# Imports
from common.toolsScript.Tools import Tools
from common.toolsScript.WindowManager import WindowManager
from common.Types import FileType


## @brief Defines a window manager on Windows. Inherits from the WindowManager class
class WindowManagerWindows(WindowManager):
    ## @brief WindowManagerWindows class initializer
    #
    # @param tools Project tools manager
    def __init__(self, tools: Tools):
        global msvcrt
        import msvcrt

        super().__init__(tools)

    def user_input(self, text, input_masking=False):
        input = super().user_input(text, input_masking)
        
        if(input_masking):
            self.print("\n", self._colors.NORMAL)

        return input

    ## @brief Close the script window
    # @note The contents of the window will be printed on the script terminal.
    def close_window(self):        
        super().close_window()
        
        # If the executable file is running and there is help text to display, it waits for a keystroke to terminate the script
        if(self._tools.file_type == FileType.EXECUTABLE):
            print("> Press ANY key to finish script: ", end='', flush=True)
            msvcrt.getwch()

