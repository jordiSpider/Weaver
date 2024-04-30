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
from .Types import FileType
from typing import Dict


## @brief Defines a project needs and functions manager for Unix. Inherits from the ProjectTools class
class UnixTools(ProjectTools):
    ## @brief UnixTools class initializer
    #
    # @param config Project configuration
    # @param SO_name Name of operative system
    def __init__(self, config: Dict, working_directory: str, tool_script_base_path: str, tool_script_name: str, SO_name: str):
        super().__init__(config, working_directory, tool_script_base_path, tool_script_name, SO_name)

    ## @brief Opens the script window
    def open_window(self):
        self._window: WindowManagerUnix = WindowManagerUnix(self)

    ## @brief Install pip with certain version
    #
    # @param package_version Pip version to install
    def install_pip(self, package_version: str):
        self.exec_command(["python3", "-m", "pip", "install", f"pip=={package_version}"])
    
    def check_initial_requirements(self):
        # Initially the global status is updated
        all_up_to_date = True
        
        # Check the status of the packages
        if("packages" in self._config["initial_requirements"]):
            packages_state, packages_to_update = self.check_packages(self._config["initial_requirements"]["packages"][self._SO_name])
            if(all_up_to_date):
                all_up_to_date = packages_state
        
        # Check the status of the python modules
        if("python_modules" in self._config["initial_requirements"]):
            python_modules_to_check = self._config["initial_requirements"]["python_modules"][self._SO_name]
            # If the executable file is running
            if(self._file_type == FileType.EXECUTABLE):
                # Removes checking of the python modules included in it
                for python_module_name in self._config["executable_python_modules_included"][self._SO_name]:
                    self.python_modules_state[python_module_name] = True
                    del python_modules_to_check[python_module_name]
            python_modules_state, python_modules_to_update = self.check_python_modules(python_modules_to_check)
            if(all_up_to_date):
                all_up_to_date = python_modules_state

        # If not all elements are up to date
        if(not all_up_to_date):
            # Ask if you want to update them
            reply = self.yes_no_question("> You do not meet the necessary requirements to use this script\n", "> Do you want to install the requirements?")
            if(not reply):
                self.exit_script(0)
            else:
                # If accepted, the elements are updated.
                print("> Installing requirements... ", end='')

                self.exec_command(["sudo", "apt", "update"])

                for package_name in packages_to_update:
                    package_version = self._config["initial_requirements"]["packages"][self._SO_name][package_name]
                    self.install_package(package_name, package_version)
                
                for python_module_name in python_modules_to_update:
                    python_module_version = self._config["initial_requirements"]["python_modules"][self._SO_name][python_module_name]
                    self.install_python_module(python_module_name, python_module_version)

                # To move the cursor to the next line
                print("")
