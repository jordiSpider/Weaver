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
import sys
import atexit
import shutil
from Unix.Linux.toolsScript.LinuxTools import LinuxTools


def init_environment():
    os.makedirs(".tmp", exist_ok=True)

def main():
    init_environment()

    tool_script_path = os.path.abspath(os.path.dirname(__file__))
    tool_script_filename = os.path.splitext(os.path.basename(__file__))[0]

    tools = LinuxTools(tool_script_path, tool_script_filename)

    # Run the tool script
    tools.run()         

## @brief Perform all necessary actions before finishing the script
@atexit.register
def close_script():
    if(os.path.exists(".tmp")):
        shutil.rmtree(".tmp")


if __name__ == "__main__":
    main()
