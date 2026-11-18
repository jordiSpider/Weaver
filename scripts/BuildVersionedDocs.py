import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def required_env(name):
    value = os.environ.get(name, "")
    if not value:
        raise ValueError(f"Missing environment variable: {name}")
    return value


def safe_version(version):
    if version in ("", ".", "..") or any(separator in version for separator in ("/", "\\")):
        raise ValueError("Version must be a non-empty path component")
    return version


def prepare_sphinx_source(product, source_dir, destination_dir, shared_dir, version, default_language, languages):
    destination_dir.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(source_dir, destination_dir)
    shared_destination = destination_dir.parents[1] / "_shared"
    if not shared_destination.exists():
        shutil.copytree(shared_dir, shared_destination)
    conf = destination_dir / "conf.py"
    template = destination_dir / "conf.in.py"
    if template.is_file():
        replacements = {
            "@LATEST_VERSION@": version,
            "@DEFAULT_LANGUAGE@": default_language,
            "@ALL_VERSIONS@": version,
            "@ALL_LANGUAGES@": ";".join(languages),
            "@CMAKE_PROJECT_NAME@": "Weaver" if product == "weaver" else "Weaver Manager",
            "@AUTHOR@": "Weaver project",
            "@COPYRIGHT_YEAR@": str(__import__("datetime").datetime.now().year),
            "@REPO_URL@": "https://github.com/jordiSpider/Weaver",
            "@DOXYGEN_OUTPUT_DIR@": str(destination_dir / "api").replace("\\", "/"),
        }
        content = template.read_text(encoding="utf-8")
        for marker, value in replacements.items():
            content = content.replace(marker, value)
        conf.write_text(content, encoding="utf-8", newline="\n")
    elif not conf.is_file():
        raise FileNotFoundError(f"Sphinx conf.py or conf.in.py not found in {source_dir}")
    if product == "weaver-manager":
        weaver_catalog = Path(required_env("DOCS_WEAVER_CATALOG"))
        latest_weaver = json.loads(weaver_catalog.read_text(encoding="utf-8"))["latest"]
        content = conf.read_text(encoding="utf-8")
        content = content.replace("../../weaver/latest/", f"../../weaver/{latest_weaver}/")
        conf.write_text(content, encoding="utf-8", newline="\n")
        weaver_source = Path(required_env("DOCS_SOURCE_ROOT")) / "weaver" / latest_weaver
        logo_source = weaver_source / "_static" / "images" / "logo.png"
        if not logo_source.is_file():
            raise FileNotFoundError(f"Weaver logo not found for latest version {latest_weaver}: {logo_source}")
        logo_destination = destination_dir.parents[1] / "weaver" / latest_weaver / "_static" / "images" / "logo.png"
        logo_destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(logo_source, logo_destination)
    return conf


def update_conf_context(conf_path, base_conf, product, version, language, default_language, catalog_path, output_root, source_root):
    catalog = json.loads(catalog_path.read_text(encoding="utf-8"))
    entries = sorted(
        catalog.get("versions", []),
        key=lambda entry: tuple(int(value) for value in re.findall(r"\d+", entry.get("tag", entry["version"]))),
        reverse=True,
    )
    entry_versions = [entry["version"] for entry in entries]
    if version not in entry_versions:
        raise ValueError(f"Version {version} is not present in the release catalog")

    languages = {default_language}
    for entry in entries:
        locale = source_root / product / entry["version"] / "locale"
        if locale.is_dir():
            languages.update(path.name for path in locale.iterdir() if path.is_dir())

    current_dir = output_root / version
    if language != default_language:
        current_dir /= language
    urls = {}
    for entry in entries:
        target_version = entry["version"]
        target_locale = source_root / product / target_version / "locale"
        target_languages = {default_language}
        if target_locale.is_dir():
            target_languages.update(path.name for path in target_locale.iterdir() if path.is_dir())
        urls[target_version] = {}
        for target_language in languages:
            actual_language = target_language if target_language in target_languages else default_language
            target_dir = output_root / target_version
            if actual_language != default_language:
                target_dir /= actual_language
            urls[target_version][target_language] = os.path.relpath(target_dir / "index.html", current_dir).replace("\\", "/")

    context = f"""
all_versions = {entry_versions!r}
all_languages = {sorted(languages)!r}
version_labels = {dict((entry['version'], entry.get('channel', 'stable')) for entry in entries)!r}
default_version = {catalog.get('latest', version)!r}
current_version = {version!r}
current_language = {language!r}
language = current_language
html_context.update({{
    'default_version': default_version,
    'current_version': current_version,
    'all_versions': all_versions,
    'version_labels': version_labels,
    'default_language': {default_language!r},
    'current_language': current_language,
    'all_languages': all_languages,
    'urls': {urls!r},
}})
"""
    conf_path.write_text(base_conf + context, encoding="utf-8", newline="\n")


def replace_directory(staged_dir, destination):
    destination.parent.mkdir(parents=True, exist_ok=True)
    backup = destination.with_name(destination.name + ".previous")
    if backup.exists():
        shutil.rmtree(backup)
    if destination.exists():
        destination.rename(backup)
    try:
        Path(staged_dir).rename(destination)
    except Exception:
        if backup.exists() and not destination.exists():
            backup.rename(destination)
        raise
    if backup.exists():
        shutil.rmtree(backup)


def main():
    source_root = Path(required_env("DOCS_SOURCE_ROOT"))
    output_root = Path(required_env("DOCS_OUTPUT_ROOT"))
    work_root = Path(required_env("DOCS_WORK_ROOT"))
    shared_source = Path(required_env("DOCS_SHARED_SOURCE"))
    product = required_env("DOCS_PRODUCT")
    version = safe_version(required_env("DOCS_VERSION"))
    default_language = os.environ.get("DOCS_DEFAULT_LANGUAGE", "en")
    catalog_path = Path(required_env("DOCS_CATALOG"))
    jobs = os.environ.get("DOCS_SPHINX_JOBS", "1")

    if product not in ("weaver", "weaver-manager"):
        raise ValueError(f"Unsupported docs product: {product}")
    if not catalog_path.is_file():
        raise FileNotFoundError(f"Release catalog not found: {catalog_path}")
    catalog = json.loads(catalog_path.read_text(encoding="utf-8"))
    catalog_versions = [entry["version"] for entry in catalog.get("versions", [])]
    if version not in catalog_versions:
        raise ValueError(f"Version {version} is not present in the release catalog")
    if product == "weaver-manager":
        weaver_catalog = Path(required_env("DOCS_WEAVER_CATALOG"))
        if not weaver_catalog.is_file():
            raise FileNotFoundError(f"Weaver release catalog not found: {weaver_catalog}")
    source_dir = source_root / product / version
    if not source_dir.is_dir():
        raise FileNotFoundError(f"Version source not found: {source_dir}")
    work_root.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory(prefix=f"{product}-{version}-", dir=work_root) as temporary:
        temp_root = Path(temporary)
        working_source = temp_root / "source" / product / version
        languages = [default_language]
        locale = source_dir / "locale"
        if locale.is_dir():
            languages.extend(sorted(path.name for path in locale.iterdir() if path.is_dir() and path.name != default_language))
        languages = list(dict.fromkeys(languages))
        conf_path = prepare_sphinx_source(product, source_dir, working_source, shared_source, version, default_language, languages)
        base_conf = conf_path.read_text(encoding="utf-8")
        staged_version_dir = temp_root / "html" / version
        staged_version_dir.mkdir(parents=True)

        for language in languages:
            update_conf_context(conf_path, base_conf, product, version, language, default_language, catalog_path, output_root, source_root)
            staged_output = staged_version_dir if language == default_language else staged_version_dir / language
            environment = os.environ.copy()
            environment.update({
                "DOCS_CATALOG": str(catalog_path),
                "DOCS_OUTPUT_ROOT": str(output_root),
                "DOCS_SOURCE_ROOT": str(source_root),
                "DOCS_VERSION": version,
                "DOCS_LANGUAGE": language,
                "DOCS_DEFAULT_LANGUAGE": default_language,
                "WEAVER_DOCS_VERSION": version,
                "WEAVER_DOCS_LANGUAGE": language,
                "WEAVER_DOCS_CATALOG": str(catalog_path if product == "weaver" else required_env("DOCS_WEAVER_CATALOG")),
                "WEAVER_DOCS_SITE_ROOT": str(output_root),
                "WEAVER_MANAGER_DOCS_VERSION": version,
                "WEAVER_MANAGER_DOCS_LANGUAGE": language,
                "WEAVER_MANAGER_DOCS_CATALOG": str(catalog_path),
                "WEAVER_MANAGER_DOCS_SITE_ROOT": str(output_root),
                "WEAVER_MANAGER_DOCS_DEFAULT_VERSION": catalog.get("latest", version),
                "WEAVER_MANAGER_DOCS_DEFAULT_LANGUAGE": default_language,
                "WEAVER_MANAGER_DOCS_LANGUAGES": ";".join(languages),
            })
            doctrees = temp_root / "doctrees" / language
            doctrees.mkdir(parents=True, exist_ok=True)
            command = [sys.executable, "-m", "sphinx", "-E", "-b", "html", "-D", f"language={language}", "-c", str(working_source), "-d", str(doctrees), "-q", "-j", jobs, str(working_source), str(staged_output)]
            subprocess.run(command, check=True, env=environment)

        replace_directory(staged_version_dir, output_root / version)

    selector_command = [
        sys.executable,
        str(Path(__file__).with_name("UpdateVersionSelectors.py")),
        "--product", product,
        "--catalog", str(catalog_path),
        "--output-root", str(output_root),
        "--source-root", str(source_root),
        "--default-language", default_language,
    ]
    subprocess.run(selector_command, check=True)
    print(f"Generated {product} {version} at {output_root / version}")


if __name__ == "__main__":
    main()
