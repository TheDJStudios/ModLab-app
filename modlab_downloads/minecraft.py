import json
import os
import platform
import shutil
import subprocess
import zipfile
from hashlib import sha1
from pathlib import Path

import requests


NATIVES_TOKEN = "${arch}"
CLASSPATH_SEPARATOR = ";" if os.name == "nt" else ":"


def load_manifest(manifest_file):
    return json.loads(Path(manifest_file).read_text(encoding="utf-8"))


def get_version_metadata(manifest_file, minecraft_version):
    manifest = load_manifest(manifest_file)
    for version in manifest["versions"]:
        if version["id"] == minecraft_version:
            response = requests.get(version["url"], timeout=30)
            response.raise_for_status()
            return response.json()
    raise ValueError(f"Minecraft version not found in manifest: {minecraft_version}")


def version_directory(target_directory, minecraft_version):
    return Path(target_directory) / minecraft_version


def _download_file(url, destination, expected_sha1=None):
    destination = Path(destination)
    if destination.exists() and destination.stat().st_size > 0:
        if expected_sha1 is None or _sha1(destination) == expected_sha1:
            return destination

    destination.parent.mkdir(parents=True, exist_ok=True)
    with requests.get(url, stream=True, timeout=60) as response:
        response.raise_for_status()
        with destination.open("wb") as file:
            for chunk in response.iter_content(chunk_size=1024 * 1024):
                if chunk:
                    file.write(chunk)
    if expected_sha1 is not None and _sha1(destination) != expected_sha1:
        raise ValueError(f"Downloaded file failed SHA-1 check: {destination}")
    return destination


def _sha1(path):
    digest = sha1()
    with Path(path).open("rb") as file:
        for chunk in iter(lambda: file.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _current_rule_os():
    system = platform.system().lower()
    if system == "darwin":
        return "osx"
    if system == "windows":
        return "windows"
    if system == "linux":
        return "linux"
    return system


def _rule_matches(rule):
    os_rule = rule.get("os")
    if not os_rule:
        return True
    return os_rule.get("name") == _current_rule_os()


def _allowed_by_rules(rules):
    if not rules:
        return True

    allowed = not any(rule.get("action") == "allow" for rule in rules)
    for rule in rules:
        if _rule_matches(rule):
            allowed = rule.get("action") == "allow"
    return allowed


def _library_artifact(library):
    downloads = library.get("downloads", {})
    classifiers = downloads.get("classifiers", {})
    natives = library.get("natives", {}).get(_current_rule_os())
    if natives:
        native_key = natives.replace(NATIVES_TOKEN, "64")
        return classifiers.get(native_key), True
    return downloads.get("artifact"), False


def download_client_jar(version_json, install_directory):
    install_directory = Path(install_directory)
    client = version_json["downloads"]["client"]
    return _download_file(
        client["url"],
        install_directory / "client.jar",
        client.get("sha1"),
    )


def download_libraries(version_json, install_directory):
    install_directory = Path(install_directory)
    library_root = install_directory / "libraries"
    native_jars = []
    library_jars = []

    for library in version_json.get("libraries", []):
        if not _allowed_by_rules(library.get("rules")):
            continue

        artifact, is_native = _library_artifact(library)
        if not artifact:
            continue

        jar_path = _download_file(
            artifact["url"],
            library_root / artifact["path"],
            artifact.get("sha1"),
        )
        if is_native:
            native_jars.append(jar_path)
        else:
            library_jars.append(jar_path)

    return library_jars, native_jars


def build_classpath(install_directory, library_jars=None):
    install_directory = Path(install_directory)
    if library_jars is None:
        library_jars = sorted((install_directory / "libraries").rglob("*.jar"))

    classpath_parts = [str(path) for path in library_jars]
    classpath_parts.append(str(install_directory / "client.jar"))
    classpath = CLASSPATH_SEPARATOR.join(classpath_parts)
    (install_directory / "classpath.txt").write_text(classpath, encoding="utf-8")
    return classpath


def download_asset_index(version_json, install_directory):
    install_directory = Path(install_directory)
    asset_index = version_json["assetIndex"]
    index_path = install_directory / "assets" / "indexes" / f"{asset_index['id']}.json"
    return _download_file(asset_index["url"], index_path, asset_index.get("sha1"))


def download_assets(asset_index_file, install_directory):
    install_directory = Path(install_directory)
    assets_root = install_directory / "assets"
    asset_index = load_manifest(asset_index_file)
    downloaded = []

    for asset in asset_index.get("objects", {}).values():
        asset_hash = asset["hash"]
        asset_path = assets_root / "objects" / asset_hash[:2] / asset_hash
        url = f"https://resources.download.minecraft.net/{asset_hash[:2]}/{asset_hash}"
        downloaded.append(_download_file(url, asset_path, asset_hash))

    return downloaded


def extract_natives(native_jars, install_directory):
    install_directory = Path(install_directory)
    natives_directory = install_directory / "natives"
    if natives_directory.exists():
        shutil.rmtree(natives_directory)
    natives_directory.mkdir(parents=True, exist_ok=True)

    exclusions = ("META-INF/",)
    for native_jar in native_jars:
        with zipfile.ZipFile(native_jar) as archive:
            for member in archive.namelist():
                if member.endswith("/") or member.startswith(exclusions):
                    continue
                archive.extract(member, natives_directory)

    return natives_directory


def prepare_minecraft(manifest_file, minecraft_version, target_directory):
    install_directory = version_directory(target_directory, minecraft_version)
    install_directory.mkdir(parents=True, exist_ok=True)

    version_json = get_version_metadata(manifest_file, minecraft_version)
    (install_directory / "version.json").write_text(
        json.dumps(version_json, indent=4),
        encoding="utf-8",
    )

    client_jar = download_client_jar(version_json, install_directory)
    library_jars, native_jars = download_libraries(version_json, install_directory)
    classpath = build_classpath(install_directory, library_jars)
    asset_index_file = download_asset_index(version_json, install_directory)
    download_assets(asset_index_file, install_directory)
    natives_directory = extract_natives(native_jars, install_directory)

    return {
        "install_directory": install_directory,
        "version_json": version_json,
        "client_jar": client_jar,
        "libraries": library_jars,
        "natives": natives_directory,
        "classpath": classpath,
        "asset_index": asset_index_file,
    }


def build_launch_arguments(version_json, install_directory, username, uuid, access_token, game_directory=None):
    install_directory = Path(install_directory)
    game_directory = Path(game_directory) if game_directory else install_directory / "game"
    game_directory.mkdir(parents=True, exist_ok=True)

    replacements = {
        "${auth_player_name}": username,
        "${version_name}": version_json["id"],
        "${game_directory}": str(game_directory),
        "${assets_root}": str(install_directory / "assets"),
        "${assets_index_name}": version_json["assetIndex"]["id"],
        "${auth_uuid}": uuid,
        "${auth_access_token}": access_token,
        "${clientid}": "",
        "${auth_xuid}": "",
        "${user_type}": "msa",
        "${version_type}": version_json.get("type", "release"),
    }

    args = []
    minecraft_arguments = version_json.get("minecraftArguments")
    if minecraft_arguments:
        raw_args = minecraft_arguments.split()
    else:
        raw_args = []
        for argument in version_json.get("arguments", {}).get("game", []):
            if isinstance(argument, str):
                raw_args.append(argument)
            elif _allowed_by_rules(argument.get("rules")):
                value = argument.get("value")
                if isinstance(value, list):
                    raw_args.extend(value)
                else:
                    raw_args.append(value)

    for arg in raw_args:
        for key, value in replacements.items():
            arg = arg.replace(key, value)
        args.append(arg)
    return args


def build_jvm_arguments(version_json, install_directory, classpath):
    install_directory = Path(install_directory)
    replacements = {
        "${natives_directory}": str(install_directory / "natives"),
        "${launcher_name}": "ModLab",
        "${launcher_version}": "0.1",
        "${classpath}": classpath,
    }

    args = []
    for argument in version_json.get("arguments", {}).get("jvm", []):
        if isinstance(argument, str):
            values = [argument]
        elif _allowed_by_rules(argument.get("rules")):
            value = argument.get("value")
            values = value if isinstance(value, list) else [value]
        else:
            values = []

        for value in values:
            for key, replacement in replacements.items():
                value = value.replace(key, replacement)
            args.append(value)

    if not args:
        args = [f"-Djava.library.path={install_directory / 'natives'}", "-cp", classpath]

    return args


def build_launch_command(java_executable, manifest_file, minecraft_version, target_directory, username, uuid, access_token, game_directory=None):
    install_directory = version_directory(target_directory, minecraft_version)
    version_json_path = install_directory / "version.json"
    if version_json_path.exists():
        version_json = load_manifest(version_json_path)
    else:
        version_json = get_version_metadata(manifest_file, minecraft_version)

    classpath_path = install_directory / "classpath.txt"
    classpath = classpath_path.read_text(encoding="utf-8") if classpath_path.exists() else build_classpath(install_directory)
    jvm_args = build_jvm_arguments(version_json, install_directory, classpath)
    game_args = build_launch_arguments(version_json, install_directory, username, uuid, access_token, game_directory)
    return [str(java_executable), *jvm_args, version_json["mainClass"], *game_args]


def launch_minecraft(java_executable, manifest_file, minecraft_version, target_directory, username, uuid, access_token, game_directory=None):
    command = build_launch_command(
        java_executable,
        manifest_file,
        minecraft_version,
        target_directory,
        username,
        uuid,
        access_token,
        game_directory,
    )
    return subprocess.Popen(command, cwd=version_directory(target_directory, minecraft_version))
