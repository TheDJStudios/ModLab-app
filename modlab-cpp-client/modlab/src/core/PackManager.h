#pragma once
#include <QObject>
#include <QList>
#include <QString>
#include "Pack.h"

// PackManager owns the in-memory pack list and syncs it with
// the packmapping.json that the Python backend also reads/writes.
// It does NOT call the backend directly — BackendProcess does that.
// PackManager just keeps Qt-side state consistent.
class PackManager : public QObject {
    Q_OBJECT
public:
    explicit PackManager(QObject* parent = nullptr);

    void        load();                     // read packmapping.json
    void        save();                     // write packmapping.json

    const QList<Pack>& packs() const { return m_packs; }
    Pack*       findPack(const QString& key);

    // Called by BackendProcess when progress events arrive
    void        setProgress(const QString& key, int progress);
    void        setStatus(const QString& key, PackStatus status);

    // Called by NewPackDialog after the backend creates the pack
    void        reloadFromDisk();

signals:
    void packsChanged();
    void packProgressChanged(const QString& key, int progress);

private:
    QList<Pack> m_packs;
    QString     m_mappingPath;
};
