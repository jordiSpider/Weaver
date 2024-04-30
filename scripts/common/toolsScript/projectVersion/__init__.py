#!/usr/bin/env python
# -*- coding: utf-8 -*-


from common.toolsScript.actions import Action
from common.toolsScript.projectVersion.Dependencies import Package, PackageScheme



class ProjectVersion:
    vcpkg_package = Package("vcpkg", PackageScheme.Date, "2023.04.15", "2023.04.07", search_sep="-")

    def __init__(self, SO_type, description, dependencies, cxx_libraries, git_package, actions):
        self._SO_type = SO_type
        self._description = description
        self._dependencies = dependencies

        self._cxx_libraries = cxx_libraries
        if(self._cxx_libraries):
            if(not any(package.name == ProjectVersion.vcpkg_package.name for package in self._dependencies.packages)):
                self._dependencies.addPackage(git_package)
                self._dependencies.addPackage(ProjectVersion.vcpkg_package)

        self._actions = Action.get_action_classes(actions, SO_type)
    
    @property
    def SO_type(self):
        return self._SO_type

    @property
    def description(self):
        return self._description

    @property
    def dependencies(self):
        return self._dependencies
    
    @property
    def cxx_libraries(self):
        return self._cxx_libraries
    
    @property
    def actions(self):
        return self._actions
