#include "PackCard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QStyle>

PackCard::PackCard(const Pack& pack, QWidget* parent)
    : QFrame(parent)
{
    setObjectName("PackCard");
    setFixedWidth(240);
    setCursor(Qt::PointingHandCursor);
    buildLayout(pack);
}

void PackCard::buildLayout(const Pack& pack) {
    m_key = pack.key;

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(15, 14, 15, 14);
    root->setSpacing(0);

    // ---- Header row: ident tile + status pill ----
    auto* headRow = new QHBoxLayout;
    headRow->setContentsMargins(0,0,0,10);

    m_identLabel = new QLabel(pack.abbr(), this);
    m_identLabel->setFixedSize(40, 40);
    m_identLabel->setAlignment(Qt::AlignCenter);
    m_identLabel->setObjectName("IdentTile");
    // Coloring is done in paintEvent / updatePack

    m_statusLabel = new QLabel(statusLabel(pack.status), this);
    m_statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    headRow->addWidget(m_identLabel);
    headRow->addStretch();
    headRow->addWidget(m_statusLabel);
    root->addLayout(headRow);

    // ---- Pack name ----
    m_nameLabel = new QLabel(pack.name, this);
    m_nameLabel->setObjectName("PackName");
    root->addWidget(m_nameLabel);
    root->addSpacing(4);

    // ---- Tag row ----
    m_tagRow = new QWidget(this);
    auto* tagLayout = new QHBoxLayout(m_tagRow);
    tagLayout->setContentsMargins(0,0,0,0);
    tagLayout->setSpacing(5);

    auto makeTag = [&](const QString& text) {
        QLabel* tag = new QLabel(text, m_tagRow);
        tag->setObjectName("TagPill");
        tagLayout->addWidget(tag);
    };
    makeTag(loaderName(pack.loader));
    makeTag(pack.mcVersion);
    if (pack.modCount > 0)
        makeTag(QString("%1 mods").arg(pack.modCount));
    tagLayout->addStretch();

    root->addWidget(m_tagRow);
    root->addSpacing(12);

    // ---- Separator line ----
    QFrame* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: rgba(56,217,245,0.12);");
    sep->setFixedHeight(1);
    root->addWidget(sep);
    root->addSpacing(11);

    // ---- Action buttons ----
    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(6);

    m_btnRun     = new QPushButton("Run",     this);
    m_btnInstall = new QPushButton("Install", this);
    m_btnDelete  = new QPushButton("Delete",  this);

    m_btnRun->setObjectName("BtnRun");
    m_btnInstall->setObjectName("BtnSecondary");
    m_btnDelete->setObjectName("BtnDanger");

    btnRow->addWidget(m_btnRun);
    btnRow->addWidget(m_btnInstall);
    btnRow->addWidget(m_btnDelete);
    root->addLayout(btnRow);

    // ---- Progress (hidden by default) ----
    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 100);
    m_progress->setValue(pack.installProgress);
    m_progress->setTextVisible(false);
    m_progress->setFixedHeight(3);

    m_progressLbl = new QLabel(this);
    m_progressLbl->setObjectName("MutedText");
    m_progressLbl->setStyleSheet("font-size: 9px;");

    root->addSpacing(9);
    root->addWidget(m_progress);
    root->addSpacing(3);
    root->addWidget(m_progressLbl);

    applyStatus(pack.status);

    connect(m_btnRun,     &QPushButton::clicked, this, [this]{ emit runRequested(m_key); });
    connect(m_btnInstall, &QPushButton::clicked, this, [this]{ emit installRequested(m_key); });
    connect(m_btnDelete,  &QPushButton::clicked, this, [this]{ emit deleteRequested(m_key); });
}

void PackCard::applyStatus(PackStatus status) {
    bool installing = (status == PackStatus::Installing);
    m_progress->setVisible(installing);
    m_progressLbl->setVisible(installing);
    m_btnRun->setEnabled(status == PackStatus::Ready || status == PackStatus::NeedsUpdate);

    switch (status) {
        case PackStatus::Ready:
            m_statusLabel->setObjectName("StatusReady");
            m_btnInstall->setText("Install");
            break;
        case PackStatus::Installing:
            m_statusLabel->setObjectName("StatusBusy");
            m_btnInstall->setText("Reinstall");
            break;
        case PackStatus::NeedsUpdate:
            m_statusLabel->setObjectName("StatusWarn");
            m_btnInstall->setText("Update");
            break;
        case PackStatus::NotInstalled:
            m_statusLabel->setObjectName("StatusWarn");
            m_btnInstall->setText("Install");
            break;
    }
    // Force QSS re-evaluation after objectName change
    m_statusLabel->style()->unpolish(m_statusLabel);
    m_statusLabel->style()->polish(m_statusLabel);
}

void PackCard::updatePack(const Pack& pack) {
    m_nameLabel->setText(pack.name);
    m_identLabel->setText(pack.abbr());
    m_statusLabel->setText(statusLabel(pack.status));
    m_progress->setValue(pack.installProgress);
    m_progressLbl->setText(QString("Downloading... %1%").arg(pack.installProgress));
    applyStatus(pack.status);
    update();
}

void PackCard::setSelected(bool selected) {
    setProperty("selected", selected);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void PackCard::mousePressEvent(QMouseEvent* event) {
    QFrame::mousePressEvent(event);
    emit cardClicked(m_key);
}

void PackCard::paintEvent(QPaintEvent* event) {
    // Let QSS handle the card background; just add the top-shine strip
    QFrame::paintEvent(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Top shine gradient strip
    QLinearGradient shine(0, 0, width(), 0);
    shine.setColorAt(0.0, Qt::transparent);
    shine.setColorAt(0.5, QColor(56, 217, 245, 30));
    shine.setColorAt(1.0, Qt::transparent);
    p.fillRect(0, 0, width(), 1, shine);

    // Inner top-glow gradient
    QLinearGradient topGlow(0, 0, 0, 40);
    topGlow.setColorAt(0.0, QColor(56, 217, 245, 10));
    topGlow.setColorAt(1.0, Qt::transparent);
    p.fillRect(0, 0, width(), 40, topGlow);
}
