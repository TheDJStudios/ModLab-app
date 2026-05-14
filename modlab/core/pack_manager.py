import json
from pathlib import Path

from PySide6.QtCore import QObject, Signal

from .pack import Loader, Pack, PackStatus, loader_from_text


class PackManager(QObject):
    """Owns the in-memory pack list and mirrors the backend packmapping file."""

    packs_changed = Signal()
    pack_progress_changed = Signal(str, int)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._packs: list[Pack] = []
        self.mapping_path = Path.home() / ".modlab" / "packmapping" / "packmapping.json"

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
            self._packs.append(
                Pack(
                    key=key,
                    name=str(obj.get("name") or key),
                    mc_version=str(obj.get("version") or ""),
                    loader=loader_from_text(str(obj.get("loader") or "vanilla")),
                    mod_count=int(obj.get("mod_count") or 0),
                    status=PackStatus.READY
                    if "launch_version" in obj
                    else PackStatus.NOT_INSTALLED,
                )
            )
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
