import json
from pathlib import Path

from modlab_downloads.java import download_java
from modlab_downloads.minecraft import (
    build_classpath,
    build_launch_command,
    download_asset_index,
    download_assets,
    download_client_jar,
    download_libraries,
    extract_natives,
    get_version_metadata,
    launch_minecraft,
    prepare_minecraft,
    version_directory,
)


def install_java(java_version, target_directory):
    return download_java(java_version, target_directory)


def install_minecraft(manifest_file, minecraft_version, target_directory):
    return prepare_minecraft(manifest_file, minecraft_version, target_directory)


def install_minecraft_steps(manifest_file, minecraft_version, target_directory):
    install_directory = Path(version_directory(target_directory, minecraft_version))
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
    assets = download_assets(asset_index_file, install_directory)
    natives_directory = extract_natives(native_jars, install_directory)

    return {
        "install_directory": install_directory,
        "version_json": version_json,
        "client_jar": client_jar,
        "libraries": library_jars,
        "native_jars": native_jars,
        "classpath": classpath,
        "asset_index": asset_index_file,
        "assets": assets,
        "natives": natives_directory,
    }


def get_launch_command(java_executable, manifest_file, minecraft_version, target_directory, username, uuid, access_token, game_directory=None):
    return build_launch_command(
        java_executable,
        manifest_file,
        minecraft_version,
        target_directory,
        username,
        uuid,
        access_token,
        game_directory,
    )


def launch(java_executable, manifest_file, minecraft_version, target_directory, username, uuid, access_token, game_directory=None):
    return launch_minecraft(
        java_executable,
        manifest_file,
        minecraft_version,
        target_directory,
        username,
        uuid,
        access_token,
        game_directory,
    )
