import os
import json

project = "Weaver Manager"
author = "Weaver project"
html_title = "Weaver Manager Documentation"
html_logo = "../../weaver/latest/_static/images/logo.png"

default_version = os.environ.get("WEAVER_MANAGER_DOCS_DEFAULT_VERSION", "latest")
default_language = os.environ.get("WEAVER_MANAGER_DOCS_DEFAULT_LANGUAGE", "en")
catalog_path = os.environ.get("WEAVER_MANAGER_DOCS_CATALOG")
if catalog_path and os.path.isfile(catalog_path):
	with open(catalog_path, encoding="utf-8") as catalog_file:
		version_catalog = json.load(catalog_file)
else:
	version_catalog = {"latest": default_version, "versions": [{"version": default_version, "channel": "stable", "latest": True}]}
default_version = version_catalog.get("latest") or default_version
version_entries = version_catalog.get("versions", [])
all_versions = [entry["version"] for entry in version_entries]
version_labels = {entry["version"]: entry.get("channel", "stable") for entry in version_entries}
all_languages = os.environ.get("WEAVER_MANAGER_DOCS_LANGUAGES", default_language).split(";")
current_version = os.environ.get("WEAVER_MANAGER_DOCS_VERSION", default_version)
current_language = os.environ.get("WEAVER_MANAGER_DOCS_LANGUAGE", default_language)
language = current_language

extensions = ["sphinx.ext.autosectionlabel"]
autosectionlabel_prefix_document = True

html_theme = "pydata_sphinx_theme"
templates_path = ["../../_shared/_templates"]
html_theme_options = {
	"navbar_end": ["version-switcher", "language-switcher", "theme-switcher"],
}
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

site_root = os.environ.get("WEAVER_MANAGER_DOCS_SITE_ROOT")
if site_root:
	language_dir = "" if current_language == default_language else current_language
	current_page = os.environ.get("WEAVER_MANAGER_DOCS_CURRENT_PAGE", "index.html")
	current_page_dir = os.path.dirname(current_page)
	current_output_dir = os.path.join(site_root, current_version, language_dir, current_page_dir)
	current_page_root = os.path.relpath(site_root, current_output_dir).replace("\\", "/")
else:
	current_page_root = ""
if current_page_root == ".":
	current_page_root = ""

urls = {}
for version in all_versions:
	urls[version] = {}
	for language_code in all_languages:
		destination_dir = os.path.join(site_root, version) if site_root else version
		if language_code != default_language:
			destination_dir = os.path.join(destination_dir, language_code)
		destination_index = os.path.join(destination_dir, "index.html")
		if site_root:
			urls[version][language_code] = os.path.relpath(destination_index, current_output_dir).replace("\\", "/")
		else:
			urls[version][language_code] = destination_index.replace("\\", "/")

html_context = {
	"default_version": default_version,
	"current_version": current_version,
	"all_versions": all_versions,
	"version_labels": version_labels,
	"default_language": default_language,
	"current_language": current_language,
	"all_languages": all_languages,
	"current_page_root": current_page_root,
	"urls": urls,
}
