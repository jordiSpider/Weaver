#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file DarwinTools.py
# @brief DarwinTools class definition
#
# @section description Description
# DarwinTools class definition
#
# @section libraries Libraries/Modules
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.



# Imports
from Unix.toolsScript.UnixTools import UnixTools
from common.Types import SO_TYPE
from typing import Dict



## @brief Defines a project needs and functions manager for MacOS. Inherits from the UnixTools class
class DarwinTools(UnixTools):
    ## @brief DarwinTools class initializer
    #
    # @param config Project configuration
    def __init__(self, config: Dict, working_directory: str, tool_script_base_path: str, tool_script_name: str):
        super().__init__(config, working_directory, tool_script_base_path, tool_script_name, str(SO_TYPE.Darwin))
        ## Extension of the executable generated from the script
        self._executable_extension: str = "app"

