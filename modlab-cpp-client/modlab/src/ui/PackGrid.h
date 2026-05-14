#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QMap>
#include "../core/Pack.h"
#include "PackCard.h"

class PackGrid : public QWidget {
    Q_OBJECT
public:
    explicit PackGrid(QWidget* parent = nullptr);
    void setPacks(const QList<Pack>& packs);
    void updatePack(const Pack& pack);
    void setSelectedKey(const QString& key);

signals:
    void runRequested(const QString& key);
    void installRequested(const QString& key);
    void deleteRequested(const QString& key);
    void packSelected(const QString& key);
    void newPackRequested();

private:
    void rebuild(const QList<Pack>& packs);
    QScrollArea*             m_scroll   = nullptr;
    QWidget*                 m_inner    = nullptr;
    QGridLayout*             m_grid     = nullptr;
    QMap<QString, PackCard*> m_cards;
    QString                  m_selected;
};
