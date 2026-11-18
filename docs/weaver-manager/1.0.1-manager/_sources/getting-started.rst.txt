Getting Started
===============

Install Weaver Manager
----------------------

Download the Manager package for your operating system from the project's published releases and install it using the standard procedure for that platform.

When the Manager starts, it creates its data, installation, and log directories. The main window has two tabs: **Available Releases** and **Installed Versions**.

Find a release
--------------

Open **Available Releases**. The Manager retrieves GitHub Releases and displays only entries that have a valid signed manifest, a matching artifact, and a package for the current platform. Use **Refresh GitHub Releases** to query again. Select a column heading to sort the list.

The **Release status** column identifies **Latest**, **Stable**, or **Pre-Release**. **Cache status** may display **INSTALLED** or **NEW**. A **NEW** marker is informational and does not indicate that the release is recommended or more secure.

Install a version
-----------------

1. Locate the required Weaver version and edition.
2. Select **Install** and confirm the prompt.
3. Wait while the Manager downloads the installer and verifies its size and SHA-256 against the signed manifest.
4. Complete any prompts from the operating-system installer, including elevation or destination choices.
5. Return to **Installed Versions** and select **Refresh Installed Versions** if the new version is not yet listed.

The Manager launches the system installer; it does not install silently or update an existing version in place. Versions can be installed side by side. Supported packages are MSI on Windows, DEB or RPM on Linux, and PKG or DMG on macOS, if a matching release asset is published.

Local test installers
---------------------

**Add Test Installer** is available only in a Manager built with ``ENABLE_TEST=ON``. Select a local installer using **Browse…**, enter or paste its path, or drag and drop one file. The file must have a supported extension and a filename matching the current platform, such as ``Weaver-Desktop-26.11.06-Windows-x64.msi``.

Local installers are not verified with the signed GitHub manifest. The Manager displays a warning before launching one. Use only a package obtained from a trusted source. Adding it to the Test list does not publish it or make it available to other users.
