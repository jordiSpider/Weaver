#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import platform

def generateSymbolicLink(originalFile, SymLinkFile):
    if(platform.system() == "Windows"):
        import win32file

        try:
            win32file.CreateSymbolicLink(SymLinkFile, originalFile, Flags=win32file.SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE)
        except:
            print("Es necesario activar el modo Desarrollador")
    else:
        import os
        os.symlink(originalFile, SymLinkFile)

def main():
    generateSymbolicLink(sys.argv[1], sys.argv[2])

if __name__ == "__main__":
    main()
