#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file ProjectTools.py
# @brief ProjectTools class definition
#
# @section description Description
# ProjectTools class definition
#
# @section libraries Libraries/Modules
# - [os](https://docs.python.org/3.7/library/os.html) standard library
#   - Access to [path](https://docs.python.org/3.7/library/os.path.html) module.
#     - Access to [splitext](https://docs.python.org/3.7/library/os.path.html#os.path.splitext) function
#     - Access to [basename](https://docs.python.org/3.7/library/os.path.html#os.path.basename) function
#     - Access to [join](https://docs.python.org/3.7/library/os.path.html#os.path.join) function
#   - Access to [chdir](https://docs.python.org/3.7/library/os.html#os.chdir) function
# - [sys](https://docs.python.org/3.7/library/sys.html) standard library
#   - Access to [executable](https://docs.python.org/3.7/library/sys.html#sys.executable) attribute
#   - Access to [exit](https://docs.python.org/3.7/library/sys.html#sys.exit) function
# - [subprocess](https://docs.python.org/3.7/library/subprocess.html) standard library
#   - Access to [run](https://docs.python.org/3.7/library/subprocess.html#subprocess.run) function
#   - Access to [CalledProcessError](https://docs.python.org/3.7/library/subprocess.html#subprocess.CalledProcessError) exception class
#   - Access to [PIPE](https://docs.python.org/3.7/library/subprocess.html#subprocess.PIPE) attribute
# - [re](https://docs.python.org/3.7/library/re.html) standard library
#   - Access to [search](https://docs.python.org/3.7/library/re.html#re.search) function
# - [concurrent.futures](https://docs.python.org/3.7/library/concurrent.futures.html) standard library
#   - Access to [ThreadPoolExecutor](https://docs.python.org/3.7/library/concurrent.futures.html#threadpoolexecutor) class
#   - Access to [Future](https://docs.python.org/3.7/library/concurrent.futures.html#concurrent.futures.Future) class
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
import os
import sys
import subprocess
import json
import glob
import inspect
import importlib
from common.Utilities import tab_4
from concurrent.futures import ThreadPoolExecutor, Future
from abc import ABC, abstractmethod
from common.toolsScript.projectVersion import ProjectVersion
from common.toolsScript.projectVersion.Dependencies import VersionNumberStr, VersionNumberBasic
from common.Types import FileType
from common.toolsScript.WindowManager import WindowManager
from common.toolsScript.LogManager import LogManager
from common.toolsScript.actions import Action
from common.toolsScript.projectVersion.Dependencies import PackageScheme
from typing import Tuple, Dict, List


## @brief Defines an exception that occurs when several different arguments are specified. Inherits from the Exception class
class ConflictingArgumentsError(Exception):
    pass

## @brief Defines an exception that occurs when the same argument is specified several times. Inherits from the Exception class
class SeveralTimesArgumentError(Exception):
    ## @brief SeveralTimesArgumentError class initializer
    #
    # @param option Name of option that produced the exception
    def __init__(self, option: str):
        self.option: str = option

## @brief Defines an exception that occurs when an unknown argument is specified. Inherits from the Exception class
class UnrecognizedArgumentError(Exception):
    ## @brief UnrecognizedArgumentError class initializer
    #
    # @param option Name of option that produced the exception
    def __init__(self, option: str):
        self.option: str = option

## @brief Defines a project needs and functions manager
# @warning It is an abstract class
class Tools(ABC):
    ## @brief ProjectTools class initializer
    #
    # @param config Project configuration
    # @param SO_name Name of operative system
    def __init__(self, tool_script_path: str, tool_script_filename: str, ListProjectVersion, EnvVarsClass, SO_type):
        ## Name of operative system
        self._SO_type = SO_type

        self._env_vars = EnvVarsClass()

        self._vcpkg_executable = f"{self._env_vars['VCPKG_ROOT']}{os.sep}vcpkg"

        self._project_versions = self.get_project_versions(ListProjectVersion)

        ## Dictionary of the state of the packages
        self._packages_state: Dict[str, bool] = {}
        ## Dictionary of the state of the python modules
        self.python_modules_state: Dict[str, bool] = {}
        ## Help text for the script
        self._help_text: str = ""
        ## Name of tool script
        self._tool_script_filename: str = tool_script_filename
        ## Name of the folder containing the scripts
        self._scripts_folder: str = "scripts"
        ## Name of the folder containing the manager's resources
        self._project_tools_folder: str = os.path.join(self._scripts_folder, "project_tools")
        ## Subprocess executor
        self._executor: ThreadPoolExecutor = ThreadPoolExecutor()
        ## Encapsulates the asynchronous execution of a callable
        self._future: Future = None

        # Obtain the name of the script executable
        executable_name, _ = os.path.splitext(os.path.basename(sys.executable))

        ## Type of file that is running
        self._file_type: FileType = None

        # Decide what type of file is being run
        if("python" in executable_name):
            self._file_type = FileType.PYTHON_FILE
        else:
            self._file_type = FileType.EXECUTABLE

        ## Log manager
        if(self._file_type == FileType.EXECUTABLE):
            tool_script_path = os.path.abspath(os.path.dirname(sys.executable))

        self._log: LogManager = LogManager(tool_script_path, self._tool_script_filename, 2*1024*1024, 1)

        self._log.info("| ################### Script initiated ###################")

        ## Action to be performed by the script
        self._action: Action = None



        self._packages_to_install = []
        self._libraries_to_install = []
        self._project_version_up_to_date = False

        self._vcpkg_repo = "https://github.com/microsoft/vcpkg.git"
        


        ## Extension of the executable generated from the script
        self._executable_extension: str = None

        self._window: WindowManager = None

    ## @brief Gets the asynchronous execution of a callable
    #
    # @return Encapsulates the asynchronous execution of a callable
    @property
    def future(self) -> Future:
        return self._future

    ## @brief Gets the name of tool script
    #
    # @return Name of tool script
    @property
    def tool_script_filename(self) -> str:
        return self._tool_script_filename

    ## @brief Gets the name of the folder containing the scripts
    #
    # @return Name of the folder containing the scripts
    @property
    def scripts_folder(self) -> str:
        return self._scripts_folder

    ## @brief Gets the name of the folder containing the manager's resources
    #
    # @return Name of the folder containing the manager's resources
    @property
    def project_tools_folder(self) -> str:
        return self._project_tools_folder

    ## @brief Gets the log manager
    #
    # @return Log manager
    @property
    def log(self) -> LogManager:
        return self._log

    ## @brief Gets the extension of the executable generated from the script
    #
    # @return Extension of the executable generated from the script
    @property
    def executable_extension(self) -> str:
        return self._executable_extension

    ## @brief Gets the help text for the script
    #
    # @return Help text for the script
    @property
    def help_text(self) -> str:
        return self._help_text

    ## @brief Gets the type of file that is running
    #
    # @return Type of file that is running
    @property
    def file_type(self) -> FileType:
        return self._file_type

    ## @brief Gets the subprocess executor
    #
    # @return Subprocess executor
    @property
    def executor(self) -> ThreadPoolExecutor:
        return self._executor

    ## @brief Gets the dictionary of the state of the packages
    #
    # @return Dictionary of the state of the packages
    @property
    def packages_state(self) -> Dict[str, bool]:
        return self._packages_state

    ## @brief Gets the project configuration
    #
    # @return Project configuration
    @property
    def config(self) -> Dict:
        return self._config

    ## @brief Gets the script window manager
    #
    # @return Script window manager
    @property
    def window(self) -> WindowManager:
        return self._window 

    ## @brief Gets the dictionary with the packages to be upgraded for each dependency
    #
    # @return Dictionary with the packages to be upgraded for each dependency
    @property
    def packages_to_update(self) -> Dict[str, List[str]]:
        return self._packages_to_update

    ## @brief Gets the dictionary with the python modules to be upgraded for each dependency
    #
    # @return Dictionary with the python modules to be upgraded for each dependency
    @property
    def python_modules_to_update(self) -> Dict[str, List[str]]:
        return self._python_modules_to_update
    
    ## @brief Gets the dictionary of the state of the dependencies
    #
    # @return Dictionary of the state of the dependencies
    @property
    def dependencies_up_to_date(self) -> Dict[str, bool]:
        return self._dependencies_up_to_date

    ## @brief Sets the asynchronous execution of a callable
    #
    # @param future Encapsulates the asynchronous execution of a callable
    def set_future(self, future: Future):
        self._future = future

    ## @brief Opens the script window
    @abstractmethod
    def open_window(self):
        pass

    ## @brief Finish the script
    #
    # @param exit_value Script return code
    def exit_script(self, exit_value: int):
        # If this is an unfinished thread, a SystemExit exception is thrown
        if(self._future != None):
            if(not self._future.done()):
                raise SystemExit
        
        if(self._window != None):
            self._window.close_window()
        else:
            print(self._help_text)
        
        self.log.info("| ################### Script finished ###################")

        sys.exit(exit_value)

    ## @brief Check if the script has administrator permissions
    @abstractmethod
    def check_admin_permissions(self):
        pass

    def get_project_versions(self, ListProjectVersion):
        # Obtener todos los nombres de los elementos definidos en el módulo
        module_members = inspect.getmembers(ListProjectVersion)

        # Filtrar solo las clases definidas en el módulo
        class_names = [name for name, member in module_members if inspect.isclass(member) and member.__module__ == ListProjectVersion.__name__]
        
        return [getattr(ListProjectVersion, class_name)() for class_name in class_names]

    ## @brief Executes a command and manages its output
    #
    # @param command Command to execute
    # @param shell Flag for executing the command through the shell
    # @param save_log Flag to save command execution in the log
    # @param exit_if_error Flag to terminate the script if the command fails
    # @return Command execution status
    # @return Output of the command execution
    def exec_command(self, command: List[str], input: str = None, shell: bool = True, save_log: bool = True, exit_if_error: bool = True) -> Tuple[bool, str]:
        try:
            # Executes the command, redirecting all its outputs to the same variable
            result = subprocess.run(command, input=input, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True, shell=shell, text=True, env=os.environ)
            if(save_log):
                self._log.info(f"\n=> Command: `{' '.join(command)}` | Input: {input} | Return Code: {result.returncode} | Output:\n{result.stdout}\n{result.stderr}")
            return True, f"{result.stdout}\n{result.stderr}"
        except (subprocess.CalledProcessError, FileNotFoundError, PermissionError) as e:
            # If the command fails due to certain exceptions
            if(hasattr(e, 'returncode')):
                error_code = e.returncode
            else:
                error_code = e.errno

            if(hasattr(e, 'stdout') and hasattr(e, 'stderr')):
                output_text = f"{e.stdout}\n{e.stderr}"
            else:
                output_text = e.strerror

            if(save_log):
                self._log.error(f"\n=> Command: `{' '.join(command)} | Input: {input}` | Return Code: {error_code} | Output:\n{output_text}")
            if(exit_if_error):
                self.print_help(f"failure to execute `{' '.join(command)}` command.")
            return False, output_text
        except KeyboardInterrupt:
            # If the command fails by pressing Control+C
            if(save_log):
                self._log.error(f"\n=> Command: `{' '.join(command)}` | Return Code: ? | Output:\nKeyboardInterrupt exception")
            if(exit_if_error):
                self.print_help(f"failure to execute `{' '.join(command)}` command.")
            return False, "KeyboardInterrupt"
        except Exception as e:
            # If the command fails due to an unanticipated exception
            self.print_help(f"failure to execute `{' '.join(command)}` command, uncaptured exception `{type(e).__name__}`.")

    ## @brief Displays the selection of the action to be performed
    #
    # @return Selected option
    def actions_choice(self, actions) -> str:
        options_text = []
        for action in actions:
            options_text.append(action.description)

        # Displays the selection menu
        selected_action = self._window.selection_menu("Select the action", options_text, add_exit=True)

        return actions[selected_action]

    def project_version_choice(self):
        options_text = []
        for project_version in self._project_versions:
            options_text.append(project_version.description)

        # Displays the selection menu
        selected_project_version = self._window.selection_menu("Select the project version", options_text, add_exit=True)

        self._window.show_option_selected("Project version selected", self._project_versions[selected_project_version].description)

        return self._project_versions[selected_project_version]

    ## @brief Changes the current working directory to the given destination path
    #
    # @param destination_path Sequence of paths to move the current working directory
    def chdir(self, *destination_path):
        for path in destination_path:
            os.chdir(path)

    ## @brief Generates the help text and ends the script
    #
    # @param error_message Message explaining the error
    def print_help(self, error_message: str = None):
        if(error_message != None):
            self._help_text += f"{self._tool_script_filename}.py: Error: {error_message}\n"
            self.exit_script(1)
        else:
            self.exit_script(0)

    @abstractmethod
    def obtain_package_actual_version(self, package):
        pass

    def obtain_package_actual_version_number(self, package):
        package_actual_version = self.obtain_package_actual_version(package)
        return VersionNumberStr(package_actual_version, package.scheme, package.search_sep)

    def is_package_up_to_date(self, package):
        required_version_number = package.required_version
        actual_version_number = self.obtain_package_actual_version_number(package)

        return actual_version_number.version >= required_version_number.version

    def obtain_library_actual_version(self, library):
        _, actual_version = self.exec_command([f"{self._vcpkg_executable} list {library}"], exit_if_error=False)
        return actual_version

    def obtain_library_actual_version_number(self, library):
        library_actual_version = self.obtain_library_actual_version(library)
        return VersionNumberStr(library_actual_version, PackageScheme.Basic)

    def is_library_up_to_date(self, library):
        actual_version_number = self.obtain_library_actual_version_number(library)
        return actual_version_number.version > VersionNumberBasic(0,0,0)

    def check_project_version_status(self, project_version):
        self._packages_to_install = []

        for package in project_version.dependencies.packages:
            if(not self.is_package_up_to_date(package)):
                self._packages_to_install.append(package)

        for library in project_version.cxx_libraries.packages:
            if(not self.is_library_up_to_date(library)):
                self._libraries_to_install.append(library)

        self._project_version_up_to_date = (len(self._packages_to_install) == 0 and len(self._libraries_to_install) == 0)

    def show_check_project_version_status(self, project_version):
        self.set_future(self._executor.submit(self.check_project_version_status, project_version))
        self._window.loading("Checking version dependencies")

    def show_installation_resume(self, packages_to_install, libraries_to_install) -> bool:
        packages_text = ""
        libraries_text = ""

        for package in packages_to_install:
            packages_text += f"{2*tab_4}+ {package.name} ({package.install_version})\n"

        for library in libraries_to_install:
            libraries_text += f"{2*tab_4}+ {library}\n"

        title_text = "> The following NEW"
        if(packages_text and libraries_text):
            title_text += " packages and C++ libraries"
        else:
            if(packages_text):
                title_text += " packages"
            else:
                title_text += " C++ libraries"
        title_text += " will be installed:\n"

        text = title_text
        if(packages_text):
            text += f"{tab_4}- Packages:\n{packages_text}"
        if(libraries_text):
            text += f"{tab_4}- C++ libraries:\n{libraries_text}"

        return self._window.yes_no_question(text, "> Do you wish to continue?")

    @abstractmethod
    def update_packages(self):
        pass

    def show_update_packages(self):
        self.set_future(self._executor.submit(self.update_packages))
        self._window.loading("Updating packages")

    @abstractmethod
    def clone_vcpkg_repo(self, package):
        pass

    @abstractmethod
    def exec_vcpkg_bootstrap(self):
        pass

    def install_vcpkg(self, package):
        self.clone_vcpkg_repo(package)
        self.exec_vcpkg_bootstrap()

    @abstractmethod
    def install_package(self, package):
        pass

    def show_install_packages(self, packages_to_install):
        self.show_update_packages()
        
        commands = []
        commands_text = []
        
        # Obtains all the information to perform the installation.
        for package in packages_to_install:
            commands.append([self.install_package, package])
            commands_text.append(f"Installing package \"{package.name}\" ({package.install_version})")

        # Displays an installation progress bar
        self._window.progress_bar(commands, commands_text)

        # Shows that the installation has been successfully completed
        self._window.show_done_job("Installing packages")

    @abstractmethod
    def install_library(self, library, triplet):
        pass

    def show_install_libraries(self, libraries_to_install, triplet):
        commands = []
        commands_text = []
        
        # Obtains all the information to perform the installation.
        for library in libraries_to_install:
            commands.append([self.install_library, library, triplet])
            commands_text.append(f"Installing C++ library \"{library}\"")

        # Displays an installation progress bar
        self._window.progress_bar(commands, commands_text)

        # Shows that the installation has been successfully completed
        self._window.show_done_job("Installing C++ libraries")

    def install_project_version(self, packages_to_install, libraries_to_install, triplet):
        self.check_admin_permissions()

        continue_installation = self.show_installation_resume(packages_to_install, libraries_to_install)

        if(continue_installation):
            if(packages_to_install):
                self.show_install_packages(packages_to_install)

            if(libraries_to_install):
                self.show_install_libraries(libraries_to_install, triplet)
        else:
            self.exit_script(0)

    ## @brief Run the script functions
    def run(self):
        self.open_window()

        project_version_selected = self.project_version_choice()

        self.show_check_project_version_status(project_version_selected)

        if(not self._project_version_up_to_date):
            self.install_project_version(self._packages_to_install, self._libraries_to_install, project_version_selected.cxx_libraries.triplet)

        while(True):
            action_selected = self.actions_choice(project_version_selected.actions)

            if(action_selected.required_admin_permissions):
                self.check_admin_permissions()

            action_selected.run_action()

