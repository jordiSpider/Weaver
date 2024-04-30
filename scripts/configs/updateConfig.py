#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys
import argparse
from pathlib import Path
from typing import List

from Config import Config, VersionNumber
from Change import Change



class Updater:
    def __init__(self, version: str, changes: List[Change]):
        self._version: VersionNumber = VersionNumber(version)
        self._changes: List[Change] = sorted(changes)

    def update(self, config: Config) -> None:
        # Move folder changes
        for change in self._changes:
            change.applyChange(config)

        config.refresh_version(self._version)
    
    @property
    def version(self) -> VersionNumber:
        return self._version

    def __lt__(self, other: "Updater") -> bool:
        return self.version < other.version
            




if __name__ == "__main__":
    updaters_list = [
        Updater(
            version="23.11.06",
            changes=[]
        )
    ]


    updaters_list.sort()


    parser = argparse.ArgumentParser(description='Descripción de tu script')

    parser.add_argument('config_path', help='Ruta a la configuración')
    parser.add_argument('output_version', nargs='?', help='Versión a la que actualizar la configuración')

    args = parser.parse_args()

    initialConfig = Config(Path(args.config_path))

    initial_updater_index = -1
    for updater_index, updater in enumerate(updaters_list):
        if(initialConfig.version < updater.version):
            initial_updater_index = updater_index

    if(initial_updater_index == -1):
        print("Error: Configuration version does not exist.")
        sys.exit(1)


    if args.output_version is None:
        final_updater_index = len(updaters_list)
    else:
        final_version = VersionNumber(args.output_version)

        final_updater_index = -1
        for updater_index, updater in enumerate(updaters_list):
            if(final_version == updater.version):
                final_updater_index = updater_index + 1
        
        if(final_updater_index == -1):
            print("Error: Output version does not exist.")
            sys.exit(1)

    
    for updater in updaters_list[initial_updater_index:final_updater_index]:
        updater.update(initialConfig)


    initialConfig.save(Path(args.config_path + "_updated"))
