#!/usr/bin/env python
# -*- coding: utf-8 -*-


import os


class EnvVars:
    def __init__(self, envVars, separator=None):
        self._envVars = envVars

        if(separator):
            path_sep = separator
        else:
            path_sep = os.sep
        
        self._envVars["VCPKG_ROOT"] = f"{self._envVars['HOME']}{path_sep}vcpkg"
    
    def __getitem__(self, key):
        return self._envVars[key]

