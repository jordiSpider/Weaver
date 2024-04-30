#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file LinuxTools.py
# @brief LinuxTools class definition
#
# @section description Description
# LinuxTools class definition
#
# @section libraries Libraries/Modules
# - [os](https://docs.python.org/3.7/library/os.html) standard library
#   - Access to [path](https://docs.python.org/3.7/library/os.path.html) module.
#     - Access to [expanduser](https://docs.python.org/3.7/library/os.path.html#os.path.expanduser) function
#   - Access to [getcwd](https://docs.python.org/3.7/library/os.html#os.getcwd) function
#   - Access to [geteuid](https://docs.python.org/3.7/library/os.html#os.geteuid) function
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.


# Imports
import os
from .UnixTools import UnixTools
from .Types import SO_TYPE
from typing import Dict, List



## @brief Defines a project needs and functions manager for Linux. Inherits from the UnixTools class
class LinuxTools(UnixTools):
    ## @brief LinuxTools class initializer
    #
    # @param config Project configuration
    def __init__(self, config: Dict, working_directory: str, tool_script_name: str):
        super().__init__(config, working_directory, tool_script_name, str(SO_TYPE.Linux))
        ## Extension of the executable generated from the script
        self._executable_extension: str = "bin"

    ## @brief Check if the script has administrator permissions
    def check_admin_permissions(self):
        # Execute a command as superuser to check if you have permissions
        # If you do not currently have permissions, you will be prompted to enter a password
        successed, _ = self.exec_command(["sudo", "echo"], save_log=False, exit_if_error=False)
        if(not successed):
            self.print_help("superuser privileges are required to perform this action.")   

    ## @brief Obtain the current version of the package
    #
    # @param package_name Name of package
    # @param first_field_version Number of the first field of the version
    # @return Current version of the package
    def obtain_package_actual_version(self, package_name: str, first_field_version: int = None) -> str:
        if(first_field_version == None):
            if(package_name == 'cmake' or package_name == "pip"):
                _, actual_version = self.exec_command([package_name, "--version"], exit_if_error=False)
            else:
                _, actual_version = self.exec_command(["apt", "policy", package_name], exit_if_error=False)
                actual_version = actual_version.split('\n')[1]
        else:
            pass

        return actual_version

    ## @brief Install Cmake with certain version
    #
    # @param package_version Cmake version to install
    def install_cmake(self, package_version: str):
        version = ".".join(package_version.split(".")[:2])
        build = package_version.split(".")[2]

        actual_path = os.getcwd()

        self.chdir(os.path.expanduser('~'))
        
        self.exec_command(["wget", f"https://cmake.org/files/v{version}/cmake-{version}.{build}.tar.gz"])
        self.exec_command(["tar", "-xzvf", f"cmake-{version}.{build}.tar.gz"])
        self.exec_command(["rm", "-f", f"cmake-{version}.{build}.tar.gz"])
        
        self.chdir(f"cmake-{version}.{build}")

        _, nproc = self.exec_command(["nproc"])
        self.exec_command(["./bootstrap", f"--parallel={nproc}"])
        self.exec_command(["make", f"--jobs={nproc}"])
        self.exec_command(["sudo", "make", "install"])

        self.chdir(actual_path)

    ## @brief Install pip with certain version
    #
    # @param package_version Pip version to install
    def install_pip(self, package_version: str):
        self.exec_command(["sudo", "apt", "install", "python3-pip", "-y"])
        super().install_pip(package_version)

    ## @brief Install package with certain version
    #
    # @param package_name Name of package
    # @param package_version Package version to install
    def install_package(self, package_name: str, package_version: str):
        if(package_name == "cmake"):
            self.install_cmake(package_version)
        elif(package_name == "pip"):
            self.install_pip(package_version)
        else:
            self.exec_command(["sudo", "apt", "install", f"{package_name}={package_version}", "-y"])

    ## @brief Run the script functions
    def run(self):
        # Checks if the script is executed with superuser privileges
        if(os.geteuid() == 0):
            self.print_help("script is running with superuser privileges, do not run the script with superuser privileges.")
        super().run()


