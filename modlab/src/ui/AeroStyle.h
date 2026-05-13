#pragma once
#include <QString>

// Central stylesheet for ModLab's aero-glass dark UI.
// Apply to QApplication: qApp->setStyleSheet(AeroStyle::sheet());
// Individual widgets can use the named snippets for dynamic states.
namespace AeroStyle {

// Colour tokens — mirrors the CSS variables exactly
static constexpr auto SKY_DEEP   = "#04111f";
static constexpr auto SKY_MID    = "#071d35";
static constexpr auto SKY_GLOW   = "#0a2a4a";
static constexpr auto CYAN       = "#38d9f5";
static constexpr auto BLUE       = "#5bc8f5";
static constexpr auto TEAL       = "#00e5cc";
static constexpr auto WHITE      = "#d8f0ff";
static constexpr auto DIM        = "#7ab8d4";
static constexpr auto MUTED      = "#4a7a96";
static constexpr auto TEXT       = "#cce8f8";
static constexpr auto GLASS_BG   = "rgba(10,42,74,0.55)";
static constexpr auto GLASS_BDR  = "rgba(56,217,245,0.18)";

// ---- Full application stylesheet ----
inline QString sheet() {
    return R"(

/* ===== BASE ===== */
QWidget {
    background-color: #04111f;
    color: #cce8f8;
    font-family: "Space Mono";
    font-size: 12px;
    selection-background-color: rgba(56,217,245,0.25);
    selection-color: #d8f0ff;
}

/* ===== SCROLLBAR ===== */
QScrollBar:vertical {
    background: transparent;
    width: 5px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: rgba(56,217,245,0.22);
    border-radius: 2px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover { background: rgba(56,217,245,0.38); }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }

/* ===== SIDEBAR ===== */
QWidget#Sidebar {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(12,34,58,0.97), stop:1 rgba(6,20,36,0.98));
    border-right: 1px solid rgba(56,217,245,0.14);
}

/* Nav buttons inside sidebar */
QPushButton#NavButton {
    background: transparent;
    border: 1px solid transparent;
    border-radius: 5px;
    color: #7ab8d4;
    font-family: "Space Mono";
    font-size: 11px;
    letter-spacing: 1px;
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

/* ===== TOPBAR ===== */
QWidget#TopBar {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(10,34,56,0.92), stop:1 rgba(6,22,38,0.95));
    border-bottom: 1px solid rgba(56,217,245,0.15);
}

/* ===== SEARCH INPUT ===== */
QLineEdit#SearchBar {
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
QLineEdit#SearchBar:focus {
    border-color: rgba(56,217,245,0.35);
}

/* ===== BUTTONS ===== */

/* Primary — cyan-tinted */
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
    letter-spacing: 1px;
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

/* Run — teal accent */
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
    letter-spacing: 1px;
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
QPushButton#BtnRun:disabled { opacity: 0.4; }

/* Secondary — neutral glass */
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
    letter-spacing: 1px;
    padding: 6px 14px;
}
QPushButton#BtnSecondary:hover {
    color: #5bc8f5;
    border-top-color: rgba(56,217,245,0.3);
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(255,255,255,0.1), stop:1 rgba(0,0,0,0.05));
}
QPushButton#BtnSecondary:pressed {
    padding-top: 7px; padding-bottom: 5px;
}
QPushButton#BtnSecondary:disabled { opacity: 0.4; }

/* Danger — red-tinted */
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
    letter-spacing: 1px;
    padding: 6px 14px;
}
QPushButton#BtnDanger:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(220,60,60,0.28), stop:1 rgba(120,30,30,0.28));
}
QPushButton#BtnDanger:pressed {
    padding-top: 7px; padding-bottom: 5px;
}

/* ===== PACK CARD ===== */
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

/* ===== DETAIL PANEL ===== */
QWidget#DetailPanel {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(8,26,46,0.97), stop:1 rgba(4,14,26,0.98));
    border-left: 1px solid rgba(56,217,245,0.14);
}

/* ===== DIALOG / MODAL ===== */
QDialog#NewPackDialog {
    background: qlineargradient(x1:0,y1:0,x2:0.3,y2:1,
        stop:0 rgba(10,32,56,0.98), stop:1 rgba(5,18,34,0.99));
    border-top: 1px solid rgba(56,217,245,0.3);
    border-left: 1px solid rgba(56,217,245,0.16);
    border-right: 1px solid rgba(20,60,90,0.7);
    border-bottom: 1px solid rgba(0,0,0,0.7);
    border-radius: 12px;
}

/* Dialog inputs */
QLineEdit#FieldInput {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(2,10,20,0.9), stop:1 rgba(4,14,26,0.85));
    border-top: 1px solid rgba(0,0,0,0.6);
    border-left: 1px solid rgba(0,0,0,0.4);
    border-right: 1px solid rgba(56,217,245,0.06);
    border-bottom: 1px solid rgba(56,217,245,0.08);
    border-radius: 5px;
    color: #cce8f8;
    font-family: "Space Mono";
    font-size: 11px;
    padding: 7px 10px;
}
QLineEdit#FieldInput:focus {
    border-bottom-color: rgba(56,217,245,0.35);
}

/* ComboBox (version picker etc.) */
QComboBox {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 rgba(12,36,60,0.8), stop:1 rgba(6,20,38,0.85));
    border-top: 1px solid rgba(56,217,245,0.2);
    border-left: 1px solid rgba(56,217,245,0.1);
    border-right: 1px solid rgba(0,0,0,0.3);
    border-bottom: 1px solid rgba(0,0,0,0.45);
    border-radius: 5px;
    color: #cce8f8;
    font-family: "Space Mono";
    font-size: 11px;
    padding: 5px 10px;
}
QComboBox:hover { border-top-color: rgba(56,217,245,0.35); }
QComboBox::drop-down { border: none; width: 20px; }
QComboBox QAbstractItemView {
    background: #071d35;
    border: 1px solid rgba(56,217,245,0.2);
    color: #cce8f8;
    selection-background-color: rgba(56,217,245,0.15);
}

/* ===== PROGRESS BAR ===== */
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

/* ===== LABELS ===== */
QLabel#SectionLabel {
    color: rgba(56,217,245,0.55);
    font-family: "Space Mono";
    font-size: 9px;
    letter-spacing: 3px;
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
    letter-spacing: 1px;
    padding: 2px 6px;
}
QLabel#StatusReady   { color: #00e5cc; font-size: 9px; letter-spacing: 2px; }
QLabel#StatusBusy    { color: #5bc8f5; font-size: 9px; letter-spacing: 2px; }
QLabel#StatusWarn    { color: #e8b450; font-size: 9px; letter-spacing: 2px; }
QLabel#MutedText     { color: #4a7a96; font-size: 10px; }
QLabel#DetailKey     { color: #4a7a96; font-size: 10px; }
QLabel#DetailValue   { color: #cce8f8; font-size: 10px; }
QLabel#DetailValueHi { color: #00e5cc; font-size: 10px; }

)";
}

} // namespace AeroStyle
