#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file Types.py
# @brief Defines types used in the script
#
# @section description Description
# Defines types used in the script
#
# @section libraries Libraries/Modules
# - [enum](https://docs.python.org/3.7/library/enum.html) standard library
#   - Access to [Enum](https://docs.python.org/es/3.7/library/enum.html#enum.Enum) class
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.



# Imports
import enum



## @brief Enumeration of file types with which the tool can be run
class FileType(enum.Enum):
    ## Python script
    PYTHON_FILE = 0
    ## Executable file (depending on OS)
    EXECUTABLE = 1

## @brief Enumeration of operative systems in which the tool can be run
class SO_TYPE(enum.Enum):
    ## Windows operative system
    Windows = 0
    ## Linux operative system
    Linux = 1
    ## Mac operative system
    Darwin = 2

    ## @brief Returns a string representation of the OS
    #
    # @return String representation of the OS
    def __str__(self) -> str:
        return self.name
