#!/usr/bin/env python
# -*- coding: utf-8 -*-


import subprocess
import os
import getpass
from Unix.UnixEnvVars import UnixEnvVars
from Windows.toolsScript.InstallWSL import InstallWSL



def main():
    env_vars = UnixEnvVars()

    WSLENV_value = ""

    for var in ["VCPKG_ROOT"]:
        os.environ[var] = env_vars[var]

        if(var == "VCPKG_ROOT"):
            if(WSLENV_value):
                WSLENV_value += f":{var}"
            else:
                WSLENV_value += f"{var}"
        else:
            # La bandera /p para traducir la ruta de Win32 o Windows a Linux.
            if(WSLENV_value):
                WSLENV_value += f":{var}/p"
            else:
                WSLENV_value += f"{var}/p"

    # La variable de entorno WSLENV se utiliza en Windows Subsystem for Linux (WSL) para controlar 
    # qué variables de entorno se comparten entre Windows y la instancia de WSL
    os.environ["WSLENV"] = WSLENV_value
    
    # Entrar al WSL
    subprocess.run(f"wsl -d Ubuntu-{InstallWSL.WSL_package.required_version}", shell=True, env=os.environ)


if __name__ == "__main__":
    main()
