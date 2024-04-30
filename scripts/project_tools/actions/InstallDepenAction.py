#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file InstallDepenAction.py
# @brief InstallDepenAction class definition
#
# @section description Description
# InstallDepenAction class definition
#
# @section libraries Libraries/Modules
# - [os](https://docs.python.org/3.7/library/os.html) standard library
#   - Access to [path](https://docs.python.org/3.7/library/os.path.html) module.
#     - Access to [join](https://docs.python.org/3.7/library/os.path.html#os.path.join) function
#   - Access to [environ](https://docs.python.org/3.7/library/os.html#os.environ) mapping object.
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.


# Imports
import os
from ..Utilities import tab_4
from ..Action import Action
from typing import List, Dict



## @brief Defines the action of installing the project dependencies. Inherits from the Action class
class InstallDepenAction(Action):
    ## @brief InstallDepenAction class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)

        ## Dependencies to install
        self._dependencies_to_install: List[str] = self._values

    ## @brief Obtains the list of available dependencies
    def obtain_available_dependencies(self):
        ## List of available dependencies
        self._available_dependencies: List[str] = []

        for dependency in self._tools.config["actions"][self._name]["required_dependencies"]:
            if(not self._tools.dependencies_up_to_date[dependency]):
                self._available_dependencies.append(dependency)

    ## @brief Displays the process of obtaining the list of available dependencies
    def show_obtain_available_dependencies(self):
        self._tools.set_future(self._tools.executor.submit(self.obtain_available_dependencies))
        self._tools.window.loading("Checking dependencies")

    ## @brief Displays the selection of the dependencies to be installed
    #
    # @return List of dependencies to install
    def dependencies_choice(self) -> List[str]:
        # Obtain the project dependencies
        dependencies = self._tools.config["dependencies"]
        
        options_text = []
        for dependency in self._available_dependencies:
            options_text.append(dependencies[dependency]["description"])

        # Displays the multiselection menu
        selected_dependencies = self._tools.window.multiselection_menu("Select the dependencies to install", options_text, add_all=True)

        return [self._available_dependencies[i] for i in selected_dependencies]

    def remove_repeated_items(self):
        packages_to_install = []
        python_modules_to_install = []

        for dependency in self._dependencies_to_install:
            for package in self._tools.packages_to_update[dependency]:
                if(package in packages_to_install):
                    self._tools.remove_package_to_update(dependency, package)
                else:
                    packages_to_install.append(package)
            for python_module in self._tools.python_modules_to_update[dependency]:
                if(python_module in python_modules_to_install):
                    self._tools.remove_python_module_to_update(dependency, python_module)
                else:
                    python_modules_to_install.append(python_module)

    ## @brief Displays a summary of python packages and modules to be installed
    #
    # @return Acceptance or rejection of the installation
    def show_elements_to_install(self) -> bool:
        packages_text = ""
        python_modules_text = ""
        for dependency in self._dependencies_to_install:
            for package in self._tools.packages_to_update[dependency]:
                packages_text += f"{2*tab_4}+ {package}\n"
            for python_module in self._tools.python_modules_to_update[dependency]:
                python_modules_text += f"{2*tab_4}+ {python_module}\n"

        title_text = "> The following NEW"
        if(packages_text):
            if(python_modules_text):
                title_text += " packages and python modules"
            else:
                title_text += " packages"
        else:
            if(python_modules_text):
                title_text += " python modules"
        title_text += " will be installed:\n"

        text = title_text
        if(packages_text):
            text += f"{tab_4}- Packages:\n{packages_text}"
        if(python_modules_text):
            text += f"{tab_4}- Python modules:\n{python_modules_text}"

        return self._tools.window.yes_no_question(text, "> Do you wish to continue?")

    ## @brief Install a list of dependencies
    def install_selected_dependencies(self):
        commands = []
        commands_text = []
        
        # Obtains all the information to perform the installation.
        for dependency in self._dependencies_to_install:
            for package_name in self._tools.packages_to_update[dependency]:
                package_version = self._tools.config["dependencies"][dependency]["packages"][self._tools.SO_name][package_name]
                commands.append([self._tools.install_package, package_name, package_version])
                commands_text.append(f"Installing package \"{package_name}\" ({package_version})")
            for python_module_name in self._tools.python_modules_to_update[dependency]:
                python_module_version = self._tools.config["dependencies"][dependency]["python_modules"][self._tools.SO_name][python_module_name]
                commands.append([self._tools.install_python_module, python_module_name, python_module_version])
                commands_text.append(f"Installing python module \"{python_module_name}\" ({python_module_version})")

        # Displays an installation progress bar
        self._tools.window.progress_bar(commands, commands_text)

        # Shows that the installation has been successfully completed
        self._tools.window.show_done_job("Installing packages")

    ## @brief Performs the action of installing the dependencies
    def run_action(self):
        # If no values are preselected for the action
        if(len(self._dependencies_to_install) == 0):
            self.show_obtain_available_dependencies()
            self._dependencies_to_install = self.dependencies_choice()
        else:
            # Traverses a copy so that deletions do not affect the loop
            for value in [i for i in self._dependencies_to_install]:
                # If the selected dependency does not exist, the error is reported
                if(not value in self._tools.config["dependencies"]):
                    error_message = f"unrecognized dependency '{value}'.\n"
                    error_message += f"Valid values are {', '.join([preset for preset in self._tools.config['dependencies'].keys()])}."
                    self._tools.print_help(error_message)
                elif(self._tools.dependencies_up_to_date[value]):
                    # If the selected unit is already updated, it is removed from the list
                    self._dependencies_to_install.remove(value)
            
            if(len(self._dependencies_to_install) == 0):
                self._tools.print_help("all listed dependencies are up-to-date.")

        # If dependencies are selected
        if(self._dependencies_to_install):
            self.remove_repeated_items()

            acceptToInstall = self.show_elements_to_install()
            
            if(acceptToInstall):
                self.install_selected_dependencies()


## @brief Defines the action of installing the project dependencies on Linux. Inherits from the InstallDepenAction class
class InstallDepenActionLinux(InstallDepenAction):
    ## @brief InstallDepenActionLinux class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)
    
    ## @brief Updates the list of available packages in the repositories
    def update_packages(self):
        self._tools.exec_command(["sudo", "apt", "update"])

    ## @brief Displays the process of updating the list of available packages in the repositories
    def show_update_packages(self):
        self._tools.set_future(self._tools.executor.submit(self.update_packages))
        self._tools.window.loading("Updating packages")

    ## @brief Install a list of dependencies
    def install_selected_dependencies(self):
        self.show_update_packages()
        super().install_selected_dependencies()


## @brief Defines the action of installing the project dependencies on Windows. Inherits from the InstallDepenAction class
class InstallDepenActionWindows(InstallDepenAction):
    ## @brief InstallDepenActionWindows class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)

    ## @brief Installs the system packages needed to install the dependencies
    #
    # @param packages_to_update Dictionary with the download link of the packages to be installed
    def install_system_packages(self, packages_to_update: Dict[str,str]):
        for package_name, package_info in packages_to_update.items():
            self._tools.install_package(package_name, package_info["download_link"])

    ## @brief Displays the process of installing the system packages needed to install the dependencies
    #
    # @param packages_to_update Dictionary with the download link of the packages to be installed
    def show_install_system_packages(self, packages_to_update: Dict[str,str]):
        self._tools.set_future(self._tools.executor.submit(self.install_system_packages, packages_to_update))
        self._tools.window.loading("Installing system packages")

    ## @brief Checks the status of the system packages required by the action
    def check_required_system_packages(self):
        # Initially the global status is updated
        all_up_to_date = True
        packages_to_update = {}

        for package_name, package_info in self._tools.config["actions"][self._name]["extra_info"]["required_system_packages"].items():
            if(not self._tools.package_up_to_date(package_name, package_info["version"])):
                packages_to_update[package_name] = package_info
                if(all_up_to_date):
                    all_up_to_date = False

        if(not all_up_to_date):
            # If PowerShell is not up to date, the error is reported; because the 
            # PowerShell update requires a system restart
            if("PowerShell" in packages_to_update):
                self._tools.print_help(f'installation of the dependencies requires PowerShell v3+, you can download the package from the link "{packages_to_update["PowerShell"]["download_link"]}" and install it by clicking on it.')
            else:
                text = "You need "
                package_text = []
                for package_name, package_version in packages_to_update.items():
                    package_text.append(f"{package_name} {package_version}+")
                text += ", ".join(package_text)
                text += " to be able to use Chocolatey, to install the dependencies\n"
                reply = self._tools.window.yes_no_question(text, "> Do you want to install the packages?")
                if(not reply):
                    self._tools.exit_script(0)
                else:
                    self.show_install_system_packages(packages_to_update)

    ## @brief Install the Chocolatey package manager
    def install_chocolatey(self):
        self._tools.exec_command(
            ["powershell.exe", 
            "Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"]
        )

        # Add the path to Chocolatey to the PATH environment variable
        chocolatey_path = os.path.join(os.environ["ProgramData"], "chocolatey", "bin")
        os.environ["PATH"] = chocolatey_path + ";" + os.environ["PATH"]

    ## @brief Displays the process of installing the Chocolatey package manager
    def show_install_chocolatey(self):
        self.check_required_system_packages()

        # Check if Chocolatey is installed
        successed, _ = self._tools.exec_command(["choco", "--version"], exit_if_error=False)
        if(not successed):
            self._tools.set_future(self._tools.executor.submit(self.install_chocolatey))
            self._tools.window.loading("Installing Chocolatey")

    ## @brief Install a list of dependencies
    def install_selected_dependencies(self):
        self.show_install_chocolatey()
        super().install_selected_dependencies()


## @brief Defines the action of installing the project dependencies on MacOS. Inherits from the InstallDepenAction class
class InstallDepenActionDarwin(InstallDepenAction):
    ## @brief InstallDepenActionDarwin class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)

