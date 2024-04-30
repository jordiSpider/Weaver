#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file WindowsTools.py
# @brief WindowsTools class definition
#
# @section description Description
# WindowsTools class definition
#
# @section libraries Libraries/Modules
# - [os](https://docs.python.org/3.7/library/os.html) standard library
#   - Access to [path](https://docs.python.org/3.7/library/os.path.html) module.
#     - Access to [join](https://docs.python.org/3.7/library/os.path.html#os.path.join) function
#   - Access to [remove](https://docs.python.org/3.7/library/os.html#os.remove) function
#   - Access to [environ](https://docs.python.org/3.7/library/os.html#os.environ) mapping object
# - [ctypes](https://docs.python.org/3.7/library/ctypes.html) standard library
#   - Access to [WinDLL](https://docs.python.org/3.7/library/ctypes.html#ctypes.WinDLL) class
#     - Access to shell32.IsUserAnAdmin function
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.


# Imports
import os
import ctypes
from .ProjectTools import ProjectTools
from .WindowManagerWindows import WindowManagerWindows
from .Types import SO_TYPE
from .Types import FileType
from typing import Dict



## @brief Defines a project needs and functions manager for Windows. Inherits from the ProjectTools class
class WindowsTools(ProjectTools):
    ## @brief WindowsTools class initializer
    #
    # @param config Project configuration
    def __init__(self, config: Dict, working_directory: str, tool_script_base_path: str, tool_script_name: str):
        super().__init__(config, working_directory, tool_script_base_path, tool_script_name, str(SO_TYPE.Windows))
        ## Extension of the executable generated from the script
        self._executable_extension: str = "exe"

    ## @brief Opens the script window
    def open_window(self):
        self._window: WindowManagerWindows = WindowManagerWindows(self)

    ## @brief Checks the state of the environment 
    def check_python_env(self):
        super().check_python_env()

        # Refresh the PATH environment variable
        successed, refresh_path = self.exec_command([os.path.join(self._project_tools_folder, "obtain_refresh_path_var.bat")], exit_if_error=False)
        if(successed):
            os.environ["PATH"] = refresh_path.strip()

    ## @brief Check if the script has administrator permissions
    def check_admin_permissions(self):
        if ctypes.windll.shell32.IsUserAnAdmin() == 0:
            self.print_help("administrator permissions are required to perform this action.")

    ## @brief Obtain the current version of the package
    #
    # @param package_name Name of package
    # @param first_field_version Number of the first field of the version
    # @return Current version of the package
    def obtain_package_actual_version(self, package_name: str, first_field_version: int = None) -> str:
        if(first_field_version == None):
            if(package_name == "graphviz"):
                _, actual_version = self.exec_command(["dot", "-V"], exit_if_error=False)
            elif(package_name == "python3"):
                _, actual_version = self.exec_command(["python", "--version"], exit_if_error=False)
            elif(package_name == "mingw"):
                _, actual_version = self.exec_command([f"gcc", "--version"], exit_if_error=False)
            elif(package_name == "PowerShell"):
                _, actual_version = self.exec_command(["powershell.exe", "$PSVersionTable"], exit_if_error=False)
                actual_version = actual_version.split('\n')[3]
            else:
                _, actual_version = self.exec_command([package_name, "--version"], exit_if_error=False)
        else:
            if(package_name == ".NET Framework"):
                _, actual_version = self.exec_command(["powershell.exe", f'reg query "HKLM\\SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v{first_field_version}\\Full" /v Version'], exit_if_error=False)
                actual_version = actual_version.split('\n')[2]

        return actual_version

    ## @brief Install pip with certain version
    #
    # @param package_version Pip version to install
    def install_pip(self, package_version: str):
        self.exec_command(["python", "-m", "pip", "install", f"pip=={package_version}"])

    def install_NET_Framework(self, download_link):
        self.exec_command(["powershell.exe", 
                           f"Start-BitsTransfer -Source {download_link} -Destination  .tmp\\"])
        # The /q option runs the installation in silent mode
        self.exec_command(["start", f"\".tmp\\{download_link.split('/')[-1]}\"", "/q"])

    ## @brief Install package with certain version
    #
    # @param package_name Name of package
    # @param package_version Package version to install
    def install_package(self, package_name: str, package_version: str):
        if(package_name == "pip"):
            self.install_pip(package_version)
        elif(package_name == "cmake"):
            # It is indicated in the installation arguments that the path to the Cmake executable must be added to the Path environment variable
            # For more details go to the Cmake section of the Chocolatey website
            self.exec_command(["choco", "install", "cmake", "--version", package_version, "-y", "--installargs", "'ADD_CMAKE_TO_PATH=System'"])
        elif(package_name == ".NET Framework"):
            self.install_NET_Framework(package_version)
        elif(package_name == "mingw"):
            self.exec_command(["choco", "install", package_name, "--version", package_version, "-y"])
            # Remove the version of python installed with mingw to avoid conflicts with the version of python installed on the system
            os.remove(os.path.join(os.environ["ProgramData"], "chocolatey", "bin", "python.exe"))
        else:
            self.exec_command(["choco", "install", package_name, "--version", package_version, "-y"])
    
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

                for package_name in packages_to_update:
                    package_version = self._config["initial_requirements"]["packages"][self._SO_name][package_name]
                    self.install_package(package_name, package_version)
                
                for python_module_name in python_modules_to_update:
                    python_module_version = self._config["initial_requirements"]["python_modules"][self._SO_name][python_module_name]
                    self.install_python_module(python_module_name, python_module_version)

                # To move the cursor to the next line
                print("")


