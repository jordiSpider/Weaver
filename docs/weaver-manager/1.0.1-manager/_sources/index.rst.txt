===============================
Weaver Manager — User Guide
===============================

**Weaver Manager** is a desktop application for discovering verified Weaver releases, installing multiple versions side by side, and managing supported local installations. It is built independently from the Weaver simulator.

This guide focuses on day-to-day use and release operations. It does not document the Manager's source-code API.

.. toctree::
   :maxdepth: 2
   :caption: User guide

   getting-started
   managing-versions
   security-and-releases
   troubleshooting

.. toctree::
   :maxdepth: 1
   :caption: Maintainers

   publishing-releases

Overview
========

Use **Available Releases** to browse the verified catalog and start an installer. Use **Installed Versions** to open and maintain installations managed by Weaver Manager. The Manager does not silently install updates; the selected platform installer runs after the download has passed verification.

Platform support
================

The catalog accepts MSI packages on Windows, DEB or RPM packages on Linux, and PKG or DMG packages on macOS, when published for the current operating system and architecture. **Open Weaver CLI** is available on all three platforms when a terminal is present. Modify and Uninstall are currently Windows-only.

Build information
=================

For CMake presets, packaging, and signing-key configuration, see ``manager/README.md`` in the source repository. The README also documents the current release-pipeline integration gap.
