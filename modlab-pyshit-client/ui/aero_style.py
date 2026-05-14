class AeroStyle:
    SKY_DEEP = "#04111f"
    SKY_MID = "#071d35"
    SKY_GLOW = "#0a2a4a"
    CYAN = "#38d9f5"
    BLUE = "#5bc8f5"
    TEAL = "#00e5cc"
    WHITE = "#d8f0ff"
    DIM = "#7ab8d4"
    MUTED = "#4a7a96"
    TEXT = "#cce8f8"
    GLASS_BG = "rgba(10,42,74,0.55)"
    GLASS_BDR = "rgba(56,217,245,0.18)"

    @staticmethod
    def sheet() -> str:
        return """
QWidget {
    background-color: #04111f;
    color: #cce8f8;
    font-family: "Space Mono";
    font-size: 12px;
    selection-background-color: rgba(56,217,245,0.25);
    selection-color: #d8f0ff;
}
QScrollBar:vertical { background: transparent; width: 5px; margin: 0; }
QScrollBar::handle:vertical {
    background: rgba(56,217,245,0.22);
    border-radius: 2px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover { background: rgba(56,217,245,0.38); }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }
QWidget#Sidebar {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(12,34,58,0.97), stop:1 rgba(6,20,36,0.98));
    border-right: 1px solid rgba(56,217,245,0.14);
}
QPushButton#NavButton {
    background: transparent;
    border: 1px solid transparent;
    border-radius: 5px;
    color: #7ab8d4;
    font-family: "Space Mono";
    font-size: 11px;
    text-align: left;
    padding: 7px 10px 7px 10px;
}
QPushButton#NavButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 rgba(56,217,245,0.07), stop:1 rgba(56,217,245,0.03));
    color: #5bc8f5;
    border-color: rgba(56,217,245,0.1);
}
QPushButton#NavButton[active="true"] {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 rgba(56,217,245,0.13), stop:1 rgba(56,217,245,0.05));
    color: #38d9f5;
    border-color: rgba(56,217,245,0.22);
}
QWidget#TopBar {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(10,34,56,0.92), stop:1 rgba(6,22,38,0.95));
    border-bottom: 1px solid rgba(56,217,245,0.15);
}
QLineEdit#SearchBar, QLineEdit#FieldInput {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(4,14,26,0.9), stop:1 rgba(6,18,32,0.85));
    border-top: 1px solid rgba(255,255,255,0.05);
    border-left: 1px solid rgba(56,217,245,0.1);
    border-right: 1px solid rgba(56,217,245,0.1);
    border-bottom: 1px solid rgba(0,0,0,0.5);
    border-radius: 5px;
    color: #cce8f8;
    font-family: "Space Mono";
    font-size: 11px;
    padding: 5px 10px;
}
QLineEdit#SearchBar:focus, QLineEdit#FieldInput:focus { border-color: rgba(56,217,245,0.35); }
QPushButton#BtnPrimary {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(56,217,245,0.22), stop:0.6 rgba(30,140,170,0.18), stop:1 rgba(20,100,130,0.22));
    border-top: 1px solid rgba(56,217,245,0.55);
    border-left: 1px solid rgba(56,217,245,0.25);
    border-right: 1px solid rgba(56,217,245,0.15);
    border-bottom: 1px solid rgba(0,0,0,0.45);
    border-radius: 5px;
    color: #38d9f5;
    font-family: "Space Mono";
    font-size: 10px;
    padding: 6px 14px;
}
QPushButton#BtnPrimary:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(56,217,245,0.3), stop:1 rgba(20,100,130,0.3));
}
QPushButton#BtnPrimary:pressed {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(20,100,130,0.28), stop:1 rgba(56,217,245,0.18));
    border-top-color: rgba(56,217,245,0.3);
    padding-top: 7px;
    padding-bottom: 5px;
}
QPushButton#BtnRun {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(0,229,204,0.2), stop:0.6 rgba(0,170,155,0.16), stop:1 rgba(0,130,120,0.2));
    border-top: 1px solid rgba(0,229,204,0.5);
    border-left: 1px solid rgba(0,229,204,0.22);
    border-right: 1px solid rgba(0,229,204,0.12);
    border-bottom: 1px solid rgba(0,0,0,0.4);
    border-radius: 5px;
    color: #00e5cc;
    font-family: "Space Mono";
    font-size: 10px;
    padding: 6px 14px;
}
QPushButton#BtnRun:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(0,229,204,0.28), stop:1 rgba(0,130,120,0.28));
}
QPushButton#BtnRun:pressed {
    padding-top: 7px; padding-bottom: 5px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(0,100,90,0.28), stop:1 rgba(0,229,204,0.15));
}
QPushButton#BtnRun:disabled { color: rgba(0,229,204,0.35); }
QPushButton#BtnSecondary {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(255,255,255,0.07), stop:0.5 rgba(255,255,255,0.03), stop:1 rgba(0,0,0,0.08));
    border-top: 1px solid rgba(255,255,255,0.14);
    border-left: 1px solid rgba(255,255,255,0.07);
    border-right: 1px solid rgba(255,255,255,0.04);
    border-bottom: 1px solid rgba(0,0,0,0.4);
    border-radius: 5px;
    color: #7ab8d4;
    font-family: "Space Mono";
    font-size: 10px;
    padding: 6px 14px;
}
QPushButton#BtnSecondary:hover {
    color: #5bc8f5;
    border-top-color: rgba(56,217,245,0.3);
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(255,255,255,0.1), stop:1 rgba(0,0,0,0.05));
}
QPushButton#BtnSecondary:pressed { padding-top: 7px; padding-bottom: 5px; }
QPushButton#BtnSecondary:disabled { color: rgba(122,184,212,0.35); }
QPushButton#BtnDanger {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(220,60,60,0.18), stop:0.6 rgba(160,40,40,0.14), stop:1 rgba(120,30,30,0.2));
    border-top: 1px solid rgba(255,100,100,0.4);
    border-left: 1px solid rgba(255,100,100,0.18);
    border-right: 1px solid rgba(255,100,100,0.1);
    border-bottom: 1px solid rgba(0,0,0,0.4);
    border-radius: 5px;
    color: #ff8888;
    font-family: "Space Mono";
    font-size: 10px;
    padding: 6px 14px;
}
QPushButton#BtnDanger:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(220,60,60,0.28), stop:1 rgba(120,30,30,0.28));
}
QPushButton#BtnDanger:pressed { padding-top: 7px; padding-bottom: 5px; }
QFrame#PackCard {
    background: qlineargradient(x1:0,y1:0,x2:0.3,y2:1,
        stop:0 rgba(12,36,60,0.85), stop:1 rgba(6,20,38,0.92));
    border-top: 1px solid rgba(56,217,245,0.22);
    border-left: 1px solid rgba(56,217,245,0.12);
    border-right: 1px solid rgba(20,60,90,0.6);
    border-bottom: 1px solid rgba(0,0,0,0.55);
    border-radius: 9px;
}
QFrame#PackCard:hover {
    border-top-color: rgba(56,217,245,0.4);
    border-left-color: rgba(56,217,245,0.22);
}
QFrame#PackCard[selected="true"] {
    border-top-color: rgba(56,217,245,0.55);
    border-left-color: rgba(56,217,245,0.3);
    background: qlineargradient(x1:0,y1:0,x2:0.3,y2:1,
        stop:0 rgba(16,44,72,0.9), stop:1 rgba(8,24,44,0.95));
}
QWidget#DetailPanel {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(8,26,46,0.97), stop:1 rgba(4,14,26,0.98));
    border-left: 1px solid rgba(56,217,245,0.14);
}
QDialog#NewPackDialog {
    background: qlineargradient(x1:0,y1:0,x2:0.3,y2:1,
        stop:0 rgba(10,32,56,0.98), stop:1 rgba(5,18,34,0.99));
    border-top: 1px solid rgba(56,217,245,0.3);
    border-left: 1px solid rgba(56,217,245,0.16);
    border-right: 1px solid rgba(20,60,90,0.7);
    border-bottom: 1px solid rgba(0,0,0,0.7);
    border-radius: 12px;
}
QProgressBar {
    background: rgba(0,0,0,0.4);
    border: none;
    border-radius: 2px;
    height: 3px;
    text-visible: false;
}
QProgressBar::chunk {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #00e5cc, stop:1 #38d9f5);
    border-radius: 2px;
}
QLabel#SectionLabel {
    color: rgba(56,217,245,0.55);
    font-family: "Space Mono";
    font-size: 9px;
}
QLabel#PackName {
    color: #d8f0ff;
    font-family: "Exo 2";
    font-size: 14px;
    font-weight: 700;
}
QLabel#TagPill {
    background: rgba(56,217,245,0.07);
    border-top: 1px solid rgba(56,217,245,0.18);
    border-bottom: 1px solid rgba(0,0,0,0.25);
    border-left: 1px solid rgba(56,217,245,0.08);
    border-right: 1px solid rgba(0,0,0,0.15);
    border-radius: 3px;
    color: #7ab8d4;
    font-family: "Space Mono";
    font-size: 9px;
    padding: 2px 6px;
}
QLabel#StatusReady { color: #00e5cc; font-size: 9px; }
QLabel#StatusBusy { color: #5bc8f5; font-size: 9px; }
QLabel#StatusWarn { color: #e8b450; font-size: 9px; }
QLabel#MutedText { color: #4a7a96; font-size: 10px; }
QLabel#DetailKey { color: #4a7a96; font-size: 10px; }
QLabel#DetailValue { color: #cce8f8; font-size: 10px; }
QLabel#DetailValueHi { color: #00e5cc; font-size: 10px; }
"""
