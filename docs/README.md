# Generador local de documentación

Este paquete es independiente del generador de la rama `gh-pages`. Copia `CMakeLists.txt`, `CMakePresets.json`, este `README.md` y la carpeta `scripts/` directamente a `docs/` del repositorio principal, junto a sus carpetas `source/` existentes. No copies las carpetas temporales `source/` y `build/` que puedan aparecer en este directorio.

## Fuentes y salida

- Weaver: `docs/source/weaver/local/`
- Weaver Manager: `docs/source/weaver-manager/local/`
- HTML: `docs/build/weaver/latest/` o `docs/build/weaver-manager/latest/`

El generador copia la fuente seleccionada a un directorio temporal, la construye como versión `latest` usando un catálogo temporal y no consulta ni actualiza GitHub Releases. Para incluir la referencia API de Weaver, la fuente local debe contener el XML de Doxygen en `api/xml/`.

## Uso en PowerShell

Ejecuta estos comandos desde el directorio `docs/` del repositorio principal:

```powershell
cmake --workflow --preset generate-local-weaver
cmake --workflow --preset generate-local-manager
```

Se necesita CMake 3.25+, Ninja, Python 3, Sphinx, `pydata_sphinx_theme` y `sphinx-design`. Weaver Manager también requiere `sphinx-copybutton` para mostrar el botón de copia en los bloques de código. Weaver requiere Breathe.
