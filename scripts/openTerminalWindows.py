#!/usr/bin/env python
# -*- coding: utf-8 -*-


import subprocess
import os
from Windows.WindowsEnvVars import WindowsEnvVars



def main():
    env_vars = WindowsEnvVars()

    for var in ["VCPKG_ROOT", "MSYS2_ROOT"]:
        os.environ[var] = env_vars[var]
    
    # Añadir al PATH todos los programas instalados de MSYS2
    os.environ["PATH"] = os.environ["PATH"] + os.pathsep + f"{env_vars['MSYS2_ROOT']}{os.sep}mingw64{os.sep}bin"
    
    # Abrir una nueva terminal CMD
    subprocess.run("cmd", shell=True, env=os.environ)

    # Moverse hasta la ruta del directorio actual en la nueva terminal CMD
    subprocess.run(f"cd /d {os.getcwd()}", shell=True)


if __name__ == "__main__":
    main()
