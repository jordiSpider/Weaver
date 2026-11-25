import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("--product", choices=("weaver", "weaver-manager"), required=True)
	parser.add_argument("--source", type=Path, required=True)
	parser.add_argument("--output-root", type=Path, required=True)
	parser.add_argument("--work-root", type=Path, required=True)
	parser.add_argument("--default-language", default="en")
	parser.add_argument("--jobs", default="1")
	args = parser.parse_args()

	source_dir = args.source.resolve()
	if not source_dir.is_dir():
		raise FileNotFoundError(f"Local documentation source not found: {source_dir}")

	args.work_root.mkdir(parents=True, exist_ok=True)
	with tempfile.TemporaryDirectory(prefix=f"{args.product}-local-", dir=args.work_root) as temporary:
		temp_root = Path(temporary)
		version_source = temp_root / "source" / args.product / "latest"
		version_source.parent.mkdir(parents=True, exist_ok=True)
		shutil.copytree(source_dir, version_source)

		catalog_path = temp_root / "versions.json"
		catalog_path.write_text(
			json.dumps({
				"schema_version": 1,
				"product": args.product,
				"latest": "latest",
				"versions": [{
					"version": "latest",
					"tag": "latest",
					"channel": "stable",
					"latest": True,
				}],
			}, indent=2) + "\n",
			encoding="utf-8",
		)

		environment = os.environ.copy()
		environment.update({
			"DOCS_SOURCE_ROOT": str(temp_root / "source"),
			"DOCS_OUTPUT_ROOT": str(args.output_root.resolve()),
			"DOCS_WORK_ROOT": str(temp_root / "work"),
			"DOCS_PRODUCT": args.product,
			"DOCS_VERSION": "latest",
			"DOCS_DEFAULT_LANGUAGE": args.default_language,
			"DOCS_CATALOG": str(catalog_path),
			"DOCS_WEAVER_CATALOG": str(catalog_path),
			"DOCS_SPHINX_JOBS": args.jobs,
			"WEAVER_MANAGER_DOCS_MANIFEST_REF": "main",
		})
		builder = Path(__file__).with_name("BuildVersionedDocs.py")
		subprocess.run([sys.executable, str(builder)], check=True, env=environment)

	print(f"Generated local {args.product} documentation at {args.output_root / 'latest'}")


if __name__ == "__main__":
	main()
