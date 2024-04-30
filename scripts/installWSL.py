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
import sys
import atexit
import shutil
from Windows.toolsScript.InstallWSL import InstallWSL


def init_environment():
    os.makedirs(".tmp", exist_ok=True)

def main():
    init_environment()

    tool_script_path = os.path.abspath(os.path.dirname(__file__))
    tool_script_filename = os.path.splitext(os.path.basename(__file__))[0]

    global tools
    tools = InstallWSL(tool_script_path, tool_script_filename)

    # Run the tool script
    tools.run()

## @brief Perform all necessary actions before finishing the script
@atexit.register
def close_script():
    if(os.path.exists(".tmp")):
        shutil.rmtree(".tmp")


if __name__ == "__main__":
    main()
