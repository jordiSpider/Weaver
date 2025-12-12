=============
Build System
=============

Common Requirements
===================

Before building, make sure you have the following tools installed:

- A **C++17** compatible compiler
- `CMake 3.25+ <https://cmake.org/download/>`_

Compiler installation by operating system
-----------------------------------------

**Windows**
^^^^^^^^^^^

On Windows, we recommend using **Microsoft Visual Studio 17 2022** as the CMake generator and compiler.

Download and install **Visual Studio 2022** (Community, Professional, or Enterprise edition) from: https://visualstudio.microsoft.com/vs/

During installation, make sure to select the following components:
   - *Desktop development with C++*


**Linux**
^^^^^^^^^

On Linux, you can use **g++** as your C++17-compatible compiler.

Install all requirements using your package manager. For example:

.. code-block:: bash

    # Ubuntu
    sudo apt update
    sudo apt install -y g++ cmake make curl pkg-config libx11-dev libxft-dev libxcursor-dev


Release Requirements
====================

Windows
-------
For Windows release packaging, the **Nullsoft Scriptable Install System (NSIS)** is required.

- **NSIS** — used to generate the project installer (`.exe`) during the release build.
- Installation guide: https://nsis.sourceforge.io/Download

Linux
-----
For Linux release packaging, there are no mandatory external requirements for the CPack packaging tool to generate the launch package in TGZ (tar.gz) format.


Documentation Requirements
==========================

To build the project documentation, the following tools are required:

- **Doxygen** 1.9.8+
    Used to generate the C++ API reference documentation.

    - **Windows**

        - Download the Windows installer from: https://www.doxygen.nl/download.html

        - During installation, make sure to check:
            ``Add doxygen to the system PATH``.

    - **Linux**
        - On Ubuntu-based systems:

            .. code-block:: bash

                sudo apt update
                sudo apt install doxygen

- **Doxygen extensions**
    In order to include class diagrams and dependency graphs, **Graphviz** must also be installed.

    - **Windows**

        - Download the installer from: https://graphviz.org/download/
        - Ensure you select the option to add Graphviz to your ``PATH`` during installation.

    - **Linux**

        - On Ubuntu-based systems:

            .. code-block:: bash

                sudo apt update
                sudo apt install graphviz

- **Python** 3.10+  
  Required to run Sphinx and its extensions.

  - Installation guide: https://www.python.org/downloads/

- **Sphinx** 8.1.3+
    Used to generate the general documentation in HTML and other formats.

    - Install using Python's package manager:

    .. code-block:: bash

       python -m pip install sphinx==8.1.3

- **Sphinx extensions**

    Several Sphinx extensions are required for styling, localization, and integration
    with Doxygen:

    - ``sphinx-rtd-theme`` — provides the *Read the Docs* visual theme.  
    - ``sphinx-intl`` — used to manage translations of the documentation.


    To install all required dependencies:

    .. code-block:: bash

       python -m pip install sphinx-rtd-theme sphinx-intl


Developer Setup
===============

To get started with the project, make sure **Git** is installed on your system.

- **Install Git:** You can download and install it from the official website: https://git-scm.com/downloads

Once Git is available, clone the repository from GitHub:

.. parsed-literal::

   # Clone the repository and submodules
   git clone --recurse-submodules |repo_git_url|


CMake Presets
=============

This project uses **CMakePresets.json** to manage all build configurations, documentation builds, and deployment workflows.

Presets provide predefined configurations for consistent builds across different systems and environments.  
All presets can be listed using:

.. code-block:: bash

   cmake --list-presets

or executed as workflows with:

.. code-block:: bash

   cmake --workflow --preset <preset-name>


Available Presets
-----------------

The presets are organized into three categories:

**1. Executable builds**

These presets compile the main application, either with or without a graphical interface.

+----------------------------------------+------------------------------------------------------------+
| **Preset**                             | **Description**                                            |
+========================================+============================================================+
| ``windows-gui-debug``                  | Builds the Windows GUI version in *Debug* mode.            |
+----------------------------------------+------------------------------------------------------------+
| ``windows-gui-release``                | Builds the Windows GUI version in *Release* mode.          |
+----------------------------------------+------------------------------------------------------------+
| ``windows-non-gui-debug``              | Builds the Windows non-GUI version in *Debug* mode.        |
+----------------------------------------+------------------------------------------------------------+
| ``windows-non-gui-release``            | Builds the Windows non-GUI version in *Release* mode.      |
+----------------------------------------+------------------------------------------------------------+
| ``linux-gui-debug``                    | Builds the Linux GUI version in *Debug* mode.              |
+----------------------------------------+------------------------------------------------------------+
| ``linux-gui-release``                  | Builds the Linux GUI version in *Release* mode.            |
+----------------------------------------+------------------------------------------------------------+
| ``linux-non-gui-debug``                | Builds the Linux non-GUI version in *Debug* mode.          |
+----------------------------------------+------------------------------------------------------------+
| ``linux-non-gui-release``              | Builds the Linux non-GUI version in *Release* mode.        |
+----------------------------------------+------------------------------------------------------------+

Example usage:

.. code-block:: bash

   cmake --workflow --preset windows-non-gui-release

This command will:
  1. Configure CMake with all preset options.
  2. Build the project using the specified compiler and triplet.


**2. Documentation builds**

The project includes dedicated presets for generating and deploying documentation built with **Sphinx** and **Doxygen**.

+-------------------------+------------------------------------------------------------+
| **Preset**              | **Description**                                            |
+=========================+============================================================+
| ``build-docs-dev``      | Builds documentation locally for development preview.      |
+-------------------------+------------------------------------------------------------+
| ``deploy-docs``         | Builds and deploys documentation to GitHub Pages.          |
+-------------------------+------------------------------------------------------------+
| ``update-docs``         | Updates the existing deployed documentation.               |
+-------------------------+------------------------------------------------------------+
| ``build-docs-locale``   | Generates translation files for documentation.             |
+-------------------------+------------------------------------------------------------+

Example usage:

.. code-block:: bash

   cmake --workflow --preset deploy-docs


**3. Common internal presets**

The following presets define shared variables and configurations used by other presets.  
They are marked as *hidden* and should not be invoked directly:

- ``common`` → Defines global paths and metadata.
- ``executables`` → Base for all executable builds.
- ``gui`` / ``non-gui`` → Enable or disable GUI mode.
- ``debug`` / ``release`` → Define build type (Debug or Release).
- ``docs`` → Base for documentation-related builds.


Updating vcpkg Version
======================

The project includes **vcpkg** as a Git submodule under ``external/vcpkg``.  
Occasionally, it may be necessary to update the version of vcpkg used by the project
(e.g., to obtain newer libraries or fixes).

When updating the vcpkg submodule, the following steps **must** be performed:

1. **Update the submodule**

    .. code-block:: bash

       cd external/vcpkg
       git pull origin master

2. **Obtain the new commit hash**

    The current vcpkg commit is used as a baseline reference in the project's
    ``vcpkg.json`` file (field ``builtin-baseline``).  
    After updating the submodule, retrieve the new commit hash with:

    .. code-block:: bash

        rev-parse HEAD

3. **Update the `builtin-baseline` field**

    Copy the hash obtained in the previous step and update the value of
    ``builtin-baseline`` in your ``vcpkg.json`` file accordingly:

    .. code-block:: json

       {
            "builtin-baseline": "new_commit_hash_here",
            "dependencies": [
                
            ]
       }

4. **Commit the changes**

    Once the submodule and baseline have been updated, commit the modifications:

    .. code-block:: bash

       git add external/vcpkg vcpkg.json
       git commit -m "Update vcpkg submodule and builtin-baseline"
