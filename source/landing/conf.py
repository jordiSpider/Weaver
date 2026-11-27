import json
import os

project = "Weaver Project"
author = "Weaver Project"
copyright = "2025, Jordi Moya Laraño"
html_title = "Weaver Project"

extensions = ["sphinx.ext.autosectionlabel", "sphinx_design"]
autosectionlabel_prefix_document = True

html_theme = "pydata_sphinx_theme"
html_sidebars = {"**": []}
html_theme_options = {
	"logo": {
		"image_light": "_static/branding/weaver-logo-horizontal.svg",
		"image_dark": "_static/branding/weaver-logo-horizontal-dark.svg",
	},
}
html_favicon = "_static/branding/favicon/favicon.ico"
html_static_path = ["_static"]
html_css_files = ["landing.css"]
html_extra_path = ["licenses"]
templates_path = []

exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

def latest_version(catalog_variable):
	catalog_path = os.environ[catalog_variable]
	with open(catalog_path, encoding="utf-8") as catalog_file:
		return json.load(catalog_file)["latest"]

weaver_latest = latest_version("WEAVER_DOCS_CATALOG")
manager_latest = latest_version("WEAVER_MANAGER_DOCS_CATALOG")
rst_epilog = f"""
.. _Open Weaver documentation: weaver/{weaver_latest}/index.html
.. _Open Weaver Manager documentation: weaver-manager/{manager_latest}/index.html
"""
