#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file Action.py
# @brief Action class definition
#
# @section description Description
# Action class definition
#
# @section libraries Libraries/Modules
# - [abc](https://docs.python.org/3.7/library/abc.html) standard library
#   - Access to [ABC](https://docs.python.org/3.7/library/abc.html#abc.ABC) class
#   - Access to [abstractmethod](https://docs.python.org/3.7/library/abc.html#abc.abstractmethod) decorator
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.



# Imports
from abc import ABC, abstractmethod
#from .ProjectTools import ProjectTools
from typing import List


## @brief Defines an action to be performed
# @warning It is an abstract class
class Action(ABC):
    ## @brief Action class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        ## Project tools manager
        self._tools: "ProjectTools" = tools
        ## Name of action
        self._name: str = name
        ## Values chosen to perform the action
        self._values: List[str] = values

    ## @brief Gets the action name
    #
    # @return Action name
    @property
    def name(self) -> str:
        return self._name

    ## @brief Executes the action
    @abstractmethod
    def run_action(self):
        pass

