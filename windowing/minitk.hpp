#pragma once

#include <QApplication>
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include <functional>
#include <string>

namespace minitk {

enum class Fill { none, x, y, both };
struct PackOptions { int padx = 0; int pady = 0; Fill fill = Fill::none; int stretch = 0; };
struct GridOptions { int row = 0; int column = 0; int rowspan = 1; int columnspan = 1; int padx = 0; int pady = 0; };

class Container;

class Widget {
public:
    virtual ~Widget() = default;
    void pack(const PackOptions& options = {});
    void grid(const GridOptions& options);
    void show() { widget_->show(); }
    void hide() { widget_->hide(); }
    void enabled(bool value) { widget_->setEnabled(value); }
protected:
    Widget(Container& parent, QWidget* widget) : parent_(&parent), widget_(widget) {}
    Container* parent_;
    QWidget* widget_;
};

class Container : public Widget {
public:
    QWidget* native() const { return widget_; }
    void addPacked(QWidget* child, const PackOptions& options) {
        ensurePackLayout();
        Qt::Alignment alignment{};
        if (options.fill == Fill::none) alignment = Qt::AlignHCenter;
        static_cast<QVBoxLayout*>(widget_->layout())->addWidget(child, options.stretch, alignment);
        child->setContentsMargins(options.padx, options.pady, options.padx, options.pady);
    }
    void addGridded(QWidget* child, const GridOptions& options) {
        ensureGridLayout();
        static_cast<QGridLayout*>(widget_->layout())->addWidget(child, options.row, options.column,
                                                                 options.rowspan, options.columnspan);
        child->setContentsMargins(options.padx, options.pady, options.padx, options.pady);
    }
protected:
    Container(Container* parent, QWidget* widget) : Widget(*parent, widget) {}
    void ensurePackLayout() {
        if (!widget_->layout()) { auto* layout = new QVBoxLayout(widget_); layout->setSpacing(8); }
    }
    void ensureGridLayout() {
        if (!widget_->layout()) { auto* layout = new QGridLayout(widget_); layout->setSpacing(8); }
    }
};

inline void Widget::pack(const PackOptions& options) { parent_->addPacked(widget_, options); }
inline void Widget::grid(const GridOptions& options) { parent_->addGridded(widget_, options); }

class Tk {
public:
    Tk(int& argc, char** argv) : app_(argc, argv), central_(new QFrame), root_(nullptr, central_) {
        window_.setCentralWidget(central_);
        auto* layout = new QVBoxLayout(central_);
        layout->setSpacing(8);
        layout->setContentsMargins(12, 12, 12, 12);
    }
    void title(const std::string& value) { window_.setWindowTitle(QString::fromStdString(value)); }
    void geometry(int width, int height) { window_.resize(width, height); }
    void resizable(bool width, bool height) {
        const QSize size = window_.size();
        window_.setMinimumSize(width ? 0 : size.width(), height ? 0 : size.height());
        window_.setMaximumSize(width ? QWIDGETSIZE_MAX : size.width(), height ? QWIDGETSIZE_MAX : size.height());
    }
    void after(int milliseconds, std::function<void()> callback) {
        QTimer::singleShot(milliseconds, central_, std::move(callback));
    }
    Container& content() { return root_; }
    int mainloop() { window_.show(); return app_.exec(); }
    void quit() { app_.quit(); }
private:
    class RootContainer : public Container {
    public: RootContainer(Container* self, QWidget* widget) : Container(self ? self : this, widget) {}
    };
    QApplication app_;
    QMainWindow window_;
    QFrame* central_;
    RootContainer root_;
};

class Frame : public Container {
public: explicit Frame(Container& parent) : Container(&parent, new QFrame(parent.native())) {}
};
class Label : public Widget {
public:
    Label(Container& parent, const std::string& text = {}) : Widget(parent, new QLabel(QString::fromStdString(text), parent.native())) {}
    void text(const std::string& value) { static_cast<QLabel*>(widget_)->setText(QString::fromStdString(value)); }
};
class Button : public Widget {
public:
    Button(Container& parent, const std::string& text, std::function<void()> command = {})
        : Widget(parent, new QPushButton(QString::fromStdString(text), parent.native())) {
        if (command) QObject::connect(static_cast<QPushButton*>(widget_), &QPushButton::clicked, std::move(command));
    }
    void text(const std::string& value) { static_cast<QPushButton*>(widget_)->setText(QString::fromStdString(value)); }
};
class Entry : public Widget {
public:
    Entry(Container& parent, const std::string& value = {}) : Widget(parent, new QLineEdit(QString::fromStdString(value), parent.native())) {}
    std::string get() const { return static_cast<QLineEdit*>(widget_)->text().toStdString(); }
    void set(const std::string& value) { static_cast<QLineEdit*>(widget_)->setText(QString::fromStdString(value)); }
    void placeholder(const std::string& value) { static_cast<QLineEdit*>(widget_)->setPlaceholderText(QString::fromStdString(value)); }
    void onChange(std::function<void(const std::string&)> callback) {
        QObject::connect(static_cast<QLineEdit*>(widget_), &QLineEdit::textChanged,
                         [callback = std::move(callback)](const QString& value) { callback(value.toStdString()); });
    }
};
class Checkbutton : public Widget {
public:
    Checkbutton(Container& parent, const std::string& text, std::function<void(bool)> command = {})
        : Widget(parent, new QCheckBox(QString::fromStdString(text), parent.native())) {
        if (command) QObject::connect(static_cast<QCheckBox*>(widget_), &QCheckBox::toggled, std::move(command));
    }
    bool get() const { return static_cast<QCheckBox*>(widget_)->isChecked(); }
    void set(bool value) { static_cast<QCheckBox*>(widget_)->setChecked(value); }
};
class Text : public Widget {
public:
    explicit Text(Container& parent) : Widget(parent, new QPlainTextEdit(parent.native())) {}
    std::string get() const { return static_cast<QPlainTextEdit*>(widget_)->toPlainText().toStdString(); }
    void set(const std::string& value) { static_cast<QPlainTextEdit*>(widget_)->setPlainText(QString::fromStdString(value)); }
    void append(const std::string& value) { static_cast<QPlainTextEdit*>(widget_)->appendPlainText(QString::fromStdString(value)); }
};

} // namespace minitk
