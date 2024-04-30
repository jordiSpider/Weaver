#!/usr/bin/env python
# -*- coding: utf-8 -*-




## @file tools.py
# @brief Python program to manage the needs and functionalities of the Weaver project
#
# @section description Description
# Python program to manage the needs and functionalities of the Weaver project
#
# @section libraries Libraries/Modules
# - [os](https://docs.python.org/3.7/library/os.html) standard library
#   - Access to [path](https://docs.python.org/3.7/library/os.path.html) module.
#     - Access to [join](https://docs.python.org/3.7/library/os.path.html#os.path.join) function
#     - Access to [splitext](https://docs.python.org/3.7/library/os.path.html#os.path.splitext) function
#     - Access to [basename](https://docs.python.org/3.7/library/os.path.html#os.path.basename) function
#   - Access to [getcwd](https://docs.python.org/3.7/library/os.html#os.getcwd) function
# - [platform](https://docs.python.org/3.7/library/platform.html) standard library
#   - Access to [system](https://docs.python.org/3.7/library/platform.html#platform.system) function
# - [json](https://docs.python.org/3.7/library/json.html) standard library
#   - Access to [load](https://docs.python.org/3.7/library/json.html#json.load) function
# - [atexit](https://docs.python.org/3.7/library/atexit.html) standard library
#   - Access to [register](https://docs.python.org/3.7/library/atexit.html#atexit.register) decorator
# - [shutil](https://docs.python.org/3.7/library/shutil.html) standard library
#   - Access to [rmtree](https://docs.python.org/3.7/library/shutil.html#shutil.rmtree) function
#
# @section notes Notes
# - This script is available on Windows, Linux and MacOS
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.



# Imports
import os
import platform
import json
import atexit
import shutil
from project_tools.Types import SO_TYPE
from project_tools.LinuxTools import LinuxTools
from project_tools.WindowsTools import WindowsTools
from project_tools.DarwinTools import DarwinTools
from project_tools.ProjectTools import ConflictingArgumentsError
from project_tools.ProjectTools import SeveralTimesArgumentError
from project_tools.ProjectTools import UnrecognizedArgumentError


def init_environment():
    os.makedirs(".tmp", exist_ok=True)

## @brief Perform all necessary actions before finishing the script
@atexit.register
def close_script():
    if(os.path.exists(".tmp")):
        shutil.rmtree(".tmp")

    tools.log.info("| ################### Script finished ###################")

def main():
    # Read configuration file
    with open(os.path.join(os.getcwd(), "scripts", "tools_config.json"), "r") as f:
        config = json.load(f)

    # Create an OS-dependent tool
    SO_name = platform.system()

    working_directory = os.getcwd()
    tool_script_base_path = os.path.abspath(os.path.dirname(__file__))
    tool_script_name = os.path.splitext(os.path.basename(__file__))[0]

    global tools
    if(SO_name == str(SO_TYPE.Linux)):
        tools = LinuxTools(config, working_directory, tool_script_base_path, tool_script_name)
    elif(SO_name == str(SO_TYPE.Windows)):
        tools = WindowsTools(config, working_directory, tool_script_base_path, tool_script_name)
    elif(SO_name == str(SO_TYPE.Darwin)):
        tools = DarwinTools(config, working_directory, tool_script_base_path, tool_script_name)

    # Parse the input arguments
    try:
        tools.parse_arguments()
    except ConflictingArgumentsError:
        tools.print_help("only one action per run can be specified.")
    except SeveralTimesArgumentError as e:
        tools.print_help(f"option '{e.option}' specified several times.")
    except UnrecognizedArgumentError as e:
        tools.print_help(f"unrecognized option '{e.option}'.")

    # Run the tool script
    tools.run()         


if __name__ == "__main__":
    main()


# Cambiar el argumento de "preset_is_available" para que reciba directamente el nombre del preset