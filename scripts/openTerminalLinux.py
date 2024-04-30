#!/usr/bin/env python
# -*- coding: utf-8 -*-


import subprocess
import os
from Unix.UnixEnvVars import UnixEnvVars



def main():
    env_vars = UnixEnvVars()

    for var in ["VCPKG_ROOT"]:
        os.environ[var] = env_vars[var]
    
    # Abrir una nueva terminal Linux
    subprocess.run("gnome-terminal", shell=True, env=os.environ)


if __name__ == "__main__":
    main()
