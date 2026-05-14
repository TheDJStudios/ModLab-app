import shutil
from pathlib import Path

from PySide6.QtCore import Qt, QUrl
from PySide6.QtGui import QDesktopServices
from PySide6.QtWidgets import QApplication, QFileDialog, QHBoxLayout, QLabel, QMainWindow, QMessageBox, QStackedWidget, QVBoxLayout, QWidget

from ..core.pack import Pack, PackStatus, loader_name
from ..core.pack_manager import PackManager
from ..ipc.backend_process import BackendProcess
from .detail_panel import DetailPanel
from .new_pack_dialog import NewPackDialog
from .pack_grid import PackGrid
from .sidebar import Sidebar
from .top_bar import TopBar


class MainWindow(QMainWindow):
    def __init__(self, python_path: str, script_path: str, parent=None):
        super().__init__(parent)
        self.setWindowTitle("ModLab")
        self.setMinimumSize(1100, 680)
        self.resize(1280, 780)

        self.backend = BackendProcess(python_path, script_path, self)
        self.pack_manager = PackManager(self)
        self.pack_manager.load()
        self._build_layout()
        self._connect_signals()
        self.on_packs_changed()

    def _build_layout(self) -> None:
        central = QWidget(self)
        self.setCentralWidget(central)
        outer_row = QHBoxLayout(central)
        outer_row.setContentsMargins(0, 0, 0, 0)
        outer_row.setSpacing(0)

        self.sidebar = Sidebar(self)
        outer_row.addWidget(self.sidebar)

        main_col = QVBoxLayout()
        main_col.setContentsMargins(0, 0, 0, 0)
        main_col.setSpacing(0)
        self.top_bar = TopBar(self)
        main_col.addWidget(self.top_bar)

        self.stack = QStackedWidget(self)
        library = QWidget()
        lib_row = QHBoxLayout(library)
        lib_row.setContentsMargins(0, 0, 0, 0)
        lib_row.setSpacing(0)
        self.pack_grid = PackGrid(library)
        lib_row.addWidget(self.pack_grid, 1)
        self.detail_panel = DetailPanel(library)
        lib_row.addWidget(self.detail_panel)
        self.stack.addWidget(library)
        self.stack.addWidget(self._make_placeholder_page("Browse Mods", "// mod browser coming soon"))
        self.stack.addWidget(self._make_placeholder_page("Downloads", "// download queue coming soon"))
        self.stack.addWidget(self._make_placeholder_page("Java Manager", "// java runtime manager coming soon"))
        self.stack.addWidget(self._make_placeholder_page("Settings", "// settings coming soon"))
        main_col.addWidget(self.stack, 1)
        outer_row.addLayout(main_col, 1)

    def _make_placeholder_page(self, title: str, sub: str) -> QWidget:
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setAlignment(Qt.AlignCenter)
        label = QLabel(sub, page)
        label.setObjectName("MutedText")
        label.setStyleSheet("font-size:12px; color:#4a7a96;")
        label.setAlignment(Qt.AlignCenter)
        layout.addWidget(label)
        return page

    def _connect_signals(self) -> None:
        self.sidebar.page_requested.connect(self.on_page_requested)
        self.top_bar.new_pack_requested.connect(self.on_new_pack_requested)
        self.top_bar.search_changed.connect(self.pack_grid.set_filter)
        self.pack_grid.new_pack_requested.connect(self.on_new_pack_requested)
        self.pack_grid.run_requested.connect(self.on_run_requested)
        self.pack_grid.install_requested.connect(self.on_install_requested)
        self.pack_grid.delete_requested.connect(self.on_delete_requested)
        self.pack_grid.pack_selected.connect(self.on_pack_selected)
        self.detail_panel.launch_requested.connect(self.on_run_requested)
        self.detail_panel.add_mods_requested.connect(self.on_add_mods_requested)
        self.detail_panel.pack_settings_requested.connect(self.on_pack_settings_requested)
        self.pack_manager.packs_changed.connect(self.on_packs_changed)
        self.backend.progress_update.connect(self.on_progress_update)
        self.backend.operation_done.connect(self.on_operation_done)
        self.backend.operation_error.connect(self.on_operation_error)
        self.backend.status_line.connect(self.on_status_line)

    def on_page_requested(self, page: str) -> None:
        page_index = {"library": 0, "browse": 1, "downloads": 2, "java": 3, "settings": 4}
        self.stack.setCurrentIndex(page_index.get(page, 0))
        titles = {
            "library": "Pack Library",
            "browse": "Browse Mods",
            "downloads": "Downloads",
            "java": "Java Manager",
            "settings": "Settings",
        }
        subs = {
            "library": "your instances",
            "browse": "find and install mods",
            "downloads": "active & recent",
            "java": "runtime management",
            "settings": "preferences",
        }
        self.top_bar.set_title(titles.get(page, "ModLab"), subs.get(page, ""))

    def on_new_pack_requested(self) -> None:
        dialog = NewPackDialog(self)
        if dialog.exec() != NewPackDialog.Accepted:
            return
        data = dialog.result_data
        if not data.name or not data.version:
            return
        self.pack_manager.add_placeholder(
            Pack(
                key=data.name.lower(),
                name=data.name,
                mc_version=data.version,
                loader=data.loader,
                status=PackStatus.INSTALLING,
            )
        )
        self.backend.make_pack(data.name, data.version, loader_name(data.loader).lower())

    def on_run_requested(self, key: str) -> None:
        pack = self.pack_manager.find_pack(key)
        if pack is not None and pack.status not in (PackStatus.READY, PackStatus.NEEDS_UPDATE):
            return
        self.backend.run_pack(key)

    def on_install_requested(self, key: str) -> None:
        self.pack_manager.set_status(key, PackStatus.INSTALLING)
        self.backend.install_pack(key)

    def on_delete_requested(self, key: str) -> None:
        confirm = QMessageBox(self)
        confirm.setWindowTitle("Delete Pack")
        confirm.setText(f"Delete <b>{key}</b>? This cannot be undone.")
        confirm.setStandardButtons(QMessageBox.Yes | QMessageBox.Cancel)
        confirm.setDefaultButton(QMessageBox.Cancel)
        app = QApplication.instance()
        if app is not None:
            confirm.setStyleSheet(app.styleSheet())
        if confirm.exec() != QMessageBox.Yes:
            return
        self.backend.delete_pack(key)

    def on_pack_selected(self, key: str) -> None:
        pack = self.pack_manager.find_pack(key)
        if pack is not None:
            self.detail_panel.show_pack(pack)

    def on_add_mods_requested(self, key: str) -> None:
        pack = self.pack_manager.find_pack(key)
        if pack is None:
            return
        if pack.mods_directory is None:
            QMessageBox.warning(self, "ModLab Error", "This pack does not have a mod directory yet.")
            return

        files, _selected_filter = QFileDialog.getOpenFileNames(
            self,
            "Add Mods",
            "",
            "Minecraft mods (*.jar);;All files (*)",
        )
        if not files:
            return

        pack.mods_directory.mkdir(parents=True, exist_ok=True)
        copied = 0
        for source in files:
            source_path = Path(source)
            if not source_path.exists():
                continue
            destination = pack.mods_directory / source_path.name
            shutil.copy2(source_path, destination)
            copied += 1

        self.pack_manager.reload_from_disk()
        refreshed = self.pack_manager.find_pack(key)
        if refreshed is not None:
            self.pack_grid.set_selected_key(key)
            self.detail_panel.show_pack(refreshed)
        if copied:
            self.pack_manager.save()

    def on_pack_settings_requested(self, key: str) -> None:
        pack = self.pack_manager.find_pack(key)
        if pack is None or pack.directory is None:
            return
        pack.directory.mkdir(parents=True, exist_ok=True)
        QDesktopServices.openUrl(QUrl.fromLocalFile(str(pack.directory)))

    def on_packs_changed(self) -> None:
        self.pack_grid.set_packs(self.pack_manager.packs)

    def on_status_line(self, key: str, _message: str) -> None:
        self.pack_manager.set_status(key, PackStatus.INSTALLING)

    def on_progress_update(self, key: str, percent: int) -> None:
        self.pack_manager.set_progress(key, percent)
        pack = self.pack_manager.find_pack(key)
        if pack is not None:
            pack.install_progress = percent
            pack.status = PackStatus.INSTALLING
            self.pack_grid.update_pack(pack)

    def on_operation_done(self, key: str) -> None:
        self.pack_manager.reload_from_disk()
        pack = self.pack_manager.find_pack(key)
        if pack is not None:
            pack.status = PackStatus.READY
            self.pack_grid.update_pack(pack)
            self.detail_panel.show_pack(pack)
        else:
            self.pack_grid.set_selected_key("")
            self.detail_panel.clear_pack()

    def on_operation_error(self, key: str, message: str) -> None:
        QMessageBox.warning(
            self,
            "ModLab Error",
            f"Operation failed for <b>{key}</b>:<br>{message}",
        )
        self.pack_manager.reload_from_disk()
