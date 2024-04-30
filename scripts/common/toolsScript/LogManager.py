#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file LogManager.py
# @brief LogManager class definition
#
# @section description Description
# LogManager class definition
#
# @section libraries Libraries/Modules
# - [os](https://docs.python.org/3.7/library/os.html) standard library
#   - Access to [path](https://docs.python.org/3.7/library/os.path.html) module.
#     - Access to [getsize](https://docs.python.org/3.7/library/os.path.html#os.path.getsize) function
#     - Access to [exists](https://docs.python.org/3.7/library/os.path.html#os.path.exists) function
# - [logging](https://docs.python.org/3.7/library/logging.html) standard library
#   - Access to [basicConfig](https://docs.python.org/3.7/library/logging.html#logging.basicConfig) function
#   - Access to [info](https://docs.python.org/3.7/library/logging.html#logging.info) function
#   - Access to [error](https://docs.python.org/3.7/library/logging.html#logging.error) function
#   - Access to INFO attribute
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.


# Imports
import os
import logging


## @brief Defines a log handler 
class LogManager:
    ## @brief LogManager class initializer
    #
    # @param filename Main log name without extension
    # @param maxBytes Maximum number of bytes each log can take up
    # @param backupCount Maximum number of files that can be accumulated
    # @pre backupCount >= 0
    # @pre maxBytes > 0
    # @note If backupCount value 0, the main log will be updated depending on its size
    def __init__(self, basePath: str, filename: str, maxBytes: int, backupCount: int = 0):
        if backupCount < 0:
            raise ValueError("The value of 'backupCount' must be positive")
        ## Maximum number of files that can be accumulated
        self._backupCount: int = backupCount
        if maxBytes <= 0:
            raise ValueError("The value of 'maxBytes' must be positive")
        ## Maximum number of bytes each log can take up
        self._maxBytes: int = maxBytes
        ## Main log name
        self._filename: str = os.path.join(basePath, f'{filename}.log')

        # Logging configuration
        # The "level" parameter indicates the minimum level of information captured by the log
        # The "format" parameter indicates the format of the log entries
        logging.basicConfig(
            filename=self._filename, 
            level=logging.INFO,
            format='=> Date: %(asctime)s | Level: %(levelname)s %(message)s'
        )

    ## @brief Updates the current backup of logs
    def _refresh_backup(self):
        # If backups have been indicated
        if(self._backupCount > 0):
            # If the main log has exceeded the maximum log size
            if(os.path.getsize(self._filename) >= self._maxBytes):
                # Save the contents of the main log in a temporary variable
                with open(self._filename, "r") as file:
                    tmp_content = file.read()

                # Clean the main log
                with open(self._filename, "w") as file:
                    file.write("")
                
                # Recurrently update backups
                current_file = 1
                while(current_file <= self._backupCount and tmp_content):
                    current_filename = f'{self._filename.split(".")[0]}.{current_file}.log'
                    # Save the contents of the backup if exists
                    if(os.path.exists(current_filename)):
                        with open(current_filename, "r") as file:
                            current_file_content = file.read()
                    else:
                        current_file_content = ""

                    # Write the contents of the temporary variable in the backup
                    with open(current_filename, "w") as file:
                        file.write(tmp_content)

                    # Update the temporary variable
                    tmp_content = current_file_content

                    # Advance to the next backup
                    current_file += 1
        else:
            # If the main log has exceeded the maximum log size
            if(os.path.getsize(self._filename) >= self._maxBytes):
                with open(self._filename, "rb") as file:
                    # Place the pointer at the end of the file
                    file.seek(0, 2)
                    end_position = file.tell()
                    # Backs up the pointer by the maximum number of bytes
                    file.seek(max(0, end_position - self._maxBytes))
                    # Save the contents of the main log from the current pointer position
                    content = file.read()

                # Update the main log
                with open(self._filename, "wb") as file:
                    file.write(content)

    ## @brief Logs a message with level INFO
    #
    # @param message Message to log
    def info(self, message: str):
        logging.info(message)
        self._refresh_backup()

    ## @brief Logs a message with level ERROR
    #
    # @param message Message to log
    def error(self, message: str):
        logging.error(message)
        self._refresh_backup()
