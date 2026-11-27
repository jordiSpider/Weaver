import itertools
import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from scripts import SelectDocsBuilds


REPOSITORY_ROOT = Path(__file__).resolve().parent.parent


class DetectPushTests(unittest.TestCase):
	def setUp(self):
		self.temporary = tempfile.TemporaryDirectory()
		self.addCleanup(self.temporary.cleanup)
		self.root = Path(self.temporary.name)
		for product, versions in (("weaver", ("1", "2")), ("weaver-manager", ("3", "4"))):
			for version in versions:
				(self.root / "source" / product / version).mkdir(parents=True)

	def detect(self, paths):
		with patch.object(SelectDocsBuilds, "ROOT", self.root), patch.object(
			SelectDocsBuilds, "git_changed_paths", return_value=paths
		):
			return SelectDocsBuilds.detect_push("base", "head")

	def test_all_combinations_of_landing_and_product_versions(self):
		components = (
			("source/landing/_static/branding/favicon/favicon.svg", None, None),
			("source/weaver/1/_static/branding/branding.css", "weaver", "1"),
			("source/weaver/2/index.rst", "weaver", "2"),
			("source/weaver-manager/3/_static/branding/branding.css", "weaver-manager", "3"),
			("source/weaver-manager/4/index.rst", "weaver-manager", "4"),
		)
		for selected in itertools.product((False, True), repeat=len(components)):
			with self.subTest(selected=selected):
				paths = [component[0] for component, include in zip(components, selected) if include]
				expected = {
					product: sorted(component[2] for component, include in zip(components, selected)
						if include and component[1] == product)
					for product in SelectDocsBuilds.PRODUCTS
				}
				versions, run_landing, operation = self.detect(paths)
				self.assertEqual(versions, expected)
				self.assertEqual(run_landing, selected[0] and not any(expected.values()))
				self.assertEqual(operation, "generate" if any(selected) else "none")

	def test_global_changes_select_every_version_and_landing_is_built_by_products(self):
		versions, run_landing, operation = self.detect(["CMakeLists.txt"])
		self.assertEqual(versions, {"weaver": ["1", "2"], "weaver-manager": ["3", "4"]})
		self.assertFalse(run_landing)
		self.assertEqual(operation, "generate")

	def test_landing_helper_and_product_change_are_both_processed(self):
		versions, run_landing, operation = self.detect([
			"scripts/BuildLanding.py", "source/weaver/1/index.rst"
		])
		self.assertEqual(versions, {"weaver": ["1"], "weaver-manager": []})
		self.assertFalse(run_landing)
		self.assertEqual(operation, "generate")


@unittest.skipUnless(shutil.which("cmake") and shutil.which("ninja"), "CMake and Ninja are required")
class ProductTargetTests(unittest.TestCase):
	def test_product_targets_build_version_and_landing(self):
		with tempfile.TemporaryDirectory() as build_dir:
			subprocess.run([
				"cmake", "-S", str(REPOSITORY_ROOT), "-B", build_dir, "-G", "Ninja",
				f"-DPython3_EXECUTABLE={sys.executable}",
			], check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
			for target in ("generate-weaver", "generate-manager"):
				with self.subTest(target=target):
					result = subprocess.run(
						["cmake", "--build", build_dir, "--target", target, "--", "-n", "-v"],
						check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
					)
					output = result.stdout.replace(os.sep, "/")
					self.assertIn("scripts/BuildVersionedDocs.py", output)
					self.assertIn("scripts/BuildLanding.py", output)


if __name__ == "__main__":
	unittest.main()
