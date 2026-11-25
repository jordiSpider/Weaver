import argparse
import json
import re
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
PRODUCTS = ("weaver", "weaver-manager")
VERSION_PATTERN = re.compile(r"[A-Za-z0-9][A-Za-z0-9._+-]*\Z")
GLOBAL_FILES = {"CMakeLists.txt", "CMakePresets.json", "requirements-docs.txt"}
LANDING_SCRIPTS = {
	"BuildLanding.py",
	"UpdateReleaseCatalogs.py",
	"UpdateVersionSelectors.py",
}
NON_RENDERING_SCRIPTS = {
	"RemoveDocsVersion.py",
	"RemoveDocsVersion.cmake",
	"SelectDocsBuilds.py",
}


def require_version(value):
	if not VERSION_PATTERN.fullmatch(value):
		raise ValueError(f"Invalid documentation version: {value!r}")
	return value


def source_versions(product):
	product_root = ROOT / "source" / product
	if not product_root.is_dir():
		return []
	return sorted(path.name for path in product_root.iterdir() if path.is_dir())


def git_changed_paths(base, head):
	if set(base) == {"0"}:
		output = subprocess.check_output(
			["git", "ls-tree", "-r", "--name-only", "-z", head],
			cwd=ROOT,
		)
	else:
		output = subprocess.check_output(
			["git", "diff", "--name-only", "--no-renames", "-z", base, head],
			cwd=ROOT,
		)
	return [path.decode("utf-8", errors="surrogateescape") for path in output.split(b"\0") if path]


def add_all_versions(versions):
	for product in PRODUCTS:
		versions[product].update(source_versions(product))


def detect_push(base, head):
	versions = {product: set() for product in PRODUCTS}
	landing_changed = False
	all_docs_changed = False

	for changed_path in git_changed_paths(base, head):
		path = Path(changed_path)
		parts = path.parts
		normalized_path = path.as_posix()

		if normalized_path in GLOBAL_FILES:
			all_docs_changed = True
			continue

		if normalized_path.startswith("scripts/"):
			filename = path.name
			if filename == "BuildVersionedDocs.py":
				all_docs_changed = True
			elif filename in LANDING_SCRIPTS:
				landing_changed = True
			elif filename not in NON_RENDERING_SCRIPTS:
				all_docs_changed = True
			continue

		if len(parts) < 2 or parts[0] != "source":
			continue

		if parts[1] == "landing":
			landing_changed = True
			continue

		product = parts[1]
		if product not in PRODUCTS:
			continue

		if len(parts) < 3:
			versions[product].update(source_versions(product))
			continue

		version = require_version(parts[2])
		version_root = ROOT / "source" / product / version
		if not version_root.is_dir():
			# Source-version deletion is intentionally handled only by the guarded manual action.
			raise ValueError(
				f"The source directory for {product} {version} was removed. "
				"Use the manual remove-weaver or remove-manager action instead."
			)
		versions[product].add(version)

	if all_docs_changed:
		add_all_versions(versions)
		landing_changed = True

	selected_versions = {product: sorted(items) for product, items in versions.items()}
	run_landing = landing_changed and not any(selected_versions.values())
	has_work = run_landing or any(selected_versions.values())
	return selected_versions, run_landing, "generate" if has_work else "none"


def detect_dispatch(scope, version, confirm_removal):
	versions = {product: [] for product in PRODUCTS}
	run_landing = False
	operation = "generate"

	if scope == "landing":
		run_landing = True
	elif scope in PRODUCTS:
		version = require_version(version)
		if not (ROOT / "source" / scope / version).is_dir():
			raise ValueError(f"Documentation source directory not found: source/{scope}/{version}")
		versions[scope] = [version]
	elif scope in ("remove-weaver", "remove-manager"):
		if not confirm_removal:
			raise ValueError("Version removal requires confirm_removal=true")
		product = "weaver" if scope == "remove-weaver" else "weaver-manager"
		version = require_version(version)
		if not (ROOT / "source" / product / version).is_dir():
			raise ValueError(f"Documentation source directory not found: source/{product}/{version}")
		operation = scope
	else:
		raise ValueError(f"Unsupported action: {scope}")

	return versions, run_landing, operation


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("--event", choices=("push", "dispatch"), required=True)
	parser.add_argument("--base", default="")
	parser.add_argument("--head", default="HEAD")
	parser.add_argument("--scope", default="")
	parser.add_argument("--version", default="")
	parser.add_argument("--confirm-removal", action="store_true")
	args = parser.parse_args()

	if args.event == "push":
		versions, run_landing, operation = detect_push(args.base, args.head)
	else:
		versions, run_landing, operation = detect_dispatch(
			args.scope,
			args.version,
			args.confirm_removal,
		)

	print(f"weaver_versions={json.dumps(versions['weaver'], separators=(',', ':'))}")
	print(f"manager_versions={json.dumps(versions['weaver-manager'], separators=(',', ':'))}")
	print(f"run_landing={str(run_landing).lower()}")
	print(f"operation={operation}")


if __name__ == "__main__":
	main()
