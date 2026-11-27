"""Check shortcut component selection in built Windows MSI packages.

Run on Windows: python test_desktop_shortcut.py <desktop.msi> <server.msi>
The test opens installation sessions for costing only; it does not install files.
"""

import ctypes
from ctypes import wintypes
from pathlib import Path
import sys
import unittest


msi = ctypes.WinDLL("msi")
msi.MsiOpenPackageExW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, ctypes.POINTER(wintypes.UINT)]
msi.MsiSetPropertyW.argtypes = [wintypes.UINT, wintypes.LPCWSTR, wintypes.LPCWSTR]
msi.MsiDoActionW.argtypes = [wintypes.UINT, wintypes.LPCWSTR]
msi.MsiGetComponentStateW.argtypes = [wintypes.UINT, wintypes.LPCWSTR, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
msi.MsiCloseHandle.argtypes = [wintypes.UINT]


def check(code):
	if code:
		raise OSError(f"Windows Installer error {code}")


def shortcut_action(package, choice, features=None):
	handle = wintypes.UINT()
	check(msi.MsiOpenPackageExW(str(package), 1, ctypes.byref(handle)))
	try:
		if choice:
			check(msi.MsiSetPropertyW(handle, "WEAVER_DESKTOP_SHORTCUT", choice))
		if features:
			check(msi.MsiSetPropertyW(handle, "ADDLOCAL", features))
		for action in ("CostInitialize", "FileCost", "CostFinalize"):
			check(msi.MsiDoActionW(handle, action))
		installed = ctypes.c_int()
		requested = ctypes.c_int()
		check(msi.MsiGetComponentStateW(handle, "weaverDesktopShortcut", ctypes.byref(installed), ctypes.byref(requested)))
		return requested.value
	finally:
		msi.MsiCloseHandle(handle)


class DesktopShortcutTests(unittest.TestCase):
	def test_shortcut_is_opt_in_for_both_editions(self):
		for package in (desktop_package, server_package):
			with self.subTest(package=package):
				self.assertNotEqual(shortcut_action(package, ""), 3)
				self.assertNotEqual(shortcut_action(package, "0"), 3)
				self.assertEqual(shortcut_action(package, "1"), 3)

	def test_desktop_shortcut_requires_gui_feature(self):
		self.assertNotEqual(
			shortcut_action(desktop_package, "1", "ProductFeature,CM_C_Unspecified,CM_C_CLI"),
			3,
		)


if __name__ == "__main__":
	if len(sys.argv) != 3:
		sys.exit("Usage: python test_desktop_shortcut.py <desktop.msi> <server.msi>")
	desktop_package, server_package = (Path(arg).resolve(strict=True) for arg in sys.argv[1:])
	unittest.main(argv=[sys.argv[0]])
