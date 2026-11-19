# GitHub Pages documentation generator

This folder contains the CMake presets and scripts used to generate and maintain the documentation site on the `gh-pages` branch. Copy its contents to the root of that branch. Versioned sources live under `source/`; generated HTML and release catalogs are written under `docs/`.

## Requirements

- CMake 3.25 or newer, with the Ninja build tool available.
- Python 3 available to CMake. No exact Python version is pinned by this project.
- Sphinx and `pydata_sphinx_theme` installed in that Python environment.
- Breathe installed for Weaver documentation, which includes the Doxygen API reference.
- Internet access to the GitHub Releases API for catalog updates; `GITHUB_TOKEN` may be set to authenticate or raise API rate limits.

Python package versions are not pinned in this folder. Use a consistent Python environment for CI and local runs.

## Source layout

```text
source/landing/
source/weaver/<version>/
source/weaver/<version>/_templates/
source/weaver-manager/<version>/
source/weaver-manager/<version>/_templates/
```

The version directory must match its release tag without a leading `v` (for example, tag `v26.10.04-rc.1` uses `source/weaver/26.10.04-rc.1/`).
Each version keeps its own templates and static assets, including its logo, so editing one version does not affect the others.

## Common workflows (PowerShell)

Generate a selected version; this updates release catalogs, builds only that version, refreshes the product's version selectors, and regenerates the landing page:

```powershell
$env:DOCS_WEAVER_VERSION = "26.10.04-rc.1"
cmake --workflow --preset generate-weaver

$env:DOCS_MANAGER_VERSION = "1.0.0-manager"
cmake --workflow --preset generate-manager
```

Refresh both catalogs, both products' existing version selectors, and the landing page without rebuilding product documentation:

```powershell
cmake --workflow --preset generate-landing
```

Remove one source/output version and refresh its catalog, product selectors, and landing page:

```powershell
$env:DOCS_WEAVER_VERSION = "26.10.04-rc.1"
cmake --workflow --preset remove-weaver-version

$env:DOCS_MANAGER_VERSION = "1.0.0-manager"
cmake --workflow --preset remove-manager-version
```

Removal is rejected if it would leave the product without an eligible documentation version. See the [detailed CMake and release workflow guide](source/landing/cmake-guide.rst) for catalog selection, channels, labels, and exact target behavior.
