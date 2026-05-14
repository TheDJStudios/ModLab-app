from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QColor, QLinearGradient, QMouseEvent, QPainter
from PySide6.QtWidgets import QFrame, QHBoxLayout, QLabel, QProgressBar, QPushButton, QVBoxLayout, QWidget

from ..core.pack import Pack, PackStatus, loader_name, status_label


class PackCard(QFrame):
    run_requested = Signal(str)
    install_requested = Signal(str)
    delete_requested = Signal(str)
    card_clicked = Signal(str)

    def __init__(self, pack: Pack, parent=None):
        super().__init__(parent)
        self.setObjectName("PackCard")
        self.setFixedWidth(240)
        self.setCursor(Qt.PointingHandCursor)
        self.key = pack.key
        self._build_layout(pack)

    def _build_layout(self, pack: Pack) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(15, 14, 15, 14)
        root.setSpacing(0)

        head_row = QHBoxLayout()
        head_row.setContentsMargins(0, 0, 0, 10)
        self.ident_label = QLabel(pack.abbr(), self)
        self.ident_label.setFixedSize(40, 40)
        self.ident_label.setAlignment(Qt.AlignCenter)
        self.ident_label.setStyleSheet(
            "font-family:'Exo 2'; font-weight:900; font-size:12px;"
            "background: qlineargradient(x1:0,y1:0,x2:0.5,y2:1,"
            "stop:0 rgba(0,200,180,0.25), stop:1 rgba(0,130,120,0.3));"
            "border-radius:8px; color:#00e5cc;"
        )
        self.status_label = QLabel(status_label(pack.status), self)
        self.status_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        head_row.addWidget(self.ident_label)
        head_row.addStretch()
        head_row.addWidget(self.status_label)
        root.addLayout(head_row)

        self.name_label = QLabel(pack.name, self)
        self.name_label.setObjectName("PackName")
        root.addWidget(self.name_label)
        root.addSpacing(4)

        self.tag_row = QWidget(self)
        tag_layout = QHBoxLayout(self.tag_row)
        tag_layout.setContentsMargins(0, 0, 0, 0)
        tag_layout.setSpacing(5)
        for text in [loader_name(pack.loader), pack.mc_version]:
            tag_layout.addWidget(self._make_tag(text))
        if pack.mod_count > 0:
            tag_layout.addWidget(self._make_tag(f"{pack.mod_count} mods"))
        tag_layout.addStretch()
        root.addWidget(self.tag_row)
        root.addSpacing(12)

        sep = QFrame(self)
        sep.setFrameShape(QFrame.HLine)
        sep.setStyleSheet("color: rgba(56,217,245,0.12);")
        sep.setFixedHeight(1)
        root.addWidget(sep)
        root.addSpacing(11)

        button_row = QHBoxLayout()
        button_row.setSpacing(6)
        self.run_button = QPushButton("Run", self)
        self.install_button = QPushButton("Install", self)
        self.delete_button = QPushButton("Delete", self)
        self.run_button.setObjectName("BtnRun")
        self.install_button.setObjectName("BtnSecondary")
        self.delete_button.setObjectName("BtnDanger")
        button_row.addWidget(self.run_button)
        button_row.addWidget(self.install_button)
        button_row.addWidget(self.delete_button)
        root.addLayout(button_row)

        self.progress = QProgressBar(self)
        self.progress.setRange(0, 100)
        self.progress.setValue(pack.install_progress)
        self.progress.setTextVisible(False)
        self.progress.setFixedHeight(3)
        self.progress_label = QLabel(self)
        self.progress_label.setObjectName("MutedText")
        self.progress_label.setStyleSheet("font-size: 9px;")
        root.addSpacing(9)
        root.addWidget(self.progress)
        root.addSpacing(3)
        root.addWidget(self.progress_label)

        self.apply_status(pack.status)
        self.run_button.clicked.connect(lambda: self.run_requested.emit(self.key))
        self.install_button.clicked.connect(lambda: self.install_requested.emit(self.key))
        self.delete_button.clicked.connect(lambda: self.delete_requested.emit(self.key))

    def _make_tag(self, text: str) -> QLabel:
        tag = QLabel(text, self.tag_row)
        tag.setObjectName("TagPill")
        return tag

    def apply_status(self, status: PackStatus) -> None:
        installing = status == PackStatus.INSTALLING
        self.progress.setVisible(installing)
        self.progress_label.setVisible(installing)
        self.run_button.setEnabled(status in (PackStatus.READY, PackStatus.NEEDS_UPDATE))
        if status == PackStatus.READY:
            self.status_label.setObjectName("StatusReady")
            self.install_button.setText("Install")
        elif status == PackStatus.INSTALLING:
            self.status_label.setObjectName("StatusBusy")
            self.install_button.setText("Reinstall")
        elif status == PackStatus.NEEDS_UPDATE:
            self.status_label.setObjectName("StatusWarn")
            self.install_button.setText("Update")
        else:
            self.status_label.setObjectName("StatusWarn")
            self.install_button.setText("Install")
        self.status_label.style().unpolish(self.status_label)
        self.status_label.style().polish(self.status_label)

    def update_pack(self, pack: Pack) -> None:
        self.name_label.setText(pack.name)
        self.ident_label.setText(pack.abbr())
        self.status_label.setText(status_label(pack.status))
        self.progress.setValue(pack.install_progress)
        self.progress_label.setText(f"Downloading... {pack.install_progress}%")
        self.apply_status(pack.status)
        self.update()

    def set_selected(self, selected: bool) -> None:
        self.setProperty("selected", selected)
        self.style().unpolish(self)
        self.style().polish(self)
        self.update()

    def mousePressEvent(self, event: QMouseEvent) -> None:
        super().mousePressEvent(event)
        self.card_clicked.emit(self.key)

    def paintEvent(self, event) -> None:
        super().paintEvent(event)
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        shine = QLinearGradient(0, 0, self.width(), 0)
        shine.setColorAt(0.0, Qt.transparent)
        shine.setColorAt(0.5, QColor(56, 217, 245, 30))
        shine.setColorAt(1.0, Qt.transparent)
        painter.fillRect(0, 0, self.width(), 1, shine)
        glow = QLinearGradient(0, 0, 0, 40)
        glow.setColorAt(0.0, QColor(56, 217, 245, 10))
        glow.setColorAt(1.0, Qt.transparent)
        painter.fillRect(0, 0, self.width(), 40, glow)
