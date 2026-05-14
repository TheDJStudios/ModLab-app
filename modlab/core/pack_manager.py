import json
from pathlib import Path

from PySide6.QtCore import QObject, Signal

from .pack import ModFile, Pack, PackStatus, loader_from_text


class PackManager(QObject):
    """Owns the in-memory pack list and mirrors the backend packmapping file."""

    packs_changed = Signal()
    pack_progress_changed = Signal(str, int)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._packs: list[Pack] = []
        self.launcher_directory = Path.home() / ".modlab"
        self.packs_directory = self.launcher_directory / "packs"
        self.mapping_path = self.launcher_directory / "packmapping" / "packmapping.json"

    @property
    def packs(self) -> list[Pack]:
        return self._packs

    def load(self) -> None:
        self.reload_from_disk()

    def reload_from_disk(self) -> None:
        self._packs.clear()
        if not self.mapping_path.exists():
            self.packs_changed.emit()
            return

        try:
            root = json.loads(self.mapping_path.read_text(encoding="utf-8") or "{}")
        except json.JSONDecodeError:
            self.packs_changed.emit()
            return

        if not isinstance(root, dict):
            self.packs_changed.emit()
            return

        for key in sorted(root):
            obj = root.get(key) or {}
            if not isinstance(obj, dict):
                continue
            pack_dir_name = str(obj.get("dir") or "")
            pack_directory = self.packs_directory / pack_dir_name if pack_dir_name else None
            mods = self._scan_mods(pack_directory)
            self._packs.append(
                Pack(
                    key=key,
                    name=str(obj.get("name") or key),
                    mc_version=str(obj.get("version") or ""),
                    loader=loader_from_text(str(obj.get("loader") or "vanilla")),
                    directory=pack_directory,
                    mods=mods,
                    mod_count=len(mods),
                    status=PackStatus.READY
                    if "launch_version" in obj
                    else PackStatus.NOT_INSTALLED,
                )
            )
        self.packs_changed.emit()

    def _scan_mods(self, pack_directory: Path | None) -> list[ModFile]:
        if pack_directory is None:
            return []
        mods_directory = pack_directory / "mods"
        if not mods_directory.exists():
            return []

        mods: list[ModFile] = []
        for path in sorted(mods_directory.glob("*.jar"), key=lambda item: item.name.lower()):
            if not path.is_file():
                continue
            mods.append(ModFile(name=path.name, path=path, size_bytes=path.stat().st_size))
        return mods

    def add_placeholder(self, pack: Pack) -> None:
        existing = self.find_pack(pack.key)
        if existing is not None:
            existing.name = pack.name
            existing.mc_version = pack.mc_version
            existing.loader = pack.loader
            existing.status = pack.status
            existing.install_progress = pack.install_progress
        else:
            self._packs.append(pack)
        self.packs_changed.emit()

    def save(self) -> None:
        if not self.mapping_path.exists():
            return
        try:
            root = json.loads(self.mapping_path.read_text(encoding="utf-8") or "{}")
        except json.JSONDecodeError:
            return
        if not isinstance(root, dict):
            return

        for pack in self._packs:
            if pack.key not in root or not isinstance(root[pack.key], dict):
                continue
            root[pack.key]["mod_count"] = pack.mod_count

        self.mapping_path.write_text(json.dumps(root, indent=4), encoding="utf-8")

    def find_pack(self, key: str) -> Pack | None:
        for pack in self._packs:
            if pack.key == key:
                return pack
        return None

    def set_progress(self, key: str, progress: int) -> None:
        pack = self.find_pack(key)
        if pack is None:
            return
        pack.install_progress = progress
        self.pack_progress_changed.emit(key, progress)

    def set_status(self, key: str, status: PackStatus) -> None:
        pack = self.find_pack(key)
        if pack is None:
            return
        pack.status = status
        self.packs_changed.emit()
