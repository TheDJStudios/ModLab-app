#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "Sidebar.h"
#include "TopBar.h"
#include "PackGrid.h"
#include "DetailPanel.h"
#include "../core/PackManager.h"
#include "../ipc/BackendProcess.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onPageRequested(const QString& page);
    void onNewPackRequested();
    void onRunRequested(const QString& key);
    void onInstallRequested(const QString& key);
    void onDeleteRequested(const QString& key);
    void onPackSelected(const QString& key);
    void onPacksChanged();
    void onProgressUpdate(const QString& key, int percent);
    void onOperationDone(const QString& key);
    void onOperationError(const QString& key, const QString& msg);

private:
    void buildLayout();
    void connectSignals();
    QWidget* makePlaceholderPage(const QString& title, const QString& sub);

    PackManager*    m_packManager  = nullptr;
    BackendProcess* m_backend      = nullptr;

    Sidebar*        m_sidebar      = nullptr;
    TopBar*         m_topBar       = nullptr;
    QStackedWidget* m_stack        = nullptr;
    PackGrid*       m_packGrid     = nullptr;
    DetailPanel*    m_detailPanel  = nullptr;

    // Placeholder widgets for unbuilt pages
    QWidget*        m_browsePage   = nullptr;
    QWidget*        m_downloadsPage= nullptr;
    QWidget*        m_javaPage     = nullptr;
    QWidget*        m_settingsPage = nullptr;
};
