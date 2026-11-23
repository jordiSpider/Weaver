Release Catalog and Security
============================

What the Manager verifies
-------------------------

The release catalog is built from GitHub Releases. Weaver Manager accepts only published immutable releases that include both ``weaver-manager-manifest.json`` and its detached Base64 Ed25519 signature, ``weaver-manager-manifest.json.sig``. Draft releases, missing files, invalid signatures, malformed manifests, and assets that do not match the current platform are omitted.

The Manager verifies the manifest using a public key pinned into the application at build time. It checks that an artifact is declared in that release and that its filename identifies a supported Weaver edition, version, operating system, and architecture. Before launching a downloaded installer, it verifies the download size and SHA-256 against the signed manifest and verifies the cached file again.

These checks protect the catalog and installer download from unauthorized or accidental changes relative to the signing key. They do not replace operating-system security controls or independent review of a release. The Manager does not run locally added Test installers through this verification flow.

Release status and selection
----------------------------

The catalog distinguishes **Latest**, **Stable**, and **Pre-Release**. The latest status refers to GitHub's latest stable release; it is not an automatic update policy. The Manager does not install updates without user confirmation. Choose an edition and version explicitly, then follow the platform installer's prompts.

Supported installer names
-------------------------

The filename carries the catalog metadata and must follow this pattern:

``Weaver-<Edition>-<YY.MM.DD>-<OS>-<Architecture>.<extension>``

For example: ``Weaver-Desktop-26.11.06-Windows-x64.msi``. The edition is ``Desktop`` or ``Server``; OS is ``Windows``, ``Linux``, or ``macOS``; architecture is currently ``x64`` or ``arm64``. Supported extensions are ``msi`` on Windows, ``deb`` or ``rpm`` on Linux, and ``pkg`` or ``dmg`` on macOS.

Private key handling
--------------------

The private Ed25519 key used to sign manifests must be kept offline and outside the repository. Never publish it in GitHub or include it in a build artifact. The corresponding public key is configured at build time; a Manager built with a different key will reject the release signature. Key rotation therefore requires a coordinated Manager build containing the new public key.

For maintainers, the complete manifest, checksum, signing, and publication checklist is in :doc:`publishing-releases` and ``manager/README.md``.
