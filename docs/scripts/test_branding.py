import shutil
import tempfile
import unittest
from pathlib import Path

from BuildVersionedDocs import prepare_sphinx_source
from PrepareBranding import BRANDING, FAVICONS, LOGOS, SCRIPTS, prepare_branding


class DocumentationBrandingTests(unittest.TestCase):
	def setUp(self):
		self.temporary = tempfile.TemporaryDirectory()
		self.addCleanup(self.temporary.cleanup)
		self.root = Path(self.temporary.name)
		self.source = self.root / "source"
		self.source.mkdir()
		(self.source / "conf.py").write_text('html_theme_options = {}\n', encoding="utf-8")

	def test_prepare_branding_and_version_snapshot(self):
		prepare_branding(self.source)
		assets = self.source / "_static" / "branding"
		for name in LOGOS:
			self.assertEqual((assets / name).read_bytes(), (BRANDING / "source" / name).read_bytes())
		for name in FAVICONS:
			self.assertEqual((assets / "favicon" / name).read_bytes(), (BRANDING / "generated" / "favicon" / name).read_bytes())
		self.assertEqual((assets / "branding.css").read_bytes(), (SCRIPTS / "branding.css").read_bytes())
		self.assertEqual((self.source / "_templates" / "layout.html").read_bytes(), (SCRIPTS / "branding-layout.html").read_bytes())
		conf = prepare_sphinx_source("weaver-manager", self.source, self.root / "output", "latest", "en", ["en"])
		self.assertEqual((conf.parent / "_static" / "branding" / LOGOS[0]).read_bytes(), (assets / LOGOS[0]).read_bytes())
		self.assertEqual(conf.read_text(encoding="utf-8"), (self.source / "conf.py").read_text(encoding="utf-8"))
		(assets / LOGOS[0]).write_text("outdated", encoding="utf-8")
		prepare_branding(self.source)
		self.assertEqual((assets / LOGOS[0]).read_bytes(), (BRANDING / "source" / LOGOS[0]).read_bytes())
		self.assertEqual((conf.parent / "_static" / "branding" / LOGOS[0]).read_bytes(), (assets / LOGOS[0]).read_bytes())

	def test_missing_asset_fails_before_copy(self):
		branding = self.root / "branding"
		shutil.copytree(BRANDING, branding)
		(branding / "generated" / "favicon" / FAVICONS[-1]).unlink()
		with self.assertRaisesRegex(FileNotFoundError, "Required documentation branding file not found"):
			prepare_branding(self.source, branding)
		self.assertFalse((self.source / "_static").exists())

	def test_old_version_without_branding_remains_unchanged(self):
		conf = prepare_sphinx_source("weaver-manager", self.source, self.root / "output", "old", "en", ["en"])
		self.assertEqual(conf.read_text(encoding="utf-8"), 'html_theme_options = {}\n')
		self.assertFalse((conf.parent / "_static").exists())


if __name__ == "__main__":
	unittest.main()
