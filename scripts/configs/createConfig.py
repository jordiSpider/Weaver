#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
from pathlib import Path
import ast

from Config import Config
from Change import SetValueJsonItemChange



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Descripción de tu script')

    parser.add_argument('base_config_path', help='Ruta a la configuración base')
    parser.add_argument('modificationsStr', help='Diccionario con las modificaciones')
    parser.add_argument('output_folder', help='Directorio de salida')
    parser.add_argument('index', help='Indice')

    args = parser.parse_args()


    output_folder_path = Path(args.output_folder)

    
    modifications = ast.literal_eval(args.modificationsStr)


    config = Config(Path(args.base_config_path))

    changesToAplly = []
    for item_info, item_value in modifications.items():
        json_file_path, item_path_str = item_info.split(":")
        
        folder_path = json_file_path.split("/")[:-1]
        json_file = json_file_path.split("/")[-1]

        item_path = item_path_str.split("/")

        changesToAplly.append(SetValueJsonItemChange(
            folder_path=folder_path,
            json_file=json_file,
            item_path=item_path,
            item_value=item_value
        ))

    for change in changesToAplly:
        change.applyChange(config)


    if(not output_folder_path.exists()):
        output_folder_path.mkdir()


    config.save(output_folder_path / (Path(args.base_config_path).name + f"_{args.index}"))
