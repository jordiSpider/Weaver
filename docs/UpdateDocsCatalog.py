import argparse
import json
import os
import re
import tempfile


def parse_args():
	parser = argparse.ArgumentParser()
	parser.add_argument("--catalog", required=True)
	parser.add_argument("--version", required=True)
	parser.add_argument("--channel", choices=("stable", "pre-release"), required=True)
	parser.add_argument("--latest", choices=("true", "false"), required=True)
	parser.add_argument("--required-assets-dir")
	return parser.parse_args()


def read_catalog(path):
	if not os.path.isfile(path):
		return {"schema_version": 1, "latest": None, "versions": []}

	with open(path, encoding="utf-8") as catalog_file:
		catalog = json.load(catalog_file)

	if catalog.get("schema_version") != 1 or not isinstance(catalog.get("versions"), list):
		raise ValueError(f"Unsupported documentation catalog schema: {path}")

	return catalog


def write_catalog(path, catalog):
	os.makedirs(os.path.dirname(path), exist_ok=True)
	descriptor, temporary_path = tempfile.mkstemp(dir=os.path.dirname(path), prefix="versions-", suffix=".json")
	try:
		with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as catalog_file:
			json.dump(catalog, catalog_file, indent=2, ensure_ascii=False)
			catalog_file.write("\n")
		os.replace(temporary_path, path)
	finally:
		if os.path.exists(temporary_path):
			os.remove(temporary_path)


def write_version_metadata(site_root, catalog):
	for entry in catalog["versions"]:
		version_directory = os.path.join(site_root, entry["version"])
		os.makedirs(version_directory, exist_ok=True)
		metadata_path = os.path.join(version_directory, "version.json")
		with open(metadata_path, "w", encoding="utf-8", newline="\n") as metadata_file:
			json.dump(entry, metadata_file, indent=2, ensure_ascii=False)
			metadata_file.write("\n")


def has_required_assets(releases_dir, version):
	required_assets = ("weaver-manager-manifest.json", "weaver-manager-manifest.json.sig")
	return any(
		os.path.isdir(os.path.join(releases_dir, directory))
		and re.sub(r"^v", "", directory) == re.sub(r"^v", "", version)
		and all(os.path.isfile(os.path.join(releases_dir, directory, asset)) for asset in required_assets)
		for directory in os.listdir(releases_dir)
	)


def version_sort_key(version):
	return tuple(int(part) for part in re.findall(r"\d+", version))


def main():
	args = parse_args()
	if args.version in (".", "..") or "/" in args.version or "\\" in args.version:
		raise ValueError("Version must be a single path component.")
	if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._+-]*", args.version):
		raise ValueError("Version contains unsupported characters.")
	if args.channel == "pre-release" and args.latest == "true":
		raise ValueError("A pre-release cannot be marked as latest.")
	if args.required_assets_dir:
		if not os.path.isdir(args.required_assets_dir):
			raise ValueError(f"Release assets directory does not exist: {args.required_assets_dir}")

	catalog = read_catalog(args.catalog)
	entries = catalog["versions"]
	if args.required_assets_dir:
		entries[:] = [entry for entry in entries if has_required_assets(args.required_assets_dir, entry["version"])]
	current_entry = next((entry for entry in entries if entry.get("version") == args.version), None)
	if current_entry is None and (not args.required_assets_dir or has_required_assets(args.required_assets_dir, args.version)):
		current_entry = {"version": args.version}
		entries.append(current_entry)

	if current_entry is not None:
		current_entry["channel"] = args.channel
		current_entry["latest"] = args.latest == "true"

	if current_entry is not None and current_entry["latest"]:
		catalog["latest"] = args.version
		for entry in entries:
			entry["latest"] = entry is current_entry
	elif catalog.get("latest") not in {entry["version"] for entry in entries} or catalog.get("latest") == args.version:
		stable_versions = sorted(
			(entry["version"] for entry in entries if entry.get("channel") == "stable"),
			key=version_sort_key,
			reverse=True,
		)
		catalog["latest"] = stable_versions[0] if stable_versions else None
		for entry in entries:
			entry["latest"] = entry.get("version") == catalog["latest"]
	if not catalog.get("latest"):
		raise ValueError("The documentation catalog must contain a latest stable version.")

	entries.sort(key=lambda entry: (not entry.get("latest", False), entry["version"]))
	write_catalog(args.catalog, catalog)
	write_version_metadata(os.path.dirname(args.catalog), catalog)


if __name__ == "__main__":
	main()
