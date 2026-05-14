from PySide6.QtCore import Qt, Signal
from PySide6.QtWidgets import QFrame, QGridLayout, QPushButton, QScrollArea, QVBoxLayout, QWidget

from ..core.pack import Pack
from .pack_card import PackCard


class PackGrid(QWidget):
    run_requested = Signal(str)
    install_requested = Signal(str)
    delete_requested = Signal(str)
    pack_selected = Signal(str)
    new_pack_requested = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.cards: dict[str, PackCard] = {}
        self.selected = ""

        root = QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        self.scroll = QScrollArea(self)
        self.scroll.setWidgetResizable(True)
        self.scroll.setFrameShape(QFrame.NoFrame)
        self.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

        self.inner = QWidget()
        self.grid = QGridLayout(self.inner)
        self.grid.setSpacing(12)
        self.grid.setContentsMargins(22, 20, 22, 20)
        self.grid.setAlignment(Qt.AlignTop | Qt.AlignLeft)
        self.scroll.setWidget(self.inner)
        root.addWidget(self.scroll)

    def set_packs(self, packs: list[Pack]) -> None:
        self._rebuild(packs)

    def _rebuild(self, packs: list[Pack]) -> None:
        for card in self.cards.values():
            card.deleteLater()
        self.cards.clear()

        while self.grid.count():
            item = self.grid.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

        cols = 3
        pos = 0
        for pack in packs:
            card = PackCard(pack, self.inner)
            self.cards[pack.key] = card
            card.run_requested.connect(self.run_requested.emit)
            card.install_requested.connect(self.install_requested.emit)
            card.delete_requested.connect(self.delete_requested.emit)
            card.card_clicked.connect(self._select_and_emit)
            if pack.key == self.selected:
                card.set_selected(True)
            self.grid.addWidget(card, pos // cols, pos % cols)
            pos += 1

        new_button = QPushButton("+ New Instance", self.inner)
        new_button.setObjectName("BtnPrimary")
        new_button.setFixedSize(240, 152)
        new_button.setCursor(Qt.PointingHandCursor)
        new_button.setStyleSheet(
            "QPushButton#BtnPrimary {"
            "  background: rgba(6,18,34,0.5);"
            "  border: 1px dashed rgba(56,217,245,0.2);"
            "  border-radius: 9px;"
            "  color: #4a7a96;"
            "  font-size: 11px;"
            "}"
            "QPushButton#BtnPrimary:hover {"
            "  background: rgba(10,30,52,0.7);"
            "  border-color: rgba(56,217,245,0.38);"
            "  color: #38d9f5;"
            "}"
        )
        new_button.clicked.connect(self.new_pack_requested.emit)
        self.grid.addWidget(new_button, pos // cols, pos % cols)

    def update_pack(self, pack: Pack) -> None:
        card = self.cards.get(pack.key)
        if card is not None:
            card.update_pack(pack)

    def set_selected_key(self, key: str) -> None:
        if self.selected and self.selected in self.cards:
            self.cards[self.selected].set_selected(False)
        self.selected = key
        if key in self.cards:
            self.cards[key].set_selected(True)

    def _select_and_emit(self, key: str) -> None:
        self.set_selected_key(key)
        self.pack_selected.emit(key)
