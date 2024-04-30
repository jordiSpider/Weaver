#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys
import argparse
from pathlib import Path
from typing import List

from Config import Config, VersionNumber
from Change import Change, AddJsonItemAllFilesChange, RegexAddJsonItemAllFilesChange, MoveJsonItemAllFilesChange, RegexMoveJsonItemAllFilesChange



class Updater:
    def __init__(self, version: str, changes: List[Change]):
        self._version: VersionNumber = VersionNumber(version)
        self._changes: List[Change] = changes

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
            version="23.11.05",
            changes=[]
        ),
        Updater(
            version="23.12.01",
            changes=[
                ###################################
                #  Add traits definition section  #
                ###################################
                AddJsonItemAllFilesChange(
                    folder_path=["species"],
                    item_path=["animal", "traits", "definition"],
                    item_value={}
                ),
                ###################################################
                #  Add fixed traits to traits definition section  #
                ###################################################
                RegexAddJsonItemAllFilesChange(
                    folder_path=["species"],
                    pattern_path=[r"^animal$", r"^traits$", r"^fixedTraits$", r".*"],
                    item_regex_path=["animal", "traits", "definition", "<match_3_0>"],
                    item_value={}
                ),
                RegexAddJsonItemAllFilesChange(
                    folder_path=["species"],
                    pattern_path=[r"^animal$", r"^traits$", r"^fixedTraits$", r".*"],
                    item_regex_path=["animal", "traits", "definition", "<match_3_0>", "type"],
                    item_value="Fixed"
                ),
                RegexMoveJsonItemAllFilesChange(
                    folder_path=["species"],
                    initial_item_regex_path=[r"^animal$", r"^traits$", r"^fixedTraits$", r".*"],
                    final_item_match_path=["animal", "traits", "definition", "<match_3_0>", "value"]
                ),
                ######################################################
                #  Add variable traits to traits definition section  #
                ######################################################
                RegexAddJsonItemAllFilesChange(
                    folder_path=["species"],
                    pattern_path=[r"^animal$", r"^traits$", r"^variableTraits$", r"^minTraitsRanges$", r".*"],
                    item_regex_path=["animal", "traits", "definition", "<match_4_0>"],
                    item_value={}
                ),
                RegexAddJsonItemAllFilesChange(
                    folder_path=["species"],
                    pattern_path=[r"^animal$", r"^traits$", r"^variableTraits$", r"^minTraitsRanges$", r".*"],
                    item_regex_path=["animal", "traits", "definition", "<match_4_0>", "type"],
                    item_value="Variable"
                ),
                ######################################
                #  Move variable traits order value  #
                ######################################
                MoveJsonItemAllFilesChange(
                    folder_path=["species"],
                    initial_item_path=["animal", "traits", "variableTraits", "order"],
                    final_item_path=["animal", "traits", "variableTraitsOrder"]
                )
            ]
        )
    ]


    updaters_list.sort()


    parser = argparse.ArgumentParser(description='Descripción de tu script')

    parser.add_argument('config_path', help='Ruta a la configuración')
    parser.add_argument('output_version', nargs='?', help='Versión a la que actualizar la configuración')

    args = parser.parse_args()

    initialConfig = Config(Path(args.config_path))

    initial_updater_index = -1
    updater_index = 0
    while(updater_index < len(updaters_list) and initial_updater_index == -1):
        if(initialConfig.version == updaters_list[updater_index].version):
            initial_updater_index = updater_index + 1

        updater_index = updater_index + 1

    if(initial_updater_index == -1):
        print("Error: Configuration version does not exist.")
        sys.exit(1)


    if args.output_version is None:
        final_updater_index = len(updaters_list)
    else:
        final_version = VersionNumber(args.output_version)

        final_updater_index = -1
        updater_index = 0
        while(updater_index < len(updaters_list) and final_updater_index == -1):
            if(final_version == updaters_list[updater_index].version):
                final_updater_index = updater_index + 1

            updater_index = updater_index + 1
        
        if(final_updater_index == -1):
            print("Error: Output version does not exist.")
            sys.exit(1)

    
    for updater in updaters_list[initial_updater_index:final_updater_index]:
        updater.update(initialConfig)


    initialConfig.save(Path(args.config_path + "_updated"))
