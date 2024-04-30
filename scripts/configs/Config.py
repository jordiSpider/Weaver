
import re
import json
from pathlib import Path
from typing import Dict, Any


## @brief Represent the version number (X.X.X) of modules or packages 
class VersionNumber:
    def __init__(self, version_number_str: str = ""):
        # Extracts the version number from the string using a regular expression
        match = re.search(fr'\d+(\.\d+){{1,2}}', version_number_str)

        if(match):
            version_number = [-1, -1, -1]

            # Removes spaces at the ends of the matched string
            version_number_matched = match.group().strip()

            # Obtain the fields by spliting the string by the version separator
            for index, value in enumerate(version_number_matched.split(sep=".")):
                version_number[index] = int(value)
        else:
            # If there are no matches, the minimum version number (0.0.0) is returned
            version_number = [0, 0, 0]
        
        ## First field of version number (X.x.x)
        self._major: int = version_number[0]
        ## Second field of version number (x.X.x)
        self._minor: int = version_number[1]
        ## Third field of version number (x.x.X)
        self._patch: int = version_number[2]

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

    def __le__(self, other):
        if(self._major < other._major):
            return True
        elif(self._major == other._major):
            if(self._minor < other._minor):
                return True
            elif(self._minor == other._minor):
                if(self._patch <= other._patch):
                    return True
                else:
                    return False
            else:
                return False
        else:
            return False

    def __lt__(self, other):
        if(self._major < other._major):
            return True
        elif(self._major == other._major):
            if(self._minor < other._minor):
                return True
            elif(self._minor == other._minor):
                if(self._patch < other._patch):
                    return True
                else:
                    return False
            else:
                return False
        else:
            return False
        
    def __eq__ (self, other: object) -> bool:
        if not isinstance(other, VersionNumber):
            return False
        else:
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
    
    def __ne__ (self, other: object) -> bool:
        return not self == other
    
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


class JsonFile:
    def __init__(self, json_file_path: Path = Path()):
        self._content: Dict[str, Any] = {}
        self._version: VersionNumber = VersionNumber()

        if(json_file_path != Path()):
            with json_file_path.open("r") as file:
                self._content = json.load(file)
                self._version = VersionNumber(self._content["version"])
    
    def save(self, new_file_path: Path):
        with new_file_path.open("w") as file:
            json.dump(self._content, file, indent=4)
    
    def refresh_version(self, new_version: VersionNumber):
        self._version = new_version
        self._content["version"] = str(new_version)

    @property
    def content(self) -> Dict[str, Any]:
        return self._content
    
    @property
    def version(self) -> VersionNumber:
        return self._version

class Folder:
    def __init__(self, folder_path: Path = Path()):
        self._subfolders: Dict[str, Folder] = {}
        self._json_files: Dict[str, JsonFile] = {}

        if(folder_path != Path()):
            for elem in folder_path.iterdir():
                if(elem.is_dir()):
                    self._subfolders[elem.name] = Folder(elem)
                elif(elem.is_file()):
                    if(elem.suffix == '.json'):
                        self._json_files[elem.name] = JsonFile(elem)
        
        self._version: VersionNumber = VersionNumber()

        for folder in self._subfolders.values():
            if(self._version == VersionNumber("-1")):
                if(self._version == VersionNumber()):
                    self._version = folder.version
                else:
                    if(self._version != folder.version):
                        self._version = VersionNumber("-1")

        for file in self._json_files.values():
            if(self._version == VersionNumber("-1")):
                if(self._version == VersionNumber()):
                    self._version = file.version
                else:
                    if(self._version != file.version):
                        self._version = VersionNumber("-1")

    def save(self, new_root_folder_path: Path):
        if(len(self._subfolders) + len(self._json_files) > 0):
            for name, folder in self._subfolders.items():
                folder_path = new_root_folder_path / name
                folder_path.mkdir()
                folder.save(folder_path)
            
            for name, file in self._json_files.items():
                file_path = new_root_folder_path / name
                file.save(file_path)
        else:
            gitkeep_path = new_root_folder_path / ".gitkeep"
            gitkeep_path.touch()

    def refresh_version(self, new_version: VersionNumber):
        self._version = new_version

        for folder in self._subfolders.values():
            folder.refresh_version(new_version)

        for file in self._json_files.values():
            file.refresh_version(new_version)

    @property
    def subfolders(self) -> Dict[str, "Folder"]:
        return self._subfolders
    
    @property
    def json_files(self) -> Dict[str, JsonFile]:
        return self._json_files

    @property
    def version(self) -> VersionNumber:
        return self._version


class Config:
    def __init__(self, config_path: Path):
        self._root_folder = Folder(config_path)

        if(self._root_folder.version is None):
            print("Error: Configuration files are not in the same version.")
            exit(1)
        else:
            self._version: VersionNumber = self._root_folder.version

    def save(self, new_config_path: Path):
        if(not new_config_path.is_dir()):
            new_config_path.mkdir()
        else:
            print(f"Error: The '{new_config_path}' directory already exists.")
            exit(1)

        self._root_folder.save(new_config_path)

    def refresh_version(self, new_version: VersionNumber):
        self._version = new_version
        self._root_folder.refresh_version(new_version)

    @property
    def root_folder(self):
        return self._root_folder
    
    @property
    def version(self):
        return self._version

