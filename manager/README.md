# Weaver Manager

Weaver Manager is a standalone desktop application for discovering verified Weaver releases, installing multiple versions side by side, and managing supported local installations. It is maintained as an independent CMake project and does not require the Weaver simulator to be built.

## What it does

- Retrieves Weaver releases from GitHub and verifies release manifests and installer SHA-256 hashes before launching the platform installer.
- Helps users find and manage supported local Weaver installations without silently installing or updating them.
- Supports Windows, Linux, and macOS packages when they are published for the current platform.

The Manager consumes releases; it does not build Weaver installers or publish releases. It does not import arbitrary installations from custom program roots.

## Documentation

The [Weaver Manager user guide](../docs/source/weaver-manager/local/index.rst) covers installation, version management, and troubleshooting. The application's **Help** menu opens the documentation for its installed version.

Maintainer references:

- [Publishing releases to the Manager catalog](../docs/source/weaver-manager/local/publishing-releases.rst)
- [Release manifest example](release-manifest.example.json)

## Build and package

### Prerequisites

- CMake 3.25 or newer.
- A bootstrapped vcpkg checkout at `../external/vcpkg` relative to this directory. Dependencies are declared in [`vcpkg.json`](vcpkg.json); this project uses Qt 6.5 or newer and OpenSSL Crypto.
- Visual Studio 18 2026 (x64) for the supplied Windows presets; NSIS 3.03 or newer is required to package Windows installers.
- Ninja for the supplied Linux and macOS presets.

Run the workflow preset for your platform from this directory:

```sh
# Windows x64 Release
cmake --workflow --preset manager-windows-release-pack

# Windows x64 test-feature build
cmake --workflow --preset manager-windows-test-release-pack

# Linux Release
cmake --workflow --preset manager-linux-release-pack

# macOS Release
cmake --workflow --preset manager-macos-release-pack
```

The workflows configure, build, and package the Manager. Build products are written under `bin/<platform>/<configuration>` and installers under `installer`. The Windows test-feature build uses separate application and data-directory names, so it can coexist with the regular Windows build.

Application and installer icons are sourced from [`assets/branding`](../assets/branding/README.md). Windows embeds separate Manager and uninstall icons in the executables and NSIS installer; Linux installs the Manager icon for its desktop entry.

The test-feature preset enables **Add Test Installer** for trying a local package. It is not an automated unit-test suite; local packages are not verified against the signed release catalog.

## Release integrity and repository integration

The Manager verifies the catalog using an Ed25519 public key embedded at build time. The public key is in [`release-signing-public.pem`](release-signing-public.pem); keep the corresponding private signing key offline and out of the repository. See the [publishing guide](../docs/source/weaver-manager/local/publishing-releases.rst) for manifest creation, signing, key rotation, and publication requirements.

The repository's [deploy-release.yml](../.github/workflows/deploy-release.yml) builds Weaver simulator packages, not Weaver Manager packages. It does not prepare the Manager catalog manifest or signature. Do not use it to publish Manager-compatible releases without first updating the workflow and verifying its outputs against the publishing guide.

## License

Weaver Manager is distributed under the [MIT License](../LICENSE).
