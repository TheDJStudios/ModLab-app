from PySide6.QtCore import Qt, Signal
from PySide6.QtWidgets import QFrame, QHBoxLayout, QLabel, QPushButton, QVBoxLayout, QWidget


class Sidebar(QWidget):
    page_requested = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setObjectName("Sidebar")
        self.setFixedWidth(210)
        self.nav_buttons: list[QPushButton] = []
        self.active_page = ""
        self._build()
        self.set_active_page("library")

    def _build(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        logo_block = QWidget(self)
        logo_block.setFixedHeight(62)
        logo_block.setStyleSheet(
            "background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
            "stop:0 rgba(56,217,245,0.05),stop:1 transparent);"
            "border-bottom: 1px solid rgba(56,217,245,0.1);"
        )
        logo_layout = QVBoxLayout(logo_block)
        logo_layout.setContentsMargins(18, 14, 18, 10)
        logo_layout.setSpacing(2)
        logo_text = QLabel("MOD<span style='color:#38d9f5'>LAB</span>", logo_block)
        logo_text.setStyleSheet(
            "font-family: 'Exo 2'; font-size: 18px; font-weight: 900;"
            "color: #d8f0ff; letter-spacing: 0px;"
        )
        logo_ver = QLabel("// launcher v0.1", logo_block)
        logo_ver.setStyleSheet("font-size: 9px; color: #4a7a96;")
        logo_layout.addWidget(logo_text)
        logo_layout.addWidget(logo_ver)
        root.addWidget(logo_block)

        nav_area = QWidget(self)
        nav_layout = QVBoxLayout(nav_area)
        nav_layout.setContentsMargins(10, 14, 10, 10)
        nav_layout.setSpacing(1)

        def add_group_label(text: str) -> None:
            label = QLabel("// " + text, nav_area)
            label.setStyleSheet(
                "font-size: 9px; color: #4a7a96; padding: 8px 8px 4px 8px;"
            )
            nav_layout.addWidget(label)

        def add_nav_button(label: str, page: str) -> None:
            button = QPushButton(label, nav_area)
            button.setObjectName("NavButton")
            button.setCursor(Qt.PointingHandCursor)
            button.clicked.connect(lambda _checked=False, p=page: self._request_page(p))
            self.nav_buttons.append(button)
            nav_layout.addWidget(button)

        add_group_label("Main")
        add_nav_button("Library", "library")
        add_nav_button("Browse Mods", "browse")
        add_nav_button("Downloads", "downloads")

        separator = QFrame(nav_area)
        separator.setFrameShape(QFrame.HLine)
        separator.setStyleSheet("color: rgba(56,217,245,0.1); margin: 6px 2px;")
        nav_layout.addWidget(separator)

        add_group_label("System")
        add_nav_button("Java Manager", "java")
        add_nav_button("Settings", "settings")
        nav_layout.addStretch()
        root.addWidget(nav_area, 1)

        footer = QWidget(self)
        footer.setFixedHeight(38)
        footer.setStyleSheet("border-top: 1px solid rgba(56,217,245,0.1);")
        foot_layout = QHBoxLayout(footer)
        foot_layout.setContentsMargins(16, 0, 16, 0)
        ver = QLabel("modlab-pyshit-client v0.1.0-alpha", footer)
        ver.setStyleSheet("font-size: 9px; color: #4a7a96;")
        foot_layout.addWidget(ver)
        root.addWidget(footer)

    def _request_page(self, page: str) -> None:
        self.set_active_page(page)
        self.page_requested.emit(page)

    def set_active_page(self, page: str) -> None:
        self.active_page = page
        label_to_page = {
            "Library": "library",
            "Browse Mods": "browse",
            "Downloads": "downloads",
            "Java Manager": "java",
            "Settings": "settings",
        }
        for button in self.nav_buttons:
            button.setProperty("active", label_to_page.get(button.text()) == page)
            button.style().unpolish(button)
            button.style().polish(button)
