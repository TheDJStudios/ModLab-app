#include "Sidebar.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPainter>
#include <QStyle>

Sidebar::Sidebar(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("Sidebar");
    setFixedWidth(210);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // ---- Logo block ----
    auto* logoBlock = new QWidget(this);
    logoBlock->setFixedHeight(62);
    logoBlock->setStyleSheet(
        "background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 rgba(56,217,245,0.05),stop:1 transparent);"
        "border-bottom: 1px solid rgba(56,217,245,0.1);"
    );
    auto* logoLayout = new QVBoxLayout(logoBlock);
    logoLayout->setContentsMargins(18,14,18,10);
    logoLayout->setSpacing(2);

    auto* logoText = new QLabel("MOD<span style='color:#38d9f5'>LAB</span>", logoBlock);
    logoText->setTextFormat(Qt::RichText);
    logoText->setStyleSheet(
        "font-family: 'Exo 2'; font-size: 18px; font-weight: 900;"
        "color: #d8f0ff; letter-spacing: -0px;"
    );
    auto* logoVer = new QLabel("// launcher v0.1", logoBlock);
    logoVer->setStyleSheet("font-size: 9px; letter-spacing: 2px; color: #4a7a96;");

    logoLayout->addWidget(logoText);
    logoLayout->addWidget(logoVer);
    root->addWidget(logoBlock);

    // ---- Nav ----
    auto* navArea = new QWidget(this);
    auto* navLayout = new QVBoxLayout(navArea);
    navLayout->setContentsMargins(10,14,10,10);
    navLayout->setSpacing(1);

    auto addGroupLabel = [&](const QString& text) {
        QLabel* lbl = new QLabel("// " + text, navArea);
        lbl->setStyleSheet(
            "font-size: 9px; letter-spacing: 3px; color: #4a7a96;"
            "padding: 8px 8px 4px 8px; text-transform: uppercase;"
        );
        navLayout->addWidget(lbl);
    };

    auto addNavBtn = [&](const QString& label, const QString& page) {
        QPushButton* btn = new QPushButton(label, navArea);
        btn->setObjectName("NavButton");
        btn->setCheckable(false);
        btn->setCursor(Qt::PointingHandCursor);
        navLayout->addWidget(btn);
        m_navBtns.append(btn);

        connect(btn, &QPushButton::clicked, this, [this, page, btn](){
            setActivePage(page);
            emit pageRequested(page);
        });
    };

    addGroupLabel("Main");
    addNavBtn("Library",     "library");
    addNavBtn("Browse Mods", "browse");
    addNavBtn("Downloads",   "downloads");

    // Separator
    QFrame* sep = new QFrame(navArea);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: rgba(56,217,245,0.1); margin: 6px 2px;");
    navLayout->addWidget(sep);

    addGroupLabel("System");
    addNavBtn("Java Manager", "java");
    addNavBtn("Settings",     "settings");

    navLayout->addStretch();
    root->addWidget(navArea, 1);

    // ---- Footer ----
    auto* footer = new QWidget(this);
    footer->setFixedHeight(38);
    footer->setStyleSheet("border-top: 1px solid rgba(56,217,245,0.1);");
    auto* footLayout = new QHBoxLayout(footer);
    footLayout->setContentsMargins(16,0,16,0);
    QLabel* ver = new QLabel("modlab v0.1.0-alpha", footer);
    ver->setStyleSheet("font-size: 9px; color: #4a7a96; letter-spacing: 1px;");
    footLayout->addWidget(ver);
    root->addWidget(footer);

    // Default active page
    setActivePage("library");
}

void Sidebar::setActivePage(const QString& page) {
    m_activePage = page;
    // Nav button labels mapped to pages
    static const QMap<QString,QString> labelToPage = {
        {"Library","library"}, {"Browse Mods","browse"}, {"Downloads","downloads"},
        {"Java Manager","java"}, {"Settings","settings"}
    };
    for (QPushButton* btn : m_navBtns) {
        bool active = (labelToPage.value(btn->text()) == page);
        btn->setProperty("active", active);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
}
