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


def clear_landing_output(build_root):
	root = Path(build_root)
	root.mkdir(parents=True, exist_ok=True)
	for path in root.iterdir():
		if path.name in ("weaver", "weaver-manager"):
			continue
		if path.name == ".nojekyll" and not path.is_dir():
			continue
		if path.is_dir() and not path.is_symlink():
			shutil.rmtree(path)
		else:
			path.unlink()


def publish_landing(staging_dir, build_root):
	staging = Path(staging_dir)
	root = Path(build_root)
	root.mkdir(parents=True, exist_ok=True)
	current_files = set()
	for path in staging.rglob("*"):
		if path.is_file():
			relative = path.relative_to(staging).as_posix()
			relative_path = Path(relative)
			if not relative_path.parts or relative_path.is_absolute() or ".." in relative_path.parts:
				raise ValueError(f"Unsafe path in landing output: {relative}")
			if relative_path.parts[0] in (".nojekyll", "weaver", "weaver-manager"):
				raise ValueError(f"Landing output overlaps preserved content: {relative}")
			current_files.add(relative)

	clear_landing_output(root)

	for relative in current_files:
		source = staging / relative
		destination = root / relative
		destination.parent.mkdir(parents=True, exist_ok=True)
		shutil.copy2(source, destination)


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
		publish_landing(staged_output, build_root)

	print(f"Regenerated landing page. Weaver latest={weaver_latest}; Manager latest={manager_latest}")


if __name__ == "__main__":
	main()
