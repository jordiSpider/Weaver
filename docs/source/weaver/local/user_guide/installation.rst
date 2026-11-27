
============
Installation
============

This section explains how to install the program on different operating systems.

Windows
=======
1. Download the installer from the `Releases`_ page.
2. Run the installer and follow the steps.
3. After installation, you can run the program from the Start Menu, Applications folder, or terminal.

Each version and edition is installed separately under ``Program Files\Weaver``:
``Weaver\<version>\Desktop`` or ``Weaver\<version>\Server``. User data defaults
to ``Documents\Weaver\<version>\<edition>``. Installer filenames use a
zero-padded date version, for example ``26.10.02``.

The installer optionally creates a desktop shortcut for all users. It is unchecked
by default. The Desktop shortcut opens the simulator and requires the graphical
interface component; the Server shortcut opens the terminal. The shortcut name
includes the version and edition, and the installer removes it on uninstall.
For unattended MSI installations, set ``WEAVER_DESKTOP_SHORTCUT=1`` to create
it or ``WEAVER_DESKTOP_SHORTCUT=0`` to leave it disabled.

Linux
=====
1. Download the package from the `Releases`_ page.
2. Install the package using your system package manager.
3. Each version and edition is installed separately under ``/opt/Weaver/<version>/<edition>``. User output defaults to ``~/Documents/Weaver/<version>/<edition>/output``.
