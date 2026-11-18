import argparse
import os
import shutil
import sys
import tempfile
from pathlib import Path

import UpdateReleaseCatalogs as catalogs


def safe_version(value):
	if value in ("", ".", "..") or any(separator in value for separator in ("/", "\\")):
		raise ValueError("Version must be a non-empty path component")
	return value


def remove_version(product, version, repository, api_url, source_root, output_root, catalog_path, token):
	version = safe_version(version)
	source = source_root / version
	output = output_root / version
	if not source.is_dir():
		raise FileNotFoundError(f"Version source not found: {source}")

	releases = catalogs.fetch_releases(repository, api_url, token)
	github_latest = catalogs.fetch_github_latest(repository, api_url, token)
	try:
		remaining_catalog = catalogs.build_catalog(product, str(source_root), releases, github_latest, {version})
	except RuntimeError as error:
		if "No eligible" in str(error):
			if "stable" in str(error):
				reason = "no eligible stable version remains to determine the latest release"
			else:
				reason = "no eligible version remains to document"
			raise ValueError(
				f"Cannot remove {product} version {version}: {reason}. "
				"At least one eligible documentation version must remain for the product. No files were removed."
			) from None
		raise

	paths = [source]
	if output.exists():
		paths.append(output)
	staged_paths = [
		(path, path.with_name(path.name + ".removing"))
		for path in paths
	]
	for _, staged in staged_paths:
		if staged.exists():
			raise FileExistsError(f"Removal staging path already exists: {staged}")

	try:
		for original, staged in staged_paths:
			original.rename(staged)
		catalogs.atomic_write_json(str(catalog_path), remaining_catalog)
	except Exception:
		for original, staged in reversed(staged_paths):
			if staged.exists() and not original.exists():
				staged.rename(original)
		raise

	for _, staged in staged_paths:
		if staged.is_dir():
			shutil.rmtree(staged)
		elif staged.exists():
			staged.unlink()

	print(f"Removed {product} documentation version {version}; {remaining_catalog['latest']} is latest")


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("--product", choices=("weaver", "weaver-manager"), required=True)
	parser.add_argument("--version", required=True)
	parser.add_argument("--repository", required=True)
	parser.add_argument("--api-url", default="https://api.github.com")
	parser.add_argument("--source-root", type=Path, required=True)
	parser.add_argument("--output-root", type=Path, required=True)
	parser.add_argument("--catalog", type=Path, required=True)
	parser.add_argument("--token", default=os.environ.get("GITHUB_TOKEN", ""))
	args = parser.parse_args()

	try:
		remove_version(
			args.product,
			args.version,
			args.repository,
			args.api_url,
			args.source_root,
			args.output_root,
			args.catalog,
			args.token,
		)
	except ValueError as error:
		print(f"\033[31mERROR: {error}\033[0m", file=sys.stderr)
		sys.exit(1)


if __name__ == "__main__":
	main()
