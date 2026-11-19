===============================
CMake Documentation Workflows
===============================

This guide describes the CMake workflows used to maintain the Weaver and Weaver Manager documentation site. It complements the short setup instructions in the repository README.

Requirements and Python environment
===================================

The project requires CMake 3.25 or newer and uses the Ninja generator. CMake locates a Python interpreter, but this repository does not pin a Python minor version or provide a Python requirements/lock file. Install the documentation dependencies into the same Python environment that CMake selects:

* Sphinx
* ``pydata_sphinx_theme``
* Breathe, for Weaver's Doxygen API documentation

The landing and Weaver Manager configurations use ``sphinx.ext.autosectionlabel``, which is included with Sphinx. Weaver also configures Breathe and requires its generated Doxygen XML. The documentation sources do not declare explicit versions for these Python packages; CI should therefore provision and maintain a consistent environment.

The catalog updater calls the GitHub Releases REST API. Network access is required when a workflow updates catalogs. ``GITHUB_TOKEN`` is optional and is passed through automatically when set; using a token can avoid unauthenticated API rate limits.

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
