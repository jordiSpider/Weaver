#!/usr/bin/env python3
"""Generate the project's raster images and platform icons from its SVG sources."""

import argparse
import io
import json
import shutil
import tempfile
from pathlib import Path

import resvg_py
from PIL import Image


ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "source"
ICON_SIZES = (16, 24, 32, 48, 64, 128, 256)
FAVICON_SIZES = (16, 32, 48)
MAC_SIZES = (32, 64, 128, 256, 512, 1024)


def render(source, width=None, height=None):
	data = resvg_py.svg_to_bytes(svg_path=str(source), width=width, height=height)
	with Image.open(io.BytesIO(data)) as image:
		return image.convert("RGBA")


def centered(source, width, height, max_width, max_height, background):
	image = render(source, width=max_width)
	if image.height > max_height:
		image = render(source, height=max_height)
	if image.width > max_width or image.height > max_height:
		raise ValueError(f"SVG did not fit the requested bounds: {source}")
	canvas = Image.new("RGBA", (width, height), background)
	canvas.alpha_composite(image, ((width - image.width) // 2, (height - image.height) // 2))
	return canvas


def icon_image(source, size, scale=1.0, background=(0, 0, 0, 0)):
	extent = max(1, round(size * scale))
	return centered(source, size, size, extent, extent, background)


def save_ico(source, output, sizes, small_source=None):
	images = [icon_image(small_source if small_source and size <= 48 else source, size) for size in sizes]
	images[-1].save(output, format="ICO", sizes=[(size, size) for size in sizes],
					append_images=images[:-1], bitmap_format="bmp")


def save_icns(source, output, small_source=None):
	images = [icon_image(small_source if small_source and size <= 48 else source, size) for size in MAC_SIZES]
	images[-1].save(output, format="ICNS", append_images=images[:-1])


def generate_variants(output):
	for name in ("weaver-manager", "weaver-uninstall"):
		variant = SOURCE / f"{name}.svg"
		small_variant = SOURCE / f"{name}-small.svg"
		for path in (variant, small_variant):
			if not path.is_file():
				raise FileNotFoundError(f"Required SVG source not found: {path}")
		for size in (256, 512, 1024):
			icon_image(variant, size).save(output / f"{name}-{size}.png")
		save_ico(variant, output / f"{name}.ico", ICON_SIZES, small_source=small_variant)
		save_icns(variant, output / f"{name}.icns", small_source=small_variant)


def generate(output, variants_only=False):
	if variants_only:
		generate_variants(output)
		return
	icon = SOURCE / "weaver-icon.svg"
	small_icon = SOURCE / "weaver-icon-small.svg"
	horizontal = SOURCE / "weaver-logo-horizontal.svg"
	required = [icon, small_icon, horizontal,
				SOURCE / "weaver-logo-horizontal-dark.svg",
				SOURCE / "weaver-logo-vertical.svg", SOURCE / "weaver-logo-vertical-dark.svg"]
	for path in required:
		if not path.is_file():
			raise FileNotFoundError(f"Required SVG source not found: {path}")

	favicon = output / "favicon"
	favicon.mkdir(parents=True)
	white = (255, 255, 255, 255)
	render(horizontal, 1200).save(output / "weaver-header.png")
	centered(horizontal, 1280, 640, 1120, 520, white).save(output / "weaver-social-card.png")
	icon_image(icon, 512, 0.8, white).save(output / "weaver-avatar.png")
	for size in (256, 512):
		icon_image(icon, size).save(output / f"weaver-icon-{size}.png")
	icon_image(icon, 1024).save(output / "weaver-mac-1024.png")
	for variant in ("horizontal", "horizontal-dark", "vertical", "vertical-dark"):
		render(SOURCE / f"weaver-logo-{variant}.svg", 1200).save(output / f"weaver-logo-{variant}.png")
	save_ico(icon, output / "weaver.ico", ICON_SIZES, small_source=small_icon)
	save_icns(icon, output / "weaver-mac-1024.icns")

	shutil.copyfile(small_icon, favicon / "favicon.svg")
	icon_image(small_icon, 96).save(favicon / "favicon-96x96.png")
	save_ico(small_icon, favicon / "favicon.ico", FAVICON_SIZES)
	icon_image(small_icon, 180, 0.8, white).save(favicon / "apple-touch-icon.png")
	for size in (192, 512):
		# A square occupying 55% of the canvas fits within the 80% maskable safe circle.
		icon_image(small_icon, size, 0.55, white).save(favicon / f"web-app-manifest-{size}x{size}.png")
	manifest = {
		"name": "Weaver",
		"short_name": "Weaver",
		"icons": [
			{"src": f"web-app-manifest-{size}x{size}.png", "sizes": f"{size}x{size}",
			 "type": "image/png", "purpose": "maskable"}
			for size in (192, 512)
		],
		"theme_color": "#ffffff",
		"background_color": "#ffffff",
		"display": "standalone",
	}
	(favicon / "site.webmanifest").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

	generate_variants(output)


def main():
	parser = argparse.ArgumentParser(description=__doc__)
	parser.add_argument("--output", type=Path, default=ROOT / "generated",
						help="destination directory (default: assets/branding/generated)")
	parser.add_argument("--variants-only", action="store_true",
						help="generate only Manager and uninstaller icons")
	args = parser.parse_args()
	destination = args.output.resolve()
	if destination == SOURCE or SOURCE in destination.parents or destination in SOURCE.parents:
		parser.error("The output directory must not contain or be inside the SVG source directory")

	with tempfile.TemporaryDirectory(prefix="weaver-branding-") as temporary:
		staging = Path(temporary)
		generate(staging, variants_only=args.variants_only)
		for path in sorted(staging.rglob("*")):
			if path.is_file():
				target = destination / path.relative_to(staging)
				target.parent.mkdir(parents=True, exist_ok=True)
				shutil.copyfile(path, target)
				print(target)


if __name__ == "__main__":
	main()
