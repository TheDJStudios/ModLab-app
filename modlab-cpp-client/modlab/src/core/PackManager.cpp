#include "PackManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

PackManager::PackManager(QObject* parent)
    : QObject(parent)
{
    // Mirror the Python backend's ~/.modlab-pyshit-client/packmapping/packmapping.json
    QString home = QDir::homePath();
    m_mappingPath = home + "/.modlab-pyshit-client/packmapping/packmapping.json";
}

void PackManager::load() {
    reloadFromDisk();
}

void PackManager::reloadFromDisk() {
    m_packs.clear();

    QFile f(m_mappingPath);
    if (!f.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    if (!doc.isObject()) return;
    QJsonObject root = doc.object();

    for (const QString& key : root.keys()) {
        QJsonObject obj = root[key].toObject();

        Pack p;
        p.key       = key;
        p.name      = obj["name"].toString(key);
        p.mcVersion = obj["version"].toString();
        p.modCount  = obj["mod_count"].toInt(0);

        QString loaderStr = obj["loader"].toString("vanilla").toLower();
        if      (loaderStr == "fabric") p.loader = Loader::Fabric;
        else if (loaderStr == "forge")  p.loader = Loader::Forge;
        else                            p.loader = Loader::Vanilla;

        // If launch_version is set the pack has been installed at least once
        p.status = obj.contains("launch_version")
            ? PackStatus::Ready
            : PackStatus::NotInstalled;

        m_packs.append(p);
    }

    emit packsChanged();
}

void PackManager::save() {
    // Qt side only writes status/mod_count extras — never overwrites
    // the Python-owned fields. For safety, reload first then patch.
    QFile f(m_mappingPath);
    if (!f.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    for (const Pack& p : m_packs) {
        if (!root.contains(p.key)) continue;
        QJsonObject obj = root[p.key].toObject();
        obj["mod_count"] = p.modCount;
        root[p.key] = obj;
    }

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    f.close();
}

Pack* PackManager::findPack(const QString& key) {
    for (Pack& p : m_packs)
        if (p.key == key) return &p;
    return nullptr;
}

void PackManager::setProgress(const QString& key, int progress) {
    if (Pack* p = findPack(key)) {
        p->installProgress = progress;
        emit packProgressChanged(key, progress);
    }
}

void PackManager::setStatus(const QString& key, PackStatus status) {
    if (Pack* p = findPack(key)) {
        p->status = status;
        emit packsChanged();
    }
}
