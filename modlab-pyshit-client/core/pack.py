from dataclasses import dataclass
from enum import Enum
from pathlib import Path


class Loader(Enum):
    VANILLA = "vanilla"
    FABRIC = "fabric"
    FORGE = "forge"


def loader_name(loader: Loader) -> str:
    return {
        Loader.FABRIC: "Fabric",
        Loader.FORGE: "Forge",
        Loader.VANILLA: "Vanilla",
    }.get(loader, "Vanilla")


def loader_abbr(loader: Loader) -> str:
    return {
        Loader.FABRIC: "FBR",
        Loader.FORGE: "FRG",
        Loader.VANILLA: "VNL",
    }.get(loader, "VNL")


def loader_from_text(value: str) -> Loader:
    value = (value or "vanilla").lower()
    if value == "fabric":
        return Loader.FABRIC
    if value == "forge":
        return Loader.FORGE
    return Loader.VANILLA


class PackStatus(Enum):
    READY = "ready"
    INSTALLING = "installing"
    NEEDS_UPDATE = "needs_update"
    NOT_INSTALLED = "not_installed"


def status_label(status: PackStatus) -> str:
    return {
        PackStatus.READY: "Ready",
        PackStatus.INSTALLING: "Installing",
        PackStatus.NEEDS_UPDATE: "Update",
        PackStatus.NOT_INSTALLED: "Not Installed",
    }.get(status, "")


@dataclass
class ModFile:
    name: str
    path: Path
    size_bytes: int = 0

    @property
    def size_label(self) -> str:
        if self.size_bytes >= 1024 * 1024:
            return f"{self.size_bytes / (1024 * 1024):.1f} MB"
        if self.size_bytes >= 1024:
            return f"{self.size_bytes / 1024:.1f} KB"
        return f"{self.size_bytes} B"


@dataclass
class Pack:
    key: str
    name: str
    mc_version: str
    loader: Loader
    directory: Path | None = None
    mods: list[ModFile] | None = None
    mod_count: int = 0
    status: PackStatus = PackStatus.NOT_INSTALLED
    install_progress: int = 0

    def abbr(self) -> str:
        source = self.name or self.key
        return source[:3].upper()

    @property
    def mods_directory(self) -> Path | None:
        if self.directory is None:
            return None
        return self.directory / "mods"
