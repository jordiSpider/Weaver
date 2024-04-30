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
import getpass
import subprocess
from Windows.toolsScript.WindowsTools import WindowsTools
from common.toolsScript.projectVersion.Dependencies import Package, PackageScheme
from Unix.Linux.toolsScript.projectVersion import ListProjectVersion
from common.toolsScript.projectVersion import ProjectVersion
from common.Types import SO_TYPE
from typing import Tuple, Dict, List



## @brief Defines a project needs and functions manager for Linux. Inherits from the UnixTools class
class InstallWSL(WindowsTools):
    WSL_package = Package("Ubuntu", PackageScheme.OS, "22.04")
    
    ## @brief LinuxTools class initializer
    #
    # @param config Project configuration
    def __init__(self, tool_script_path: str, tool_script_filename: str):
        super().__init__(tool_script_path, tool_script_filename)

        self._Windows_features = [
            "VirtualMachinePlatform",
            "Microsoft-Windows-Subsystem-Linux"
        ]

        self._all_Windows_features_enabled = True
        self._WSL_installed = False

    def check_Windows_features_status(self):
        self._all_Windows_features_enabled = True

        for feature in self._Windows_features:
            _, feature_info = self.exec_command(["powershell", "-Command", "Get-WindowsOptionalFeature", "-Online", "-FeatureName", feature])

            if("Disabled" in feature_info):
                self._all_Windows_features_enabled = False

    def show_check_Windows_features_status(self):
        self.set_future(self._executor.submit(self.check_Windows_features_status))
        self._window.loading("Checking Windows features status")

    def enable_Windows_features(self):
        for feature in self._Windows_features:
            self.exec_command(["dism.exe", "/online", "/enable-feature", f"/featurename:{feature}", "/all", "/norestart"], exit_if_error=False)

    def show_enable_Windows_features(self):
        self.set_future(self._executor.submit(self.enable_Windows_features))
        self._window.loading("Enabling Windows features")

    def check_WSL_distro(self):
        self._WSL_installed, _ = self.exec_command(["wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "echo"], exit_if_error=False)

    def show_check_WSL_distro(self):
        self.set_future(self._executor.submit(self.check_WSL_distro))
        self._window.loading("Checking Windows distribution")

    def install_WSL(self):
        self.exec_command(["curl", "-L", "-o", f".tmp{os.sep}ubuntu-22.04-jammy-wsl.tar.gz", "https://cloud-images.ubuntu.com/wsl/jammy/current/ubuntu-jammy-wsl-amd64-wsl.rootfs.tar.gz"])
        # Creación de la carpeta donde se montará la distro de WSL
        self.exec_command(["mkdir", f"c:{os.sep}Ubuntu-{InstallWSL.WSL_package.install_version}"])
        self.exec_command(["wsl", "--import", f"Ubuntu-{InstallWSL.WSL_package.install_version}", f"c:{os.sep}Ubuntu-{InstallWSL.WSL_package.install_version}", f".tmp{os.sep}ubuntu-22.04-jammy-wsl.tar.gz"])

    def show_install_WSL(self):
        self.set_future(self._executor.submit(self.install_WSL))
        self._window.loading("Installing WSL")

    def add_user(self, passwd):
        # Crear el usuario
        self.exec_command(["wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "adduser", getpass.getuser(), "--quiet", "--gecos", "", "--disabled-password"])
        # Asignarle una contraseña
        self.exec_command(["wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "usermod", "--password", f"$(openssl passwd -1 {passwd})", getpass.getuser()])
        # Darle acceso al usuario a los permisos de superusuario
        self.exec_command(["echo", getpass.getuser(), "ALL=(ALL:ALL)", "ALL", "|", "wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "tee", "-a", "/etc/sudoers"])
        
        # Fijar al usuario como el usuario por defecto de WSL
        self.exec_command(["wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "echo", "[user]", "|", "wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "tee", "-a", "/etc/wsl.conf"])
        self.exec_command(["wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "echo", f"default={getpass.getuser()}", "|", "wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "tee", "-a", "/etc/wsl.conf"])
        self.exec_command(["wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "echo", "|", "wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "tee", "-a", "/etc/wsl.conf"])
        # Darle permisos de escritura sobre los directorios de Windows al usuario
        self.exec_command(["wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "echo", "[automount]", "|", "wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "tee", "-a", "/etc/wsl.conf"])
        self.exec_command(["wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "echo", "options=metadata", "|", "wsl", "-d", f"Ubuntu-{InstallWSL.WSL_package.required_version}", "tee", "-a", "/etc/wsl.conf"])
        
        # Reinicio de WSL para hacer efectivos los cambios
        self.exec_command(["wsl", "--terminate", f"Ubuntu-{InstallWSL.WSL_package.required_version}"])

    def show_add_user(self):
        passwd = self._window.user_input("Please enter your password", input_masking=True)
        
        self.set_future(self._executor.submit(self.add_user, passwd))
        self._window.loading("Generating default user")
        

    ## @brief Run the script functions
    def run(self):
        self.open_window()

        self.check_admin_permissions()

        self.show_check_Windows_features_status()

        if(not self._all_Windows_features_enabled):
            self.show_enable_Windows_features()
            self.window.show_warning_message("Reboot the system to continue the installation")
        else:
            self.show_check_WSL_distro()
            
            if(not self._WSL_installed):
                self.show_install_WSL()
        
            self.show_add_user()

        self.exit_script(0)
