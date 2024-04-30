#!/usr/bin/env python
# -*- coding: utf-8 -*-



import os
import sys
import shutil
import platform
import argparse
from common.Types import SO_TYPE
import PyInstaller.__main__

def generate_script_exe(script_path, output_path, output_filename, SO_options):
    ## @brief Deletes files created during executable generation
    def remove_temporal_files():
        shutil.rmtree(os.path.splitext(os.path.abspath(__file__))[0])
    
    # Comprobar si la ruta del archivo es válida
    if not os.path.isfile(script_path):
        print(f"El archivo {script_path} no existe.")
        return

    os.path.splitext(os.path.basename(__file__))[0]

    # Executable generation options
    options = [
        "--onefile",
        "--name", 
        output_filename,
        "--workpath",
        os.path.splitext(os.path.abspath(__file__))[0],
        "--specpath",
        os.path.splitext(os.path.abspath(__file__))[0],
        "--distpath",
        output_path,
        "--clean",
        "--noconfirm",
        *SO_options
    ]

    # Generates the executable
    PyInstaller.__main__.run([script_path, *options])

    remove_temporal_files()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Descripción de tu script')

    parser.add_argument('script_path', help='Ruta al archivo del script')
    parser.add_argument('output_path', help='Ruta al directorio de salida')
    parser.add_argument('output_filename', nargs='?', help='Nombre del archivo de salida')

    args = parser.parse_args()

    if args.output_filename is None:
        args.output_filename = os.path.splitext(os.path.basename(args.script_path))[0]

    SO_name = platform.system()
    SO_options = []

    if(SO_name == str(SO_TYPE.Linux)):
        pass
    elif(SO_name == str(SO_TYPE.Windows)):
        SO_options = [
            "--nowindowed"
        ]
    elif(SO_name == str(SO_TYPE.Darwin)):
        SO_options = [
            "--nowindowed"
        ]
    
    generate_script_exe(args.script_path, args.output_path, args.output_filename, SO_options)

