#include "mainwindow.h"

#include "editorwidget.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_editor(new EditorWidget(this))
{
    resize(960, 640);
    setCentralWidget(m_editor);
    createMenus();

    connect(m_editor, &EditorWidget::documentChanged, this, &MainWindow::updateWindowTitle);
    updateWindowTitle();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::createMenus()
{
    auto *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), QKeySequence::New, this, &MainWindow::newFile);
    fileMenu->addAction(tr("&Open…"), QKeySequence::Open, this, &MainWindow::openFile);
    fileMenu->addAction(tr("&Save"), QKeySequence::Save, this, &MainWindow::saveFile);
    fileMenu->addAction(tr("Save &As…"), QKeySequence::SaveAs, this, &MainWindow::saveFileAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), QKeySequence::Quit, this, &QWidget::close);
}

void MainWindow::updateWindowTitle()
{
    QString name = tr("Untitled");
    if (!m_editor->filePath().isEmpty()) {
        name = QFileInfo(m_editor->filePath()).fileName();
    }

    if (m_editor->isDirty()) {
        name.prepend(QLatin1Char('*'));
    }

    setWindowTitle(tr("%1 — Apollo IDE").arg(name));
}

bool MainWindow::maybeSave()
{
    if (!m_editor->isDirty()) {
        return true;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("Apollo IDE"),
        tr("The document has been modified.\nDo you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    switch (result) {
    case QMessageBox::Save:
        return saveDocument();
    case QMessageBox::Discard:
        return true;
    default:
        return false;
    }
}

bool MainWindow::saveDocument()
{
    if (m_editor->filePath().isEmpty()) {
        return saveDocumentAs();
    }

    if (!m_editor->save()) {
        QMessageBox::warning(this, tr("Apollo IDE"),
                             tr("Could not save file:\n%1").arg(m_editor->filePath()));
        return false;
    }
    return true;
}

bool MainWindow::saveDocumentAs()
{
    const QString path = QFileDialog::getSaveFileName(
        this,
        tr("Save As"),
        m_editor->filePath().isEmpty() ? QStringLiteral("untitled.pas") : m_editor->filePath(),
        tr("Pascal sources (*.pas);;All files (*)"));

    if (path.isEmpty()) {
        return false;
    }

    if (!m_editor->saveToFile(path)) {
        QMessageBox::warning(this, tr("Apollo IDE"), tr("Could not save file:\n%1").arg(path));
        return false;
    }
    return true;
}

void MainWindow::newFile()
{
    if (!maybeSave()) {
        return;
    }
    m_editor->newFile();
}

void MainWindow::openFile()
{
    if (!maybeSave()) {
        return;
    }

    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        QString(),
        tr("Pascal sources (*.pas);;All files (*)"));

    if (path.isEmpty()) {
        return;
    }

    if (!m_editor->loadFile(path)) {
        QMessageBox::warning(this, tr("Apollo IDE"), tr("Could not open file:\n%1").arg(path));
    }
}

void MainWindow::saveFile()
{
    saveDocument();
}

void MainWindow::saveFileAs()
{
    saveDocumentAs();
}
