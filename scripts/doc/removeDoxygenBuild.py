#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import shutil

def removeDoxygenBuild(folderPath):
    if(os.path.exists(folderPath)):
        shutil.rmtree(folderPath)

def main():
    removeDoxygenBuild(sys.argv[1])

if __name__ == "__main__":
    main()
