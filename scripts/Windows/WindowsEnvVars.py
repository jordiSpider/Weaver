#!/usr/bin/env python
# -*- coding: utf-8 -*-

from common.EnvVars import EnvVars
import os


class WindowsEnvVars(EnvVars):
    def __init__(self):
        envVars = {}
        envVars["MSYS2_ROOT"] = f"C:{os.sep}msys64"
        envVars["HOME"] = f"{envVars['MSYS2_ROOT']}{os.sep}home"

        super().__init__(envVars)

