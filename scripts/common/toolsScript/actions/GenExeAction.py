#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file GenExeAction.py
# @brief GenExeAction class definition
#
# @section description Description
# GenExeAction class definition
#
# @section libraries Libraries/Modules
# - [os](https://docs.python.org/3.7/library/os.html) standard library
#   - Access to [path](https://docs.python.org/3.7/library/os.path.html) module.
#     - Access to [join](https://docs.python.org/3.7/library/os.path.html#os.path.join) function
#     - Access to [splitext](https://docs.python.org/3.7/library/os.path.html#os.path.splitext) function
#     - Access to [relpath](https://docs.python.org/3.7/library/os.path.html#os.path.relpath) function
#     - Access to [isfile](https://docs.python.org/3.7/library/os.path.html#os.path.isfile) function
#   - Access to [getcwd](https://docs.python.org/3.7/library/os.html#os.getcwd) function
# - [shutil](https://docs.python.org/3.7/library/shutil.html) standard library
#   - Access to [rmtree](https://docs.python.org/3.7/library/shutil.html#shutil.rmtree) function
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.



# Imports
import os
import shutil
from common.toolsScript.actions import Action
from typing import List


## @brief Defines the action of generating an executable from this python script. Inherits from the Action class
class GenExeAction(Action):
    ## @brief GenExeAction class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)

    ## @brief Generates the executable from the python script
    #
    # @param SO_options List of options exclusive to the OS where the script runs
    def generate_executable(self, SO_options: List[str]):
        ## @brief Deletes files created during executable generation
        def remove_temporal_files():
            shutil.rmtree(os.path.join(self._tools.project_tools_folder, self._name))

        import PyInstaller.__main__

        # Defines the modules included in the executable
        included_imports = [ f"--hidden-import={elem}" for elem in self._tools.config["executable_python_modules_included"][self._tools.SO_name] ]

        # Executable generation options
        options = [
            "--onefile",
            "--onefile",
            "--workpath",
            os.path.join(self._tools.project_tools_folder, self._name),
            "--specpath",
            os.path.join(self._tools.project_tools_folder, self._name),
            "--distpath",
            os.getcwd(),
            *included_imports,
            "--clean",
            "--noconfirm",
            *SO_options
        ]

        # Generates the executable
        PyInstaller.__main__.run([
            os.path.join(self._tools.scripts_folder, f"{self._tools.tool_script_name}.py"),
            *options
        ])

        remove_temporal_files()

    ## @brief Displays the process of generating the executable
    def show_generate_executable(self):
        self._tools.set_future(self._tools.executor.submit(self.generate_executable))
        self._tools.window.loading("Generating executable")

    ## @brief Question to overwrite the existing executable
    #
    # @return Confirmation or rejection to overwrite existing executable
    def confirm_executable_overwrite(self) -> bool:
        return self._tools.window.yes_no_question("> There is already an executable\n", "> Do you wish to overwrite this executable?")

    ## @brief Performs the action of generating the executable
    def run_action(self):
        if(os.path.isfile(f"{self._tools.tool_script_name}.{self._tools.executable_extension}")):
            acceptToOverwrite = self.confirm_executable_overwrite()
        else:
            acceptToOverwrite = True

        if(acceptToOverwrite):
            self.show_generate_executable()


## @brief Defines the action of generating an executable from this python script on Linux. Inherits from the GenExeAction class
class GenExeActionLinux(GenExeAction):
    ## @brief GenExeActionLinux class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)

    ## @brief Generates the executable from the script
    def generate_executable(self):
        # Executable generation options on Linux
        SO_options = []
        super().generate_executable(SO_options)
        
        # Rename the executable to add the extension
        os.rename(self._tools.tool_script_name, f"{self._tools.tool_script_name}.{self._tools.executable_extension}")

## @brief Defines the action of generating an executable from this python script on Windows. Inherits from the GenExeAction class
class GenExeActionWindows(GenExeAction):
    ## @brief GenExeActionWindows class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)
    
    ## @brief Generates the executable from the script
    def generate_executable(self):
        # Executable generation options on Windows
        SO_options = [
            "--nowindowed",
            f'--icon={os.path.join(os.getcwd(), "icons", f"{self._tools.tool_script_name}.ico")}'
        ]

        super().generate_executable(SO_options)

## @brief Defines the action of generating an executable from this python script on MacOS. Inherits from the GenExeAction class
class GenExeActionDarwin(GenExeAction):
    ## @brief GenExeActionDarwin class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)
    
    ## @brief Generates the executable from the script
    def generate_executable(self):
        # Executable generation options on MacOS
        SO_options = [
            "--nowindowed",
            f'--icon={os.path.join(os.getcwd(), "icons", f"{self._tools.tool_script_name}.ico")}'
        ]

        super().generate_executable(SO_options)

