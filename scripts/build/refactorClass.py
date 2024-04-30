#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys


def refactorClass(className):
    newClassName = '_'.join(className.split('/'))
    return newClassName

def main():
    # Elimino el salto de línea de la salida estándar
    print(refactorClass(sys.argv[1]), end='')

if __name__ == "__main__":
    main()