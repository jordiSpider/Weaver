import argparse
import json
import os
import re
import tempfile
import urllib.error
import urllib.parse
import urllib.request


REQUIRED_WEAVER_ASSETS = {
	"weaver-manager-manifest.json",
	"weaver-manager-manifest.json.sig",
}


def parse_args():
	parser = argparse.ArgumentParser()
	parser.add_argument("--repository", required=True)
	parser.add_argument("--api-url", default="https://api.github.com")
	parser.add_argument("--token", default=os.environ.get("GITHUB_TOKEN", ""))
	parser.add_argument("--product", choices=("weaver", "weaver-manager"), required=True)
	parser.add_argument("--source-root", required=True)
	parser.add_argument("--catalog", required=True)
	return parser.parse_args()


def request_json(url, token):
	headers = {
		"Accept": "application/vnd.github+json",
		"X-GitHub-Api-Version": "2022-11-28",
		"User-Agent": "Weaver-GitHub-Pages-Docs",
	}
	if token:
		headers["Authorization"] = f"Bearer {token}"

	request = urllib.request.Request(url, headers=headers)
	try:
		with urllib.request.urlopen(request, timeout=30) as response:
			return json.load(response)
	except urllib.error.HTTPError as error:
		raise RuntimeError(f"GitHub API request failed ({error.code}) for {url}: {error.read().decode('utf-8', errors='replace')}") from error
	except urllib.error.URLError as error:
		raise RuntimeError(f"Could not reach GitHub API at {url}: {error.reason}") from error


def fetch_releases(repository, api_url, token):
	repository = urllib.parse.quote(repository, safe="/")
	releases = []
	page_number = 1
	url = f"{api_url.rstrip('/')}/repos/{repository}/releases?per_page=100&page={page_number}"
	while url:
		page = request_json(url, token)
		if not isinstance(page, list):
			raise RuntimeError("GitHub Releases API returned an unexpected response")
		releases.extend(page)
		page_number += 1
		url = f"{api_url.rstrip('/')}/repos/{repository}/releases?per_page=100&page={page_number}" if len(page) == 100 else None
	return releases


def fetch_github_latest(repository, api_url, token):
	repository = urllib.parse.quote(repository, safe="/")
	url = f"{api_url.rstrip('/')}/repos/{repository}/releases/latest"
	try:
		return request_json(url, token)
	except RuntimeError as error:
		if "(404)" in str(error):
			return None
		raise


def normalize_version(tag_name):
	version = tag_name[1:] if tag_name.startswith("v") else tag_name
	if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._+-]*", version):
		return None
	return version


def has_local_source(source_root, version):
	return os.path.isdir(os.path.join(source_root, version))


def release_channel(release):
	return "pre-release" if release.get("prerelease", False) else "stable"


def version_sort_key(tag_name):
	components = re.findall(r"\d+", tag_name)
	return tuple(int(component) for component in components) if components else ()


def build_catalog(product, source_root, releases, github_latest=None, excluded_versions=None):
	excluded_versions = excluded_versions or set()
	version_releases = {}
	for release in releases:
		if release.get("draft", False):
			continue
		version = normalize_version(release.get("tag_name", ""))
		if not version or version in excluded_versions or not has_local_source(source_root, version):
			continue
		if product == "weaver":
			assets = {asset.get("name") for asset in release.get("assets", [])}
			if not REQUIRED_WEAVER_ASSETS.issubset(assets):
				continue
		version_releases[version] = release

	versions = []
	for version, release in version_releases.items():
		versions.append({
			"version": version,
			"tag": release.get("tag_name", version),
			"channel": release_channel(release),
			"latest": False,
			"published_at": release.get("published_at"),
			"html_url": release.get("html_url"),
		})

	if not versions:
		raise RuntimeError(f"No eligible releases with local source found for {product}")

	latest_tag = github_latest.get("tag_name") if github_latest else None
	latest_version = normalize_version(latest_tag or "")
	eligible_versions = {entry["version"] for entry in versions}
	if latest_version not in eligible_versions:
		stable_versions = [entry for entry in versions if entry["channel"] == "stable"]
		if not stable_versions:
			raise RuntimeError(f"No eligible stable releases with local source found for {product}")
		latest_version = max(
			stable_versions,
			key=lambda entry: (version_sort_key(entry["tag"]), entry["published_at"] or "", entry["version"]),
		)["version"]

	for entry in versions:
		entry["latest"] = entry["version"] == latest_version
	versions.sort(key=lambda entry: version_sort_key(entry["tag"]), reverse=True)
	return {"schema_version": 1, "product": product, "latest": latest_version, "versions": versions}


def atomic_write_json(path, data):
	directory = os.path.dirname(os.path.abspath(path))
	os.makedirs(directory, exist_ok=True)
	descriptor, temporary_path = tempfile.mkstemp(dir=directory, prefix="catalog-", suffix=".json")
	try:
		with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as output:
			json.dump(data, output, indent=2, ensure_ascii=False)
			output.write("\n")
		os.replace(temporary_path, path)
	finally:
		if os.path.exists(temporary_path):
			os.remove(temporary_path)


def main():
	args = parse_args()
	if not os.path.isdir(args.source_root):
		raise ValueError(f"Documentation source directory not found: {args.source_root}")
	releases = fetch_releases(args.repository, args.api_url, args.token)
	github_latest = fetch_github_latest(args.repository, args.api_url, args.token)
	catalog = build_catalog(args.product, args.source_root, releases, github_latest)
	atomic_write_json(args.catalog, catalog)
	print(f"Updated {args.product} documentation catalog: {catalog['latest']} latest, {len(catalog['versions'])} version(s)")


if __name__ == "__main__":
	main()
