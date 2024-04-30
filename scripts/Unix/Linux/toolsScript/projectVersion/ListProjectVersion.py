#!/usr/bin/env python
# -*- coding: utf-8 -*-


from Unix.Linux.toolsScript.projectVersion import LinuxProjectVersion
from common.toolsScript.projectVersion.Dependencies import Package, PackageScheme, Dependencies, Libraries


class Development(LinuxProjectVersion):
    def __init__(self):
        description = "Development version"
        dependencies = Dependencies(
            [
                Package("gcc", PackageScheme.Basic, "11.2.0"),
                Package("g++", PackageScheme.Basic, "11.2.0"),
                Package("make", PackageScheme.Basic, "4.3"),
                Package("cmake", PackageScheme.Basic, "3.25.1")
            ]
        )
        cxx_libraries = Libraries(
            [
                "magic-enum",
                "cxxopts",
                "nlohmann-json",
                "fmt"
            ],
            "x64-linux"
        )
        actions = [
        ]

        super().__init__(description, dependencies, cxx_libraries, actions)

class Release(LinuxProjectVersion):
    def __init__(self):
        description = "Release version"
        dependencies = Dependencies()
        cxx_libraries = Libraries()
        actions = []

        super().__init__(description, dependencies, cxx_libraries, actions)

"""
class ReleaseMPI(LinuxProjectVersion):
    def __init__(self):
        description = "Release version with MPI"
        dependencies = Dependencies()
        cxx_libraries = Libraries()
        actions = []

        super().__init__(description, dependencies, cxx_libraries, actions)
"""