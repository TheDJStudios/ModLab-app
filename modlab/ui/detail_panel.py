from PySide6.QtCore import Qt, Signal
from PySide6.QtWidgets import QFrame, QHBoxLayout, QLabel, QPushButton, QScrollArea, QVBoxLayout, QWidget

from ..core.pack import Pack, PackStatus, loader_name, status_label


def make_info_row(parent: QWidget, key: str, value: str, highlight: bool = False) -> QWidget:
    row = QWidget(parent)
    row.setStyleSheet("border-bottom: 1px solid rgba(56,217,245,0.06);")
    layout = QHBoxLayout(row)
    layout.setContentsMargins(0, 6, 0, 6)
    key_label = QLabel(key, row)
    key_label.setObjectName("DetailKey")
    value_label = QLabel(value, row)
    value_label.setObjectName("DetailValueHi" if highlight else "DetailValue")
    layout.addWidget(key_label)
    layout.addStretch()
    layout.addWidget(value_label)
    return row


class DetailPanel(QWidget):
    launch_requested = Signal(str)
    add_mods_requested = Signal(str)
    pack_settings_requested = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setObjectName("DetailPanel")
        self.setFixedWidth(265)
        self.current_key = ""
        self._build_layout()

    def _build_layout(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        header = QWidget(self)
        header.setFixedHeight(110)
        header.setStyleSheet(
            "background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
            "stop:0 rgba(56,217,245,0.04),stop:1 transparent);"
            "border-bottom: 1px solid rgba(56,217,245,0.1);"
        )
        header_layout = QVBoxLayout(header)
        header_layout.setContentsMargins(18, 16, 18, 12)
        header_layout.setSpacing(3)
        self.ident_label = QLabel(header)
        self.ident_label.setFixedSize(50, 50)
        self.ident_label.setAlignment(Qt.AlignCenter)
        self.ident_label.setStyleSheet(
            "font-family:'Exo 2'; font-weight:900; font-size:14px;"
            "background: qlineargradient(x1:0,y1:0,x2:0.5,y2:1,"
            "stop:0 rgba(0,200,180,0.25), stop:1 rgba(0,130,120,0.3));"
            "border-radius:9px; color:#00e5cc;"
        )
        self.name_label = QLabel(header)
        self.name_label.setObjectName("PackName")
        self.name_label.setStyleSheet("font-size: 15px; font-weight: 900;")
        self.sub_label = QLabel(header)
        self.sub_label.setObjectName("MutedText")
        header_layout.addWidget(self.ident_label)
        header_layout.addWidget(self.name_label)
        header_layout.addWidget(self.sub_label)
        root.addWidget(header)

        scroll = QScrollArea(self)
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QFrame.NoFrame)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        body = QWidget()
        body_layout = QVBoxLayout(body)
        body_layout.setContentsMargins(18, 14, 18, 14)
        body_layout.setSpacing(0)
        info_label = QLabel("// Instance Info", body)
        info_label.setObjectName("SectionLabel")
        info_label.setStyleSheet("font-size: 9px; color: rgba(56,217,245,0.45); margin-bottom: 7px;")
        body_layout.addWidget(info_label)

        self.info_widget = QWidget(body)
        self.info_layout = QVBoxLayout(self.info_widget)
        self.info_layout.setContentsMargins(0, 0, 0, 0)
        self.info_layout.setSpacing(0)
        body_layout.addWidget(self.info_widget)
        body_layout.addSpacing(14)

        self.mod_header = QLabel("// Mods (0)", body)
        self.mod_header.setObjectName("SectionLabel")
        self.mod_header.setStyleSheet("font-size: 9px; color: rgba(56,217,245,0.45); margin-bottom: 7px;")
        body_layout.addWidget(self.mod_header)
        self.mod_widget = QWidget(body)
        self.mod_layout = QVBoxLayout(self.mod_widget)
        self.mod_layout.setContentsMargins(0, 0, 0, 0)
        self.mod_layout.setSpacing(4)
        body_layout.addWidget(self.mod_widget)
        body_layout.addStretch()
        scroll.setWidget(body)
        root.addWidget(scroll, 1)

        footer = QWidget(self)
        footer.setStyleSheet("border-top: 1px solid rgba(56,217,245,0.1);")
        footer_layout = QVBoxLayout(footer)
        footer_layout.setContentsMargins(14, 12, 14, 12)
        footer_layout.setSpacing(6)
        self.launch_button = QPushButton("Launch", footer)
        self.add_mods_button = QPushButton("+ Add Mods", footer)
        self.settings_button = QPushButton("Pack Settings", footer)
        self.launch_button.setObjectName("BtnRun")
        self.add_mods_button.setObjectName("BtnPrimary")
        self.settings_button.setObjectName("BtnSecondary")
        footer_layout.addWidget(self.launch_button)
        footer_layout.addWidget(self.add_mods_button)
        footer_layout.addWidget(self.settings_button)
        root.addWidget(footer)

        self.launch_button.clicked.connect(lambda: self.launch_requested.emit(self.current_key))
        self.add_mods_button.clicked.connect(lambda: self.add_mods_requested.emit(self.current_key))
        self.settings_button.clicked.connect(lambda: self.pack_settings_requested.emit(self.current_key))
        self.clear_pack()

    def _clear_layout(self, layout: QVBoxLayout) -> None:
        while layout.count():
            item = layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

    def _rebuild_info_rows(self, pack: Pack) -> None:
        self._clear_layout(self.info_layout)
        self.info_layout.addWidget(make_info_row(self.info_widget, "Loader", loader_name(pack.loader), True))
        self.info_layout.addWidget(make_info_row(self.info_widget, "MC Version", pack.mc_version, False))
        self.info_layout.addWidget(make_info_row(self.info_widget, "Java", "Bundled (auto)", False))
        self.info_layout.addWidget(make_info_row(self.info_widget, "Mods", str(pack.mod_count), False))
        self.info_layout.addWidget(make_info_row(self.info_widget, "Status", status_label(pack.status), True))

    def show_pack(self, pack: Pack) -> None:
        self.current_key = pack.key
        self.ident_label.setText(pack.abbr())
        self.name_label.setText(pack.name)
        self.sub_label.setText(f"{pack.mc_version} · {loader_name(pack.loader)}")
        self._rebuild_info_rows(pack)
        self._clear_layout(self.mod_layout)
        self.mod_header.setText(f"// Mods ({pack.mod_count})")

        mods = pack.mods or []
        if not mods:
            empty = QLabel("No mods installed.", self.mod_widget)
            empty.setObjectName("MutedText")
            empty.setStyleSheet("font-size: 10px; padding: 6px 0;")
            self.mod_layout.addWidget(empty)
        else:
            for mod in mods[:8]:
                row = QWidget(self.mod_widget)
                row.setStyleSheet(
                    "background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                    "stop:0 rgba(10,30,52,0.6),stop:1 rgba(6,18,34,0.4));"
                    "border-top: 1px solid rgba(56,217,245,0.09);"
                    "border-bottom: 1px solid rgba(0,0,0,0.35);"
                    "border-radius: 4px;"
                )
                layout = QHBoxLayout(row)
                layout.setContentsMargins(9, 5, 9, 5)
                name = QLabel(mod.name, row)
                name.setObjectName("DetailValue")
                ver = QLabel(mod.size_label, row)
                ver.setObjectName("MutedText")
                ver.setStyleSheet("font-size: 9px;")
                layout.addWidget(name)
                layout.addStretch()
                layout.addWidget(ver)
                self.mod_layout.addWidget(row)
            if len(mods) > 8:
                more = QLabel(f"+ {len(mods) - 8} more", self.mod_widget)
                more.setObjectName("MutedText")
                more.setAlignment(Qt.AlignCenter)
                more.setStyleSheet("font-size: 9px; padding: 4px 0;")
                self.mod_layout.addWidget(more)

        self.launch_button.setEnabled(pack.status in (PackStatus.READY, PackStatus.NEEDS_UPDATE))

    def clear_pack(self) -> None:
        self.current_key = ""
        self.ident_label.setText("-")
        self.name_label.setText("No pack selected")
        self.sub_label.setText("")
        self.launch_button.setEnabled(False)
