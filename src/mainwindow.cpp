#include "mainwindow.h"

#include <QKeySequence>
#include <QLabel>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Apollo IDE"));
    resize(960, 640);

    auto *placeholder = new QLabel(tr("Apollo IDE — Milestone 0 skeleton"), this);
    placeholder->setAlignment(Qt::AlignCenter);
    setCentralWidget(placeholder);

    auto *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Quit"), QKeySequence::Quit, this, &QWidget::close);
}
