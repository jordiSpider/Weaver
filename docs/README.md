# Local documentation generator

This local generator is independent of the generator on the `gh-pages` branch, but uses the same pinned Python dependencies.

## Sources and output

- Weaver: `docs/source/weaver/local/`
- Weaver Manager: `docs/source/weaver-manager/local/`
- HTML: `docs/build/weaver/latest/` or `docs/build/weaver-manager/latest/`

Each local build first prepares its source directory: it copies the light and dark logos, favicons, header stylesheet and layout template into that product's `local` source. Weaver also generates Doxygen XML in `source/weaver/local/api/xml/` for the API reference. The generator then copies the prepared source to a temporary directory and builds it as version `latest` using a temporary catalog; it does not query or update GitHub Releases.

The branding comes from `assets/branding/`; see the [branding guide](../assets/branding/README.md) for regeneration. The copied resources live under `_static/branding/` and `_templates/layout.html` so the complete prepared source can be copied to the documentation site as a version-specific snapshot. The versioned builder uses each version's own configuration and resources, including historical versions; it does not replace them with the current branding. Missing source assets cause preparation to fail.

## Usage in PowerShell

Run these commands from the repository's `docs/` directory:

```powershell
cmake --workflow --preset generate-local-weaver
cmake --workflow --preset generate-local-manager
```

To prepare sources without building HTML or installing Python packages, run `cmake --workflow --preset prepare-local-weaver` or `cmake --workflow --preset prepare-local-manager`. The Weaver preset includes XML generation; the Manager preset copies branding only. Run these before copying the source directories elsewhere, and include the generated files in the copy.

CMake 3.25+, Ninja, and Python 3.10+ with `pip` are required. Weaver also requires Doxygen 1.9.8+ and Graphviz to generate the API reference. CMake reports an error during configuration if it cannot find a compatible Python interpreter; select one with `-DPython3_EXECUTABLE=<path>` when configuring.

Before generating HTML, CMake runs `python -m pip install -r requirements-docs.txt` using the selected interpreter, installing the same pinned versions of Sphinx, PyData Sphinx Theme, sphinx-design, Breathe, and sphinx-copybutton as `gh-pages`. This requires access to the packages and modifies the selected Python environment; using a virtual environment is recommended. If installation fails, the build fails too.
