#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Apollo IDE"));
    QApplication::setOrganizationName(QStringLiteral("Apollo"));
    QApplication::setApplicationVersion(QStringLiteral(APOLLO_IDE_VERSION));

    MainWindow window;
    window.show();

    return app.exec();
}
