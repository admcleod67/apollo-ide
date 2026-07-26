#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Apollo IDE"));
    QApplication::setOrganizationName(QStringLiteral("Apollo"));
    QApplication::setApplicationVersion(QStringLiteral("0.0.0"));

    MainWindow window;
    window.show();

    return app.exec();
}
