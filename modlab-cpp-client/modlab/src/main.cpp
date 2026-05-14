#include <QApplication>
#include <QFontDatabase>
#include "ui/MainWindow.h"
#include "ui/AeroStyle.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("ModLab");
    app.setApplicationVersion("0.1.0");

    // Load bundled fonts if present, fall back to system fonts
    QFontDatabase::addApplicationFont(":/fonts/SpaceMono-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/SpaceMono-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Exo2-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Exo2-Black.ttf");

    app.setStyleSheet(AeroStyle::sheet());

    MainWindow w;
    w.show();

    return app.exec();
}
