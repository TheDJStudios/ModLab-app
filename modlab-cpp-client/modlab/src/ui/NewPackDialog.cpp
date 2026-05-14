#include "NewPackDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QStyle>

NewPackDialog::NewPackDialog(QWidget* parent)
    : QDialog(parent)
{
    setObjectName("NewPackDialog");
    setWindowTitle("New Instance");
    setModal(true);
    setFixedSize(370, 340);
    // Frameless rounded dialog — real border is drawn by QSS
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    buildLayout();
}

void NewPackDialog::buildLayout() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // ---- Header ----
    auto* header = new QWidget(this);
    header->setFixedHeight(52);
    header->setStyleSheet(
        "background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 rgba(56,217,245,0.06),stop:1 transparent);"
        "border-bottom: 1px solid rgba(56,217,245,0.1);"
        "border-top-left-radius: 12px; border-top-right-radius: 12px;"
    );
    auto* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(22,0,14,0);

    auto* titleLbl = new QLabel(header);
    titleLbl->setText("New <span style='color:#38d9f5'>Instance</span>");
    titleLbl->setTextFormat(Qt::RichText);
    titleLbl->setStyleSheet("font-family:'Exo 2'; font-weight:700; font-size:14px; color:#d8f0ff;");

    auto* closeBtn = new QPushButton("✕", header);
    closeBtn->setFixedSize(26, 22);
    closeBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 rgba(255,255,255,0.07),stop:1 rgba(0,0,0,0.1));"
        "border-top:1px solid rgba(255,255,255,0.1);"
        "border-bottom:1px solid rgba(0,0,0,0.3);"
        "border-left:1px solid rgba(255,255,255,0.05);"
        "border-right:1px solid rgba(0,0,0,0.2);"
        "border-radius:4px; color:#7ab8d4; font-size:11px; }"
        "QPushButton:hover { color:#d8f0ff; }"
    );
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);

    hLayout->addWidget(titleLbl);
    hLayout->addStretch();
    hLayout->addWidget(closeBtn);
    root->addWidget(header);

    // ---- Body ----
    auto* body = new QWidget(this);
    body->setStyleSheet("background: transparent;");
    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(22, 18, 22, 18);
    bodyLayout->setSpacing(13);

    auto makeFieldLabel = [](const QString& text, QWidget* parent) {
        QLabel* lbl = new QLabel(
            "<span style='color:#00e5cc'>&#62;</span> " + text,
            parent
        );
        lbl->setTextFormat(Qt::RichText);
        lbl->setStyleSheet("font-size: 9px; letter-spacing: 3px; color: #7ab8d4;");
        return lbl;
    };

    // Pack name
    auto* nameBlock = new QWidget(body);
    auto* nameLayout = new QVBoxLayout(nameBlock);
    nameLayout->setContentsMargins(0,0,0,0);
    nameLayout->setSpacing(5);
    nameLayout->addWidget(makeFieldLabel("PACK NAME", nameBlock));
    m_nameEdit = new QLineEdit(nameBlock);
    m_nameEdit->setObjectName("FieldInput");
    m_nameEdit->setPlaceholderText("e.g. My Survival World");
    nameLayout->addWidget(m_nameEdit);
    bodyLayout->addWidget(nameBlock);

    // Version
    auto* verBlock = new QWidget(body);
    auto* verLayout = new QVBoxLayout(verBlock);
    verLayout->setContentsMargins(0,0,0,0);
    verLayout->setSpacing(5);
    verLayout->addWidget(makeFieldLabel("MINECRAFT VERSION", verBlock));
    m_versionEdit = new QLineEdit(verBlock);
    m_versionEdit->setObjectName("FieldInput");
    m_versionEdit->setPlaceholderText("e.g. 1.21.4");
    verLayout->addWidget(m_versionEdit);
    bodyLayout->addWidget(verBlock);

    // Loader selector
    auto* loaderBlock = new QWidget(body);
    auto* loaderLayout = new QVBoxLayout(loaderBlock);
    loaderLayout->setContentsMargins(0,0,0,0);
    loaderLayout->setSpacing(5);
    loaderLayout->addWidget(makeFieldLabel("MOD LOADER", loaderBlock));

    auto* loaderRow = new QHBoxLayout;
    loaderRow->setSpacing(7);

    auto makeLoaderBtn = [&](const QString& abbr, const QString& name) -> QPushButton* {
        QPushButton* btn = new QPushButton(loaderBlock);
        btn->setFixedHeight(56);
        btn->setText(QString("<span style='font-size:12px;font-weight:900;"
                             "font-family:Exo 2;letter-spacing:1px'>%1</span>"
                             "<br><span style='font-size:8px;letter-spacing:2px;"
                             "color:#7ab8d4'>%2</span>").arg(abbr, name));
        // QPushButton doesn't render HTML natively; we use a QLabel trick below instead
        return btn;
    };
    (void)makeLoaderBtn; // suppress unused warning — using labels instead

    // Use QFrame+QLabel for each loader tile (QPushButton doesn't do multiline HTML well)
    auto makeTile = [&](const QString& abbr, const QString& label, Loader loader) -> QFrame* {
        QFrame* tile = new QFrame(loaderBlock);
        tile->setFixedHeight(56);
        tile->setCursor(Qt::PointingHandCursor);
        tile->setProperty("loaderSel", false);

        auto applyTileStyle = [tile](bool selected) {
            if (selected) {
                tile->setStyleSheet(
                    "QFrame { background: qlineargradient(x1:0,y1:0,x2:0.5,y2:1,"
                    "stop:0 rgba(56,217,245,0.12),stop:1 rgba(20,80,110,0.15));"
                    "border-top:1px solid rgba(56,217,245,0.5);"
                    "border-left:1px solid rgba(56,217,245,0.22);"
                    "border-right:1px solid rgba(0,0,0,0.25);"
                    "border-bottom:1px solid rgba(0,0,0,0.45);"
                    "border-radius:6px; }"
                );
            } else {
                tile->setStyleSheet(
                    "QFrame { background: qlineargradient(x1:0,y1:0,x2:0.5,y2:1,"
                    "stop:0 rgba(10,30,52,0.7),stop:1 rgba(4,14,26,0.8));"
                    "border-top:1px solid rgba(255,255,255,0.08);"
                    "border-left:1px solid rgba(255,255,255,0.04);"
                    "border-right:1px solid rgba(0,0,0,0.25);"
                    "border-bottom:1px solid rgba(0,0,0,0.45);"
                    "border-radius:6px; }"
                    "QFrame:hover { border-top-color: rgba(56,217,245,0.25); }"
                );
            }
        };
        applyTileStyle(loader == Loader::Fabric); // Fabric selected by default

        auto* tileLayout = new QVBoxLayout(tile);
        tileLayout->setContentsMargins(0,6,0,6);
        tileLayout->setAlignment(Qt::AlignCenter);

        QLabel* abbrLbl = new QLabel(abbr, tile);
        abbrLbl->setAlignment(Qt::AlignCenter);
        abbrLbl->setStyleSheet(
            loader == Loader::Fabric
            ? "font-family:'Exo 2'; font-weight:900; font-size:12px; color:#38d9f5; letter-spacing:1px;"
            : "font-family:'Exo 2'; font-weight:900; font-size:12px; color:#7ab8d4; letter-spacing:1px;"
        );

        QLabel* nameLbl = new QLabel(label, tile);
        nameLbl->setAlignment(Qt::AlignCenter);
        nameLbl->setStyleSheet("font-size:8px; letter-spacing:2px; color:#4a7a96; text-transform:uppercase;");

        tileLayout->addWidget(abbrLbl);
        tileLayout->addWidget(nameLbl);

        // Click handler via event filter isn't needed — use mousePressEvent via installEventFilter
        // Simpler: use a transparent QPushButton overlaid, but cleanest is subclassing.
        // Here we capture with a child QPushButton that fills the tile.
        auto* clickTarget = new QPushButton(tile);
        clickTarget->setFixedSize(tile->width(), 56);
        clickTarget->move(0,0);
        clickTarget->setStyleSheet("background:transparent; border:none;");
        clickTarget->raise();

        connect(clickTarget, &QPushButton::clicked, this, [this, loader, applyTileStyle,
                abbrLbl, label]() mutable {
            selectLoader(loader);
        });
        // Store applyTileStyle ref per tile — easier to just refresh all in selectLoader
        // Store tile pointer indexed by loader for re-styling
        tile->setObjectName(QString("loader_%1").arg((int)loader));

        return tile;
    };

    m_loaderFabric  = nullptr; // using tiles instead
    QFrame* fabricTile  = makeTile("FBR", "Fabric",  Loader::Fabric);
    QFrame* forgeTile   = makeTile("FRG", "Forge",   Loader::Forge);
    QFrame* vanillaTile = makeTile("VNL", "Vanilla", Loader::Vanilla);

    loaderRow->addWidget(fabricTile);
    loaderRow->addWidget(forgeTile);
    loaderRow->addWidget(vanillaTile);
    loaderLayout->addLayout(loaderRow);
    bodyLayout->addWidget(loaderBlock);
    root->addWidget(body, 1);

    // ---- Footer ----
    auto* footer = new QWidget(this);
    footer->setFixedHeight(52);
    footer->setStyleSheet(
        "background: transparent;"
        "border-top: 1px solid rgba(56,217,245,0.1);"
        "border-bottom-left-radius: 12px; border-bottom-right-radius: 12px;"
    );
    auto* footLayout = new QHBoxLayout(footer);
    footLayout->setContentsMargins(22, 0, 22, 0);

    // Step dots
    auto* dotsRow = new QHBoxLayout;
    dotsRow->setSpacing(5);
    for (int i = 0; i < 3; ++i) {
        QLabel* dot = new QLabel(footer);
        dot->setFixedSize(6,6);
        dot->setStyleSheet(i == 0
            ? "background:#38d9f5; border-radius:3px;"
            : "background:rgba(56,217,245,0.15); border:1px solid rgba(56,217,245,0.12); border-radius:3px;"
        );
        dotsRow->addWidget(dot);
    }
    footLayout->addLayout(dotsRow);
    footLayout->addStretch();

    auto* cancelBtn = new QPushButton("Cancel", footer);
    cancelBtn->setObjectName("BtnSecondary");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_btnCreate = new QPushButton("Create", footer);
    m_btnCreate->setObjectName("BtnPrimary");
    connect(m_btnCreate, &QPushButton::clicked, this, [this]() {
        m_result = { m_nameEdit->text().trimmed(),
                     m_versionEdit->text().trimmed(),
                     m_loader };
        accept();
    });

    footLayout->addWidget(cancelBtn);
    footLayout->addSpacing(7);
    footLayout->addWidget(m_btnCreate);
    root->addWidget(footer);

    connect(m_nameEdit,    &QLineEdit::textChanged, this, &NewPackDialog::validate);
    connect(m_versionEdit, &QLineEdit::textChanged, this, &NewPackDialog::validate);
    validate();
}

void NewPackDialog::selectLoader(Loader l) {
    m_loader = l;
    // Re-style all loader tiles
    static const QMap<Loader,QString> names = {
        {Loader::Fabric,"Fabric"},{Loader::Forge,"Forge"},{Loader::Vanilla,"Vanilla"}
    };
    // Find tiles by objectName
    for (Loader tl : {Loader::Fabric, Loader::Forge, Loader::Vanilla}) {
        QFrame* tile = findChild<QFrame*>(QString("loader_%1").arg((int)tl));
        if (!tile) continue;
        bool sel = (tl == l);
        if (sel) {
            tile->setStyleSheet(
                "QFrame { background: qlineargradient(x1:0,y1:0,x2:0.5,y2:1,"
                "stop:0 rgba(56,217,245,0.12),stop:1 rgba(20,80,110,0.15));"
                "border-top:1px solid rgba(56,217,245,0.5);"
                "border-left:1px solid rgba(56,217,245,0.22);"
                "border-right:1px solid rgba(0,0,0,0.25);"
                "border-bottom:1px solid rgba(0,0,0,0.45);"
                "border-radius:6px; }"
            );
            // Update abbr label color
            QList<QLabel*> labels = tile->findChildren<QLabel*>();
            if (!labels.isEmpty())
                labels[0]->setStyleSheet(
                    "font-family:'Exo 2'; font-weight:900; font-size:12px;"
                    "color:#38d9f5; letter-spacing:1px;"
                );
        } else {
            tile->setStyleSheet(
                "QFrame { background: qlineargradient(x1:0,y1:0,x2:0.5,y2:1,"
                "stop:0 rgba(10,30,52,0.7),stop:1 rgba(4,14,26,0.8));"
                "border-top:1px solid rgba(255,255,255,0.08);"
                "border-left:1px solid rgba(255,255,255,0.04);"
                "border-right:1px solid rgba(0,0,0,0.25);"
                "border-bottom:1px solid rgba(0,0,0,0.45);"
                "border-radius:6px; }"
                "QFrame:hover { border-top-color:rgba(56,217,245,0.25); }"
            );
            QList<QLabel*> labels = tile->findChildren<QLabel*>();
            if (!labels.isEmpty())
                labels[0]->setStyleSheet(
                    "font-family:'Exo 2'; font-weight:900; font-size:12px;"
                    "color:#7ab8d4; letter-spacing:1px;"
                );
        }
    }
}

void NewPackDialog::validate() {
    bool ok = !m_nameEdit->text().trimmed().isEmpty()
           && !m_versionEdit->text().trimmed().isEmpty();
    m_btnCreate->setEnabled(ok);
    m_btnCreate->setObjectName(ok ? "BtnPrimary" : "BtnSecondary");
    m_btnCreate->style()->unpolish(m_btnCreate);
    m_btnCreate->style()->polish(m_btnCreate);
}
