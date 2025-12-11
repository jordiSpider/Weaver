# ====================================================================================================
# Sphinx configuration file
# ----------------------------------------------------------------------------------------------------
# This file defines all parameters used by Sphinx to build the documentation.
#
# It is automatically configured by CMake at build time. Any variable enclosed in @...@ is replaced
# by its corresponding CMake value before the documentation build begins.
#
# References:
#   - Sphinx documentation: https://www.sphinx-doc.org/en/master/usage/configuration.html
# ====================================================================================================

from sys import argv

# ----------------------------------------------------------------------------------------------------
# Project Information
# ----------------------------------------------------------------------------------------------------
# Basic metadata that appears throughout the documentation and is used by Sphinx internally.
# ----------------------------------------------------------------------------------------------------
project = "@CMAKE_PROJECT_NAME@"
copyright = "@COPYRIGHT_YEAR@, @AUTHOR@"
author = "@AUTHOR@"
repo_url = "@REPO_URL@"

# ----------------------------------------------------------------------------------------------------
# Version and Language Management
# ----------------------------------------------------------------------------------------------------
# The documentation system supports multiple versions and multiple languages.
# These values can be overridden dynamically when invoking Sphinx using the -D option, e.g.:
#
#   sphinx-build -D current_version=1.0.0 -D current_language=es ...
#
# This mechanism is used by CMake and the GitHub Actions workflows to generate documentation for
# all available versions and locales automatically.
# ----------------------------------------------------------------------------------------------------
default_version = "@LATEST_VERSION@"
default_language = "@DEFAULT_LANGUAGE@"

current_version = default_version
current_language = default_language

# Parse runtime arguments passed via sphinx-build -D
for i, arg in enumerate(argv):
    if arg == "-D" and i + 1 < len(argv):
        if argv[i + 1].startswith("current_version="):
            current_version = argv[i + 1].split("=")[1]
        if argv[i + 1].startswith("current_language="):
            current_language = argv[i + 1].split("=")[1]

# ----------------------------------------------------------------------------------------------------
# Versions and Languages Lists
# ----------------------------------------------------------------------------------------------------
# The following variables are expanded by CMake with semicolon-separated values.
# We convert them into Python lists so they can be used to dynamically generate
# version/language selectors in the HTML context.
# ----------------------------------------------------------------------------------------------------
all_languages_raw = "@ALL_LANGUAGES@"
all_languages = all_languages_raw.split(";") if all_languages_raw else []

all_versions_raw= "@ALL_VERSIONS@"
all_versions = all_versions_raw.split(";") if all_versions_raw else []

# ----------------------------------------------------------------------------------------------------
# General Configuration
# ----------------------------------------------------------------------------------------------------
# These parameters control Sphinx’s general behavior and file search paths.
# ----------------------------------------------------------------------------------------------------
templates_path = ["_templates"]  # Custom Jinja2 templates
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

html_theme = "sphinx_rtd_theme"

html_theme_options = {
    # ==========================
    # Navigation configuration
    # ==========================
    "collapse_navigation": "false",       # Keep navigation expanded
    "sticky_navigation": "true",          # Navigation scrolls with page content
    "navigation_depth": -1,               # Unlimited ToC depth
    "includehidden": "false",             # Do not include hidden toctrees
    "titles_only": "false",               # Show subheadings in sidebar

    # ==========================
    # Miscellaneous
    # ==========================
    "analytics_id": "",                   # Optional Google Analytics ID
    "analytics_anonymize_ip": "false",    # IP anonymization
    "logo_only": "true",                  # Display only logo, hide project name
    "prev_next_buttons_location": "bottom",
    "style_external_links": "false",
    "vcs_pageview_mode": "view",          # Mode for GitHub/GitLab view links
    "style_nav_header_background": "#2980B9"  # Navbar color
}

# Additional static files (CSS, JS, images)
# These files override Sphinx defaults if names match.
html_static_path = ["_static"]

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
    "default_language": default_language,
    "current_language" : current_language,
    "all_languages" : all_languages,
    "urls": {}
}

# Build the URL map for version/language switching in the sidebar
for version in all_versions:
    html_context["urls"][version] = {}
    for language in all_languages:
        new_url = ""
        if(version != default_version):
            new_url += version + "/"
        if(language != default_language):
            new_url += language + "/"
        new_url += "index.html"
        html_context["urls"][version][language] = new_url

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
    "sphinx.ext.autosectionlabel"
]

# ----------------------------------------------------------------------------------------------------
# Autosectionlabel Configuration
# ----------------------------------------------------------------------------------------------------
# Ensures that section labels are unique across documents.
# This is essential for large projects with multiple files referencing each other.
# ----------------------------------------------------------------------------------------------------
autosectionlabel_prefix_document = True
