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
import pkgutil
import importlib
from abc import ABC, abstractmethod
from typing import List


## @brief Defines an action to be performed
# @warning It is an abstract class
class Action(ABC):
    @classmethod
    def get_action_classes(cls, action_names, SO_type):
        modules = []
        for module in pkgutil.walk_packages(path=__path__, prefix=__name__ + '.'):
            modules.append(module.name)

        return [ getattr(importlib.import_module(action), f"{action.split('.')[-1]}{SO_type}")() for action in modules if action.split('.')[-1] in action_names ]


   


    ## @brief Action class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", required_admin_permissions, description, SO_name):
        ## Project tools manager
        self._tools: "ProjectTools" = tools

        self._required_admin_permissions = required_admin_permissions

        self._description = description

        self._SO_name = SO_name

    @property
    def description(self):
        return self._description
    
    @property
    def required_admin_permissions(self):
        return self._required_admin_permissions

    ## @brief Executes the action
    @abstractmethod
    def run_action(self):
        pass



