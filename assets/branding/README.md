# Branding assets

`source/` contains the editable SVG artwork. Keep text converted to paths and avoid external images and fonts so the exports do not depend on the machine.

## Regenerate exports

Use Python 3.11 or newer and install the pinned dependencies into a virtual environment. From the repository root in PowerShell:

```powershell
python -m venv "$env:TEMP\weaver-branding-venv"
& "$env:TEMP\weaver-branding-venv\Scripts\python.exe" -m pip install -r assets\branding\requirements.txt
& "$env:TEMP\weaver-branding-venv\Scripts\python.exe" assets\branding\generate.py --output "$env:TEMP\weaver-branding-preview"
& "$env:TEMP\weaver-branding-venv\Scripts\python.exe" -m unittest discover -s assets\branding -p test_generate.py -v
```

Inspect the preview, including 16- and 32-pixel icons, before replacing the current assets. To publish the exports to the repository, run:

```powershell
& "$env:TEMP\weaver-branding-venv\Scripts\python.exe" assets\branding\generate.py
```

The command overwrites its named outputs in `generated/` but does not delete unrelated files. Review and commit the resulting changes separately. Generation happens in a temporary directory first, so rendering failures do not replace existing exports; a filesystem error during the final copy can still leave a partial update. The PNG, ICO and ICNS encoding depends on the pinned library versions; the tests check reproducibility on the current system, not across different library versions or operating systems.

## Mapping

- `weaver-logo-horizontal.svg`: 1200-pixel transparent `weaver-header.png` and `weaver-logo-horizontal.png`, plus the centered 1280x640 white `weaver-social-card.png`.
- The dark horizontal and both vertical SVGs: corresponding transparent 1200-pixel-wide PNGs. The SVGs themselves are used directly by the documentation.
- `weaver-icon.svg`: square transparent application icons at 256 and 512 pixels, a 512-pixel white avatar, a 1024-pixel PNG and a multiresolution ICNS. A multiresolution Windows ICO uses `weaver-icon-small.svg` up to 48 pixels and the full SVG for larger sizes. The ICNS is generated but is not yet integrated into an application bundle.
- `weaver-icon-small.svg` produces the favicon SVG, 96-pixel PNG, 16/32/48 ICO, white 180-pixel Apple touch icon, and white 192/512 maskable icons. The latter fit inside the maskable safe circle. The web manifest uses relative icon URLs so it works under a site prefix.
- `weaver-manager.svg` and `weaver-uninstall.svg` produce 256-, 512- and 1024-pixel PNGs, multi-resolution ICOs and ICNS files. The corresponding `-small.svg` is used for ICO frames up to 48 pixels and the 32-pixel ICNS representation; larger frames use the standard SVG. All four sources are required. Manager embeds the PNGs for its windows, uses the ICOs for Windows executables and NSIS, and installs the Manager SVG for the Linux desktop. The ICNS files are not yet integrated into a macOS application bundle.

To regenerate only Manager and uninstaller derivatives without replacing other assets, append `--variants-only` to the generation command. The avatar and social card must be uploaded separately to the relevant platform. Website Open Graph metadata and the independent `gh-pages` generator are not configured by this script.

The Manager and uninstaller SVGs use square viewBoxes sized and positioned to align their base symbols with `weaver-icon.svg` and `weaver-icon-small.svg`, respectively. Preserve that alignment when editing the artwork; the framing test checks the visible base-symbol bounds.

## Third-party artwork

The Manager gear and uninstaller minus are adapted from [Bootstrap Icons](https://icons.getbootstrap.com/) (`gear-fill` and `dash`). Their MIT license and copyright notice are in [`licenses/bootstrap-icons/LICENSE`](licenses/bootstrap-icons/LICENSE). Retain that notice when distributing these source images or their derivatives.
