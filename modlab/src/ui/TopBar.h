#pragma once
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class TopBar : public QWidget {
    Q_OBJECT
public:
    explicit TopBar(QWidget* parent = nullptr);
    void setTitle(const QString& title, const QString& sub = {});

signals:
    void newPackRequested();
    void searchChanged(const QString& text);

private:
    QLabel*      m_titleLabel = nullptr;
    QLineEdit*   m_search     = nullptr;
    QPushButton* m_newBtn     = nullptr;
};
