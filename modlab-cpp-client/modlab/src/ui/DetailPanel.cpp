#include "DetailPanel.h"
#include <QScrollArea>
#include <QHBoxLayout>
#include <QFrame>

DetailPanel::DetailPanel(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("DetailPanel");
    setFixedWidth(265);
    buildLayout();
}

void DetailPanel::buildLayout() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // ---- Header ----
    auto* header = new QWidget(this);
    header->setFixedHeight(110);
    header->setStyleSheet(
        "background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 rgba(56,217,245,0.04),stop:1 transparent);"
        "border-bottom: 1px solid rgba(56,217,245,0.1);"
    );
    auto* headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(18,16,18,12);
    headerLayout->setSpacing(3);

    m_identLabel = new QLabel(header);
    m_identLabel->setFixedSize(50, 50);
    m_identLabel->setAlignment(Qt::AlignCenter);
    m_identLabel->setStyleSheet(
        "font-family: 'Exo 2'; font-weight: 900; font-size: 14px;"
        "letter-spacing: 1px;"
        "background: qlineargradient(x1:0,y1:0,x2:0.5,y2:1,"
        "  stop:0 rgba(0,200,180,0.25), stop:1 rgba(0,130,120,0.3));"
        "border-top: 1px solid rgba(0,229,204,0.4);"
        "border-bottom: 1px solid rgba(0,0,0,0.45);"
        "border-left: 1px solid rgba(0,229,204,0.2);"
        "border-right: 1px solid rgba(0,0,0,0.2);"
        "border-radius: 9px;"
        "color: #00e5cc;"
    );

    m_nameLabel = new QLabel(header);
    m_nameLabel->setObjectName("PackName");
    m_nameLabel->setStyleSheet("font-size: 15px; font-weight: 900;");

    m_subLabel = new QLabel(header);
    m_subLabel->setObjectName("MutedText");

    headerLayout->addWidget(m_identLabel);
    headerLayout->addWidget(m_nameLabel);
    headerLayout->addWidget(m_subLabel);
    root->addWidget(header);

    // ---- Scrollable body ----
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* body = new QWidget;
    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(18, 14, 18, 14);
    bodyLayout->setSpacing(0);

    // Section label
    auto* infoLbl = new QLabel("// Instance Info", body);
    infoLbl->setObjectName("SectionLabel");
    infoLbl->setStyleSheet("font-size: 9px; letter-spacing: 3px; color: rgba(56,217,245,0.45); margin-bottom: 7px;");
    bodyLayout->addWidget(infoLbl);

    // Info rows container
    m_infoWidget = new QWidget(body);
    m_infoLayout = new QVBoxLayout(m_infoWidget);
    m_infoLayout->setContentsMargins(0,0,0,0);
    m_infoLayout->setSpacing(0);
    bodyLayout->addWidget(m_infoWidget);

    bodyLayout->addSpacing(14);

    // Mod list header
    m_modHeader = new QLabel("// Mods (0)", body);
    m_modHeader->setObjectName("SectionLabel");
    m_modHeader->setStyleSheet("font-size: 9px; letter-spacing: 3px; color: rgba(56,217,245,0.45); margin-bottom: 7px;");
    bodyLayout->addWidget(m_modHeader);

    m_modWidget = new QWidget(body);
    m_modLayout = new QVBoxLayout(m_modWidget);
    m_modLayout->setContentsMargins(0,0,0,0);
    m_modLayout->setSpacing(4);
    bodyLayout->addWidget(m_modWidget);

    bodyLayout->addStretch();
    scroll->setWidget(body);
    root->addWidget(scroll, 1);

    // ---- Footer buttons ----
    auto* footer = new QWidget(this);
    footer->setStyleSheet("border-top: 1px solid rgba(56,217,245,0.1);");
    auto* footLayout = new QVBoxLayout(footer);
    footLayout->setContentsMargins(14, 12, 14, 12);
    footLayout->setSpacing(6);

    m_btnLaunch   = new QPushButton("Launch",       footer);
    m_btnAddMods  = new QPushButton("+ Add Mods",   footer);
    m_btnSettings = new QPushButton("Pack Settings",footer);

    m_btnLaunch->setObjectName("BtnRun");
    m_btnAddMods->setObjectName("BtnPrimary");
    m_btnSettings->setObjectName("BtnSecondary");

    footLayout->addWidget(m_btnLaunch);
    footLayout->addWidget(m_btnAddMods);
    footLayout->addWidget(m_btnSettings);
    root->addWidget(footer);

    connect(m_btnLaunch,   &QPushButton::clicked, this, [this]{ emit launchRequested(m_currentKey); });
    connect(m_btnAddMods,  &QPushButton::clicked, this, [this]{ emit addModsRequested(m_currentKey); });
    connect(m_btnSettings, &QPushButton::clicked, this, [this]{ emit packSettingsRequested(m_currentKey); });
}

static QWidget* makeInfoRow(QWidget* parent, const QString& key, const QString& value, bool highlight) {
    auto* row = new QWidget(parent);
    row->setStyleSheet("border-bottom: 1px solid rgba(56,217,245,0.06);");
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 6, 0, 6);

    QLabel* k = new QLabel(key, row);
    k->setObjectName("DetailKey");

    QLabel* v = new QLabel(value, row);
    v->setObjectName(highlight ? "DetailValueHi" : "DetailValue");

    layout->addWidget(k);
    layout->addStretch();
    layout->addWidget(v);
    return row;
}

void DetailPanel::rebuildInfoRows(const Pack& pack) {
    // Clear existing rows
    while (QLayoutItem* item = m_infoLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    m_infoLayout->addWidget(makeInfoRow(m_infoWidget, "Loader",     loaderName(pack.loader), true));
    m_infoLayout->addWidget(makeInfoRow(m_infoWidget, "MC Version", pack.mcVersion,          false));
    m_infoLayout->addWidget(makeInfoRow(m_infoWidget, "Java",       "Bundled (auto)",        false));
    m_infoLayout->addWidget(makeInfoRow(m_infoWidget, "Mods",       QString::number(pack.modCount), false));
    m_infoLayout->addWidget(makeInfoRow(m_infoWidget, "Status",     statusLabel(pack.status), true));
}

void DetailPanel::showPack(const Pack& pack) {
    m_currentKey = pack.key;

    m_identLabel->setText(pack.abbr());
    m_nameLabel->setText(pack.name);
    m_subLabel->setText(pack.mcVersion + " · " + loaderName(pack.loader));

    rebuildInfoRows(pack);

    // Mod list (placeholder — real data comes from backend later)
    while (QLayoutItem* item = m_modLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }
    m_modHeader->setText(QString("// Mods (%1)").arg(pack.modCount));

    if (pack.modCount == 0) {
        QLabel* empty = new QLabel("No mods installed.", m_modWidget);
        empty->setObjectName("MutedText");
        empty->setStyleSheet("font-size: 10px; opacity: 0.5; padding: 6px 0;");
        m_modLayout->addWidget(empty);
    } else {
        // Placeholder rows — will be populated from a future mods-list JSON
        QStringList placeholders = {"(mod data loads here)", "...", "..."};
        for (int i = 0; i < qMin(pack.modCount, 5); ++i) {
            auto* row = new QWidget(m_modWidget);
            row->setStyleSheet(
                "background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "stop:0 rgba(10,30,52,0.6),stop:1 rgba(6,18,34,0.4));"
                "border-top: 1px solid rgba(56,217,245,0.09);"
                "border-bottom: 1px solid rgba(0,0,0,0.35);"
                "border-left: 1px solid rgba(56,217,245,0.06);"
                "border-right: 1px solid rgba(0,0,0,0.2);"
                "border-radius: 4px;"
            );
            auto* rl = new QHBoxLayout(row);
            rl->setContentsMargins(9,5,9,5);
            QLabel* name = new QLabel(QString("Mod slot %1").arg(i+1), row);
            name->setObjectName("DetailValue");
            QLabel* ver = new QLabel("—", row);
            ver->setObjectName("MutedText");
            ver->setStyleSheet("font-size: 9px;");
            rl->addWidget(name);
            rl->addStretch();
            rl->addWidget(ver);
            m_modLayout->addWidget(row);
        }
        if (pack.modCount > 5) {
            QLabel* more = new QLabel(QString("+ %1 more").arg(pack.modCount - 5), m_modWidget);
            more->setObjectName("MutedText");
            more->setAlignment(Qt::AlignCenter);
            more->setStyleSheet("font-size: 9px; padding: 4px 0; opacity: 0.4;");
            m_modLayout->addWidget(more);
        }
    }

    m_btnLaunch->setEnabled(pack.status == PackStatus::Ready || pack.status == PackStatus::NeedsUpdate);
}

void DetailPanel::clearPack() {
    m_currentKey.clear();
    m_identLabel->setText("—");
    m_nameLabel->setText("No pack selected");
    m_subLabel->setText("");
}
