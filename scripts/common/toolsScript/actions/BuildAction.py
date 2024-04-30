#!/usr/bin/env python
# -*- coding: utf-8 -*-



## @file BuildAction.py
# @brief BuildAction class definition
#
# @section description Description
# BuildAction class definition
#
# @section libraries Libraries/Modules
# - [os](https://docs.python.org/3.7/library/os.html) standard library
#   - Access to [path](https://docs.python.org/3.7/library/os.path.html) module.
#     - Access to [join](https://docs.python.org/3.7/library/os.path.html#os.path.join) function
# - [json](https://docs.python.org/3.7/library/json.html) standard library
#   - Access to [load](https://docs.python.org/3.7/library/json.html#json.load) function
#
# @section todo TO-DO
# - None.
#
# @section author Author(s)
# - Created by [Mario Carmona Segovia](https://github.com/Mario-Carmona) on 31/01/2023.



# Imports
import os
import json
from common.toolsScript.actions import Action
from typing import List, Dict, Set



## @brief Defines the action of building a preset workflow with Cmake. Inherits from the Action class
class BuildAction(Action):
    ## @brief BuildAction class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)

        ## Name of the workflow preset to be built
        self._preset_selected: str = None
        if(self._values):
            self._preset_selected = self._values[0]

    ## @brief Recursively obtains the basic presets from which it inherits a certain preset
    #
    # @param preset_name Name of the preset to get its inheritance
    # @return Set of basic presets
    # @note A basic preset is any preset that only inherits from the base preset
    def obtain_presets_inherits(self, preset_name: str) -> Set[str]:
        # Set that stores the inheritance of the current preset
        # @note Use a set to avoid repeated presets
        preset_inherits = set()

        # Searches for the preset in the project configuration presets
        preset = next((preset for preset in self._config_presets["configurePresets"] if preset["name"] == preset_name), None)
        # If it inherits from any preset
        if("inherits" in preset):
            # Obtains the inherited presets of each preset included in the current preset's inheritance list
            for inherited in preset["inherits"]:
                result = self.obtain_presets_inherits(inherited)
                if(not result):
                    preset_inherits.add(preset_name)
                else:
                    preset_inherits = preset_inherits.union(result)

        return preset_inherits

    ## @brief Obtains all necessary preset information
    def obtain_presets(self):
        # Obtains the preset configuration
        preset_file = self._tools.config["actions"][self._name]["extra_info"]["preset_file"]
        with open(os.path.join(self._tools.working_directory, preset_file), "r") as f:
            self._config_presets = json.load(f)

        # Set containing all basic presets
        all_inherits = set()
        ## Dictionary containing the inherited presets of each preset
        self._presets: Dict[str, Set[str]] = {}
        ## Dictionary containing the description of each preset
        self._presets_descrip: Dict[str, str] = {}
        # Obtains all information from each workflow preset 
        for preset in self._config_presets["workflowPresets"]:
            preset_inherits = self.obtain_presets_inherits(preset["name"])
            all_inherits.update(preset_inherits)
            self._presets[preset["name"]] = preset_inherits
            self._presets_descrip[preset["name"]] = next((i["displayName"] for i in self._config_presets["configurePresets"] if i["name"] == preset["name"]), None)

        ## Dictionary of the state of the basic presets
        self._inherits_state: Dict[str, bool] = {}

        # Checks the status of the basic presets
        for inherits in all_inherits:
            # If the preset has requirements defined in the configuration
            if(inherits in self._tools.config["actions"][self._name]["extra_info"]["requiriments"][self._tools.SO_name]):
                state = True

                # Checks the status of the preset requirements to know its availability
                inherits_info = self._tools.config["actions"][self._name]["extra_info"]["requiriments"][self._tools.SO_name][inherits]

                if("dependencies" in inherits_info):
                    for dependency in inherits_info["dependencies"]:
                        if(not self._tools.dependencies_up_to_date[dependency]):
                            state = False

                if("packages" in inherits_info):
                    for package in inherits_info["packages"]:
                        if(not self._tools.packages_state[package]):
                            state = False
            else:
                # If the preset has no requirements, it is always available
                state = True
            
            self._inherits_state[inherits] = state    

    ## @brief Checks if the workflow preset is available
    #
    # @param preset_inherits List of basic presets that the workflow preset inherits from 
    # @return Availability of the workflow preset
    def preset_is_available(self, preset_inherits: List[str]) -> bool:
        if(self._tools.SO_name in preset_inherits):
            for inherits in preset_inherits:
                if(not self._inherits_state[inherits]):
                    return False
        else:
            # If the OS where the script is executed is not within the inheritance
            return False

        return True

    ## @brief Obtains the list of available workflow presets
    def obtain_available_presets(self):
        self.obtain_presets()

        ## List of available workflow presets
        self._available_presets = []

        for preset_name, preset_inherits in self._presets.items():
            if(self.preset_is_available(preset_inherits)):
                self._available_presets.append(preset_name)

    ## @brief Displays the process of obtaining the list of available workflow presets
    def show_obtain_available_presets(self):
        self._tools.set_future(self._tools.executor.submit(self.obtain_available_presets))
        self._tools.window.loading("Checking presets")

    ## @brief Displays the selection of the workflow preset to be build
    #
    # @return Selected workflow preset
    def presets_choice(self) -> str:
        options_text = []
        for preset in self._available_presets:
            options_text.append(self._presets_descrip[preset])
        
        # Displays the selection menu
        selected_preset = self._tools.window.selection_menu("Select the build preset", options_text, add_exit=True)

        return self._available_presets[selected_preset]

    ## @brief Build the workflow preset selected
    def build_preset(self):
        self._tools.exec_command(["cmake", "--workflow", "--preset", self._preset_selected])

    ## @brief Displays the process of building the workflow preset
    def show_build_preset(self):
        self._tools.set_future(self._tools.executor.submit(self.build_preset))
        self._tools.window.loading("Building preset")
    
    ## @brief Performs the action of building the workflow preset
    def run_action(self):
        self.show_obtain_available_presets()

        # If no values are preselected for the action
        if(self._preset_selected == None):
            self._preset_selected = self.presets_choice()
        else:
            if(not self._preset_selected in self._presets):
                error_message = f"unrecognized preset '{self._preset_selected}'.\n"
                error_message += f"Valid values are {', '.join([preset for preset in self._presets.keys()])}."
                self._tools.print_help(error_message)
            if(not self.preset_is_available(self._presets[self._preset_selected])):
                self._tools.print_help(f"preset '{self._preset_selected}' not available, check preset requirements.")

        self.show_build_preset()


## @brief Defines the action of building a preset workflow with Cmake on Linux. Inherits from the BuildAction class
class BuildActionLinux(BuildAction):
    ## @brief BuildActionLinux class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)

## @brief Defines the action of building a preset workflow with Cmake on Windows. Inherits from the BuildAction class
class BuildActionWindows(BuildAction):
    ## @brief BuildActionWindows class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)

## @brief Defines the action of building a preset workflow with Cmake on MacOS. Inherits from the BuildAction class
class BuildActionDarwin(BuildAction):
    ## @brief BuildActionDarwin class initializer
    #
    # @param tools Project tools manager
    # @param name Name of action
    # @param values Values chosen to perform the action
    def __init__(self, tools: "ProjectTools", name: str, values: List[str] = []):
        super().__init__(tools, name, values)
