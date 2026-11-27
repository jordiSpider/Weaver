# GitHub Pages documentation generator

This folder contains the CMake presets and scripts used to generate and maintain the documentation site on the `gh-pages` branch. Copy its contents to the root of that branch. Versioned sources live under `source/`; generated HTML and release catalogs are written under `docs/`.

## Requirements

- CMake 3.25 or newer, with the Ninja build tool available.
- Python 3.10, matching the GitHub Actions environment.
- The pinned Python documentation dependencies from `requirements-docs.txt` (Sphinx, `pydata_sphinx_theme`, `sphinx-design`, Breathe, and `sphinx-copybutton`).
- Internet access to the GitHub Releases API for catalog updates; `GITHUB_TOKEN` may be set to authenticate or raise API rate limits.

Install the same dependencies locally with `python -m pip install -r requirements-docs.txt`. Weaver's Doxygen XML is included in its versioned source, so CI does not need to install or run Doxygen.

The GitHub Actions workflow uses the hosted Ubuntu runner's CMake and Ninja installations, installs Python 3.10, and caches pip dependencies.

## Source layout

```text
source/landing/
source/weaver/<version>/
source/weaver/<version>/_templates/
source/weaver-manager/<version>/
source/weaver-manager/<version>/_templates/
```

The version directory must match its release tag without a leading `v` (for example, tag `v26.10.04-rc.1` uses `source/weaver/26.10.04-rc.1/`).
Each version keeps its own templates and static assets, including its logo, so editing one version does not affect the others. The landing page uses its own branding in `source/landing/_static/branding/` and licenses in `source/landing/licenses/`, independently of product versions.

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

Removal is rejected if it would leave the product without an eligible documentation version. See the [documentation site maintenance guide](source/landing/documentation-site-maintenance.rst) for catalog selection, channels, labels, CI behavior, and exact target behavior.

## GitHub Actions on `gh-pages`

The `Update documentation` workflow in `.github/workflows/update-docs.yml` runs automatically for relevant pushes to `gh-pages`. It detects changed paths and runs only the affected CMake workflow preset and version:

- `source/landing/**` refreshes the landing page and release catalogs.
- `source/weaver/<version>/**` generates that Weaver version.
- `source/weaver-manager/<version>/**` generates that Manager version.
- Changes to shared build configuration or the versioned-documentation generator rebuild all available versions; changes to landing/catalog helpers refresh the landing page.

Product generation also refreshes the landing page as part of the existing CMake targets. Changes limited to generated `docs/**` files do not trigger another run. A successful generation is the validation step; only then does Actions commit the updated source/output and catalogs to `gh-pages`.

Run the workflow manually from **Actions → Update documentation → Run workflow**, selecting the `gh-pages` ref. Choose one of these scopes:

- `auto`: compares `gh-pages` with `.github/docs-generated-from` and generates only for changes not yet processed. Landing changes run `generate-landing`; a changed Weaver or Manager version runs only its matching product preset. Shared build/generator changes can require rebuilding all local versions. If no marker exists yet, the current tree is treated as the initial state and all available versions may be generated.
- `landing`: updates both release catalogs, both products' version selectors, and the landing page. It does not rebuild product-version documentation.
- `weaver` or `weaver-manager`: requires `version` to be the exact source directory name (without a leading `v`). It generates that version, updates its selectors, and regenerates the landing page; it does not rebuild versions of the other product.
- `remove-weaver` or `remove-manager`: requires `version` and `confirm_removal=true`. It removes that version's source and generated HTML, updates its catalog and selectors, and regenerates the landing page. It does not delete the GitHub release.

Run a removal scope **before** deleting the source version directory; the removal action needs that directory to calculate a safe replacement catalog. If the directory has already been deleted, restore it before running the removal action. Pushes that delete a whole version directory are rejected and do not remove published files automatically.

GitHub requires the workflow file to exist on the repository's default branch (`main`) for manual runs and `repository_dispatch`. Keep the workflow YAML in sync on `main` and `gh-pages`; the job still checks out and publishes to `gh-pages`.

If another workflow pushes documentation sources using the default `GITHUB_TOKEN`, that push will not start this workflow automatically. After pushing the sources, that workflow must dispatch the `update-docs` event (and have permission to dispatch it). Pushes made with a PAT or GitHub App token can trigger the normal `push` event instead.
