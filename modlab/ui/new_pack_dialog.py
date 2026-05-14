from dataclasses import dataclass

from PySide6.QtCore import Qt
from PySide6.QtWidgets import QDialog, QFrame, QHBoxLayout, QLabel, QLineEdit, QPushButton, QVBoxLayout, QWidget

from ..core.pack import Loader


@dataclass
class NewPackData:
    name: str = ""
    version: str = ""
    loader: Loader = Loader.FABRIC


class NewPackDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setObjectName("NewPackDialog")
        self.setWindowTitle("New Instance")
        self.setModal(True)
        self.setFixedSize(370, 340)
        self.setWindowFlags(Qt.Dialog | Qt.FramelessWindowHint)
        self.setAttribute(Qt.WA_TranslucentBackground)
        self.loader = Loader.FABRIC
        self.result_data = NewPackData()
        self.loader_tiles: dict[Loader, QFrame] = {}
        self._build_layout()

    def _build_layout(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        header = QWidget(self)
        header.setFixedHeight(52)
        header.setStyleSheet(
            "background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
            "stop:0 rgba(56,217,245,0.06),stop:1 transparent);"
            "border-bottom: 1px solid rgba(56,217,245,0.1);"
            "border-top-left-radius: 12px; border-top-right-radius: 12px;"
        )
        header_layout = QHBoxLayout(header)
        header_layout.setContentsMargins(22, 0, 14, 0)
        title = QLabel("New <span style='color:#38d9f5'>Instance</span>", header)
        title.setStyleSheet("font-family:'Exo 2'; font-weight:700; font-size:14px; color:#d8f0ff;")
        close = QPushButton("x", header)
        close.setFixedSize(26, 22)
        close.setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
            "stop:0 rgba(255,255,255,0.07),stop:1 rgba(0,0,0,0.1));"
            "border-top:1px solid rgba(255,255,255,0.1);"
            "border-bottom:1px solid rgba(0,0,0,0.3);"
            "border-radius:4px; color:#7ab8d4; font-size:11px; }"
            "QPushButton:hover { color:#d8f0ff; }"
        )
        close.clicked.connect(self.reject)
        header_layout.addWidget(title)
        header_layout.addStretch()
        header_layout.addWidget(close)
        root.addWidget(header)

        body = QWidget(self)
        body.setStyleSheet("background: transparent;")
        body_layout = QVBoxLayout(body)
        body_layout.setContentsMargins(22, 18, 22, 18)
        body_layout.setSpacing(13)

        self.name_edit = self._add_field(body, body_layout, "PACK NAME", "e.g. My Survival World")
        self.version_edit = self._add_field(body, body_layout, "MINECRAFT VERSION", "e.g. 1.21.4")

        loader_block = QWidget(body)
        loader_layout = QVBoxLayout(loader_block)
        loader_layout.setContentsMargins(0, 0, 0, 0)
        loader_layout.setSpacing(5)
        loader_layout.addWidget(self._field_label("MOD LOADER", loader_block))
        row = QHBoxLayout()
        row.setSpacing(7)
        for abbr, label, loader in (
            ("FBR", "Fabric", Loader.FABRIC),
            ("FRG", "Forge", Loader.FORGE),
            ("VNL", "Vanilla", Loader.VANILLA),
        ):
            tile = self._make_loader_tile(loader_block, abbr, label, loader)
            self.loader_tiles[loader] = tile
            row.addWidget(tile)
        loader_layout.addLayout(row)
        body_layout.addWidget(loader_block)
        root.addWidget(body, 1)

        footer = QWidget(self)
        footer.setFixedHeight(52)
        footer.setStyleSheet(
            "background: transparent; border-top: 1px solid rgba(56,217,245,0.1);"
            "border-bottom-left-radius: 12px; border-bottom-right-radius: 12px;"
        )
        footer_layout = QHBoxLayout(footer)
        footer_layout.setContentsMargins(22, 0, 22, 0)
        dots = QHBoxLayout()
        dots.setSpacing(5)
        for i in range(3):
            dot = QLabel(footer)
            dot.setFixedSize(6, 6)
            dot.setStyleSheet(
                "background:#38d9f5; border-radius:3px;"
                if i == 0
                else "background:rgba(56,217,245,0.15); border:1px solid rgba(56,217,245,0.12); border-radius:3px;"
            )
            dots.addWidget(dot)
        footer_layout.addLayout(dots)
        footer_layout.addStretch()

        cancel = QPushButton("Cancel", footer)
        cancel.setObjectName("BtnSecondary")
        cancel.clicked.connect(self.reject)
        self.create_button = QPushButton("Create", footer)
        self.create_button.setObjectName("BtnPrimary")
        self.create_button.clicked.connect(self._accept_result)
        footer_layout.addWidget(cancel)
        footer_layout.addSpacing(7)
        footer_layout.addWidget(self.create_button)
        root.addWidget(footer)

        self.name_edit.textChanged.connect(self._validate)
        self.version_edit.textChanged.connect(self._validate)
        self.select_loader(Loader.FABRIC)
        self._validate()

    def _field_label(self, text: str, parent: QWidget) -> QLabel:
        label = QLabel(f"<span style='color:#00e5cc'>&gt;</span> {text}", parent)
        label.setStyleSheet("font-size: 9px; color: #7ab8d4;")
        return label

    def _add_field(self, body: QWidget, body_layout: QVBoxLayout, label: str, placeholder: str) -> QLineEdit:
        block = QWidget(body)
        layout = QVBoxLayout(block)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(5)
        layout.addWidget(self._field_label(label, block))
        edit = QLineEdit(block)
        edit.setObjectName("FieldInput")
        edit.setPlaceholderText(placeholder)
        layout.addWidget(edit)
        body_layout.addWidget(block)
        return edit

    def _make_loader_tile(self, parent: QWidget, abbr: str, label: str, loader: Loader) -> QFrame:
        tile = QFrame(parent)
        tile.setFixedHeight(56)
        tile.setCursor(Qt.PointingHandCursor)
        layout = QVBoxLayout(tile)
        layout.setContentsMargins(0, 6, 0, 6)
        layout.setAlignment(Qt.AlignCenter)
        abbr_label = QLabel(abbr, tile)
        abbr_label.setObjectName("LoaderAbbr")
        abbr_label.setAlignment(Qt.AlignCenter)
        name_label = QLabel(label, tile)
        name_label.setAlignment(Qt.AlignCenter)
        name_label.setStyleSheet("font-size:8px; color:#4a7a96; text-transform:uppercase;")
        layout.addWidget(abbr_label)
        layout.addWidget(name_label)
        tile.mousePressEvent = lambda _event, l=loader: self.select_loader(l)
        return tile

    def select_loader(self, loader: Loader) -> None:
        self.loader = loader
        for candidate, tile in self.loader_tiles.items():
            selected = candidate == loader
            tile.setStyleSheet(
                "QFrame { background: qlineargradient(x1:0,y1:0,x2:0.5,y2:1,"
                + (
                    "stop:0 rgba(56,217,245,0.12),stop:1 rgba(20,80,110,0.15));"
                    "border-top:1px solid rgba(56,217,245,0.5);"
                    "border-left:1px solid rgba(56,217,245,0.22);"
                    if selected
                    else "stop:0 rgba(10,30,52,0.7),stop:1 rgba(4,14,26,0.8));"
                    "border-top:1px solid rgba(255,255,255,0.08);"
                    "border-left:1px solid rgba(255,255,255,0.04);"
                )
                + "border-right:1px solid rgba(0,0,0,0.25);"
                "border-bottom:1px solid rgba(0,0,0,0.45);"
                "border-radius:6px; }"
            )
            labels = tile.findChildren(QLabel)
            if labels:
                labels[0].setStyleSheet(
                    "font-family:'Exo 2'; font-weight:900; font-size:12px;"
                    f"color:{'#38d9f5' if selected else '#7ab8d4'};"
                )

    def _validate(self) -> None:
        ok = bool(self.name_edit.text().strip()) and bool(self.version_edit.text().strip())
        self.create_button.setEnabled(ok)
        self.create_button.setObjectName("BtnPrimary" if ok else "BtnSecondary")
        self.create_button.style().unpolish(self.create_button)
        self.create_button.style().polish(self.create_button)

    def _accept_result(self) -> None:
        self.result_data = NewPackData(
            name=self.name_edit.text().strip(),
            version=self.version_edit.text().strip(),
            loader=self.loader,
        )
        self.accept()
