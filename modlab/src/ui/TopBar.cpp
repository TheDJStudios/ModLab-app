#include "TopBar.h"
#include <QHBoxLayout>

TopBar::TopBar(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("TopBar");
    setFixedHeight(50);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 0, 20, 0);
    layout->setSpacing(10);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet(
        "font-family: 'Exo 2'; font-weight: 700; font-size: 13px;"
        "color: #d8f0ff; letter-spacing: 1px;"
    );
    layout->addWidget(m_titleLabel, 1);

    m_search = new QLineEdit(this);
    m_search->setObjectName("SearchBar");
    m_search->setPlaceholderText("Search packs...");
    m_search->setFixedWidth(190);
    connect(m_search, &QLineEdit::textChanged, this, &TopBar::searchChanged);
    layout->addWidget(m_search);

    m_newBtn = new QPushButton("+ New Pack", this);
    m_newBtn->setObjectName("BtnPrimary");
    connect(m_newBtn, &QPushButton::clicked, this, &TopBar::newPackRequested);
    layout->addWidget(m_newBtn);

    setTitle("Pack Library", "your instances");
}

void TopBar::setTitle(const QString& title, const QString& sub) {
    QString html = QString("<span style='color:#d8f0ff'>%1</span>").arg(title);
    if (!sub.isEmpty())
        html += QString(" <span style='color:#4a7a96; font-size:10px;"
                        " font-family:Space Mono; font-weight:400;"
                        " letter-spacing:2px'>// %1</span>").arg(sub);
    m_titleLabel->setText(html);
    m_titleLabel->setTextFormat(Qt::RichText);
}
