===============================
Documentation Site Maintenance
===============================

This guide explains how to build and maintain the Weaver and Weaver Manager documentation site on the ``gh-pages`` branch. It covers the CMake workflows, release catalogs, version selection, and maintenance operations, complementing the setup instructions in the repository README.

Requirements and Python environment
===================================

The project requires CMake 3.25 or newer, Ninja, and Python 3.10. Install the pinned documentation dependencies from ``requirements-docs.txt`` into the same Python environment that CMake selects:

* ``sphinx==8.1.3``
* ``pydata-sphinx-theme==0.19.0``
* ``sphinx-design==0.6.1``
* ``breathe==4.36.0``
* ``sphinx-copybutton==0.5.2``

Install the same environment locally with ``python -m pip install -r requirements-docs.txt``. The GitHub Actions workflow uses Python 3.10 and caches these pip dependencies. The hosted Ubuntu runner provides CMake and Ninja. Breathe renders Weaver's Doxygen API, and its generated XML is already included under ``source/weaver/<version>/api/xml/``; CI does not need to install or run Doxygen. ``sphinx-copybutton`` is used by Weaver Manager.

The catalog updater calls the GitHub Releases REST API. Network access is required when a workflow updates catalogs. ``GITHUB_TOKEN`` is passed through automatically; it authenticates the API requests and avoids unauthenticated rate limits. The publishing workflow also needs permission to write repository contents, and repository settings must allow GitHub Actions to push to ``gh-pages``.

Source and output layout
========================

The source root is ``source/``. It contains:

.. code-block:: text

   source/landing/
   source/weaver/<version>/
   source/weaver-manager/<version>/
   source/weaver/<version>/_templates/
   source/weaver-manager/<version>/_templates/

Version directory names are normalized release tags: remove the leading ``v`` but retain the rest of the tag. For example, ``v26.10.04-rc.1`` maps to ``source/weaver/26.10.04-rc.1/``. Manager tags can have their own suffix, such as ``v1.0.0-manager`` mapping to ``source/weaver-manager/1.0.0-manager/``.

Each version directory owns its templates and static assets, including its logo. Changes to one version therefore do not alter another version's files.

Generated files are stored under ``docs/``:

.. code-block:: text

   docs/index.html
   docs/weaver/versions.json
   docs/weaver/<version>/
   docs/weaver-manager/versions.json
   docs/weaver-manager/<version>/

The catalogs are JSON files generated from eligible GitHub releases and local version source directories.

CMake presets and commands
==========================

Run commands from the ``gh-pages`` directory. Presets configure independent build directories and workflows combine one configure step with its corresponding build target.

Generate one selected Weaver version (PowerShell):

.. code-block:: powershell

   $env:DOCS_WEAVER_VERSION = "26.10.04-rc.1"
   cmake --workflow --preset generate-weaver

Generate one selected Weaver Manager version:

.. code-block:: powershell

   $env:DOCS_MANAGER_VERSION = "1.0.0-manager"
   cmake --workflow --preset generate-manager

Refresh both release catalogs, update the version selectors in existing product HTML, and regenerate the landing page without rebuilding product documentation:

.. code-block:: console

   cmake --workflow --preset generate-landing

Remove a documentation version:

.. code-block:: powershell

   $env:DOCS_WEAVER_VERSION = "26.10.04-rc.1"
   cmake --workflow --preset remove-weaver-version

   $env:DOCS_MANAGER_VERSION = "1.0.0-manager"
   cmake --workflow --preset remove-manager-version

The available build targets are ``generate-weaver``, ``generate-manager``, ``generate-landing``, ``remove-weaver-version``, and ``remove-manager-version``. Their workflow presets have the same names. The configure presets are ``docs-weaver``, ``docs-manager``, ``docs-landing``, ``remove-weaver``, and ``remove-manager``.

GitHub Actions on ``gh-pages``
==============================

The ``Update documentation`` workflow in ``.github/workflows/update-docs.yml`` handles relevant pushes to ``gh-pages`` and can also be started manually. Its source paths are detected before Python documentation dependencies are installed, so irrelevant changes can end without provisioning the build environment.

For automatic runs, the workflow selects the smallest applicable preset:

* Changes under ``source/landing/`` run ``generate-landing``.
* Changes under ``source/weaver/<version>/`` run ``generate-weaver`` for that version.
* Changes under ``source/weaver-manager/<version>/`` run ``generate-manager`` for that version.
* Landing/catalog/selector helper changes refresh the landing page. Changes to shared CMake configuration, pinned requirements, or the versioned documentation builder rebuild all local versions because they affect every rendered version.
* Changes only under ``docs/`` do not trigger the workflow.

The selected Sphinx generation is also the validation: Actions does not run a second, duplicate documentation build. If generation succeeds, it commits updated sources (for a manual removal), generated HTML, and catalogs to ``gh-pages``. Runs are serialized, and the last processed source revision is recorded so a later run can include changes from pushes queued while another build was running.

Manual workflow scopes
----------------------

Open **Actions → Update documentation → Run workflow**, select the ``gh-pages`` ref, and choose a scope. The ``version`` input must be the exact local source directory name, such as ``26.11.18`` or ``1.1.0-manager`` (do not include a leading ``v``).

* ``auto`` compares the current ``gh-pages`` revision with ``.github/docs-generated-from`` and processes only changes since the last successful generation. Landing-only changes run ``generate-landing``; changes under a product version run the matching product preset for that version. Shared CMake configuration, pinned requirements, or the versioned documentation builder can affect every output, so those changes rebuild all local versions. If there is no recorded revision yet, the current tree is treated as the initial state and all available versions may be generated. If there are no pending changes, the workflow exits without installing Sphinx dependencies or publishing.
* ``landing`` runs ``generate-landing``. It refreshes both release catalogs, both products' selectors, and the landing page, but does not rebuild their versioned documentation.
* ``weaver`` runs ``generate-weaver`` for the supplied version. It rebuilds that Weaver version, refreshes the Weaver selectors and regenerates the landing page; it does not rebuild Manager versions.
* ``weaver-manager`` runs ``generate-manager`` for the supplied version. It rebuilds that Manager version, refreshes the Manager selectors and regenerates the landing page; it does not rebuild Weaver versions.
* ``remove-weaver`` and ``remove-manager`` run the corresponding removal preset. Supply the version and set ``confirm_removal`` to ``true``. The action removes the version's source and published HTML, refreshes the product catalog/selectors and landing page, but does not delete the GitHub release. It refuses to remove the last eligible version.

Run a removal scope before deleting the source directory: the action requires that directory to calculate and validate the replacement catalog. If it has already been deleted, restore it first. Automatic pushes that remove a whole version directory are rejected; they never delete published files implicitly.

GitHub uses the repository's default branch (currently ``main``) to expose ``workflow_dispatch`` and ``repository_dispatch``. Keep the workflow YAML in sync on ``main`` and ``gh-pages``. When started manually, select ``gh-pages``; the job processes and publishes that branch. A ``repository_dispatch`` run also explicitly checks out ``gh-pages``.

If the release/source-ingestion workflow pushes sources using the default ``GITHUB_TOKEN``, its push will not start another workflow automatically. After the source push, it must send a ``repository_dispatch`` event with type ``update-docs`` and permission to dispatch repository events. For example, from a GitHub-hosted runner with the ``gh`` CLI and ``GITHUB_TOKEN`` available:

.. code-block:: console

	 GH_TOKEN="$GITHUB_TOKEN" gh api repos/jordiSpider/Weaver/dispatches -f event_type=update-docs

A source push made with a PAT or GitHub App token can instead trigger the normal ``push`` event. Automatic version deletion is deliberately unsupported; use the confirmed manual removal action so catalog eligibility is checked first.

Catalog eligibility and ``latest`` selection
============================================

The updater requests the repository's paginated releases list and also queries GitHub's ``/repos/<owner>/<repo>/releases/latest`` endpoint. Draft releases are ignored. A release is included only when its normalized version has a matching local source directory. Weaver releases must also contain both assets:

* ``weaver-manager-manifest.json``
* ``weaver-manager-manifest.json.sig``

The current Manager catalog does not apply the Weaver-specific asset requirement.

The ``latest`` marker in the generated documentation catalog is selected as follows:

#. If GitHub's ``/releases/latest`` tag is eligible for this documentation catalog, that release is marked latest.
#. Otherwise, the updater chooses the eligible **stable** release with the greatest numeric sequence found in its tag. Components are compared from left to right, so a larger left component has greater priority than any later component (for example, ``1.10.0`` is newer than ``1.9.99``). Non-numeric suffix text does not take part in the comparison. If numeric sequences tie, the publication timestamp and then the normalized version string break the tie.
#. Pre-releases remain listed, but the numeric fallback never selects one as latest. If no eligible stable release remains, catalog generation fails.

GitHub's latest endpoint represents the repository's latest stable release, rather than a pre-release. The generated catalog stores this choice in its top-level ``latest`` property and marks the matching entry with ``"latest": true``.

GitHub's ``prerelease`` field determines whether a catalog entry is ``stable`` or ``pre-release``; the updater does not infer a channel from tag suffix text. Catalog entries are displayed in descending numeric tag order, independently of the latest marker and channel. Version labels show the version and either ``stable`` or ``pre-release``; the latest entry additionally shows ``latest``. The selector uses green for latest, blue for other stable entries, and orange for pre-release entries. The collapsed navbar selector shows the current version and its same status/color.

Generation and selector refresh behavior
========================================

``generate-weaver`` and ``generate-manager`` first update both product catalogs. They then render the selected version and its available languages into a temporary directory. If Sphinx succeeds, the selected version's HTML directory is replaced. A selector updater subsequently changes only the version-switcher markup in the existing HTML pages for that product. Other page content is not rebuilt. Finally, the landing page is regenerated so its links use the latest values from both catalogs.

``generate-landing`` updates both catalogs first, refreshes the selectors in both products' existing HTML trees, and then builds the landing page. It does not run Sphinx over Weaver or Manager version documentation. ``BuildLanding.py`` reads the catalog ``latest`` values and uses them to define the landing links to each product. It publishes only landing-owned files and preserves the product HTML trees.

Version removal
===============

A removal workflow queries the Releases API and computes the resulting catalog with the requested version excluded before it deletes anything. It fails if no eligible documentation version would remain, or if no eligible stable release would remain to identify ``latest``. In that case the error explains the reason and no source, HTML, or catalog file is removed.

When validation succeeds, the workflow removes ``source/<product>/<version>/`` and ``docs/<product>/<version>/``, writes the new catalog, updates only that product's selectors, and rebuilds the landing page. The GitHub release itself is not deleted or changed.

CMake settings
==============

The main configurable cache variables are:

* ``DOCS_SOURCE_ROOT`` — documentation sources; defaults to ``<sourceDir>/source``.
* ``DOCS_BUILD_ROOT`` — generated site; defaults to ``<sourceDir>/docs``.
* ``DOCS_WORK_ROOT`` — temporary work files; defaults to the CMake binary directory's ``work`` folder.
* ``DOCS_DEFAULT_LANGUAGE`` — defaults to ``en``.
* ``DOCS_GITHUB_REPOSITORY`` — defaults to ``jordiSpider/Weaver``.
* ``DOCS_GITHUB_API_URL`` — defaults to ``https://api.github.com``.
* ``DOCS_SPHINX_JOBS`` — Sphinx worker count; defaults to ``1``.
* ``DOCS_GENERATION`` — optional validation mode: empty, ``weaver``, ``manager``, ``landing``, ``remove-weaver``, or ``remove-manager``. The matching configure presets set it automatically.

Product generation and removal presets require the matching version environment variable during configure: ``DOCS_WEAVER_VERSION`` or ``DOCS_MANAGER_VERSION``. Version values must be one path component and must match the corresponding local source directory.
