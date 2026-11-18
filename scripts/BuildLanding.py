import json
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def required_env(name):
	value = os.environ.get(name, "")
	if not value:
		raise ValueError(f"Missing environment variable: {name}")
	return value


def latest_version(path):
	with open(path, encoding="utf-8") as catalog_file:
		catalog = json.load(catalog_file)
	version = catalog.get("latest")
	if not version:
		raise ValueError(f"Catalog has no latest version: {path}")
	return version


def copy_landing_sources(source_dir, destination_dir):
	shutil.copytree(source_dir, destination_dir, dirs_exist_ok=True)


def sync_manifest(staging_dir, build_root, manifest_path):
	staging = Path(staging_dir)
	root = Path(build_root)
	manifest = Path(manifest_path)
	root.mkdir(parents=True, exist_ok=True)
	previous_files = set()
	if manifest.is_file():
		previous_files = set(json.loads(manifest.read_text(encoding="utf-8")))
	for relative in previous_files:
		if not isinstance(relative, str):
			raise ValueError(f"Invalid path in landing manifest: {relative!r}")
		path = Path(relative)
		if not path.parts or path.is_absolute() or ".." in path.parts or path.parts[0] in ("weaver", "weaver-manager"):
			raise ValueError(f"Unsafe path in landing manifest: {relative}")

	current_files = set()
	for path in staging.rglob("*"):
		if path.is_file():
			relative = path.relative_to(staging).as_posix()
			if relative.split("/", 1)[0] in ("weaver", "weaver-manager"):
				raise ValueError(f"Landing output overlaps product documentation: {relative}")
			current_files.add(relative)

	for relative in current_files:
		destination = root / relative
		if destination.is_dir():
			raise ValueError(f"Landing output conflicts with existing directory: {destination}")
		parent = destination.parent
		while parent != root:
			if parent.is_file():
				raise ValueError(f"Landing output parent conflicts with existing file: {parent}")
			parent = parent.parent
	for relative in previous_files - current_files:
		obsolete = root / relative
		if obsolete.is_dir():
			raise ValueError(f"Landing manifest points to a directory: {obsolete}")

	for relative in previous_files - current_files:
		obsolete = root / relative
		if obsolete.is_file():
			obsolete.unlink()
			parent = obsolete.parent
			while parent != root and parent.exists() and not any(parent.iterdir()):
				parent.rmdir()
				parent = parent.parent

	for relative in current_files:
		source = staging / relative
		destination = root / relative
		destination.parent.mkdir(parents=True, exist_ok=True)
		shutil.copy2(source, destination)

	manifest.parent.mkdir(parents=True, exist_ok=True)
	temporary_manifest = manifest.with_name(manifest.name + ".tmp")
	temporary_manifest.write_text(json.dumps(sorted(current_files), indent=2) + "\n", encoding="utf-8", newline="\n")
	temporary_manifest.replace(manifest)


def main():
	source_dir = Path(required_env("DOCS_LANDING_SOURCE"))
	work_root = Path(required_env("DOCS_WORK_ROOT"))
	build_root = Path(required_env("DOCS_BUILD_ROOT"))
	weaver_catalog = Path(required_env("DOCS_WEAVER_CATALOG"))
	manager_catalog = Path(required_env("DOCS_MANAGER_CATALOG"))
	sphinx_jobs = os.environ.get("DOCS_SPHINX_JOBS", "1")

	if not source_dir.is_dir():
		raise FileNotFoundError(f"Landing source not found: {source_dir}")
	source_root = Path(required_env("DOCS_SOURCE_ROOT"))
	weaver_latest = latest_version(weaver_catalog)
	manager_latest = latest_version(manager_catalog)
	weaver_logo = source_root / "weaver" / weaver_latest / "_static" / "images" / "logo.png"
	if not weaver_logo.is_file():
		raise FileNotFoundError(f"Weaver logo not found for latest version {weaver_latest}: {weaver_logo}")
	work_root.mkdir(parents=True, exist_ok=True)
	with tempfile.TemporaryDirectory(prefix="landing-", dir=work_root) as temporary_root:
		temp_root = Path(temporary_root)
		work_source = temp_root / "source" / "landing"
		copy_landing_sources(source_dir, work_source)
		staged_logo = work_source.parent / "weaver" / "latest" / "_static" / "images" / "logo.png"
		staged_logo.parent.mkdir(parents=True, exist_ok=True)
		shutil.copy2(weaver_logo, staged_logo)

		environment = os.environ.copy()
		environment["DOCS_WEAVER_CATALOG"] = str(weaver_catalog)
		environment["DOCS_MANAGER_CATALOG"] = str(manager_catalog)
		environment["WEAVER_DOCS_CATALOG"] = str(weaver_catalog)
		environment["WEAVER_MANAGER_DOCS_CATALOG"] = str(manager_catalog)
		environment["DOCS_SOURCE_ROOT"] = str(source_root)
		staged_output = temp_root / "html"
		doctrees = temp_root / "doctrees"
		command = [
			sys.executable, "-m", "sphinx", "-E", "-b", "html",
			"-d", str(doctrees), "-q", "-j", sphinx_jobs,
			str(work_source), str(staged_output),
		]
		subprocess.run(command, check=True, env=environment)
		sync_manifest(staged_output, build_root, build_root / ".landing-files.json")

	print(f"Regenerated landing page. Weaver latest={weaver_latest}; Manager latest={manager_latest}")


if __name__ == "__main__":
	main()
