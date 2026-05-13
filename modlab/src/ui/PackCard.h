#pragma once
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include "../core/Pack.h"

// PackCard is one card in the pack grid.
// It paints an ident tile, name, tags, action buttons, and optionally a progress bar.
class PackCard : public QFrame {
    Q_OBJECT
public:
    explicit PackCard(const Pack& pack, QWidget* parent = nullptr);

    void updatePack(const Pack& pack);
    const QString& packKey() const { return m_key; }
    void setSelected(bool selected);

signals:
    void runRequested(const QString& key);
    void installRequested(const QString& key);
    void deleteRequested(const QString& key);
    void cardClicked(const QString& key);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void buildLayout(const Pack& pack);
    void applyStatus(PackStatus status);
    QColor identColor(const Pack& pack) const;

    QString      m_key;
    QLabel*      m_identLabel  = nullptr;
    QLabel*      m_statusLabel = nullptr;
    QLabel*      m_nameLabel   = nullptr;
    QWidget*     m_tagRow      = nullptr;
    QPushButton* m_btnRun      = nullptr;
    QPushButton* m_btnInstall  = nullptr;
    QPushButton* m_btnDelete   = nullptr;
    QProgressBar* m_progress   = nullptr;
    QLabel*      m_progressLbl = nullptr;
};
