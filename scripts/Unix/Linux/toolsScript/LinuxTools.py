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
from Unix.toolsScript.UnixTools import UnixTools
from Unix.Linux.toolsScript.projectVersion import ListProjectVersion
from common.toolsScript.projectVersion import ProjectVersion
from common.toolsScript.projectVersion.Dependencies import VersionNumberStr
from common.Types import SO_TYPE
from common.toolsScript.projectVersion.Dependencies import Package
from typing import Dict, List



## @brief Defines a project needs and functions manager for Linux. Inherits from the UnixTools class
class LinuxTools(UnixTools):
    ## @brief LinuxTools class initializer
    #
    # @param config Project configuration
    def __init__(self, tool_script_path: str, tool_script_filename: str):
        super().__init__(tool_script_path, tool_script_filename, ListProjectVersion, SO_TYPE.Linux)

    ## @brief Check if the script has administrator permissions
    def check_admin_permissions(self):
        # Execute a command as superuser to check if you have permissions
        # If you do not currently have permissions, you will be prompted to enter a password

        # Con la opción -S se ejecuta el comando en modo no interactivo
        successed, _ = self.exec_command([f"sudo -S echo"], input="\n", exit_if_error=False)
        
        if(not successed):
            passwd = self._window.user_input("Please enter your password", input_masking=True)
            successed, _ = self.exec_command([f"sudo -S echo"], input=f"{passwd}\n", exit_if_error=False)

            if(not successed):
                self.print_help("superuser privileges are required to perform this action.")
             

    ## @brief Obtain the current version of the package
    #
    # @param package_name Name of package
    # @param first_field_version Number of the first field of the version
    # @return Current version of the package
    def obtain_package_actual_version(self, package):
        if(package.name == ProjectVersion.vcpkg_package.name):
            _, actual_version = self.exec_command([f"{self._vcpkg_executable} --version"], exit_if_error=False)
        else:
            _, actual_version = self.exec_command([f"dpkg -s {package.name} | grep Version"], exit_if_error=False)

        return actual_version

    def update_packages(self):
        self.exec_command(["sudo apt update"])

    def clone_vcpkg_repo(self, package):
        self.exec_command([f"git clone --branch {package.install_version} --single-branch {self._vcpkg_repo} {self._env_vars['VCPKG_ROOT']}"])

    def exec_vcpkg_bootstrap(self):
        self.exec_command([f"{self._env_vars['VCPKG_ROOT']}{os.sep}bootstrap-vcpkg.sh"])

    def search_package_version(self, package):
        _, search_info = self.exec_command([f"apt-cache madison {package.name}"])

        for package_info in search_info.split("\n"):
            if("|" in package_info):
                package_version = package_info.split("|")[1].strip()

                if(package.install_version.version == VersionNumberStr(package_version, package.scheme).version):
                    return package_version
        
        self.print_help(f"requested version of the {package.name} package could not be found.")

    def install_apt_package(self, package):
        package_version = self.search_package_version(package)

        # Se pone un input para que no surga un error al ejecutar el comando de instalación
        self.exec_command([f"sudo apt install {package.name}={package_version} -y -qq"], input="\n")

    def install_cmake(self, package):
        self.exec_command(["wget -P .tmp https://apt.kitware.com/kitware-archive.sh"])
        # Se pone un input para que no surga un error al ejecutar el comando de instalación
        self.exec_command(["sudo sh .tmp/kitware-archive.sh"], input="\n")
        self.update_packages()

        cmake_data_package = Package("cmake-data", package.scheme, str(package.install_version), str(package.required_version), package.search_sep)

        self.install_apt_package(cmake_data_package)
        self.install_apt_package(package)

    def install_package(self, package):
        if(ProjectVersion.vcpkg_package.name == package.name):
            self.install_vcpkg(package)
        elif("cmake" == package.name):
            self.install_cmake(package)
        else:
            self.install_apt_package(package)

    def install_library(self, library, triplet):
        self.exec_command([f"{self._vcpkg_executable} install {library} --host-triplet={triplet} --triplet={triplet}"])

    ## @brief Run the script functions
    def run(self):
        # Checks if the script is executed with superuser privileges
        if(os.geteuid() == 0):
            self.print_help("script is running with superuser privileges, do not run the script with superuser privileges.")
        super().run()


