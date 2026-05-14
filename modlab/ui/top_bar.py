from PySide6.QtCore import Signal
from PySide6.QtWidgets import QHBoxLayout, QLabel, QLineEdit, QPushButton, QWidget


class TopBar(QWidget):
    new_pack_requested = Signal()
    search_changed = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setObjectName("TopBar")
        self.setFixedHeight(50)

        layout = QHBoxLayout(self)
        layout.setContentsMargins(20, 0, 20, 0)
        layout.setSpacing(10)

        self.title_label = QLabel(self)
        self.title_label.setStyleSheet(
            "font-family: 'Exo 2'; font-weight: 700; font-size: 13px;"
            "color: #d8f0ff;"
        )
        layout.addWidget(self.title_label, 1)

        self.search = QLineEdit(self)
        self.search.setObjectName("SearchBar")
        self.search.setPlaceholderText("Search packs...")
        self.search.setFixedWidth(190)
        self.search.textChanged.connect(self.search_changed.emit)
        layout.addWidget(self.search)

        self.new_button = QPushButton("+ New Pack", self)
        self.new_button.setObjectName("BtnPrimary")
        self.new_button.clicked.connect(self.new_pack_requested.emit)
        layout.addWidget(self.new_button)

        self.set_title("Pack Library", "your instances")

    def set_title(self, title: str, sub: str = "") -> None:
        html = f"<span style='color:#d8f0ff'>{title}</span>"
        if sub:
            html += (
                " <span style='color:#4a7a96; font-size:10px;"
                f" font-family:Space Mono; font-weight:400;'>// {sub}</span>"
            )
        self.title_label.setText(html)
