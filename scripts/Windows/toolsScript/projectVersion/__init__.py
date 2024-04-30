#!/usr/bin/env python
# -*- coding: utf-8 -*-


from common.toolsScript.projectVersion import ProjectVersion
from common.Types import SO_TYPE
from common.toolsScript.projectVersion.Dependencies import Package, PackageScheme


class WindowsProjectVersion(ProjectVersion):
    MSYS2_package = Package("MSYS2", PackageScheme.Date, "2023.05.26", "6.0.2")
    git_package = Package("mingw-w64-x86_64-git-lfs", PackageScheme.Basic, "3.3.0")

    def __init__(self, description, dependencies, cxx_libraries, actions):
        super().__init__(SO_TYPE.Windows, description, dependencies, cxx_libraries, WindowsProjectVersion.git_package, actions)

        gcc_index = -1
        gcc_package = None
        for index, package in enumerate(self._dependencies.packages):
            if(package.name == "mingw-w64-x86_64-gcc"):
                gcc_index = index
                gcc_package = package

        if(gcc_index >= 0):
            self._dependencies.addPackage(Package("mingw-w64-x86_64-gcc-libs", PackageScheme.Basic, str(gcc_package.install_version)), gcc_index)

        if(len(self._dependencies.packages) != 0):
            if(not any(package.name == WindowsProjectVersion.MSYS2_package.name for package in self._dependencies.packages)):
                self._dependencies.addPackage(WindowsProjectVersion.MSYS2_package, 0)
