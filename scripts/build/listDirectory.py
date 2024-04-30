#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os


def listDirectory(directoryPath, ignoredFiles):
    content = os.listdir(directoryPath)
    files = []
    for file in content:
        filename = file.split('.')[0]
        if not filename in ignoredFiles:
            if os.path.isfile(os.path.join(directoryPath, file)):
                files.append(filename)
            elif os.path.isdir(os.path.join(directoryPath, file)):
                filesDir = listDirectory(os.path.join(directoryPath, file), ignoredFiles)
                filesDir = [os.path.join(file, f) for f in filesDir]
                files += filesDir
    files.sort()
    return files

def main():
    ignoredFiles = sys.argv[2].split(';')
    result = listDirectory(sys.argv[1], ignoredFiles)
    result = ';'.join(result)
    # Elimino el salto de línea de la salida estándar
    print(result, end='')

if __name__ == "__main__":
    main()