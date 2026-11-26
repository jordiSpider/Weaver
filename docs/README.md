# Local documentation generator

This local generator is independent of the generator on the `gh-pages` branch, but uses the same pinned Python dependencies.

## Sources and output

- Weaver: `docs/source/weaver/local/`
- Weaver Manager: `docs/source/weaver-manager/local/`
- HTML: `docs/build/weaver/latest/` or `docs/build/weaver-manager/latest/`

The generator copies the selected source to a temporary directory, builds it as version `latest` using a temporary catalog, and does not query or update GitHub Releases. The Weaver HTML build first generates Doxygen XML in `source/weaver/local/api/xml/` for the API reference.

## Usage in PowerShell

Run these commands from the repository's `docs/` directory:

```powershell
cmake --workflow --preset generate-local-weaver
cmake --workflow --preset generate-local-manager
```

CMake 3.25+, Ninja, and Python 3.10+ with `pip` are required. Weaver also requires Doxygen 1.9.8+ and Graphviz to generate the API reference. CMake reports an error during configuration if it cannot find a compatible Python interpreter; select one with `-DPython3_EXECUTABLE=<path>` when configuring.

Before generating HTML, CMake runs `python -m pip install -r requirements-docs.txt` using the selected interpreter, installing the same pinned versions of Sphinx, PyData Sphinx Theme, sphinx-design, Breathe, and sphinx-copybutton as `gh-pages`. This requires access to the packages and modifies the selected Python environment; using a virtual environment is recommended. If installation fails, the build fails too. The `generate-local-weaver-xml` target does not install Python packages.
