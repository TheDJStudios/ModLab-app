#pragma once
#include <QWidget>
#include <QPushButton>
#include <QList>

class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget* parent = nullptr);

signals:
    void pageRequested(const QString& page);

private:
    void setActivePage(const QString& page);
    QList<QPushButton*> m_navBtns;
    QString             m_activePage;
};
