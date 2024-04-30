#!/usr/bin/env python
# -*- coding: utf-8 -*-


from Windows.toolsScript.projectVersion import WindowsProjectVersion
from common.toolsScript.projectVersion.Dependencies import Package, PackageScheme, Dependencies, Libraries


class Development(WindowsProjectVersion):
    def __init__(self):
        description = "Development version"
        dependencies = Dependencies(
            [
                Package("mingw-w64-x86_64-gcc", PackageScheme.Basic, "11.2.0"),
                Package("mingw-w64-x86_64-make", PackageScheme.Basic, "4.3"),
                Package("mingw-w64-x86_64-cmake", PackageScheme.Basic, "3.25.1")
            ]
        )
        cxx_libraries = Libraries(
            [
                "magic-enum",
                "cxxopts",
                "nlohmann-json",
                "fmt"
            ],
            "x64-mingw-static"
        )
        actions = [
        ]

        super().__init__(description, dependencies, cxx_libraries, actions)

class Release(WindowsProjectVersion):
    def __init__(self):
        description = "Release version"
        dependencies = Dependencies()
        cxx_libraries = Libraries()
        actions = []

        super().__init__(description, dependencies, cxx_libraries, actions)

"""
class ReleaseMPI(WindowsProjectVersion):
    def __init__(self):
        description = "Release version with MPI"
        dependencies = Dependencies()
        cxx_libraries = Libraries()
        actions = []

        super().__init__(description, dependencies, cxx_libraries, actions)
"""
