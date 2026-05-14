#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include "../core/Pack.h"

struct NewPackData {
    QString name;
    QString version;
    Loader  loader;
};

class NewPackDialog : public QDialog {
    Q_OBJECT
public:
    explicit NewPackDialog(QWidget* parent = nullptr);
    NewPackData result() const { return m_result; }

private:
    void buildLayout();
    void validate();

    QLineEdit*   m_nameEdit    = nullptr;
    QLineEdit*   m_versionEdit = nullptr;
    Loader       m_loader      = Loader::Fabric;
    QPushButton* m_btnCreate   = nullptr;
    NewPackData  m_result;

    // Loader selector buttons
    QPushButton* m_loaderFabric  = nullptr;
    QPushButton* m_loaderForge   = nullptr;
    QPushButton* m_loaderVanilla = nullptr;
    void selectLoader(Loader l);
};
