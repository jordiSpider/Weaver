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
from .ProjectTools import ProjectTools
from .WindowManagerUnix import WindowManagerUnix
from typing import Dict


## @brief Defines a project needs and functions manager for Unix. Inherits from the ProjectTools class
class UnixTools(ProjectTools):
    ## @brief UnixTools class initializer
    #
    # @param config Project configuration
    # @param SO_name Name of operative system
    def __init__(self, config: Dict, working_directory: str, tool_script_name: str, SO_name: str):
        super().__init__(config, working_directory, tool_script_name, SO_name)

    ## @brief Opens the script window
    def open_window(self):
        self._window: WindowManagerUnix = WindowManagerUnix(self)
