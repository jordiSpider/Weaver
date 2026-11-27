import hashlib
import io
import json
import shutil
import tempfile
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path
from urllib.parse import urlsplit

import resvg_py
from PIL import Image, ImageChops

import generate


class BrandingGenerationTests(unittest.TestCase):
	@classmethod
	def setUpClass(cls):
		cls.temporary = tempfile.TemporaryDirectory()
		cls.root = Path(cls.temporary.name)
		cls.output = cls.root / "generated"
		cls.output.mkdir()
		generate.generate(cls.output)

	@classmethod
	def tearDownClass(cls):
		cls.temporary.cleanup()

	def test_png_dimensions_and_backgrounds(self):
		expected = {
			"weaver-header.png": ((1200, 544), 0),
			"weaver-social-card.png": ((1280, 640), 255),
			"weaver-avatar.png": ((512, 512), 255),
			"weaver-icon-256.png": ((256, 256), 0),
			"weaver-icon-512.png": ((512, 512), 0),
			"weaver-mac-1024.png": ((1024, 1024), 0),
			"weaver-logo-horizontal.png": ((1200, 544), 0),
			"weaver-logo-horizontal-dark.png": ((1200, 544), 0),
			"weaver-logo-vertical.png": ((1200, 1200), 0),
			"weaver-logo-vertical-dark.png": ((1200, 1200), 0),
			"favicon/favicon-96x96.png": ((96, 96), 0),
			"favicon/apple-touch-icon.png": ((180, 180), 255),
			"favicon/web-app-manifest-192x192.png": ((192, 192), 255),
			"favicon/web-app-manifest-512x512.png": ((512, 512), 255),
		}
		for name, (size, alpha) in expected.items():
			with self.subTest(name=name), Image.open(self.output / name) as image:
				self.assertEqual(image.size, size)
				self.assertEqual(image.convert("RGBA").getpixel((0, 0))[3], alpha)
				self.assertIsNotNone(image.getbbox())

	def test_ico_sizes_and_transparency(self):
		for name, sizes in (("weaver.ico", generate.ICON_SIZES),
							("favicon/favicon.ico", generate.FAVICON_SIZES)):
			with self.subTest(name=name), Image.open(self.output / name) as image:
				self.assertEqual(image.ico.sizes(), {(size, size) for size in sizes})
				for size in sizes:
					frame = image.ico.getimage((size, size)).convert("RGBA")
					self.assertLess(frame.getchannel("A").getextrema()[0], 255)
					self.assertGreater(frame.getchannel("A").getextrema()[1], 0)
					source = "weaver-icon-small.svg" if name.startswith("favicon/") or size <= 48 else "weaver-icon.svg"
					self.assertEqual(frame.tobytes(), generate.icon_image(generate.SOURCE / source, size).tobytes())

	def test_icns_sizes(self):
		with Image.open(self.output / "weaver-mac-1024.icns") as image:
			actual = {width * scale for width, height, scale in image.info["sizes"] if width == height}
			self.assertTrue(set(generate.MAC_SIZES).issubset(actual))

	def test_maskable_safe_area_and_manifest_paths(self):
		manifest_path = self.output / "favicon" / "site.webmanifest"
		manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
		for item in manifest["icons"]:
			self.assertEqual(item["purpose"], "maskable")
			url = urlsplit(item["src"])
			self.assertFalse(url.scheme or url.netloc or url.path.startswith("/"))
			with Image.open(manifest_path.parent / url.path) as image:
				image = image.convert("RGB")
				self.assertEqual(item["sizes"], f"{image.width}x{image.height}")
				bounds = ImageChops.difference(image, Image.new("RGB", image.size, "white")).getbbox()
				self.assertIsNotNone(bounds)
				center = image.width / 2
				radius = image.width * 0.4
				for x in (bounds[0], bounds[2]):
					for y in (bounds[1], bounds[3]):
						self.assertLessEqual((x - center) ** 2 + (y - center) ** 2, radius ** 2)

	def test_generation_is_deterministic(self):
		second = self.root / "second"
		second.mkdir(exist_ok=True)
		generate.generate(second)
		for path in self.output.rglob("*"):
			if path.is_file():
				other = second / path.relative_to(self.output)
				self.assertEqual(hashlib.sha256(path.read_bytes()).digest(), hashlib.sha256(other.read_bytes()).digest())

	def test_variant_icons(self):
		for name in ("weaver-manager", "weaver-uninstall"):
			with self.subTest(name=name):
				for size in (256, 512, 1024):
					with Image.open(self.output / f"{name}-{size}.png") as image:
						self.assertEqual(image.size, (size, size))
						self.assertIsNotNone(image.convert("RGBA").getchannel("A").getbbox())
						self.assertEqual(image.convert("RGBA").tobytes(),
										 generate.icon_image(generate.SOURCE / f"{name}.svg", size).tobytes())
				with Image.open(self.output / f"{name}.ico") as image:
					self.assertEqual(image.ico.sizes(), {(size, size) for size in generate.ICON_SIZES})
					for size in generate.ICON_SIZES:
						suffix = "-small" if size <= 48 else ""
						frame = image.ico.getimage((size, size)).convert("RGBA")
						self.assertEqual(frame.tobytes(), generate.icon_image(generate.SOURCE / f"{name}{suffix}.svg", size).tobytes())
				with Image.open(self.output / f"{name}.icns") as image:
					actual = {width * scale for width, height, scale in image.info["sizes"] if width == height}
					self.assertTrue(set(generate.MAC_SIZES).issubset(actual))
					frame = image.icns.getimage((16, 16, 2)).convert("RGBA")
					self.assertEqual(frame.tobytes(),
									 generate.icon_image(generate.SOURCE / f"{name}-small.svg", 32).tobytes())

	def test_variant_base_symbol_matches_weaver_framing(self):
		namespace = "{http://www.w3.org/2000/svg}"

		def base_bounds(name):
			root = ET.parse(generate.SOURCE / name).getroot()
			symbol = ET.Element(namespace + "svg", root.attrib)
			symbol.append(root.find(namespace + "path"))
			data = resvg_py.svg_to_bytes(svg_string=ET.tostring(symbol, encoding="unicode"), width=512)
			with Image.open(io.BytesIO(data)) as image:
				return image.convert("RGBA").getchannel("A").point(lambda alpha: 255 if alpha >= 128 else 0).getbbox()

		for suffix in ("", "-small"):
			reference = base_bounds(f"weaver-icon{suffix}.svg")
			self.assertIsNotNone(reference)
			for name in ("weaver-manager", "weaver-uninstall"):
				with self.subTest(name=name, suffix=suffix):
					bounds = base_bounds(f"{name}{suffix}.svg")
					self.assertIsNotNone(bounds)
					for actual, expected in zip(bounds, reference):
						self.assertLessEqual(abs(actual - expected), 2)

	def test_missing_small_variant_fails(self):
		source = self.root / "source"
		shutil.copytree(generate.SOURCE, source)
		(source / "weaver-manager-small.svg").unlink()
		old_source = generate.SOURCE
		try:
			generate.SOURCE = source
			with self.assertRaisesRegex(FileNotFoundError, "weaver-manager-small.svg"):
				generate.generate_variants(self.root)
		finally:
			generate.SOURCE = old_source

	def test_variants_only(self):
		output = self.root / "variants-only"
		output.mkdir()
		generate.generate(output, variants_only=True)
		self.assertEqual({path.name for path in output.iterdir()},
						 {f"{name}{suffix}" for name in ("weaver-manager", "weaver-uninstall")
						  for suffix in ("-256.png", "-512.png", "-1024.png", ".ico", ".icns")})


if __name__ == "__main__":
	unittest.main()
