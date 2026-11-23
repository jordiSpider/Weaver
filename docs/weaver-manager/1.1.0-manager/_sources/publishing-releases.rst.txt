Publishing a Weaver Release
============================

Scope
-----

Weaver Manager is a release consumer. It does not build Weaver packages, create GitHub Releases, generate manifests, or sign assets. These steps belong to the release-maintenance process.

Every release intended for the Manager catalog must be published as immutable and include the platform installer assets, ``weaver-manager-manifest.json``, and detached Base64 signature ``weaver-manager-manifest.json.sig``. The manifest is signed as exact bytes; even a formatting change after signing invalidates the signature.

Prepare the assets
------------------

1. Build, test, and finalize each installer. Do not modify an installer after calculating its hash.
2. Use a supported filename such as ``Weaver-Desktop-26.11.06-Windows-x64.msi``. The filename defines the edition, version, OS, and architecture shown in the catalog.
3. Calculate a SHA-256 hash for each installer. On Windows PowerShell:

   .. code-block:: powershell

	  Get-FileHash .\Weaver-Desktop-26.11.06-Windows-x64.msi -Algorithm SHA256

   On Linux use ``sha256sum <installer-file>``; on macOS use ``shasum -a 256 <installer-file>``. Copy the 64 hexadecimal characters into the artifact's ``sha256`` value.

4. Create a schema-version 1 manifest. Each artifact object must include the exact asset filename and its SHA-256. See ``manager/release-manifest.example.json`` in the repository for a sample schema.
5. Sign the manifest using the offline private key and Base64-encode the detached signature:

   .. code-block:: console

	  openssl pkeyutl -sign -rawin -inkey release-signing-private.pem -in weaver-manager-manifest.json -out signature.bin
	  openssl base64 -A -in signature.bin -out weaver-manager-manifest.json.sig

   Never commit, upload, or place the private key in a public CI secret.

Publish and verify
------------------

1. Create a draft GitHub Release and attach every installer, ``weaver-manager-manifest.json``, and ``weaver-manager-manifest.json.sig``.
2. Check that each manifest filename exactly matches an attached installer and that the hash was calculated from the final file.
3. Confirm the signature file is the Base64 detached Ed25519 signature of the exact manifest bytes.
4. Review the release assets, then publish only when all checks pass and release immutability is enabled for the repository.
5. Test catalog discovery and installation with a Manager built using the matching public key.

Published immutable release assets should be treated as final. If an asset or manifest is wrong, prepare a corrected release rather than relying on replacing published files.

Manager-compatible filename and formats
---------------------------------------

The supported filename pattern is ``Weaver-<Edition>-<YY.MM.DD>-<OS>-<Architecture>.<extension>``. Editions are ``Desktop`` or ``Server``; OS names are ``Windows``, ``Linux``, or ``macOS``; architectures are currently ``x64`` and ``arm64``. The supported package formats are MSI for Windows, DEB or RPM for Linux, and PKG or DMG for macOS. Packages must be attached to the same GitHub Release as the signed manifest.

Key configuration and rotation
-------------------------------

The Manager's public verification key is pinned at build time. CMake reads ``manager/release-signing-public.pem`` by default (PEM or DER input is supported) or can use ``WEAVER_RELEASE_PUBLIC_KEY_BASE64`` as a fallback override. Commit only the public key. If the signing key changes, build and distribute a Manager version containing the new public key in coordination with release publication; otherwise the catalog will reject signatures from the new key.

Current automation limitation
-----------------------------

The repository's current ``.github/workflows/deploy-release.yml`` does not generate or attach the Manager manifest and signature. Its generated artifact names and formats also differ from the Manager catalog's required installer names and extensions. The workflow must be updated and its output validated before it can be used as an end-to-end publishing pipeline for Manager-compatible releases.

The full build, package, manifest and signing instructions are also maintained in ``manager/README.md``.
