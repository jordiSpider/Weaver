# Weaver Manager

Weaver Manager is a standalone desktop application for discovering verified Weaver releases, installing versions side by side, and opening or maintaining local installations. It is built independently from the Weaver simulator.

## User guide

### Install Weaver Manager

Download the package for your operating system from the project's published releases and install it using the normal procedure for that platform.

### Browse and install versions

Open **Available Releases**. The Manager loads GitHub releases and displays verified entries for the current operating system and CPU architecture. Select **Refresh GitHub Releases** to check again; click a column heading to sort. Release status distinguishes **Latest**, **Stable**, and **Pre-Release**. **NEW** marks a recently discovered entry; it is not a security or compatibility rating.

Select **Install** beside the desired edition and version, then confirm. The Manager downloads the installer, checks its size and SHA-256 against the signed release manifest, saves and re-verifies it in a temporary cache, and only then launches the platform installer. The installer may request administrator privileges. Complete its prompts to finish installation; the Manager does not silently install or update Weaver.

Supported installer formats are MSI on Windows, DEB or RPM on Linux, and PKG or DMG on macOS, if published for the current platform. Catalog entries appear only when their release metadata and artifact match the current platform and pass validation.

### Manage installed versions

The **Installed Versions** tab lists Weaver versions the Manager can detect. It checks its managed program directory and `/opt/Weaver/<version>/<edition>` on Linux. On macOS, it checks `/Applications/Weaver/<version>/<edition>`, `~/Applications/Weaver/<version>/<edition>`, `/usr/local/<version>/<edition>`, and `/usr/local/Weaver/<version>/<edition>`. Versions are kept side by side. Select **Refresh Installed Versions** after installing or uninstalling outside the Manager.

Expand **Actions** to open an installation or version data folder, start Weaver CLI in a terminal, or open R using that installation's private R library. The CLI action uses Command Prompt on Windows, Terminal.app on macOS, or an available Linux terminal emulator (such as GNOME Terminal, Konsole, XFCE Terminal, or xterm). On Linux, a supported terminal emulator must be installed. The terminal stays open after the CLI exits so you can review its output. Desktop installations may also offer **Open Weaver Hub** and **Open Weaver GUI** when the required components are installed. **Modify** and **Uninstall** are currently Windows-only. Modify opens Windows Installer maintenance; Uninstall asks for confirmation.

The Manager does not import arbitrary existing installations or provide a UI to choose a custom program-install root. An installation must be in one of the locations above and contain the expected Weaver CLI executable to appear in the list.

### Install a local test build

Only a Manager built with `ENABLE_TEST=ON` shows **Add Test Installer**. Use **Browse…**, enter or paste a path, or drag one installer onto the dialog. The filename must follow `Weaver-Edition-Version-OS-Architecture.extension`, for example `Weaver-Desktop-26.11.06-Windows-x64.msi`, and match the current platform. Select the local entry's **Install** button and review the warning before launching it.

Local test packages are not checked against a signed GitHub manifest or SHA-256 catalog entry. Use only installer files from a trusted source. This feature is temporary for testing; it does not publish a release or make the package available to other users.

### Data and logs

By default, Manager data is stored in `Documents/Weaver Manager` (or `Documents/Weaver Manager Test`). Weaver program files are under the Manager installation directory in `installed_versions`; mutable data is under the Manager data directory in `installed_versions`, and diagnostic logs under `logs`. Windows installation may set another Manager data directory through machine-level configuration. The Weaver installer can allow a separate data folder for an individual simulator installation. On Windows, removing Weaver Manager first attempts to uninstall detected Manager-managed Weaver versions and removes Manager logs, but it does not delete non-empty mutable data folders.

`weaver-manager.log` records Manager activity. On Windows, `msiexec-install.log` contains verbose installer output and maintenance operations write `msiexec-modify.log`. The downloaded verified-installer cache is removed when the Manager closes. Keep the Manager open until an installation operation finishes.

### Troubleshooting

- **No releases appear:** refresh and check network access to GitHub. Draft or non-immutable releases, missing manifest/signature files, invalid signatures, malformed manifests, or artifacts for another platform are not shown.
- **Installer verification fails:** do not run the file manually as a workaround. The download may be incomplete or the published hash/signature incorrect. Contact the release maintainer and include the Manager log.
- **A version is missing:** refresh the installed list. Only installations in the Manager-managed program directory with a detectable CLI executable are listed; custom-root copies are not imported.
- **Windows installer errors:** review the status message and `msiexec-install.log` or `msiexec-modify.log`; confirm the package and required privileges before retrying.
- **R, Hub, or GUI does not start:** R must be installed and discoverable. Hub requires R 4.2 or newer and the Desktop R package. Hub and GUI are Desktop-only and require their optional features.

## Publish a Weaver release to the Manager catalog

The Manager consumes GitHub Releases; it does not create, sign, or publish them. Each immutable published release must include supported installer assets, `weaver-manager-manifest.json`, and detached Base64 signature `weaver-manager-manifest.json.sig`. See [`release-manifest.example.json`](release-manifest.example.json) for the schema. Each artifact entry contains the exact asset filename and its 64-character SHA-256. Version, edition, OS, and architecture are parsed from the filename. Accepted names follow `Weaver-Desktop-26.11.06-Windows-x64.msi`; use `Server` for that edition and the appropriate OS, architecture, and supported extension.

Release checklist:

1. Build and finalize each installer. Do not modify it after hashing.
2. Use the supported filename and calculate SHA-256 for every artifact.
3. Create a schema-version 1 manifest listing all filenames and hashes.
4. Sign the exact manifest bytes with the offline Ed25519 private key and Base64-encode the detached signature.
5. Attach the installers, manifest, and signature to a draft GitHub Release. Check names and contents before publication.
6. Publish only after review. The Manager accepts immutable releases; published assets should be treated as final.

On Windows, calculate a hash with PowerShell:

```powershell
Get-FileHash .\Weaver-Desktop-26.11.06-Windows-x64.msi -Algorithm SHA256
```

Keep the private key outside the repository. Sign with OpenSSL:

```sh
openssl pkeyutl -sign -rawin -inkey release-signing-private.pem -in weaver-manager-manifest.json -out signature.bin
openssl base64 -A -in signature.bin -out weaver-manager-manifest.json.sig
```

The Manager's public verification key is pinned at build time. Configure with the matching committed PEM/DER key or the `WEAVER_RELEASE_PUBLIC_KEY_BASE64` fallback. Key rotation requires a coordinated Manager build containing the new public key; releases signed by an unknown key will not be displayed.

> **Repository integration note:** `.github/workflows/deploy-release.yml` currently does not generate or attach the Manager manifest/signature, and its artifact filenames/formats do not match the catalog naming convention. Release maintainers must reconcile the workflow and package outputs with this checklist before relying on it to publish Manager-compatible releases.

## Build and package Weaver Manager

### Requirements

- CMake 3.25 or newer
- Bootstrapped vcpkg checkout at `../external/vcpkg`
- Ninja for the supplied Linux and macOS presets
- NSIS 3.03 or newer on `PATH` when packaging Windows installers

Run from the `manager/` directory:

```sh
# Windows x64 Release
cmake --workflow --preset manager-windows-release-pack

# Windows Test build (includes Add Test Installer)
cmake --workflow --preset manager-windows-test-release-pack

# Linux
cmake --workflow --preset manager-linux-release-pack

# macOS
cmake --workflow --preset manager-macos-release-pack
```

All presets use the same sources. Test changes the application, installer, and data-directory names and enables test-only UI; Release and Test can coexist on Windows. The root Weaver CMake project is not required. Build outputs are under `bin/<platform>/<configuration>` and packages under `installer`.