
======================================================
Continuous Integration & Continuous Deployment (CI/CD)
======================================================

The project uses CI/CD pipelines to automate building, testing, releasing executables, 
and updating the documentation. This ensures code quality and fast delivery while keeping 
documentation up-to-date.

Configuration
=============
- The CI/CD is configured using GitHub Actions.
- Workflow files are located in `.github/workflows/`.

Workflows
=========

Deploy Release Workflow
-----------------------
This workflow automates the process of building, packaging, and deploying new releases of the project,
including the generation and publication of documentation.

**Workflow name:** ``Deploy release``  

**Trigger:** ``workflow_dispatch`` (manual trigger from the GitHub Actions interface)

**Purpose:**
- Build and package both GUI and non-GUI Windows releases
- Create a new GitHub release with the generated installers
- Build and deploy the latest documentation to GitHub Pages
- Merge the updated documentation back into the ``develop`` branch

**Overview:**
The workflow is composed of three main jobs:

1. ``build-windows`` — Builds the Windows release versions (GUI and non-GUI).
2. ``release`` — Creates a new GitHub release and attaches the generated installers.
3. ``deploy-docs`` — Builds and publishes the documentation to GitHub Pages.

Each job runs in sequence, ensuring that binaries and documentation are consistent with the latest code.

Job 1 — build-windows
~~~~~~~~~~~~~~~~~~~~~
**Runs on:** ``windows-latest`` 

**Matrix:** preset = [``windows-gui-release``, ``windows-non-gui-release``]

**Purpose:**  
Builds the Windows release binaries and packages them into installers.

**Main steps:**

1. **Checkout repository**  
   Clones the full repository, including submodules and complete commit history.  
   Required for ``vcpkg`` to work correctly.

2. **Bootstrap vcpkg**  
   Initializes the vcpkg package manager.

3. **Install NSIS**  
   Installs NSIS using Chocolatey for creating Windows installers.

4. **Restore cached vcpkg**  
   Restores cached vcpkg dependencies to speed up builds.

5. **Configure and build**  
   Builds the project using CMake workflow presets.

6. **Save vcpkg cache**  
   Saves the current vcpkg installation for reuse in future builds.

7. **Package**  
   Uses CPack to generate the Windows installer (.exe file).

8. **Upload installer**  
   Uploads the generated installer as an artifact for later use in the ``release`` job.

Job 2 — release
~~~~~~~~~~~~~~~
**Runs on:** ``ubuntu-latest`` 

**Needs:** ``build-windows``

**Purpose:**  
Creates a new GitHub release and attaches the generated Windows installers.

**Main steps:**

1. **Checkout repository**  
   Fetches the ``main`` branch with full commit history.

2. **Download installers**  
   Downloads the artifacts created by the previous job.

3. **Setup git user**  
   Configures Git credentials for tagging and commits.

4. **Create unique tag**  
   Generates a tag based on the current date (e.g., ``v2025.10.30``).  
   The job fails if a tag with the same name already exists.

5. **Create GitHub release**  
   Uses ``softprops/action-gh-release`` to create a **draft release** and attach the installer files.

Job 3 — deploy-docs
~~~~~~~~~~~~~~~~~~~
**Runs on:** ``ubuntu-latest`` 

**Needs:** ``release``  

**Permissions:** ``contents: write``

**Purpose:**  
Builds the documentation and deploys it to GitHub Pages.

**Main steps:**

1. **Checkout repository**  
   Pulls the ``main`` branch to access the latest source and documentation files.

2. **Install dependencies**  
   Installs **Doxygen**, **Graphviz**, and **Sphinx** (with ``sphinx-rtd-theme`` and ``sphinx-intl``).

3. **Build documentation**  
   Uses the CMake workflow preset ``deploy-docs`` to generate HTML documentation.

4. **Deploy to GitHub Pages**  
   Publishes the generated HTML files to the ``gh-pages`` branch using
   ``peaceiris/actions-gh-pages``.

5. **Commit and push updated docs source**  
   Adds and commits updated documentation sources under ``docs/source/``.

6. **Merge main into develop**  
   Synchronizes the ``develop`` branch with the ``main`` branch, including the new documentation changes.
   Merge conflicts are logged but do not stop the workflow.

Deploy Release Workflow Notes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- The workflow is **manually triggered**, ensuring that new releases are only created when explicitly approved.
- Each release is created as a **draft**, allowing maintainers to review it before publishing.
- Documentation deployment automatically updates both **GitHub Pages** and the **develop** branch.
- All jobs are designed to fail early if an error occurs, ensuring consistent and traceable release artifacts.


Update Documentation Workflow
-----------------------------

.. important::

   The workflow determines the **latest release** based only on the **most recent published release**
   — that is, the **latest release that is not marked as a draft**.  
   If you intend to update the documentation to reflect the latest release version, make sure 
   that the desired release has been **published** and is **not in draft state**.  
   Otherwise, the workflow may reference an earlier release instead.

This workflow automates the process of rebuilding and updating the project documentation 
on GitHub Pages **without creating a new software release**.  
It is intended for situations where only documentation content changes (e.g., text edits, 
new guides, or updated examples) and the codebase itself remains unchanged.

**Workflow name:** ``Update docs``

**Trigger:** ``workflow_dispatch`` (manual trigger from the GitHub Actions interface)

**Purpose:**
- Rebuild the latest documentation using Sphinx and CMake
- Publish the updated documentation to GitHub Pages
- Maintain synchronization between the documentation and the main branch

**Overview:**
The workflow is composed of a single job:

1. ``update`` — Builds and deploys the documentation to GitHub Pages.

Job — update
~~~~~~~~~~~~
**Runs on:** ``ubuntu-latest``  

**Purpose:**  
Rebuilds the documentation from the ``main`` branch and redeploys it to GitHub Pages.

**Main steps:**

1. **Checkout repository**  
   Fetches the ``main`` branch with full commit history to ensure all documentation 
   source files and CMake presets are available.

2. **Install Sphinx**  
   Installs the required Python packages to build the documentation:
   - ``sphinx`` — core documentation generator  
   - ``sphinx-rtd-theme`` — provides the Read the Docs HTML theme  
   - ``sphinx-intl`` — enables translation and internationalization support

3. **Configure and build project docs**  
   Uses the CMake workflow preset ``update-docs`` to generate the HTML documentation.  
   The output is stored in ``docs/build/html``.

4. **Deploy to GitHub Pages**  
   Publishes the generated HTML documentation to the ``gh-pages`` branch using  
   ``peaceiris/actions-gh-pages``.  
   The commit is made by the ``github-actions[bot]`` user and overwrites existing documentation 
   with the new build.

Update Documentation Workflow Notes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- The workflow is **manually triggered**, allowing maintainers to update documentation 
  independently of code releases.  
- It does **not** create new tags or modify the repository source code.  
- It uses the same build system and structure as the ``Deploy release`` workflow to ensure 
  consistency across documentation updates.
- Use this workflow when updating text, images, or examples within the documentation, 
  but not when releasing new versions of the software.
