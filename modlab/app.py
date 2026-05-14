import os
import sys

from PySide6.QtGui import QFontDatabase
from PySide6.QtWidgets import QApplication

from .ui.aero_style import AeroStyle
from .ui.main_window import MainWindow


def main() -> int:
    app = QApplication(sys.argv)
    app.setApplicationName("ModLab")
    app.setApplicationVersion("0.1.0")

    for font in (
        ":/fonts/SpaceMono-Regular.ttf",
        ":/fonts/SpaceMono-Bold.ttf",
        ":/fonts/Exo2-Bold.ttf",
        ":/fonts/Exo2-Black.ttf",
    ):
        QFontDatabase.addApplicationFont(font)

    app.setStyleSheet(AeroStyle.sheet())

    window = MainWindow(
        python_path=os.environ.get("MODLAB_PYTHON", "python3"),
        script_path=os.environ.get(
            "MODLAB_SCRIPT",
            os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "main.py")),
        ),
    )
    window.show()
    return app.exec()
