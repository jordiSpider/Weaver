#!/usr/bin/env python
# -*- coding: utf-8 -*-


from common.toolsScript.projectVersion import ProjectVersion
from common.Types import SO_TYPE
from common.toolsScript.projectVersion.Dependencies import Package, PackageScheme


class LinuxProjectVersion(ProjectVersion):
    git_package = Package("git", PackageScheme.Basic, "2.34.1")

    def __init__(self, description, dependencies, cxx_libraries, actions):
        super().__init__(SO_TYPE.Linux, description, dependencies, cxx_libraries, LinuxProjectVersion.git_package, actions)

        if(self._dependencies.packages[-1].name == "vcpkg"):
            vcpkg_index = len(self._dependencies.packages)-1
            self._dependencies.addPackage(Package("curl", PackageScheme.Basic, "7.81.0"), vcpkg_index)
            self._dependencies.addPackage(Package("zip", PackageScheme.Basic, "3.0"), vcpkg_index)
            self._dependencies.addPackage(Package("unzip", PackageScheme.Basic, "6.0"), vcpkg_index)
            self._dependencies.addPackage(Package("tar", PackageScheme.Basic, "1.34"), vcpkg_index)
            self._dependencies.addPackage(Package("pkg-config", PackageScheme.Basic, "0.29.2"), vcpkg_index)
