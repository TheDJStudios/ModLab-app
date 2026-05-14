#include "MainWindow.h"
#include "NewPackDialog.h"
#include "AeroStyle.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("ModLab");
    setMinimumSize(1100, 680);
    resize(1280, 780);

    // ---- Backend process ----
    // python3 by default; override MODLAB_PYTHON env var to use a venv
    QString python = qEnvironmentVariable("MODLAB_PYTHON", "python3");
    QString script = qEnvironmentVariable("MODLAB_SCRIPT", MODLAB_BACKEND_PATH);
    m_backend = new BackendProcess(python, script, this);

    // ---- Pack manager ----
    m_packManager = new PackManager(this);
    m_packManager->load();

    buildLayout();
    connectSignals();

    // Populate grid with whatever is on disk at launch
    onPacksChanged();
}

void MainWindow::buildLayout() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* outerRow = new QHBoxLayout(central);
    outerRow->setContentsMargins(0,0,0,0);
    outerRow->setSpacing(0);

    // Sidebar
    m_sidebar = new Sidebar(this);
    outerRow->addWidget(m_sidebar);

    // Main column: topbar + stacked pages
    auto* mainCol = new QVBoxLayout;
    mainCol->setContentsMargins(0,0,0,0);
    mainCol->setSpacing(0);

    m_topBar = new TopBar(this);
    mainCol->addWidget(m_topBar);

    // Stacked widget — one widget per page
    m_stack = new QStackedWidget(this);

    // Page 0: Library (grid + detail panel side by side)
    auto* libraryPage = new QWidget;
    auto* libRow = new QHBoxLayout(libraryPage);
    libRow->setContentsMargins(0,0,0,0);
    libRow->setSpacing(0);

    m_packGrid = new PackGrid(libraryPage);
    libRow->addWidget(m_packGrid, 1);

    m_detailPanel = new DetailPanel(libraryPage);
    libRow->addWidget(m_detailPanel);

    m_stack->addWidget(libraryPage);   // index 0

    // Page 1: Browse (placeholder)
    m_browsePage = makePlaceholderPage("Browse Mods", "// mod browser coming soon");
    m_stack->addWidget(m_browsePage);  // index 1

    // Page 2: Downloads (placeholder)
    m_downloadsPage = makePlaceholderPage("Downloads", "// download queue coming soon");
    m_stack->addWidget(m_downloadsPage); // index 2

    // Page 3: Java Manager (placeholder)
    m_javaPage = makePlaceholderPage("Java Manager", "// java runtime manager coming soon");
    m_stack->addWidget(m_javaPage);    // index 3

    // Page 4: Settings (placeholder)
    m_settingsPage = makePlaceholderPage("Settings", "// settings coming soon");
    m_stack->addWidget(m_settingsPage); // index 4

    mainCol->addWidget(m_stack, 1);
    outerRow->addLayout(mainCol, 1);
}

// Small helper to create a centred placeholder for unbuilt pages
QWidget* MainWindow::makePlaceholderPage(const QString& title, const QString& sub) {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* lbl = new QLabel(sub, page);
    lbl->setObjectName("MutedText");
    lbl->setStyleSheet("font-size:12px; color:#4a7a96; letter-spacing:2px;");
    lbl->setAlignment(Qt::AlignCenter);
    layout->addWidget(lbl);
    (void)title;
    return page;
}

void MainWindow::connectSignals() {
    // Sidebar navigation
    connect(m_sidebar, &Sidebar::pageRequested,
            this, &MainWindow::onPageRequested);

    // TopBar
    connect(m_topBar, &TopBar::newPackRequested,
            this, &MainWindow::onNewPackRequested);

    // Pack grid actions
    connect(m_packGrid, &PackGrid::newPackRequested,
            this, &MainWindow::onNewPackRequested);
    connect(m_packGrid, &PackGrid::runRequested,
            this, &MainWindow::onRunRequested);
    connect(m_packGrid, &PackGrid::installRequested,
            this, &MainWindow::onInstallRequested);
    connect(m_packGrid, &PackGrid::deleteRequested,
            this, &MainWindow::onDeleteRequested);
    connect(m_packGrid, &PackGrid::packSelected,
            this, &MainWindow::onPackSelected);

    // Detail panel actions
    connect(m_detailPanel, &DetailPanel::launchRequested,
            this, &MainWindow::onRunRequested);
    connect(m_detailPanel, &DetailPanel::addModsRequested,
            this, [](const QString&){ /* TODO: open mod browser filtered to pack */ });
    connect(m_detailPanel, &DetailPanel::packSettingsRequested,
            this, [](const QString&){ /* TODO: open pack settings dialog */ });

    // PackManager
    connect(m_packManager, &PackManager::packsChanged,
            this, &MainWindow::onPacksChanged);
    connect(m_packManager, &PackManager::packProgressChanged,
            this, [this](const QString& key, int pct){
                onProgressUpdate(key, pct);
            });

    // BackendProcess
    connect(m_backend, &BackendProcess::progressUpdate,
            this, &MainWindow::onProgressUpdate);
    connect(m_backend, &BackendProcess::operationDone,
            this, &MainWindow::onOperationDone);
    connect(m_backend, &BackendProcess::operationError,
            this, &MainWindow::onOperationError);
    connect(m_backend, &BackendProcess::statusLine,
            this, [this](const QString& key, const QString& msg){
                m_packManager->setStatus(key, PackStatus::Installing);
                Q_UNUSED(msg);
            });
}

// ---- Slots ----

void MainWindow::onPageRequested(const QString& page) {
    static const QMap<QString,int> pageIndex = {
        {"library",0}, {"browse",1}, {"downloads",2}, {"java",3}, {"settings",4}
    };
    int idx = pageIndex.value(page, 0);
    m_stack->setCurrentIndex(idx);

    static const QMap<QString,QString> titles = {
        {"library","Pack Library"}, {"browse","Browse Mods"},
        {"downloads","Downloads"},  {"java","Java Manager"},
        {"settings","Settings"}
    };
    static const QMap<QString,QString> subs = {
        {"library","your instances"}, {"browse","find and install mods"},
        {"downloads","active & recent"}, {"java","runtime management"},
        {"settings","preferences"}
    };
    m_topBar->setTitle(titles.value(page,"ModLab"), subs.value(page,""));
}

void MainWindow::onNewPackRequested() {
    NewPackDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    NewPackData data = dlg.result();
    if (data.name.isEmpty() || data.version.isEmpty()) return;

    // Immediately mark a placeholder as installing while backend works
    Pack placeholder;
    placeholder.key       = data.name.toLower();
    placeholder.name      = data.name;
    placeholder.mcVersion = data.version;
    placeholder.loader    = data.loader;
    placeholder.status    = PackStatus::Installing;

    // Backend creates + installs
    m_backend->makePack(data.name, data.version, loaderName(data.loader).toLower());

    // Reload from disk once backend signals DONE
    // (handled in onOperationDone)
}

void MainWindow::onRunRequested(const QString& key) {
    if (Pack* p = m_packManager->findPack(key)) {
        if (p->status != PackStatus::Ready && p->status != PackStatus::NeedsUpdate)
            return;
    }
    m_backend->runPack(key);
}

void MainWindow::onInstallRequested(const QString& key) {
    m_packManager->setStatus(key, PackStatus::Installing);
    m_backend->installPack(key);
}

void MainWindow::onDeleteRequested(const QString& key) {
    QMessageBox confirm(this);
    confirm.setWindowTitle("Delete Pack");
    confirm.setText(QString("Delete <b>%1</b>? This cannot be undone.").arg(key));
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirm.setDefaultButton(QMessageBox::Cancel);
    confirm.setStyleSheet(qApp->styleSheet());
    if (confirm.exec() != QMessageBox::Yes) return;

    m_backend->deletePack(key);
    // Reload after DONE signal
}

void MainWindow::onPackSelected(const QString& key) {
    if (const Pack* p = m_packManager->findPack(key))
        m_detailPanel->showPack(*p);
}

void MainWindow::onPacksChanged() {
    m_packGrid->setPacks(m_packManager->packs());

    // Keep detail panel in sync if a pack is already showing
    // (it will just refresh with updated data)
}

void MainWindow::onProgressUpdate(const QString& key, int percent) {
    m_packManager->setProgress(key, percent);
    Pack tmp;
    tmp.key             = key;
    tmp.installProgress = percent;
    tmp.status          = PackStatus::Installing;
    if (Pack* p = m_packManager->findPack(key)) {
        p->installProgress = percent;
        m_packGrid->updatePack(*p);
    }
}

void MainWindow::onOperationDone(const QString& key) {
    m_packManager->reloadFromDisk();
    if (Pack* p = m_packManager->findPack(key)) {
        p->status = PackStatus::Ready;
        m_packGrid->updatePack(*p);
        m_detailPanel->showPack(*p);
    }
}

void MainWindow::onOperationError(const QString& key, const QString& msg) {
    QMessageBox::warning(this, "ModLab Error",
        QString("Operation failed for <b>%1</b>:<br>%2").arg(key, msg));
    m_packManager->reloadFromDisk();
}
