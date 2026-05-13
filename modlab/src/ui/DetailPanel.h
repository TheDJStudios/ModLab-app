#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../core/Pack.h"

class DetailPanel : public QWidget {
    Q_OBJECT
public:
    explicit DetailPanel(QWidget* parent = nullptr);
    void showPack(const Pack& pack);
    void clearPack();

signals:
    void launchRequested(const QString& key);
    void addModsRequested(const QString& key);
    void packSettingsRequested(const QString& key);

private:
    void buildLayout();
    void setInfoRow(const QString& key, const QString& value, bool highlight = false);
    void rebuildInfoRows(const Pack& pack);

    QString      m_currentKey;

    // Header
    QLabel*      m_identLabel  = nullptr;
    QLabel*      m_nameLabel   = nullptr;
    QLabel*      m_subLabel    = nullptr;

    // Info rows container
    QWidget*     m_infoWidget  = nullptr;
    QVBoxLayout* m_infoLayout  = nullptr;

    // Mod list container
    QWidget*     m_modWidget   = nullptr;
    QVBoxLayout* m_modLayout   = nullptr;
    QLabel*      m_modHeader   = nullptr;

    // Footer buttons
    QPushButton* m_btnLaunch   = nullptr;
    QPushButton* m_btnAddMods  = nullptr;
    QPushButton* m_btnSettings = nullptr;
};
