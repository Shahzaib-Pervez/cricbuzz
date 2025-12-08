#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("Cricket App");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Cricket App Team");

    // Create and show main window
    MainWindow window;
    window.show();

    return app.exec();
}
