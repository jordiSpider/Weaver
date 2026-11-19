# ====================================================================================================
# Sphinx configuration file
# ----------------------------------------------------------------------------------------------------
# This file defines all parameters used by Sphinx to build the documentation.
#
# It is automatically configured by CMake at build time. Any variable enclosed in  is replaced
# by its corresponding CMake value before the documentation build begins.
#
# References:
#   - Sphinx documentation: https://www.sphinx-doc.org/en/master/usage/configuration.html
# ====================================================================================================

import os
import json

# ----------------------------------------------------------------------------------------------------
# Project Information
# ----------------------------------------------------------------------------------------------------
# Basic metadata that appears throughout the documentation and is used by Sphinx internally.
# ----------------------------------------------------------------------------------------------------
project = "Weaver"
copyright = ", "
author = ""
repo_url = ""

# ----------------------------------------------------------------------------------------------------
# Version and Language Management
# ----------------------------------------------------------------------------------------------------
# The documentation system supports multiple versions and multiple languages.
# These values can be overridden dynamically by the documentation build environment.
#
# CMake sets WEAVER_DOCS_VERSION and WEAVER_DOCS_LANGUAGE for each version/language build.
# ----------------------------------------------------------------------------------------------------
default_version = "26.10.03"
default_language = "en"

catalog_path = os.environ.get("WEAVER_DOCS_CATALOG")
if catalog_path and os.path.isfile(catalog_path):
    with open(catalog_path, encoding="utf-8") as catalog_file:
        version_catalog = json.load(catalog_file)
else:
    version_catalog = {"latest": default_version, "versions": [{"version": default_version, "channel": "stable", "latest": True}]}

default_version = version_catalog.get("latest") or default_version
version_entries = version_catalog.get("versions", [])
all_versions = [entry["version"] for entry in version_entries]
version_labels = {entry["version"]: entry.get("channel", "stable") for entry in version_entries}
current_version = os.environ.get("WEAVER_DOCS_VERSION", default_version)
current_language = os.environ.get("WEAVER_DOCS_LANGUAGE", default_language)

# ----------------------------------------------------------------------------------------------------
# Versions and Languages Lists
# ----------------------------------------------------------------------------------------------------
# The following variables are expanded by CMake with semicolon-separated values.
# We convert them into Python lists so they can be used to dynamically generate
# version/language selectors in the HTML context.
# ----------------------------------------------------------------------------------------------------
all_languages_raw = "en"
all_languages = all_languages_raw.split(";") if all_languages_raw else []

# ----------------------------------------------------------------------------------------------------
# General Configuration
# ----------------------------------------------------------------------------------------------------
# These parameters control Sphinx’s general behavior and file search paths.
# ----------------------------------------------------------------------------------------------------
exclude_patterns = []            # Files and folders to ignore during the build

# ----------------------------------------------------------------------------------------------------
# Localization (i18n) Configuration
# ----------------------------------------------------------------------------------------------------
# This section defines how translations are handled and where gettext catalogs are located.
# ----------------------------------------------------------------------------------------------------
locale_dirs = ["locale/"]  # Directory containing translation message catalogs (.po files)

# If False, each document gets its own gettext domain (recommended for multi-language setups).
# This allows incremental updates to translations per document.
gettext_compact = False

# ----------------------------------------------------------------------------------------------------
# HTML Output Configuration
# ----------------------------------------------------------------------------------------------------
# Defines the HTML theme, logo, and additional static resources.
# ----------------------------------------------------------------------------------------------------
html_logo = "_static/images/logo.png"

html_theme = "pydata_sphinx_theme"
templates_path = ["_templates"]
html_theme_options = {
    "navbar_end": ["version-switcher", "language-switcher", "theme-switcher"],
}

# Additional static files (CSS, JS, images)
# These files override Sphinx defaults if names match.
html_static_path = ["_static"]

site_root = os.environ.get("WEAVER_DOCS_SITE_ROOT")
if site_root:
    language_dir = "" if current_language == default_language else current_language
    current_page = os.environ.get("WEAVER_DOCS_CURRENT_PAGE", "index.html")
    current_page_dir = os.path.dirname(current_page)
    current_output_dir = os.path.join(site_root, current_version, language_dir, current_page_dir)
    current_page_root = os.path.relpath(site_root, current_output_dir).replace("\\", "/")
else:
    current_page_root = ""
if current_page_root == ".":
    current_page_root = ""

# ----------------------------------------------------------------------------------------------------
# Dynamic HTML Context
# ----------------------------------------------------------------------------------------------------
# The following dictionary is injected into every template.
# It enables dynamic navigation between versions and languages.
# ----------------------------------------------------------------------------------------------------
html_context = {
    "default_version": default_version,
    "current_version" : current_version,
    "all_versions" : all_versions,
    "version_labels": version_labels,
    "default_language": default_language,
    "current_language" : current_language,
    "all_languages" : all_languages,
    "current_page_root": current_page_root,
    "urls": {}
}

# Build the URL map for version/language switching in the sidebar
for version in all_versions:
    html_context["urls"][version] = {}
    for language in all_languages:
        destination_dir = os.path.join(site_root, version) if site_root else version
        if(language != default_language):
            destination_dir = os.path.join(destination_dir, language)
        destination_index = os.path.join(destination_dir, "index.html")
        if site_root:
            html_context["urls"][version][language] = os.path.relpath(destination_index, current_output_dir).replace("\\", "/")
        else:
            html_context["urls"][version][language] = destination_index.replace("\\", "/")

# ----------------------------------------------------------------------------------------------------
# Global Substitutions
# ----------------------------------------------------------------------------------------------------
# The rst_epilog variable defines global text replacements (reusable links, common labels, etc.)
# available in all reStructuredText (.rst) files.
# ----------------------------------------------------------------------------------------------------
rst_epilog = f"""
.. _Project Repository: {repo_url}
.. _Report Issues: {repo_url}/issues
.. _Releases: {repo_url}/releases
.. |repo_git_url| replace:: {repo_url}.git
"""

# ----------------------------------------------------------------------------------------------------
# Extensions Configuration
# ----------------------------------------------------------------------------------------------------
# List of active Sphinx extensions. Custom extensions can also be loaded here.
# ----------------------------------------------------------------------------------------------------
extensions = [
    "sphinx.ext.autosectionlabel",
    "breathe",
]

doxygen_xml_dir = os.path.join(os.path.abspath(r"C:/Devs/Github/Weaver/docs/source/weaver/latest/api"), "xml").replace("\\", "/")

breathe_projects = {
    "Weaver": doxygen_xml_dir,
}
breathe_default_project = "Weaver"

# ----------------------------------------------------------------------------------------------------
# Autosectionlabel Configuration
# ----------------------------------------------------------------------------------------------------
# Ensures that section labels are unique across documents.
# This is essential for large projects with multiple files referencing each other.
# ----------------------------------------------------------------------------------------------------
autosectionlabel_prefix_document = True
