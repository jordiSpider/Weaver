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
from common.toolsScript.Tools import Tools
from Windows.toolsScript.WindowManagerWindows import WindowManagerWindows
from common.Types import SO_TYPE
from common.Types import FileType
from typing import Dict
import pkgutil
import importlib
from common.toolsScript.projectVersion import ProjectVersion
from Windows.toolsScript.projectVersion import ListProjectVersion
from Windows.toolsScript.projectVersion import WindowsProjectVersion
from Windows.WindowsEnvVars import WindowsEnvVars



## @brief Defines a project needs and functions manager for Windows. Inherits from the ProjectTools class
class WindowsTools(Tools):
    ## @brief WindowsTools class initializer
    #
    # @param config Project configuration
    def __init__(self, tool_script_path: str, tool_script_filename: str):
        super().__init__(tool_script_path, tool_script_filename, ListProjectVersion, WindowsEnvVars, SO_TYPE.Windows)

        self._mingw_packages_repo = "https://repo.msys2.org/mingw/mingw64"
        self._msys2_repo = "https://github.com/msys2/msys2-installer/releases/download"

        os.environ["PATH"] = os.environ["PATH"] + os.pathsep + f"{self._env_vars['MSYS2_ROOT']}{os.sep}mingw64{os.sep}bin"
        os.environ["PATH"] = os.environ["PATH"] + os.pathsep + f"{self._env_vars['MSYS2_ROOT']}{os.sep}usr{os.sep}bin"

    ## @brief Opens the script window
    def open_window(self):
        self._window: WindowManagerWindows = WindowManagerWindows(self)

    ## @brief Check if the script has administrator permissions
    def check_admin_permissions(self):
        if ctypes.windll.shell32.IsUserAnAdmin() == 0:
            self.print_help("administrator permissions are required to perform this action.")

    def obtain_package_actual_version(self, package):
        if(package.name == ProjectVersion.vcpkg_package.name):
            _, actual_version = self.exec_command([self._vcpkg_executable, "--version"], exit_if_error=False)
        elif(package.name == WindowsProjectVersion.MSYS2_package.name):
            _, actual_version = self.exec_command(["pacman", "--version"], exit_if_error=False)
        else:
            _, actual_version = self.exec_command(["pacman", "-Qi", package.name], exit_if_error=False)

        return actual_version

    def update_packages(self):
        self.exec_command(["pacman", "-Sy"])
    
    def clone_vcpkg_repo(self, package):
        self.exec_command(["git", "clone", "--branch", f"{package.install_version}", "--single-branch", f"{self._vcpkg_repo}", f"{self._env_vars['VCPKG_ROOT']}"])

    def exec_vcpkg_bootstrap(self):
        self.exec_command([f"{self._env_vars['VCPKG_ROOT']}{os.sep}bootstrap-vcpkg.bat"])

    def install_MINGW_package(self, package):
        zst_file = f"{package.name}-{package.install_version}-1-any.pkg.tar.zst"
        xz_file = f"{package.name}-{package.install_version}-1-any.pkg.tar.xz"

        successful_download, _ = self.exec_command(["wget", "-P", ".tmp", f"{self._mingw_packages_repo}/{zst_file}"], exit_if_error=False)
        if(successful_download):
            self.exec_command(["pacman", "-U", "--noconfirm", f".tmp/{zst_file}"])
        else:
            self.exec_command(["wget", "-P", ".tmp", f"{self._mingw_packages_repo}/{xz_file}"])
            self.exec_command(["pacman", "-U", "--noconfirm", f".tmp/{xz_file}"])

    def install_MSYS2(self, package):
        exe_file = f"msys2-x86_64-{package.install_version.getWithoutSep()}.exe"

        self.exec_command(["curl", "-L", "-o", f".tmp{os.sep}{exe_file}", f"{self._msys2_repo}/{package.install_version.getSchemeStr()}/{exe_file}"])
        self.exec_command([f".tmp{os.sep}{exe_file}", "install", "--root", self._env_vars["MSYS2_ROOT"], "--confirm-command"])

    def show_install_MSYS2(self, package):
        self.set_future(self._executor.submit(self.install_MSYS2, package))
        self._window.loading(f"Installing MSYS2 ({package.install_version})")

    def install_package(self, package):
        if("vcpkg" == package.name):
            self.install_vcpkg(package)
        elif("MSYS2" == package.name):
            self.install_MSYS2(package)
        else:
            self.install_MINGW_package(package)

    def show_install_packages(self, packages_to_install):
        if(any(package.name == WindowsProjectVersion.MSYS2_package.name for package in packages_to_install)):
            self.show_install_MSYS2(WindowsProjectVersion.MSYS2_package)
            packages_to_install = [ package for package in packages_to_install if package.name != WindowsProjectVersion.MSYS2_package.name ]

        super().show_install_packages(packages_to_install)

    def install_library(self, library, triplet):
        self.exec_command([f"{self._vcpkg_executable}", "install", f"{library}", f"--host-triplet={triplet}", f"--triplet={triplet}"])
