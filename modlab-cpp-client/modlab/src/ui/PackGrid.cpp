#include "PackGrid.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

PackGrid::PackGrid(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);

    m_scroll = new QScrollArea(this);
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_inner  = new QWidget;
    m_grid   = new QGridLayout(m_inner);
    m_grid->setSpacing(12);
    m_grid->setContentsMargins(22, 20, 22, 20);
    m_grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_scroll->setWidget(m_inner);
    root->addWidget(m_scroll);
}

void PackGrid::setPacks(const QList<Pack>& packs) {
    rebuild(packs);
}

void PackGrid::rebuild(const QList<Pack>& packs) {
    // Clear old cards
    for (PackCard* c : m_cards) c->deleteLater();
    m_cards.clear();

    // Remove everything from layout (including the New Pack button if present)
    while (QLayoutItem* item = m_grid->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    const int COLS = 3;
    int pos = 0;

    for (const Pack& p : packs) {
        auto* card = new PackCard(p, m_inner);
        m_cards[p.key] = card;

        connect(card, &PackCard::runRequested,     this, &PackGrid::runRequested);
        connect(card, &PackCard::installRequested, this, &PackGrid::installRequested);
        connect(card, &PackCard::deleteRequested,  this, &PackGrid::deleteRequested);
        connect(card, &PackCard::cardClicked, this, [this](const QString& key){
            setSelectedKey(key);
            emit packSelected(key);
        });

        if (p.key == m_selected) card->setSelected(true);

        m_grid->addWidget(card, pos / COLS, pos % COLS);
        ++pos;
    }

    // "New Instance" button tile
    auto* newBtn = new QPushButton("+ New Instance", m_inner);
    newBtn->setObjectName("BtnPrimary");
    newBtn->setFixedSize(240, 152);
    newBtn->setCursor(Qt::PointingHandCursor);
    newBtn->setStyleSheet(
        "QPushButton#BtnPrimary {"
        "  background: rgba(6,18,34,0.5);"
        "  border: 1px dashed rgba(56,217,245,0.2);"
        "  border-radius: 9px;"
        "  color: #4a7a96;"
        "  font-size: 11px;"
        "  letter-spacing: 2px;"
        "}"
        "QPushButton#BtnPrimary:hover {"
        "  background: rgba(10,30,52,0.7);"
        "  border-color: rgba(56,217,245,0.38);"
        "  color: #38d9f5;"
        "}"
    );
    connect(newBtn, &QPushButton::clicked, this, &PackGrid::newPackRequested);
    m_grid->addWidget(newBtn, pos / COLS, pos % COLS);
}

void PackGrid::updatePack(const Pack& pack) {
    if (PackCard* c = m_cards.value(pack.key, nullptr))
        c->updatePack(pack);
}

void PackGrid::setSelectedKey(const QString& key) {
    if (!m_selected.isEmpty())
        if (PackCard* old = m_cards.value(m_selected, nullptr))
            old->setSelected(false);
    m_selected = key;
    if (PackCard* cur = m_cards.value(key, nullptr))
        cur->setSelected(true);
}
