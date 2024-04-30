

from typing import List, Any
from abc import ABC, abstractmethod
import re

from Config import Config, Folder, JsonFile


class Change(ABC):
    def __init__(self, priority: int):
        self._priority: int = priority

    @abstractmethod
    def applyChange(self, config: Config):
        pass

    @property
    def priority(self) -> int:
        return self._priority

    def __lt__(self, other: "Change") -> bool:
        return self.priority < other.priority


class AddFolderChange(Change):
    def __init__(self, folder_path: List[str]):
        super().__init__(1)

        if(len(folder_path) == 0):
            print("Error: The folder cannot be the root folder of the configuration.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
    
    def applyChange(self, config: Config):
        root_folder_path = self._folder_path[:-1]

        root_folder = config.root_folder
        for elem in root_folder_path:
            root_folder = root_folder.subfolders[elem]

        folder_name = self._folder_path[-1]

        root_folder.subfolders[folder_name] = Folder()


class AddJsonFileChange(Change):
    def __init__(self, folder_path: List[str], json_file: str):
        super().__init__(2)

        if(len(json_file) == 0):
            print("Error: The name of the new json file must be specified.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
    
    def applyChange(self, config: Config):
        folder = config.root_folder
        for elem in self._folder_path:
            folder = folder.subfolders[elem]
        
        folder.json_files[self._json_file] = JsonFile()


class SetValueJsonItemChange(Change):
    def __init__(self, folder_path: List[str], item_path: List[str], item_value: Any, json_file: str = "", all_json_files: bool = False):
        super().__init__(3)

        if(not all_json_files and len(json_file) == 0):
            print("Error: The name of the new json file must be specified or set the 'all_json_files' parameter to True.")
            exit(1)
        
        if(all_json_files and len(json_file) > 0):
            print("Error: Choose between a specific json file or all json files, both options are mutually exclusive.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
        self._item_path: List[str] = item_path
        self._item_value: Any = item_value
        self._all_json_files: bool = all_json_files
    
    def applyChange(self, config: Config):
        folder_path = self._folder_path

        folder = config.root_folder
        for elem in folder_path:
            folder = folder.subfolders[elem]


        if(self._all_json_files):
            json_content_list = []
            for file in folder.json_files.value():
                json_content_list.append(file.content)
        else:
            json_content_list = [folder.json_files[self._json_file].content]


        root_item_path = self._item_path[:-1]
        item_name = self._item_path[-1]

        for json_content in json_content_list:
            root_item = json_content
            for elem in root_item_path:
                matches = re.search("item_(\d+)", elem)

                if(matches):
                    root_item = root_item[int(matches.group(1))]
                else:
                    root_item = root_item[elem]

            matches = re.search("item_(\d+)", item_name)

            if(matches):
                root_item[int(matches.group(1))] = self._item_value
            else:
                root_item[item_name] = self._item_value


class MoveFolderChange(Change):
    def __init__(self, initial_folder_path, final_folder_path):
        super().__init__(4)

        if(len(initial_folder_path) == 0):
            print("Error: The initial folder cannot be the root folder of the configuration.")
            exit(1)
        
        if(len(final_folder_path) == 0):
            print("Error: The final folder cannot be the root folder of the configuration.")
            exit(1)

        self._initial_folder_path = initial_folder_path
        self._final_folder_path = final_folder_path

    def applyChange(self, config: Config):
        initial_root_folder_path = self._initial_folder_path[:-1]

        initial_root_folder = config.root_folder
        for elem in initial_root_folder_path:
            initial_root_folder = initial_root_folder.subfolders[elem]

        initial_folder_name = self._initial_folder_path[-1]

        
        final_root_folder_path = self._final_folder_path[:-1]

        final_root_folder = config.root_folder
        for elem in final_root_folder_path:
            final_root_folder = final_root_folder.subfolders[elem]

        final_folder_name = self._final_folder_path[-1]


        final_root_folder.subfolders[final_folder_name] = initial_root_folder.subfolders.pop(initial_folder_name)


class MoveJsonFileChange(Change):
    def __init__(self, initial_folder_path: List[str], final_folder_path: List[str], initial_json_file: str = "", 
                 final_json_file: str = "", all_json_files: bool = False):
        super().__init__(5)

        if(len(initial_json_file) == 0 and len(final_json_file) > 0 or 
           len(initial_json_file) > 0 and len(final_json_file) == 0):
            print("Error: The initial number of json files must be equal to the final number of json files.")
            exit(1)
        
        if(not all_json_files and len(initial_json_file) == 0):
            print("Error: The name of the initial json file must be specified or set the 'all_json_files' parameter to True.")
            exit(1)
        
        if(not all_json_files and len(final_json_file) == 0):
            print("Error: The name of the final json file must be specified or set the 'all_json_files' parameter to True.")
            exit(1)

        if(all_json_files and len(initial_json_file) > 0):
            print("Error: Choose between a specific initial json file or all json files, both options are mutually exclusive.")
            exit(1)
        
        if(all_json_files and len(final_json_file) > 0):
            print("Error: Choose between a specific final json file or all json files, both options are mutually exclusive.")
            exit(1)
        
        self._initial_folder_path: List[str] = initial_folder_path
        self._initial_json_file: str = initial_json_file
        self._final_folder_path: List[str] = final_folder_path
        self._final_json_file: str = final_json_file
        self._all_json_files: bool = all_json_files

    def applyChange(self, config: Config):
        initial_folder = config.root_folder
        for elem in self._initial_folder_path:
            initial_folder = initial_folder.subfolders[elem]

        
        final_folder = config.root_folder
        for elem in self._final_folder_path:
            final_folder = final_folder.subfolders[elem]


        if(self._all_json_files):
            final_folder.json_files = initial_folder.json_files.copy()
            initial_folder.json_files.clear()
        else:
            final_folder.json_files[self._final_json_file] = initial_folder.json_files.pop(self._initial_json_file)


class MoveJsonItemChange(Change):
    def __init__(self, folder_path: List[str], initial_item_path: List[str], final_item_path: List[str], 
                 json_file: str = "", all_json_files: bool = False):
        super().__init__(6)

        if(not all_json_files and len(json_file) == 0):
            print("Error: The name of the json file must be specified or set the 'all_json_files' parameter to True.")
            exit(1)
        
        if(all_json_files and len(json_file) > 0):
            print("Error: Choose between a specific json file or all json files, both options are mutually exclusive.")
            exit(1)

        if(len(initial_item_path) == 0):
            print("Error: The initial item cannot be the root of the json content.")
            exit(1)

        if(len(final_item_path) == 0):
            print("Error: The final item cannot be the root of the json content.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
        self._initial_item_path: List[str] = initial_item_path
        self._final_item_path: List[str] = final_item_path
        self._all_json_files: bool = all_json_files
    
    def applyChange(self, config: Config):
        folder_path = self._folder_path

        folder = config.root_folder
        for elem in folder_path:
            folder = folder.subfolders[elem]


        if(self._all_json_files):
            json_content_list = []
            for file in folder.json_files.value():
                json_content_list.append(file.content)
        else:
            json_content_list = [folder.json_files[self._json_file].content]


        initial_root_item_path = self._initial_item_path[:-1]
        initial_item_name = self._initial_item_path[-1]

        final_root_item_path = self._final_item_path[:-1]
        final_item_name = self._final_item_path[-1]


        for json_content in json_content_list:
            initial_root_item = json_content
            for elem in initial_root_item_path:
                initial_root_item = initial_root_item[elem]

            final_root_item = json_content
            for elem in final_root_item_path:
                final_root_item = final_root_item[elem]

            final_root_item[final_item_name] = initial_root_item.pop(initial_item_name)


class RemoveFolderChange(Change):
    def __init__(self, folder_path: List[str]):
        super().__init__(7)

        if(len(folder_path) == 0):
            print("Error: The folder cannot be the root folder of the configuration.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
    
    def applyChange(self, config: Config):
        root_folder_path = self._folder_path[:-1]

        root_folder = config.root_folder
        for elem in root_folder_path:
            root_folder = root_folder.subfolders[elem]

        folder_name = self._folder_path[-1]

        root_folder.subfolders.pop(folder_name)
    

class RemoveJsonFileChange(Change):
    def __init__(self, folder_path: List[str], json_file: str = "", all_json_files: bool = False):
        super().__init__(8)

        if(not all_json_files and len(json_file) == 0):
            print("Error: The name of the json file must be specified or set the 'all_json_files' parameter to True.")
            exit(1)
        
        if(all_json_files and len(json_file) > 0):
            print("Error: Choose between a specific json file or all json files, both options are mutually exclusive.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
        self._all_json_files: bool = all_json_files
    
    def applyChange(self, config: Config):
        folder = config.root_folder
        for elem in self._folder_path:
            folder = folder.subfolders[elem]
        
        if(self._all_json_files):
            folder.json_files.clear()
        else:
            folder.json_files.pop(self._json_file)


class RemoveJsonItemChange(Change):
    def __init__(self, folder_path: List[str], item_path: List[str], json_file: str = "", all_json_files: bool = False):
        super().__init__(9)

        if(not all_json_files and len(json_file) == 0):
            print("Error: The name of the json file must be specified or set the 'all_json_files' parameter to True.")
            exit(1)
        
        if(all_json_files and len(json_file) > 0):
            print("Error: Choose between a specific json file or all json files, both options are mutually exclusive.")
            exit(1)

        if(len(item_path) == 0):
            print("Error: The item cannot be the root of the json content.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
        self._item_path: List[str] = item_path
        self._all_json_files: bool = all_json_files
    
    def applyChange(self, config: Config):
        folder = config.root_folder
        for elem in self._folder_path:
            folder = folder.subfolders[elem]


        if(self._all_json_files):
            json_content_list = []
            for file in folder.json_files.value():
                json_content_list.append(file.content)
        else:
            json_content_list = [folder.json_files[self._json_file].content]


        root_item_path = self._item_path[:-1]
        item_name = self._item_path[-1]

        for json_content in json_content_list:
            root_item = json_content
            for elem in root_item_path:
                root_item = root_item[elem]

            root_item.pop(item_name)

