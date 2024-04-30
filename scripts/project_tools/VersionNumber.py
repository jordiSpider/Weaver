#!/usr/bin/env python
# -*- coding: utf-8 -*-


## @file VersionNumber.py
# @brief VersionNumber class definition
#
# @section description Description
# VersionNumber class definition
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.


## @brief Represent the version number (X.X.X) of modules or packages 
class VersionNumber:
    ## @brief VersionNumber class initializer
    #
    # @param first_field  First field of version number
    # @param second_field Second field of version number
    # @param third_field  Third field of version number
    # @pre first_field >= 0
    # @pre second_field >= 0
    # @pre third_field >= 0
    def __init__(self, first_field: int = 0, second_field: int = 0, third_field: int = 0):
        ## First field of version number (X.x.x)
        self._first_field: int = first_field
        ## Second field of version number (x.X.x)
        self._second_field: int = second_field
        ## Third field of version number (x.x.X)
        self._third_field: int = third_field

    ## @brief Comparison of greater than or equal to between two instances of the VersionNumber class
    #
    # @param other Instance of VersionNumber to compare
    # @return Affirmation or negation of comparison
    def __ge__ (self, other: "VersionNumber") -> bool:
        if(self._first_field < other._first_field):
            return False
        elif(self._second_field < other._second_field):
            return False
        elif(self._third_field < other._third_field):
            return False
        else:
            return True
    
    ## @brief Gets the first field of version number (X.x.x)
    #
    # @return First field of version number (X.x.x)
    @property
    def first_field(self) -> int:
        return self._first_field

## @brief Represent the version number (X.X.X) of modules or packages in string format. Inherits from the VersionNumber class
class VersionNumberStr(VersionNumber):
    ## @brief VersionNumberStr class initializer
    #
    # @param version_number_str Version number in string format
    def __init__(self, version_number_str: str):
        # Obtain the fields by spliting the string by the version separator
        version_number_str = version_number_str.split(sep='.')
        version_number_str = [ int(i) for i in version_number_str ]
        super().__init__(*version_number_str)

    ## @brief Gets the first field of version number (X.x.x)
    #
    # @return First field of version number (X.x.x)
    @property
    def first_field(self) -> int:
        return self._first_field