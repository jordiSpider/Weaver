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
# - [argparse](https://docs.python.org/3.7/library/argparse.html) standard library
#   - Access to [ArgumentParser](https://docs.python.org/3.7/library/argparse.html#argparse.ArgumentParser) class
#   - Access to SUPPRESS attribute
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
import argparse
import subprocess
import re
from concurrent.futures import ThreadPoolExecutor, Future
from abc import ABC, abstractmethod
from .VersionNumber import VersionNumberStr, VersionNumber
from .Types import FileType
from .WindowManager import WindowManager
from .LogManager import LogManager
from . import actions
from .Action import Action
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
class ProjectTools(ABC):
    ## @brief ProjectTools class initializer
    #
    # @param config Project configuration
    # @param SO_name Name of operative system
    def __init__(self, config: Dict, working_directory: str, tool_script_name: str, SO_name: str):
        ## Project configuration
        self._config: Dict = config
        ## Name of operative system
        self._SO_name: str = SO_name
        ## Dictionary of the state of the packages
        self._packages_state: Dict[str, bool] = {}
        ## Dictionary of the state of the python modules
        self.python_modules_state: Dict[str, bool] = {}
        ## Help text for the script
        self._help_text: str = ""
        ## Working directory of the manager
        self._working_directory: str = working_directory
        ## Name of tool script
        self._tool_script_name: str = tool_script_name
        ## Name of the folder containing the scripts
        self._scripts_folder: str = "scripts"
        ## Name of the folder containing the manager's resources
        self._project_tools_folder: str = os.path.join(self._scripts_folder, "project_tools")
        ## Subprocess executor
        self._executor: ThreadPoolExecutor = ThreadPoolExecutor()
        ## Encapsulates the asynchronous execution of a callable
        self._future: Future = None
        ## Log manager
        self._log: LogManager = LogManager(self._tool_script_name, 2*1024*1024, 1)

        self._log.info("| ################### Script initiated ###################")

        ## Action to be performed by the script
        self._action: Action = None

        ## Dictionary of the classes associated with each action
        self._actions_class: Dict[str, str] = {}
        for action_name, action_info in self._config["actions"].items():
            self._actions_class[action_name] = action_info["python_class"]

        # Obtain the name of the script executable
        executable_name, _ = os.path.splitext(os.path.basename(sys.executable))

        ## Type of file that is running
        self._file_type: FileType = None

        # Decide what type of file is being run
        if("python" in executable_name):
            self._file_type = FileType.PYTHON_FILE
        else:
            self._file_type = FileType.EXECUTABLE

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
    def tool_script_name(self) -> str:
        return self._tool_script_name

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

    ## @brief Gets the working directory of the manager
    #
    # @return Working directory of the manager
    @property
    def working_directory(self) -> str:
        return self._working_directory

    ## @brief Gets the name of operative system
    #
    # @return Name of operative system
    @property
    def SO_name(self) -> str:
        return self._SO_name

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

    def remove_package_to_update(self, dependency_name: str, package_name: str):
        self._packages_to_update[dependency_name].remove(package_name)

    def remove_python_module_to_update(self, dependency_name: str, python_module_name: str):
        self._python_modules_to_update[dependency_name].remove(python_module_name)

    ## @brief Checks the state of the environment 
    def check_python_env(self):
        self.check_python_version()      

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
        
        sys.exit(exit_value)

    ## @brief Check python version requirements 
    def check_python_version(self):
        # Obtain current python version number
        actual_version = self.obtain_package_actual_version_number("python3")
        # Obtain required python version number
        required_version = VersionNumberStr(self._config["python_version"])
        # If the requirement is not met, the error is reported
        if(not actual_version >= required_version):
            self.print_help(f'this script requires a python version {self._config["python_version"]}+.')

    ## @brief Checks the status of a list of packages
    #
    # @param packages List of packages to check
    # @return Global status of the package list
    # @return List of packages to update
    def check_packages(self, packages: Dict[str, str]) -> Tuple[bool, List[str]]:
        # Initially the global status is updated
        all_up_to_date = True
        packages_to_update = []

        for package_name, package_version in packages.items():
            # Checks the status of the package
            if(not self.package_up_to_date(package_name, package_version)):
                # If it does not meet the version requirements
                # Added to the list of packages to update
                packages_to_update.append(package_name)
                # The current status is saved
                self._packages_state[package_name] = False
                if(all_up_to_date):
                    # Global status value is updated to not updated
                    all_up_to_date = False
            else:
                # The current status is saved
                self._packages_state[package_name] = True
        
        return all_up_to_date, packages_to_update

    ## @brief Obtain the current version of the python module
    #
    # @param python_module_name Name of python module
    # @return Current version of the python module
    def obtain_python_module_actual_version(self, python_module_name: str) -> str:
        successed, actual_version = self.exec_command(["pip", "show", python_module_name], exit_if_error=False)
        if(successed):
            actual_version = actual_version.split('\n')[1]

        return actual_version

    ## @brief Obtain the current version number of the python module
    #
    # @param python_module_name Name of python module
    # @return Current version number of the python module
    def obtain_python_module_actual_version_number(self, python_module_name: str) -> VersionNumber:
        python_module_actual_version = self.obtain_python_module_actual_version(python_module_name)
        return self.obtain_version_number(python_module_actual_version, with_prev_space=True)

    ## @brief Checks the status of the python module
    #
    # @param python_module_name Name of python module
    # @param python_module_required_version Required version of python module
    # @return Status of the python module
    def python_module_up_to_date(self, python_module_name: str, python_module_required_version: str) -> bool:
        required_version_number = self.obtain_version_number(python_module_required_version, with_prev_space=False)
        actual_version_number = self.obtain_python_module_actual_version_number(python_module_name)

        return actual_version_number >= required_version_number

    ## @brief Checks the status of a list of python modules
    #
    # @param python_modules List of python modules to check
    # @return Global status of the python modules list
    # @return List of python modules to update
    def check_python_modules(self, python_modules: Dict[str, str]) -> Tuple[bool, List[str]]:
        # Initially the global status is updated
        all_up_to_date = True
        python_modules_to_update = []

        for python_module_name, python_module_version in python_modules.items():
            # Checks the status of the python module
            if(not self.python_module_up_to_date(python_module_name, python_module_version)):
                # If it does not meet the version requirements
                # Added to the list of python modules to update
                python_modules_to_update.append(python_module_name)
                # The current status is saved
                self.python_modules_state[python_module_name] = False
                if(all_up_to_date):
                    # Global status value is updated to not updated
                    all_up_to_date = False
            else:
                # The current status is saved
                self.python_modules_state[python_module_name] = True
        
        return all_up_to_date, python_modules_to_update

    ## @brief Install a python module with certain version
    #
    # @param python_module_name Name of python module
    # @param python_module_version Version of python module
    def install_python_module(self, python_module_name: str, python_module_version: str):
        self.exec_command(["pip", "install", f"{python_module_name}=={python_module_version}"])

    ## @brief Displays a decision question
    #
    # @param text Pre-question text
    # @param question Decision question
    # @return Acceptance or rejection of the decision
    def yes_no_question(self, text: str, question: str) -> bool:
        # Displays the pre-question text
        print(text, end='')

        reply = False

        finish = False
        while(not finish):
            # Capture the key pressed
            key = input(f"{question} [Y/n] ")

            # Performs the function of the key pressed
            if(key == "Y" or key == "y"):
                reply = True
                finish = True
            elif(key == "N" or key == "n"):
                finish = True

        return reply

    ## @brief Checks if the initial requirements for the use of the script are met
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

    ## @brief Check if the script has administrator permissions
    @abstractmethod
    def check_admin_permissions(self):
        pass

    ## @brief Obtains the version number of a string
    #
    # @param version String containing the version number
    # @param with_prev_space Flag of the existence of spaces immediately before the version number
    # @return Version number
    def obtain_version_number(self, version: str, with_prev_space: bool) -> VersionNumber:
        # Extracts the version number from the string using a regular expression
        if(with_prev_space):
            match = re.search(r' \d+(\.\d+(\.\d+))?', version)
        else:
            match = re.search(r'\d+(\.\d+(\.\d+))?', version)

        if(match):
            # Removes spaces at the ends of the matched string
            version_number = match.group(0).strip()
            return VersionNumberStr(version_number)
        else:
            # If there are no matches, the minimum version number (0.0.0) is returned
            return VersionNumber()

    ## @brief Executes a command and manages its output
    #
    # @param command Command to execute
    # @param shell Flag for executing the command through the shell
    # @param save_log Flag to save command execution in the log
    # @param exit_if_error Flag to terminate the script if the command fails
    # @return Command execution status
    # @return Output of the command execution
    def exec_command(self, command: List[str], shell: bool = False, save_log: bool = True, exit_if_error: bool = True) -> Tuple[bool, str]:
        try:
            # Executes the command, redirecting all its outputs to the same variable
            result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True, shell=shell, text=True)
            if(save_log):
                self._log.info(f"\n=> Command: `{' '.join(command)}` | Return Code: {result.returncode} | Output:\n{result.stdout}\n{result.stderr}")
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
                self._log.error(f"\n=> Command: `{' '.join(command)}` | Return Code: {error_code} | Output:\n{output_text}")
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

    ## @brief Obtain the current version of the package
    #
    # @param package_name Name of package
    # @param first_field_version Number of the first field of the version
    # @return Current version of the package
    @abstractmethod
    def obtain_package_actual_version(self, package_name: str, first_field_version: int = None) -> str:
        pass

    ## @brief Obtain the current version number of the package
    #
    # @param package_name Name of package
    # @param first_field_version Number of the first field of the version
    # @return Current version number of the package
    def obtain_package_actual_version_number(self, package_name: str, first_field_version: int = None) -> VersionNumber:
        package_actual_version = self.obtain_package_actual_version(package_name, first_field_version)
        return self.obtain_version_number(package_actual_version, with_prev_space=True)

    ## @brief Checks the status of the package
    #
    # @param package_name Name of package
    # @param package_required_version Required version of package
    # @return Status of the package
    def package_up_to_date(self, package_name: str, package_required_version: str) -> bool:
        required_version_number = self.obtain_version_number(package_required_version, with_prev_space=False)
        if(package_name == ".NET Framework"):
            actual_version_number = self.obtain_package_actual_version_number(package_name, required_version_number.first_field)
        else:
            actual_version_number = self.obtain_package_actual_version_number(package_name)

        return actual_version_number >= required_version_number

    ## @brief Checks if the dependency is up to date
    #
    # @param dependency_name Name of dependency
    # @param dependency_info Information of dependency
    # @return Status of the dependency
    def check_dependency(self, dependency_name: str, dependency_info: Dict[str, Dict]) -> bool:
        # Initially the global status is updated
        all_up_to_date = True
        
        # Check the status of the packages
        if("packages" in dependency_info):
            packages_state, packages_to_update = self.check_packages(dependency_info["packages"][self._SO_name])
            if(all_up_to_date):
                all_up_to_date = packages_state
            # Add unupdated packages to the list of packages to be updated
            self._packages_to_update[dependency_name].extend(packages_to_update)
        
        # Check the status of the python modules
        if("python_modules" in dependency_info):
            python_modules_state, python_modules_to_update = self.check_python_modules(dependency_info["python_modules"][self._SO_name])
            if(all_up_to_date):
                all_up_to_date = python_modules_state
            # Add unupdated python modules to the list of python modules to be updated
            self._python_modules_to_update[dependency_name].extend(python_modules_to_update)

        return all_up_to_date

    ## @brief Checks the status of all dependencies
    def obtain_dependencies_state(self):
        # Obtain the project dependencies
        dependencies = self._config["dependencies"]

        ## Dictionary with the packages to be upgraded for each dependency
        self._packages_to_update: Dict[str, List[str]] = {}
        ## Dictionary with the python modules to be upgraded for each dependency
        self._python_modules_to_update: Dict[str, List[str]] = {}
        ## Dictionary of the state of the dependencies
        self._dependencies_up_to_date: Dict[str, bool] = {}

        for dependency_name, dependency_info in dependencies.items():
            self._packages_to_update[dependency_name] = []
            self._python_modules_to_update[dependency_name] = []
            state = self.check_dependency(dependency_name, dependency_info)
            # The current status is saved
            self._dependencies_up_to_date[dependency_name] = state

    ## @brief Check if the action is available
    #
    # @param required_dependencies List of dependencies required by the action
    # @param skip_if_up_to_date Flag to deactivate the action if all its dependencies are up to date
    # @return Action availability
    def action_is_available(self, required_dependencies: List[str], skip_if_up_to_date: bool) -> bool:
        is_available = True

        for dependency in required_dependencies:
            if(not self._dependencies_up_to_date[dependency]):
                is_available = False

        if(skip_if_up_to_date):
            is_available = not is_available

        return is_available

    ## @brief Obtains the list of available actions
    def obtain_available_actions(self):
        self.obtain_dependencies_state()

        ## List of available actions
        self._available_actions: List[str] = []

        for action_name, action_info in self._config["actions"].items():
            if((self._file_type == FileType.EXECUTABLE and action_info["available_in_executable"]) or self._file_type == FileType.PYTHON_FILE):
                # If the action is available in the executable file
                if(self.action_is_available(action_info["required_dependencies"], action_info["skip_if_up_to_date"])):
                    self._available_actions.append(action_name)

    ## @brief Displays the process of obtaining the list of available actions
    def show_obtain_available_actions(self):
        self._future = self._executor.submit(self.obtain_available_actions)
        self._window.loading("Checking actions")

    ## @brief Displays the selection of the action to be performed
    #
    # @return Selected option
    def actions_choice(self) -> str:
        # Obtain the project actions
        actions = self._config["actions"]

        options_text = []
        for action in self._available_actions:
            options_text.append(actions[action]["description"])

        # Displays the selection menu
        selected_action = self._window.selection_menu("Select the action", options_text, add_exit=True)

        return self._available_actions[selected_action]

    ## @brief Install pip with certain version
    #
    # @param package_version Pip version to install
    def install_pip(self, package_version: str):
        self.exec_command(["python", "-m", "pip", "install", f"pip=={package_version}"])

    ## @brief Install package with certain version
    #
    # @param package_name Name of package
    # @param package_version Package version to install
    @abstractmethod
    def install_package(self, package_name: str, package_version: str):
        pass

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
            self._help_text += f"{self._tool_script_name}.py: Error: {error_message}\n"
            self._help_text += f"{self._tool_script_name}.py: Error: Run '{self._tool_script_name}.py --help' for all supported options.\n"
        else:
            # Adds the help text generated by the argument parser
            self._help_text += parser.format_help()
        if(error_message != None):
            self.exit_script(1)
        else:
            self.exit_script(0)

    def create_dynamic_class(self, module_name: str, class_name: str) -> "ActionClass":
        action_module = globals().get("actions")
        module = getattr(action_module, module_name)
        return getattr(module, class_name)

    ## @brief Parses the script arguments
    def parse_arguments(self):
        global parser
        parser = argparse.ArgumentParser(description='Toolkit for the use of Weaver', add_help=False)
        
        # Adds all the arguments defined for the script
        for option, option_info in self._config["options"].items():
            if((self._file_type == FileType.EXECUTABLE and self._config["actions"][option_info["action"]]["available_in_executable"]) or self._file_type == FileType.PYTHON_FILE):
                if(self._config["options"][option]["have_option_value"]):
                    if(self._config["options"][option]["multiple_values"]):
                        nargs = '*'
                    else:
                        nargs = '?'
                    parser.add_argument(f"-{option}", f'--{option_info["action"]}', nargs=nargs, action='append', default=argparse.SUPPRESS, type=str, help=option_info["help"], metavar=option_info["metavar"])
                else:
                    parser.add_argument(f"-{option}", f'--{option_info["action"]}', action='store_true', default=argparse.SUPPRESS, help=option_info["help"])

        # Adds the help argument
        parser.add_argument("-h", "--help", action='store_true', default=argparse.SUPPRESS, help="Show this help message and exit")

        args, unknown = parser.parse_known_args()

        # If the help argument is specified, the script help is printed
        if(hasattr(args, 'help')):
            if(args.help):
                self.print_help()

        # If there are unknown arguments, an exception is thrown
        if(unknown):
            raise UnrecognizedArgumentError(unknown[0])

        # Checks whether multiple arguments have been specified
        if(len(vars(args)) > 1):
            raise ConflictingArgumentsError

        # Checks if you have specified the same argument several times
        for option, values in vars(args).items():
            if(isinstance(values, list)):
                if(len(values) > 1):
                    raise SeveralTimesArgumentError(option)

        # If actions have been selected
        if(vars(args)):
            # Obtains the value of the action
            for option, values in vars(args).items():
                action_name = option
                ActionClass = self.create_dynamic_class(self._actions_class[action_name], f"{self._actions_class[action_name]}{self._SO_name}")
                if(isinstance(values, list)):
                    if(values[0] == None):
                        self._action = ActionClass(self, action_name)
                    else:
                        if(isinstance(values[0], list)):
                            self._action = ActionClass(self, action_name, values[0])
                        else:
                            self._action = ActionClass(self, action_name, values)
                else:
                    self._action = ActionClass(self, action_name)
    

    ## @brief Run the script functions
    def run(self):
        self.check_python_env()

        self.check_initial_requirements()

        self.open_window()

        self.show_obtain_available_actions()

        if(self._action == None):
            action_name = self.actions_choice()
            ActionClass = self.create_dynamic_class(self._actions_class[action_name], f"{self._actions_class[action_name]}{self._SO_name}")
            self._action = ActionClass(self, action_name)
        else:
            required_dependencies = self._config["actions"][self._action.name]["required_dependencies"]
            skip_if_up_to_date = self._config["actions"][self._action.name]["skip_if_up_to_date"]
            if(not self.action_is_available(required_dependencies, skip_if_up_to_date)):
                text = f"action '{self._action.name}' is not available"
                
                dependencies_to_install = ""

                for dependency in required_dependencies:
                    if(not self._dependencies_up_to_date[dependency]):
                        dependencies_to_install += f"{dependency} "

                if(not dependencies_to_install):
                    self.print_help(f"{text}. All dependencies are up-to-date.")
                else:
                    self.print_help(f"{text}. Install the following dependencies: {dependencies_to_install}.")
    
        self._window.show_action_selected(self._action.name)

        if(self._config["actions"][self._action.name]["required_admin_permissions"]):
            self.check_admin_permissions()

        self._action.run_action()

        self.exit_script(0)

