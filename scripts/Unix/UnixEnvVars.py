#!/usr/bin/env python
# -*- coding: utf-8 -*-


from pathlib import Path
import platform
import os
from common.EnvVars import EnvVars
from common.Types import SO_TYPE
from Windows.toolsScript.InstallWSL import InstallWSL
import subprocess
import getpass


class UnixEnvVars(EnvVars):
    def __init__(self):
        envVars = {}

        if(str(SO_TYPE.Linux) == platform.system()):
            envVars["HOME"] = str(Path("~").expanduser())

            super().__init__(envVars)
        elif(str(SO_TYPE.Windows) == platform.system()):
            result = subprocess.run(f"wsl -d Ubuntu-{InstallWSL.WSL_package.required_version} -u {getpass.getuser()} echo $HOME", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, text=True, env=os.environ)
            envVars["HOME"] = result.stdout.strip()

            super().__init__(envVars, separator="/")

