Managing Installed Versions
============================

View and refresh
----------------

The **Installed Versions** tab lists Weaver versions that the Manager can detect. It checks its managed program directory and ``/opt/Weaver/<version>/<edition>`` on Linux. On macOS, it checks ``/Applications/Weaver/<version>/<edition>``, ``~/Applications/Weaver/<version>/<edition>``, ``/usr/local/<version>/<edition>``, and ``/usr/local/Weaver/<version>/<edition>``. Installations are kept side by side. Select **Refresh Installed Versions** after changing installations outside the Manager.

The Manager does not currently import arbitrary existing installations or provide a UI for choosing a custom program-install root. A detected installation must be in one of the checked locations and include the expected Weaver CLI executable.

Available actions
-----------------

Expand **Actions** on an installation to access its available shortcuts:

- **Open installation folder** opens the program files.
- **Open data folder** opens the data folder for that version and edition, when it exists.
- **Open Weaver CLI** opens the installed CLI in a terminal: Command Prompt on Windows, Terminal.app on macOS, or an available Linux terminal emulator such as GNOME Terminal, Konsole, XFCE Terminal, or xterm. On Linux, a supported terminal emulator must be installed. The terminal remains open after the CLI exits so its output can be reviewed.
- **Open R Studio** launches R with the selected installation's private R library. R must be installed and discoverable on the system.
- **Open Weaver Hub** is available for Desktop installations with the R package installed. It requires R 4.2 or newer and a discoverable ``Rscript`` executable.
- **Open Weaver GUI** is available only for Desktop installations with the graphical-interface feature installed.
- **Modify** opens Windows Installer maintenance. This action is Windows-only.
- **Uninstall** asks for confirmation and removes the selected version through Windows Installer. This action is Windows-only.

The Manager may show a version as no longer available if it was removed externally. Refresh the list to update the display.

Storage locations
-----------------

By default, Manager data is under ``Documents/Weaver Manager``; the Test product uses ``Documents/Weaver Manager Test``. On Windows, the Manager installer can configure a machine-level data-directory location. Program files are installed under the Manager installation directory in ``installed_versions``. Mutable simulator data defaults to ``<Manager data directory>/installed_versions`` and Manager logs are kept in ``<Manager data directory>/logs``.

The Weaver installer may let you choose a different data directory for an individual simulator installation. Changing or deleting program files does not necessarily remove mutable data. On Windows, uninstalling Weaver Manager first attempts to uninstall detected Manager-managed Weaver versions and removes Manager logs; it does not delete non-empty mutable data folders. Back up or delete remaining data separately if required.

Logs and installer cache
------------------------

``weaver-manager.log`` records Manager activity. Windows installer operations produce ``msiexec-install.log``; Modify operations produce ``msiexec-modify.log``. These files are useful when contacting support or diagnosing an installation failure.

Verified release installers are cached temporarily and the Manager removes this cache when it closes. If an installer is still running, keep the Manager open until that operation completes.
