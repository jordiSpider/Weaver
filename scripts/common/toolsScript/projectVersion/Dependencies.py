#!/usr/bin/env python
# -*- coding: utf-8 -*-



import enum
import re
from abc import ABC, abstractmethod




class PackageScheme(enum.Enum):
    Basic = 0
    Date = 1
    OS = 2


## @brief Represent the version number (X.X.X) of modules or packages 
class VersionNumber(ABC):
    ## @brief VersionNumber class initializer
    #
    # @param first_field  First field of version number
    # @param second_field Second field of version number
    # @param third_field  Third field of version number
    # @pre first_field >= 0
    # @pre second_field >= 0
    # @pre third_field >= 0
    def __init__(self, major: int, minor: int, patch: int):
        ## First field of version number (X.x.x)
        self._major: int = major
        ## Second field of version number (x.X.x)
        self._minor: int = minor
        ## Third field of version number (x.x.X)
        self._patch: int = patch

    ## @brief Comparison of greater than or equal to between two instances of the VersionNumber class
    #
    # @param other Instance of VersionNumber to compare
    # @return Affirmation or negation of comparison
    def __ge__ (self, other: "VersionNumber") -> bool:
        if(self._major > other._major):
            return True
        elif(self._major == other._major):
            if(self._minor > other._minor):
                return True
            elif(self._minor == other._minor):
                if(self._patch >= other._patch):
                    return True
                else:
                    return False
            else:
                return False
        else:
            return False
    
    def __gt__ (self, other: "VersionNumber") -> bool:
        if(self._major > other._major):
            return True
        elif(self._major == other._major):
            if(self._minor > other._minor):
                return True
            elif(self._minor == other._minor):
                if(self._patch > other._patch):
                    return True
                else:
                    return False
            else:
                return False
        else:
            return False
        
    def __eq__ (self, other: "VersionNumber") -> bool:
        if(self._major == other._major):
            if(self._minor == other._minor):
                if(self._patch == other._patch):
                    return True
                else:
                    return False
            else:
                return False
        else:
            return False
    
    ## @brief Gets the first field of version number (X.x.x)
    #
    # @return First field of version number (X.x.x)
    @property
    def major(self) -> int:
        return self._major

    @property
    def minor(self) -> int:
        return self._minor
    
    @property
    def patch(self) -> int:
        return self._patch
    
    @abstractmethod
    def __str__(self):
        pass

    @abstractmethod
    def getSchemeStr(self):
        pass

    @abstractmethod
    def getWithoutSep(self):
        pass


class VersionNumberBasic(VersionNumber):
    def __init__(self, major: int, minor: int = -1, patch: int = -1):
        super().__init__(major, minor, patch)

        self._separator = "."

    def __str__(self):
        result = f"{self._major}"
        if(self._minor >= 0):
            result += f"{self._separator}{self._minor}"
            if(self._patch >= 0):
                result += f"{self._separator}{self._patch}"
        
        return result

    def getSchemeStr(self):
        return str(self)
    
    def getWithoutSep(self):
        result = f"{self._major}"
        if(self._minor >= 0):
            result += f"{self._minor}"
            if(self._patch >= 0):
                result += f"{self._patch}"
        
        return result


class VersionNumberDate(VersionNumber):
    def __init__(self, major: int, minor: int = -1, patch: int = -1):
        super().__init__(major, minor, patch)

        self._separator = "-"

    def __str__(self):
        result = f"{self._major}"
        if(self._minor >= 0):
            result += "."
            if(self._minor < 10):
                result += f"0{self._minor}"
            else:
                result += f"{self._minor}"

            if(self._patch >= 0):
                result += "."
                if(self._patch < 10):
                    result += f"0{self._patch}"
                else:
                    result += f"{self._patch}"

        return result

    def getSchemeStr(self):
        result = f"{self._major}"
        if(self._minor >= 0):
            result += self._separator
            if(self._minor < 10):
                result += f"0{self._minor}"
            else:
                result += f"{self._minor}"

            if(self._patch >= 0):
                result += self._separator
                if(self._patch < 10):
                    result += f"0{self._patch}"
                else:
                    result += f"{self._patch}"

        return result
    
    def getWithoutSep(self):
        result = f"{self._major}"
        if(self._minor >= 0):
            if(self._minor < 10):
                result += f"0{self._minor}"
            else:
                result += f"{self._minor}"

            if(self._patch >= 0):
                if(self._patch < 10):
                    result += f"0{self._patch}"
                else:
                    result += f"{self._patch}"

        return result


class VersionNumberOS(VersionNumber):
    def __init__(self, major: int, minor: int = -1, patch: int = -1):
        super().__init__(major, minor, patch)

        self._separator = "."

    def __str__(self):
        result = f"{self._major}"
        if(self._minor >= 0):
            result += self._separator
            if(self._minor < 10):
                result += f"0{self._minor}"
            else:
                result += f"{self._minor}"

            if(self._patch >= 0):
                result += self._separator
                if(self._patch < 10):
                    result += f"0{self._patch}"
                else:
                    result += f"{self._patch}"

        return result

    def getSchemeStr(self):
        return str(self)
    
    def getWithoutSep(self):
        result = f"{self._major}"
        if(self._minor >= 0):
            if(self._minor < 10):
                result += f"0{self._minor}"
            else:
                result += f"{self._minor}"

            if(self._patch >= 0):
                if(self._patch < 10):
                    result += f"0{self._patch}"
                else:
                    result += f"{self._patch}"

        return result





## @brief Represent the version number (X.X.X) of modules or packages in string format. Inherits from the VersionNumber class
class VersionNumberStr:
    ## @brief VersionNumberStr class initializer
    #
    # @param version_number_str Version number in string format
    def __init__(self, version_number_str: str, scheme, separator="."):
        # Extracts the version number from the string using a regular expression
        if(separator == "."):
            regex_sep = f"\\{separator}"
        else:
            regex_sep = separator
        
        match = re.search(fr'\d+({regex_sep}\d+){{1,2}}', version_number_str)

        if(match):
            # Removes spaces at the ends of the matched string
            version_number_matched = match.group().strip()

            # Obtain the fields by spliting the string by the version separator
            version_number = [ int(i) for i in version_number_matched.split(sep=separator) ]
        else:
            # If there are no matches, the minimum version number (0.0.0) is returned
            version_number = [0, 0, 0]
        
        if(scheme == PackageScheme.Basic):
            self._version = VersionNumberBasic(*version_number)
        elif(scheme == PackageScheme.Date):
            self._version = VersionNumberDate(*version_number)
        elif(scheme == PackageScheme.OS):
            self._version = VersionNumberOS(*version_number)

    @property
    def version(self):
        return self._version

    def __str__(self):
        return str(self._version)

    def getSchemeStr(self):
        return self._version.getSchemeStr()
    
    def getWithoutSep(self):
        return self._version.getWithoutSep()


class Package:
    def __init__(self, name, scheme, install_version, required_version="", search_sep="."):
        self._name = name
        self._scheme = scheme
        self._install_version = VersionNumberStr(install_version, scheme)
        if(required_version):
            self._required_version = VersionNumberStr(required_version, scheme)
        else:
            self._required_version = self._install_version
        self._search_sep = search_sep
    
    @property
    def name(self):
        return self._name
    
    @property
    def install_version(self):
        return self._install_version
    
    @property
    def required_version(self):
        return self._required_version
    
    @property
    def scheme(self):
        return self._scheme

    @property
    def search_sep(self):
        return self._search_sep


class Dependencies:
    def __init__(self, packages=[]):
        self._packages = packages

    @property
    def packages(self):
        return self._packages
    
    def addPackage(self, package, position=-1):
        if(position == -1):
            self._packages.append(package)
        else:
            self._packages.insert(position, package)

class Libraries:
    def __init__(self, packages = [], triplet = ""):
        self._packages = packages
        self._triplet = triplet

    @property
    def packages(self):
        return self._packages
    
    @property
    def triplet(self):
        return self._triplet

