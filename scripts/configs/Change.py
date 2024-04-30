

from typing import List, Any
from abc import ABC, abstractmethod
import re
import copy

from Config import Config, Folder, JsonFile


def getFolder(root_folder, folder_path):
    folder = root_folder
    for elem in folder_path:
        folder = folder.subfolders[elem]
    
    return folder

def getJsonFileContent(root_folder, folder_path, json_file):
    folder = getFolder(root_folder, folder_path)

    return folder.json_files[json_file].content

def getJsonItem(json_content, item_path):
    json_item = json_content

    for elem in item_path:
        if isinstance(json_item, list):
            matches = re.search("^item_(\d+)$", elem)

            if(matches):
                json_item = json_item[int(matches.group(1))]
            else:
                print("Error: To access a list you must create strings of the style: 'item_{index number}'.")
                exit(1)
        else:
            json_item = json_item[elem]

    return json_item

def setJsonItem(json_content, item_path, value):
    json_item = getJsonItem(json_content, item_path[:-1])

    if isinstance(json_item, list):
        matches = re.search("^item_(\d+)$", item_path[-1])

        if(matches):
            json_item[int(matches.group(1))] = value
        else:
            print("Error: To access a list you must create strings of the style: 'item_{index number}'.")
            exit(1)
    else:
        json_item[item_path[-1]] = value
    
def removeJsonItem(json_content, item_path):
    json_item = getJsonItem(json_content, item_path[:-1])

    if isinstance(json_item, list):
        matches = re.search("^item_(\d+)$", item_path[-1])

        if(matches):
            del json_item[int(matches.group(1))]
        else:
            print("Error: To access a list you must create strings of the style: 'item_{index number}'.")
            exit(1)
    else:
        del json_item[item_path[-1]]

def matchJsonItemPath(json_content, regex_item_path):
    item_path_list = []

    if(len(regex_item_path) > 0):
        if isinstance(json_content, list):
            matches = re.search("^item_*$", regex_item_path[0])

            if(matches):
                for i in range(len(json_content)):
                    item_match = re.search(regex_item_path[0], "item_" + str(i))

                    if item_match:
                        item_groups = [item_match.group(0)] + list(item_match.groups())

                        item_paths = matchJsonItemPath(json_content[i], regex_item_path[1:])

                        if(len(key_paths) > 0):
                            for i in range(len(item_paths)):
                                item_paths[i] = [item_groups] + item_paths[i]
                        else:
                            key_paths = [[item_groups]]

                        item_path_list = item_path_list + item_paths
            else:
                print("Error: To access a list you must create regex strings of the style: 'item_*'.")
                exit(1)
        else:
            for key in json_content.keys():
                key_match = re.search(regex_item_path[0], key)

                if key_match:
                    key_groups = [key_match.group(0)] + list(key_match.groups())

                    key_paths = matchJsonItemPath(json_content[key], regex_item_path[1:])

                    if(len(key_paths) > 0):
                        for i in range(len(key_paths)):
                            key_paths[i] = [key_groups] + key_paths[i]
                    else:
                        key_paths = [[key_groups]]

                    item_path_list = item_path_list + key_paths

    return item_path_list

def replaceJsonItemPathMatch(regex_item_path, valuesToReplace):
    def replaceMatch(match, valuesToReplace):
        item_index = int(match.group(1))
        group_index = int(match.group(2))

        return valuesToReplace[item_index][group_index]

    
    item_path = []

    for elem in regex_item_path:
        elem_replaced = re.sub(r'<match_(\d+)_(\d+)>', lambda match: replaceMatch(match, valuesToReplace), elem)

        item_path.append(elem_replaced)
    
    return item_path


class Change(ABC):
    def __init__(self):
        pass

    @abstractmethod
    def applyChange(self, config: Config):
        pass


class AddFolderChange(Change):
    def __init__(self, folder_path: List[str]):
        super().__init__()

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
        super().__init__()

        if(len(json_file) == 0):
            print("Error: The name of the new json file must be specified.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
    
    def applyChange(self, config: Config):
        folder = getFolder(config.root_folder, self._folder_path)
        folder.json_files[self._json_file] = JsonFile()


class AddJsonItemChange(Change):
    def __init__(self, folder_path: List[str], item_path: List[str], item_value: Any, json_file: str):
        super().__init__()

        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
        self._item_path: List[str] = item_path
        self._item_value: Any = item_value
    
    def applyChange(self, config: Config):
        json_content = getJsonFileContent(config.root_folder, self._folder_path, self._json_file)

        setJsonItem(json_content, self._item_path, self._item_value)


class AddJsonItemAllFilesChange(Change):
    def __init__(self, folder_path: List[str], item_path: List[str], item_value: Any):
        super().__init__()

        self._folder_path: List[str] = folder_path
        self._item_path: List[str] = item_path
        self._item_value: Any = item_value
    
    def applyChange(self, config: Config):
        folder = getFolder(config.root_folder, self._folder_path)

        for file in folder.json_files.keys():
            file_change = AddJsonItemChange(self._folder_path, self._item_path, self._item_value, file)
            file_change.applyChange(config)


class RegexAddJsonItemChange(Change):
    def __init__(self, folder_path: List[str], pattern_path: List[str], item_regex_path: List[str], item_value: Any, json_file: str):
        super().__init__()

        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
        self._pattern_path: List[str] = pattern_path
        self._item_regex_path: List[str] = item_regex_path
        self._item_value: Any = item_value
    
    def applyChange(self, config: Config):
        json_content = getJsonFileContent(config.root_folder, self._folder_path, self._json_file)

        matches = matchJsonItemPath(json_content, self._pattern_path)

        for match in matches:
            item_path = replaceJsonItemPathMatch(self._item_regex_path, match)
            setJsonItem(json_content, item_path, self._item_value)


class RegexAddJsonItemAllFilesChange(Change):
    def __init__(self, folder_path: List[str], pattern_path: List[str], item_regex_path: List[str], item_value: Any):
        super().__init__()

        self._folder_path: List[str] = folder_path
        self._pattern_path: List[str] = pattern_path
        self._item_regex_path: List[str] = item_regex_path
        self._item_value: Any = item_value
    
    def applyChange(self, config: Config):
        folder = getFolder(config.root_folder, self._folder_path)

        for file in folder.json_files.keys():
            file_change = RegexAddJsonItemChange(self._folder_path, self._pattern_path, self._item_regex_path, self._item_value, file)
            file_change.applyChange(config)


class MoveFolderChange(Change):
    def __init__(self, initial_folder_path, final_folder_path):
        super().__init__()

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
        super().__init__()

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
        initial_folder = getFolder(config.root_folder, self._initial_folder_path)
        final_folder = getFolder(config.root_folder, self._final_folder_path)

        if(self._all_json_files):
            final_folder.json_files = initial_folder.json_files.copy()
            initial_folder.json_files.clear()
        else:
            final_folder.json_files[self._final_json_file] = initial_folder.json_files.pop(self._initial_json_file)


class MoveJsonItemChange(Change):
    def __init__(self, folder_path: List[str], initial_item_path: List[str], final_item_path: List[str], 
                 json_file: str):
        super().__init__()

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
    
    def applyChange(self, config: Config):
        json_content = getJsonFileContent(config.root_folder, self._folder_path, self._json_file)

        initial_item = getJsonItem(json_content, self._initial_item_path)
        setJsonItem(json_content, self._final_item_path, copy.deepcopy(initial_item))
        removeJsonItem(json_content, self._initial_item_path)


class MoveJsonItemAllFilesChange(Change):
    def __init__(self, folder_path: List[str], initial_item_path: List[str], final_item_path: List[str]):
        super().__init__()

        if(len(initial_item_path) == 0):
            print("Error: The initial item cannot be the root of the json content.")
            exit(1)

        if(len(final_item_path) == 0):
            print("Error: The final item cannot be the root of the json content.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._initial_item_path: List[str] = initial_item_path
        self._final_item_path: List[str] = final_item_path
    
    def applyChange(self, config: Config):
        folder = getFolder(config.root_folder, self._folder_path)

        for file in folder.json_files.keys():
            file_change = MoveJsonItemChange(self._folder_path, self._initial_item_path, self._final_item_path, file)
            file_change.applyChange(config)


class RegexMoveJsonItemChange(Change):
    def __init__(self, folder_path: List[str], initial_item_regex_path: List[str], final_item_match_path: List[str], 
                 json_file: str):
        super().__init__()

        if(len(initial_item_regex_path) == 0):
            print("Error: The initial item cannot be the root of the json content.")
            exit(1)

        if(len(final_item_match_path) == 0):
            print("Error: The final item cannot be the root of the json content.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
        self._initial_item_regex_path: List[str] = initial_item_regex_path
        self._final_item_match_path: List[str] = final_item_match_path
    
    def applyChange(self, config: Config):
        json_content = getJsonFileContent(config.root_folder, self._folder_path, self._json_file)

        matches = matchJsonItemPath(json_content, self._initial_item_regex_path)

        for match in matches:
            initial_item_path = [elem_match[0] for elem_match in match]

            initial_item = getJsonItem(json_content, initial_item_path)

            final_item_path = replaceJsonItemPathMatch(self._final_item_match_path, match)

            setJsonItem(json_content, final_item_path, copy.deepcopy(initial_item))
            removeJsonItem(json_content, initial_item_path)


class RegexMoveJsonItemAllFilesChange(Change):
    def __init__(self, folder_path: List[str], initial_item_regex_path: List[str], final_item_match_path: List[str]):
        super().__init__()

        if(len(initial_item_regex_path) == 0):
            print("Error: The initial item cannot be the root of the json content.")
            exit(1)

        if(len(final_item_match_path) == 0):
            print("Error: The final item cannot be the root of the json content.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._initial_item_regex_path: List[str] = initial_item_regex_path
        self._final_item_match_path: List[str] = final_item_match_path
    
    def applyChange(self, config: Config):
        folder = getFolder(config.root_folder, self._folder_path)

        for file in folder.json_files.keys():
            file_change = RegexMoveJsonItemChange(self._folder_path, self._initial_item_regex_path, self._final_item_match_path, file)
            file_change.applyChange(config)


class CopyJsonItemChange(Change):
    def __init__(self, folder_path: List[str], initial_item_path: List[str], final_item_path: List[str], 
                 json_file: str):
        super().__init__()

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
    
    def applyChange(self, config: Config):
        json_content = getJsonFileContent(config.root_folder, self._folder_path, self._json_file)

        initial_item = getJsonItem(json_content, self._initial_item_path)
        setJsonItem(json_content, self._final_item_path, copy.deepcopy(initial_item))


class CopyJsonItemAllFilesChange(Change):
    def __init__(self, folder_path: List[str], initial_item_path: List[str], final_item_path: List[str]):
        super().__init__()

        if(len(initial_item_path) == 0):
            print("Error: The initial item cannot be the root of the json content.")
            exit(1)

        if(len(final_item_path) == 0):
            print("Error: The final item cannot be the root of the json content.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._initial_item_path: List[str] = initial_item_path
        self._final_item_path: List[str] = final_item_path
    
    def applyChange(self, config: Config):
        folder = getFolder(config.root_folder, self._folder_path)

        for file in folder.json_files.keys():
            file_change = CopyJsonItemChange(self._folder_path, self._initial_item_path, self._final_item_path, file)
            file_change.applyChange(config)


class RegexCopyJsonItemChange(Change):
    def __init__(self, folder_path: List[str], initial_item_regex_path: List[str], final_item_regex_path: List[str], 
                 json_file: str):
        super().__init__()

        if(len(initial_item_regex_path) == 0):
            print("Error: The initial item cannot be the root of the json content.")
            exit(1)

        if(len(final_item_regex_path) == 0):
            print("Error: The final item cannot be the root of the json content.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._json_file: str = json_file
        self._initial_item_regex_path: List[str] = initial_item_regex_path
        self._final_item_regex_path: List[str] = final_item_regex_path
    
    def applyChange(self, config: Config):
        json_content = getJsonFileContent(config.root_folder, self._folder_path, self._json_file)

        matches = matchJsonItemPath(json_content, self._initial_item_regex_path)

        for match in matches:
            initial_item_path = [elem_match[0] for elem_match in match]

            initial_item = getJsonItem(json_content, initial_item_path)

            final_item_path = replaceJsonItemPathMatch(self._final_item_regex_path, match)

            setJsonItem(json_content, final_item_path, copy.deepcopy(initial_item))


class RegexCopyJsonItemAllFilesChange(Change):
    def __init__(self, folder_path: List[str], initial_item_regex_path: List[str], final_item_regex_path: List[str]):
        super().__init__()

        if(len(initial_item_regex_path) == 0):
            print("Error: The initial item cannot be the root of the json content.")
            exit(1)

        if(len(final_item_regex_path) == 0):
            print("Error: The final item cannot be the root of the json content.")
            exit(1)
        
        self._folder_path: List[str] = folder_path
        self._initial_item_regex_path: List[str] = initial_item_regex_path
        self._final_item_regex_path: List[str] = final_item_regex_path
    
    def applyChange(self, config: Config):
        folder = getFolder(config.root_folder, self._folder_path)

        for file in folder.json_files.keys():
            file_change = CopyJsonItemChange(self._folder_path, self._initial_item_regex_path, self._final_item_regex_path, file)
            file_change.applyChange(config)


class RemoveFolderChange(Change):
    def __init__(self, folder_path: List[str]):
        super().__init__()

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
        super().__init__()

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
        folder = getFolder(config.root_folder, self._folder_path)
        
        if(self._all_json_files):
            folder.json_files.clear()
        else:
            folder.json_files.pop(self._json_file)


class RemoveJsonItemChange(Change):
    def __init__(self, folder_path: List[str], item_path: List[str], json_file: str = "", all_json_files: bool = False):
        super().__init__()

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
        folder = getFolder(config.root_folder, self._folder_path)

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

