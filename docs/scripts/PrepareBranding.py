import argparse
import shutil
from pathlib import Path


SCRIPTS = Path(__file__).resolve().parent
BRANDING = SCRIPTS.parents[1] / "assets" / "branding"
LOGOS = ("weaver-logo-horizontal.svg", "weaver-logo-horizontal-dark.svg")
FAVICONS = (
	"favicon.svg", "favicon.ico", "favicon-96x96.png", "apple-touch-icon.png",
	"web-app-manifest-192x192.png", "web-app-manifest-512x512.png", "site.webmanifest",
)


def prepare_branding(source, branding=BRANDING):
	assets = [(branding / "source" / name, Path("_static/branding") / name) for name in LOGOS]
	assets += [(branding / "generated" / "favicon" / name, Path("_static/branding/favicon") / name)
			   for name in FAVICONS]
	assets += [(SCRIPTS / "branding.css", Path("_static/branding/branding.css")),
			   (SCRIPTS / "branding-layout.html", Path("_templates/layout.html"))]
	if not source.is_dir():
		raise FileNotFoundError(f"Documentation source directory not found: {source}")
	for input_path, _ in assets:
		if not input_path.is_file():
			raise FileNotFoundError(f"Required documentation branding file not found: {input_path}")
	for input_path, relative_path in assets:
		destination = source / relative_path
		destination.parent.mkdir(parents=True, exist_ok=True)
		shutil.copy2(input_path, destination)


def main():
	parser = argparse.ArgumentParser(description="Copy branding into a documentation source directory.")
	parser.add_argument("source", type=Path)
	args = parser.parse_args()
	prepare_branding(args.source)


if __name__ == "__main__":
	main()
