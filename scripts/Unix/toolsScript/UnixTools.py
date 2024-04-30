#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file UnixTools.py
# @brief UnixTools class definition
#
# @section description Description
# UnixTools class definition
#
# @section libraries Libraries/Modules
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.


# Imports
from common.toolsScript.Tools import Tools
from Unix.toolsScript.WindowManagerUnix import WindowManagerUnix
from Unix.UnixEnvVars import UnixEnvVars
from common.Types import FileType
from typing import Dict


## @brief Defines a project needs and functions manager for Unix. Inherits from the ProjectTools class
class UnixTools(Tools):
    ## @brief UnixTools class initializer
    #
    # @param config Project configuration
    # @param SO_name Name of operative system
    def __init__(self, tool_script_path: str, tool_script_filename: str, ListProjectVersion, SO_type):
        super().__init__(tool_script_path, tool_script_filename, ListProjectVersion, UnixEnvVars, SO_type)

    ## @brief Opens the script window
    def open_window(self):
        self._window: WindowManagerUnix = WindowManagerUnix(self)
