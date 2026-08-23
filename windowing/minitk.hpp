#pragma once

// A small header-only, Tk-inspired facade over Qt 6 Widgets. Every wrapper
// exposes native() so launcher code can use Qt directly for uncommon features.

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialog>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProcessEnvironment>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace minitk {

enum class Fill { none, x, y, both };
enum class Side { top, bottom, left, right };
enum class Align { start, center, end };
enum class Orientation { horizontal, vertical };

struct PackOptions {
    int padx = 0, pady = 0;
    Fill fill = Fill::none;
    int stretch = 0;
    Side side = Side::top;
    Align align = Align::center;
};
struct GridOptions {
    int row = 0, column = 0, rowspan = 1, columnspan = 1, padx = 0, pady = 0;
    Fill fill = Fill::none;
    Align align = Align::center;
};

namespace detail {
inline QString q(const std::string& value) { return QString::fromStdString(value); }
inline std::string s(const QString& value) { return value.toStdString(); }
inline Qt::Orientation orientation(Orientation value) { return value == Orientation::horizontal ? Qt::Horizontal : Qt::Vertical; }
inline Qt::Alignment alignment(Align align, Fill fill) {
    if (fill != Fill::none) return {};
    if (align == Align::start) return Qt::AlignLeft;
    if (align == Align::end) return Qt::AlignRight;
    return Qt::AlignCenter;
}
inline void sizePolicy(QWidget* widget, Fill fill) {
    const bool x = fill == Fill::x || fill == Fill::both;
    const bool y = fill == Fill::y || fill == Fill::both;
    widget->setSizePolicy(x ? QSizePolicy::Expanding : QSizePolicy::Preferred,
                          y ? QSizePolicy::Expanding : QSizePolicy::Preferred);
}
} // namespace detail

class Container;

class Widget {
public:
    virtual ~Widget() = default;
    QWidget* native() const { return widget_; }
    void pack(const PackOptions& options = {});
    void grid(const GridOptions& options);
    void show() { widget_->show(); }
    void hide() { widget_->hide(); }
    void enabled(bool value) { widget_->setEnabled(value); }
    bool enabled() const { return widget_->isEnabled(); }
    void visible(bool value) { widget_->setVisible(value); }
    bool visible() const { return widget_->isVisible(); }
    void focus() { widget_->setFocus(); }
    void tooltip(const std::string& value) { widget_->setToolTip(detail::q(value)); }
    void objectName(const std::string& value) { widget_->setObjectName(detail::q(value)); }
    void style(const std::string& css) { widget_->setStyleSheet(detail::q(css)); }
    void minimumSize(int w, int h) { widget_->setMinimumSize(w, h); }
    void maximumSize(int w, int h) { widget_->setMaximumSize(w, h); }
    void fixedSize(int w, int h) { widget_->setFixedSize(w, h); }
    void font(const std::string& family, int points, bool bold = false, bool italic = false) {
        QFont f(detail::q(family), points); f.setBold(bold); f.setItalic(italic); widget_->setFont(f);
    }
protected:
    Widget(Container* parent, QWidget* widget) : parent_(parent), widget_(widget) {}
    Container* parent_;
    QWidget* widget_;
};

class Container : public Widget {
public:
    QWidget* native() const { return widget_; }
    void padding(int l, int t, int r, int b) { ensureBox()->setContentsMargins(l, t, r, b); }
    void spacing(int value) { ensureBox()->setSpacing(value); }
    void gridColumnWeight(int column, int weight) { ensureGrid()->setColumnStretch(column, weight); }
    void gridRowWeight(int row, int weight) { ensureGrid()->setRowStretch(row, weight); }
    void addPacked(QWidget* child, const PackOptions& o) {
        auto* layout = ensureBox(o.side == Side::left || o.side == Side::right);
        detail::sizePolicy(child, o.fill);
        child->setContentsMargins(o.padx, o.pady, o.padx, o.pady);
        if (o.side == Side::bottom || o.side == Side::right) layout->insertWidget(0, child, o.stretch, detail::alignment(o.align, o.fill));
        else layout->addWidget(child, o.stretch, detail::alignment(o.align, o.fill));
    }
    void addGridded(QWidget* child, const GridOptions& o) {
        detail::sizePolicy(child, o.fill);
        child->setContentsMargins(o.padx, o.pady, o.padx, o.pady);
        ensureGrid()->addWidget(child, o.row, o.column, o.rowspan, o.columnspan, detail::alignment(o.align, o.fill));
    }
protected:
    Container(Container* parent, QWidget* widget) : Widget(parent, widget) {}
    QBoxLayout* ensureBox(bool horizontal = false) {
        if (!widget_->layout()) {
            QBoxLayout* layout = horizontal ? static_cast<QBoxLayout*>(new QHBoxLayout(widget_)) : static_cast<QBoxLayout*>(new QVBoxLayout(widget_));
            layout->setSpacing(8); return layout;
        }
        auto* layout = qobject_cast<QBoxLayout*>(widget_->layout());
        Q_ASSERT_X(layout, "minitk", "a container cannot mix pack() and grid()"); return layout;
    }
    QGridLayout* ensureGrid() {
        if (!widget_->layout()) { auto* layout = new QGridLayout(widget_); layout->setSpacing(8); return layout; }
        auto* layout = qobject_cast<QGridLayout*>(widget_->layout());
        Q_ASSERT_X(layout, "minitk", "a container cannot mix grid() and pack()"); return layout;
    }
};

inline void Widget::pack(const PackOptions& o) { Q_ASSERT(parent_); parent_->addPacked(widget_, o); }
inline void Widget::grid(const GridOptions& o) { Q_ASSERT(parent_); parent_->addGridded(widget_, o); }

class Menu {
public:
    explicit Menu(QMenu* menu) : menu_(menu) {}
    Menu& item(const std::string& label, std::function<void()> command, const std::string& shortcut = {}) {
        auto* action = menu_->addAction(detail::q(label));
        if (!shortcut.empty()) action->setShortcut(QKeySequence(detail::q(shortcut)));
        QObject::connect(action, &QAction::triggered, menu_, [command = std::move(command)] { if (command) command(); });
        return *this;
    }
    Menu& check(const std::string& label, bool checked, std::function<void(bool)> command) {
        auto* action = menu_->addAction(detail::q(label)); action->setCheckable(true); action->setChecked(checked);
        QObject::connect(action, &QAction::toggled, menu_, std::move(command)); return *this;
    }
    Menu& separator() { menu_->addSeparator(); return *this; }
    Menu& disabledItem(const std::string& label) { auto* action = menu_->addAction(detail::q(label)); action->setEnabled(false); return *this; }
    QMenu* native() const { return menu_; }
private: QMenu* menu_;
};

class Menubar {
public:
    explicit Menubar(QMenuBar* bar) : bar_(bar) {}
    Menu& menu(const std::string& label) {
        menus_.push_back(std::make_unique<Menu>(bar_->addMenu(detail::q(label)))); return *menus_.back();
    }
    QAction* item(const std::string& label, std::function<void()> command) {
        auto* action = bar_->addAction(detail::q(label));
        QObject::connect(action, &QAction::triggered, bar_, [command = std::move(command)] { if (command) command(); }); return action;
    }
    QMenuBar* native() const { return bar_; }
private: QMenuBar* bar_; std::vector<std::unique_ptr<Menu>> menus_;
};

class Tk {
public:
    Tk(int& argc, char** argv) : app_(argc, argv), central_(new QFrame), root_(central_), menubar_(window_.menuBar()) {
        window_.setCentralWidget(central_); window_.resize(800, 500); window_.guard = &closeGuard_;
    }
    void title(const std::string& value) { window_.setWindowTitle(detail::q(value)); }
    void geometry(int w, int h) { window_.resize(w, h); }
    void minimumSize(int w, int h) { window_.setMinimumSize(w, h); }
    void maximumSize(int w, int h) { window_.setMaximumSize(w, h); }
    void resizable(bool width, bool height) {
        const QSize s = window_.size();
        window_.setMinimumSize(width ? 0 : s.width(), height ? 0 : s.height());
        window_.setMaximumSize(width ? QWIDGETSIZE_MAX : s.width(), height ? QWIDGETSIZE_MAX : s.height());
    }
    void icon(const std::string& path) { window_.setWindowIcon(QIcon(detail::q(path))); }
    void style(const std::string& css) { app_.setStyleSheet(detail::q(css)); }
    void darkTheme() { style("QWidget{background:#17191d;color:#e8eaf0} QLineEdit,QPlainTextEdit,QListWidget,QComboBox,QSpinBox{background:#22252b;border:1px solid #3b404a;border-radius:5px;padding:5px} QPushButton{background:#3b6eea;border:0;border-radius:5px;padding:7px 12px;color:white} QPushButton:disabled{background:#343842;color:#7f8490} QProgressBar{border:1px solid #3b404a;border-radius:4px;text-align:center} QProgressBar::chunk{background:#3b6eea}"); }
    void after(int ms, std::function<void()> callback) { QTimer::singleShot(ms, central_, std::move(callback)); }
    void invoke(std::function<void()> callback) { QTimer::singleShot(0, central_, std::move(callback)); }
    void update() { QApplication::processEvents(); }
    void clipboard(const std::string& value) { QApplication::clipboard()->setText(detail::q(value)); }
    std::string clipboard() const { return detail::s(QApplication::clipboard()->text()); }
    void onClose(std::function<bool()> callback) { closeGuard_ = std::move(callback); }
    Container& content() { return root_; }
    Menubar& menubar() { return menubar_; }
    QMainWindow* native() { return &window_; }
    int mainloop() { window_.show(); return app_.exec(); }
    void quit() { app_.quit(); }
private:
    class MainWindow : public QMainWindow {
    public: std::function<bool()>* guard = nullptr;
    protected: void closeEvent(QCloseEvent* e) override { if (!guard || !*guard || (*guard)()) e->accept(); else e->ignore(); }
    };
    class Root : public Container { public: explicit Root(QWidget* w) : Container(nullptr, w) {} };
    QApplication app_; MainWindow window_; QFrame* central_; Root root_; Menubar menubar_; std::function<bool()> closeGuard_;
};

class Frame : public Container { public: explicit Frame(Container& p) : Container(&p, new QFrame(p.native())) {} };
class GroupBox : public Container { public: GroupBox(Container& p, const std::string& title) : Container(&p, new QGroupBox(detail::q(title), p.native())) {} };
class Label : public Widget {
public: Label(Container& p, const std::string& value = {}) : Widget(&p, new QLabel(detail::q(value), p.native())) {}
    void text(const std::string& v) { label()->setText(detail::q(v)); } std::string text() const { return detail::s(label()->text()); }
    void wrap(bool v = true) { label()->setWordWrap(v); } void selectable(bool v = true) { label()->setTextInteractionFlags(v ? Qt::TextSelectableByMouse : Qt::NoTextInteraction); }
private: QLabel* label() const { return static_cast<QLabel*>(widget_); }
};
class Image : public Widget {
public: Image(Container& p, const std::string& path = {}) : Widget(&p, new QLabel(p.native())) { if (!path.empty()) load(path); }
    bool load(const std::string& path, int w = 0, int h = 0) { QPixmap image(detail::q(path)); if (image.isNull()) return false; if (w && h) image = image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation); static_cast<QLabel*>(widget_)->setPixmap(image); return true; }
    void scaled(bool v = true) { static_cast<QLabel*>(widget_)->setScaledContents(v); }
};
class Button : public Widget {
public: Button(Container& p, const std::string& value, std::function<void()> command = {}) : Widget(&p, new QPushButton(detail::q(value), p.native())) { onClick(std::move(command)); }
    void text(const std::string& v) { button()->setText(detail::q(v)); } void defaultButton(bool v = true) { button()->setDefault(v); }
    void onClick(std::function<void()> command) { if (command) QObject::connect(button(), &QPushButton::clicked, widget_, [command = std::move(command)] { command(); }); }
private: QPushButton* button() const { return static_cast<QPushButton*>(widget_); }
};
class Entry : public Widget {
public: Entry(Container& p, const std::string& value = {}) : Widget(&p, new QLineEdit(detail::q(value), p.native())) {}
    std::string get() const { return detail::s(entry()->text()); } void set(const std::string& v) { entry()->setText(detail::q(v)); }
    void placeholder(const std::string& v) { entry()->setPlaceholderText(detail::q(v)); } void password(bool v = true) { entry()->setEchoMode(v ? QLineEdit::Password : QLineEdit::Normal); }
    void readOnly(bool v = true) { entry()->setReadOnly(v); } void clear() { entry()->clear(); }
    void onChange(std::function<void(const std::string&)> cb) { QObject::connect(entry(), &QLineEdit::textChanged, widget_, [cb = std::move(cb)](const QString& v) { cb(detail::s(v)); }); }
    void onSubmit(std::function<void()> cb) { QObject::connect(entry(), &QLineEdit::returnPressed, widget_, std::move(cb)); }
private: QLineEdit* entry() const { return static_cast<QLineEdit*>(widget_); }
};
class Checkbutton : public Widget {
public: Checkbutton(Container& p, const std::string& value, std::function<void(bool)> cb = {}) : Widget(&p, new QCheckBox(detail::q(value), p.native())) { if (cb) QObject::connect(check(), &QCheckBox::toggled, widget_, std::move(cb)); }
    bool get() const { return check()->isChecked(); } void set(bool v) { check()->setChecked(v); }
private: QCheckBox* check() const { return static_cast<QCheckBox*>(widget_); }
};
class Text : public Widget {
public: explicit Text(Container& p) : Widget(&p, new QPlainTextEdit(p.native())) {}
    std::string get() const { return detail::s(text()->toPlainText()); } void set(const std::string& v) { text()->setPlainText(detail::q(v)); }
    void append(const std::string& v) { text()->appendPlainText(detail::q(v)); } void clear() { text()->clear(); } void readOnly(bool v = true) { text()->setReadOnly(v); }
    void maximumBlocks(int v) { text()->document()->setMaximumBlockCount(v); } void onChange(std::function<void()> cb) { QObject::connect(text(), &QPlainTextEdit::textChanged, widget_, std::move(cb)); }
private: QPlainTextEdit* text() const { return static_cast<QPlainTextEdit*>(widget_); }
};
class Combobox : public Widget {
public: Combobox(Container& p, const std::vector<std::string>& values = {}) : Widget(&p, new QComboBox(p.native())) { setValues(values); }
    void setValues(const std::vector<std::string>& v) { combo()->clear(); for (const auto& x : v) combo()->addItem(detail::q(x)); } void add(const std::string& v) { combo()->addItem(detail::q(v)); }
    std::string get() const { return detail::s(combo()->currentText()); } int index() const { return combo()->currentIndex(); } void select(int i) { combo()->setCurrentIndex(i); } void select(const std::string& v) { combo()->setCurrentText(detail::q(v)); }
    void editable(bool v = true) { combo()->setEditable(v); } void onChange(std::function<void(const std::string&)> cb) { QObject::connect(combo(), &QComboBox::currentTextChanged, widget_, [cb = std::move(cb)](const QString& v) { cb(detail::s(v)); }); }
private: QComboBox* combo() const { return static_cast<QComboBox*>(widget_); }
};
class Listbox : public Widget {
public: explicit Listbox(Container& p) : Widget(&p, new QListWidget(p.native())) {}
    void add(const std::string& v) { list()->addItem(detail::q(v)); } void clear() { list()->clear(); } int count() const { return list()->count(); } int index() const { return list()->currentRow(); }
    std::string get() const { auto* i = list()->currentItem(); return i ? detail::s(i->text()) : std::string{}; } void select(int r) { list()->setCurrentRow(r); } void remove(int r) { delete list()->takeItem(r); }
    void onSelect(std::function<void(int, const std::string&)> cb) { QObject::connect(list(), &QListWidget::currentRowChanged, widget_, [this, cb = std::move(cb)](int r) { cb(r, get()); }); }
    void onActivate(std::function<void(int, const std::string&)> cb) { QObject::connect(list(), &QListWidget::itemDoubleClicked, widget_, [this, cb = std::move(cb)] { cb(index(), get()); }); }
private: QListWidget* list() const { return static_cast<QListWidget*>(widget_); }
};
class Progressbar : public Widget {
public: Progressbar(Container& p, int maximum = 100) : Widget(&p, new QProgressBar(p.native())) { range(0, maximum); }
    void set(int v) { bar()->setValue(v); } int get() const { return bar()->value(); } void range(int a, int b) { bar()->setRange(a, b); } void indeterminate(bool v = true) { if (v) range(0, 0); else range(0, 100); } void textVisible(bool v) { bar()->setTextVisible(v); }
private: QProgressBar* bar() const { return static_cast<QProgressBar*>(widget_); }
};
class Slider : public Widget {
public: Slider(Container& p, int a, int b, Orientation o = Orientation::horizontal) : Widget(&p, new QSlider(detail::orientation(o), p.native())) { slider()->setRange(a, b); }
    int get() const { return slider()->value(); } void set(int v) { slider()->setValue(v); } void onChange(std::function<void(int)> cb) { QObject::connect(slider(), &QSlider::valueChanged, widget_, std::move(cb)); }
private: QSlider* slider() const { return static_cast<QSlider*>(widget_); }
};
class Spinbox : public Widget {
public: Spinbox(Container& p, int a, int b) : Widget(&p, new QSpinBox(p.native())) { spin()->setRange(a, b); }
    int get() const { return spin()->value(); } void set(int v) { spin()->setValue(v); } void suffix(const std::string& v) { spin()->setSuffix(detail::q(v)); } void onChange(std::function<void(int)> cb) { QObject::connect(spin(), &QSpinBox::valueChanged, widget_, std::move(cb)); }
private: QSpinBox* spin() const { return static_cast<QSpinBox*>(widget_); }
};
class Separator : public Widget {
public: Separator(Container& p, Orientation o = Orientation::horizontal) : Widget(&p, new QFrame(p.native())) { auto* f = static_cast<QFrame*>(widget_); f->setFrameShape(o == Orientation::horizontal ? QFrame::HLine : QFrame::VLine); f->setFrameShadow(QFrame::Sunken); }
};
class ScrollFrame : public Container {
public: explicit ScrollFrame(Container& p) : Container(&p, new QFrame), area_(new QScrollArea(p.native())) { area_->setWidgetResizable(true); area_->setWidget(widget_); }
    void pack(const PackOptions& o = {}) { parent_->addPacked(area_, o); } void grid(const GridOptions& o) { parent_->addGridded(area_, o); } QScrollArea* shell() const { return area_; }
private: QScrollArea* area_;
};
class Tabs : public Widget {
public: explicit Tabs(Container& p) : Widget(&p, new QTabWidget(p.native())) {}
    Container& add(const std::string& title) { auto page = std::make_unique<Page>(new QFrame(tabs())); tabs()->addTab(page->native(), detail::q(title)); pages_.push_back(std::move(page)); return *pages_.back(); }
    int index() const { return tabs()->currentIndex(); } void select(int i) { tabs()->setCurrentIndex(i); } void onChange(std::function<void(int)> cb) { QObject::connect(tabs(), &QTabWidget::currentChanged, widget_, std::move(cb)); }
private: class Page : public Container { public: explicit Page(QWidget* w) : Container(nullptr, w) {} }; QTabWidget* tabs() const { return static_cast<QTabWidget*>(widget_); } std::vector<std::unique_ptr<Page>> pages_;
};
class Window : public Container {
public: explicit Window(Tk& app, const std::string& title = {}) : Container(nullptr, new QFrame), window_(new QDialog(app.native())) { window_->setWindowTitle(detail::q(title)); auto* l = new QVBoxLayout(window_); l->addWidget(widget_); }
    void title(const std::string& v) { window_->setWindowTitle(detail::q(v)); } void geometry(int w, int h) { window_->resize(w, h); } void show() { window_->show(); } int exec() { return window_->exec(); } void close() { window_->close(); } QDialog* nativeWindow() const { return window_; }
private: QDialog* window_;
};

namespace dialog {
inline void info(Tk& a, const std::string& t, const std::string& m) { QMessageBox::information(a.native(), detail::q(t), detail::q(m)); }
inline void warning(Tk& a, const std::string& t, const std::string& m) { QMessageBox::warning(a.native(), detail::q(t), detail::q(m)); }
inline void error(Tk& a, const std::string& t, const std::string& m) { QMessageBox::critical(a.native(), detail::q(t), detail::q(m)); }
inline bool askYesNo(Tk& a, const std::string& t, const std::string& m) { return QMessageBox::question(a.native(), detail::q(t), detail::q(m)) == QMessageBox::Yes; }
inline std::string openFile(Tk& a, const std::string& t = "Open file", const std::string& f = {}) { return detail::s(QFileDialog::getOpenFileName(a.native(), detail::q(t), {}, detail::q(f))); }
inline std::string saveFile(Tk& a, const std::string& t = "Save file", const std::string& f = {}) { return detail::s(QFileDialog::getSaveFileName(a.native(), detail::q(t), {}, detail::q(f))); }
inline std::string directory(Tk& a, const std::string& t = "Choose directory") { return detail::s(QFileDialog::getExistingDirectory(a.native(), detail::q(t))); }
} // namespace dialog

inline bool openUrl(const std::string& url) { return QDesktopServices::openUrl(QUrl(detail::q(url))); }
inline bool revealFile(const std::string& path) { return QDesktopServices::openUrl(QUrl::fromLocalFile(detail::q(path))); }

class Process {
public:
    explicit Process(QObject* parent = nullptr) : process_(parent) {
        QObject::connect(&process_, &QProcess::readyReadStandardOutput, [&] { if (onOutput_) onOutput_(detail::s(QString::fromUtf8(process_.readAllStandardOutput()))); });
        QObject::connect(&process_, &QProcess::readyReadStandardError, [&] { if (onError_) onError_(detail::s(QString::fromUtf8(process_.readAllStandardError()))); });
        QObject::connect(&process_, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), [&](int code, QProcess::ExitStatus) { if (onFinished_) onFinished_(code); });
    }
    void workingDirectory(const std::string& p) { process_.setWorkingDirectory(detail::q(p)); }
    void environment(const std::string& k, const std::string& v) { auto e = process_.processEnvironment(); if (e.isEmpty()) e = QProcessEnvironment::systemEnvironment(); e.insert(detail::q(k), detail::q(v)); process_.setProcessEnvironment(e); }
    void onOutput(std::function<void(const std::string&)> cb) { onOutput_ = std::move(cb); } void onError(std::function<void(const std::string&)> cb) { onError_ = std::move(cb); } void onFinished(std::function<void(int)> cb) { onFinished_ = std::move(cb); }
    void start(const std::string& executable, const std::vector<std::string>& arguments = {}) { QStringList args; for (const auto& x : arguments) args.push_back(detail::q(x)); process_.start(detail::q(executable), args); }
    bool running() const { return process_.state() != QProcess::NotRunning; } void terminate() { process_.terminate(); } void kill() { process_.kill(); } QProcess* native() { return &process_; }
private: QProcess process_; std::function<void(const std::string&)> onOutput_, onError_; std::function<void(int)> onFinished_;
};

} // namespace minitk
