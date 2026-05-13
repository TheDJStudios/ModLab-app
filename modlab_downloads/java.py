import os
import platform
import shutil
import tarfile
import zipfile
from pathlib import Path

import requests



ADOPTIUM_API = "https://api.adoptium.net/v3/binary/latest/{version}/ga/{os}/{arch}/jre/hotspot/normal/eclipse"


def _platform_parts():
    system = platform.system().lower()
    machine = platform.machine().lower()

    if system == "darwin":
        api_os = "mac"
    elif system == "windows":
        api_os = "windows"
    elif system == "linux":
        api_os = "linux"
    else:
        raise ValueError(f"Unsupported OS: {platform.system()}")

    if machine in {"x86_64", "amd64"}:
        arch = "x64"
    elif machine in {"arm64", "aarch64"}:
        arch = "aarch64"
    else:
        raise ValueError(f"Unsupported CPU architecture: {platform.machine()}")

    return api_os, arch


def _download_file(url, destination):
    destination.parent.mkdir(parents=True, exist_ok=True)
    with requests.get(url, stream=True, timeout=60) as response:
        response.raise_for_status()
        with destination.open("wb") as file:
            for chunk in response.iter_content(chunk_size=1024 * 1024):
                if chunk:
                    file.write(chunk)
    return destination


def _extract_archive(archive_path, destination):
    destination.mkdir(parents=True, exist_ok=True)
    if archive_path.suffix == ".zip":
        with zipfile.ZipFile(archive_path) as archive:
            archive.extractall(destination)
    else:
        with tarfile.open(archive_path) as archive:
            archive.extractall(destination)


def _find_java_executable(directory):
    executable = "java.exe" if os.name == "nt" else "java"
    for path in directory.rglob(executable):
        if path.parent.name == "bin":
            return path
    return None


def download_java(java_version, target_directory):
    target_directory = Path(target_directory)
    install_directory = target_directory / f"java-{java_version}"
    existing_java = _find_java_executable(install_directory)
    if existing_java:
        return existing_java

    api_os, arch = _platform_parts()
    archive_name = "java.zip" if api_os == "windows" else "java.tar.gz"
    archive_path = target_directory / "downloads" / f"java-{java_version}-{api_os}-{arch}-{archive_name}"
    url = ADOPTIUM_API.format(version=java_version, os=api_os, arch=arch)

    if install_directory.exists():
        shutil.rmtree(install_directory)

    _download_file(url, archive_path)
    _extract_archive(archive_path, install_directory)

    java_path = _find_java_executable(install_directory)
    if not java_path:
        raise FileNotFoundError(f"Could not find Java executable in {install_directory}")
    return java_path
