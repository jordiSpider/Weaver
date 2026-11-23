import argparse
import html
import json
import os
import re
from pathlib import Path


VERSION_SWITCHER = re.compile(
	r'(<div class="dropdown">\s*<button\b[^>]*id="version-switcher"[^>]*>.*?</button>\s*'
	r'<ul\b[^>]*aria-labelledby="version-switcher"[^>]*>)(.*?)(</ul>\s*</div>)',
	re.DOTALL,
)
VERSION_SWITCHER_BUTTON = re.compile(
	r'(<button\b[^>]*id="version-switcher"[^>]*)(>\s*)(.*?)(\s*</button>)',
	re.DOTALL,
)
PRERELEASE_NOTICE = re.compile(
	r'\s*<div\b[^>]*id="prerelease-notice"[^>]*>.*?</div>',
	re.DOTALL,
)
HEADER_END_TAG = re.compile(r'(</header\s*>)', re.IGNORECASE)


def refresh_prerelease_notice(content, catalog, current_version):
	content = PRERELEASE_NOTICE.sub("", content)
	entry = next(
		(item for item in catalog.get("versions", []) if item.get("version") == current_version),
		None,
	)
	if not entry or entry.get("channel") != "pre-release":
		return content

	notice = (
		'\n<div id="prerelease-notice" class="alert alert-warning rounded-0 mb-0" '
		'role="alert" style="text-align: center">'
		'<strong>Pre-release documentation:</strong> This version is for testing and may contain errors. '
		'Content and behavior may change frequently. For a stable release, choose a version marked '
		'<strong>Latest</strong> or <strong>Stable</strong> in the version selector.'
		'</div>'
	)
	return HEADER_END_TAG.sub(lambda match: match.group(1) + notice, content, count=1)


def selector_items(catalog, output_root, source_root, current_version, current_language, page_path, default_language):
	versions = [
		entry for entry in catalog.get("versions", [])
		if (output_root / entry["version"] / "index.html").is_file()
	]
	versions.sort(key=lambda entry: tuple(int(value) for value in re.findall(r"\d+", entry.get("tag", entry["version"]))), reverse=True)
	if current_version not in {entry["version"] for entry in versions}:
		versions.append({"version": current_version, "channel": "stable", "latest": False})

	page_dir = page_path.parent
	items = []
	for entry in versions:
		version = entry["version"]
		locale_dir = source_root / version / "locale"
		available_languages = {path.name for path in locale_dir.iterdir() if path.is_dir()} if locale_dir.is_dir() else set()
		target_language = current_language if current_language in available_languages else default_language
		target = output_root / version
		if target_language != default_language:
			translated_index = target / target_language / "index.html"
			if translated_index.is_file():
				target = target / target_language
		href = os.path.relpath(target / "index.html", page_dir).replace("\\", "/")
		label = f"{version} ({entry.get('channel', 'stable')}"
		if entry.get("latest", False) or version == catalog.get("latest"):
			label += ", latest"
		label += ")"
		active = " active" if version == current_version else ""
		color = "#198754" if version == catalog.get("latest") else "#fd7e14" if entry.get("channel") == "pre-release" else "#0d6efd"
		items.append(
			'    <li>\n'
			f'      <a class="dropdown-item{active}" style="color: {color} !important" href="{html.escape(href, quote=True)}">\n'
			f'        {html.escape(label)}\n'
			'      </a>\n'
			'    </li>'
		)
	return "\n" + "\n".join(items) + "\n    "


def selector_button(catalog, current_version):
	entry = next(
		(item for item in catalog.get("versions", []) if item.get("version") == current_version),
		{"version": current_version, "channel": "stable", "latest": False},
	)
	channel = entry.get("channel", "stable")
	is_latest = entry.get("latest", False) or current_version == catalog.get("latest")
	color = "#198754" if is_latest else "#fd7e14" if channel == "pre-release" else "#0d6efd"
	label = f"{current_version} ({channel}"
	if is_latest:
		label += ", latest"
	label += ")"
	return color, label


def refresh_selector_button(content, catalog, current_version):
	color, label = selector_button(catalog, current_version)

	def replace_button(match):
		open_tag = re.sub(r'\sstyle="[^"]*"', "", match.group(1))
		open_tag += f' style="color: {color} !important"'
		return f'{open_tag}{match.group(2)}{html.escape(label)}{match.group(4)}'

	return VERSION_SWITCHER_BUTTON.sub(replace_button, content)


def refresh_selectors(product, catalog_path, output_root, source_root, default_language):
	catalog = json.loads(catalog_path.read_text(encoding="utf-8"))
	updates = []
	for version_dir in sorted(path for path in output_root.iterdir() if path.is_dir()):
		version = version_dir.name
		for page in version_dir.rglob("*.html"):
			language = default_language
			relative = page.relative_to(version_dir)
			locale_dir = source_root / product / version / "locale"
			available_languages = {path.name for path in locale_dir.iterdir() if path.is_dir()} if locale_dir.is_dir() else set()
			if relative.parts and relative.parts[0] in available_languages:
				language = relative.parts[0]

			content = page.read_text(encoding="utf-8")
			updated = refresh_prerelease_notice(content, catalog, version)
			updated = refresh_selector_button(updated, catalog, version)
			replacement = selector_items(catalog, output_root, source_root / product, version, language, page, default_language)
			updated = VERSION_SWITCHER.sub(lambda match: match.group(1) + replacement + match.group(3), updated)
			if updated != content:
				updates.append((page, updated))

	for page, content in updates:
		page.write_text(content, encoding="utf-8", newline="\n")
	print(f"Updated version selectors in {len(updates)} {product} HTML page(s)")


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("--product", choices=("weaver", "weaver-manager"), required=True)
	parser.add_argument("--catalog", type=Path, required=True)
	parser.add_argument("--output-root", type=Path, required=True)
	parser.add_argument("--source-root", type=Path, required=True)
	parser.add_argument("--default-language", default="en")
	args = parser.parse_args()
	refresh_selectors(args.product, args.catalog, args.output_root, args.source_root, args.default_language)


if __name__ == "__main__":
	main()
