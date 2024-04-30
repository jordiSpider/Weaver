#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import sys
from refactorClass import refactorClass
import platform


def listLinkLibraries(filePath, fileName, classesNames):
    classesNames = classesNames.split(';')

    if(platform.system() == "Windows"):
        filePath = filePath.replace("/", "\\")
    with open(filePath, "r", encoding="utf-8") as file:
        content = file.read()
        
    regex = re.compile('#include \"(.+)\.h\"')
    iterator = regex.finditer(content)

    libraryNames = []
    for match in iterator:
        name = match.group(1)
        
        if name in classesNames and name != fileName:
            libraryNames.append(name)
    libraryNames = [refactorClass(name) for name in libraryNames]
    libraryNames.sort()
    result = ';'.join(libraryNames)
    
    return result


def main():
    # Elimino el salto de línea de la salida estándar
    print(listLinkLibraries(sys.argv[1], sys.argv[2], sys.argv[3]), end='')

if __name__ == "__main__":
    main()
