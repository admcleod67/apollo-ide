#include "mainwindow.h"

#include "compileradapter.h"
#include "editorwidget.h"

#include <QAction>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcessEnvironment>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_editor(new EditorWidget(this))
    , m_compilerOutput(new QPlainTextEdit(this))
    , m_compiler(new CompilerAdapter(this))
{
    resize(960, 640);

    m_compilerOutput->setReadOnly(true);
    m_compilerOutput->setPlaceholderText(tr("Compiler output"));

    auto *outputContainer = new QWidget(this);
    auto *outputLayout = new QVBoxLayout(outputContainer);
    outputLayout->setContentsMargins(0, 0, 0, 0);
    outputLayout->setSpacing(2);
    outputLayout->addWidget(new QLabel(tr("Compiler"), outputContainer));
    outputLayout->addWidget(m_compilerOutput);

    auto *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_editor);
    splitter->addWidget(outputContainer);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    const QString envCompiler =
        QProcessEnvironment::systemEnvironment().value(QStringLiteral("APOLLO_COMPILER"));
    if (!envCompiler.isEmpty()) {
        m_compiler->setCompilerPath(envCompiler);
    }

    createMenus();

    connect(m_editor, &EditorWidget::documentChanged, this, &MainWindow::updateWindowTitle);
    connect(m_compiler, &CompilerAdapter::compileStarted, this, &MainWindow::onCompileStarted);
    connect(m_compiler, &CompilerAdapter::compileFinished, this, &MainWindow::onCompileFinished);
    connect(m_compiler, &CompilerAdapter::compileFailedToStart, this,
            &MainWindow::onCompileFailedToStart);

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

    auto *newAction = fileMenu->addAction(tr("&New"));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);

    auto *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    auto *saveAction = fileMenu->addAction(tr("&Save"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    auto *saveAsAction = fileMenu->addAction(tr("Save &As..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);

    fileMenu->addSeparator();

    auto *quitAction = fileMenu->addAction(tr("&Quit"));
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    auto *buildMenu = menuBar()->addMenu(tr("&Build"));
    auto *compileAction = buildMenu->addAction(tr("&Compile"));
    compileAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    connect(compileAction, &QAction::triggered, this, &MainWindow::compileFile);
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
        m_editor->filePath(),
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

void MainWindow::compileFile()
{
    if (m_compiler->isRunning()) {
        return;
    }

    if (m_editor->isDirty() || m_editor->filePath().isEmpty()) {
        if (!saveDocument()) {
            return;
        }
    }

    if (m_editor->filePath().isEmpty()) {
        QMessageBox::information(this, tr("Apollo IDE"),
                                 tr("Save the source file before compiling."));
        return;
    }

    m_compiler->compileFile(m_editor->filePath());
}

void MainWindow::onCompileStarted()
{
    m_compilerOutput->setPlainText(tr("Compiling…"));
}

void MainWindow::onCompileFinished(bool ok, const QString &stdoutText,
                                   const QString &stderrText, int exitCode)
{
    Q_UNUSED(stdoutText);

    QString text;
    if (!stderrText.isEmpty()) {
        text = stderrText;
        if (!text.endsWith(QLatin1Char('\n'))) {
            text += QLatin1Char('\n');
        }
    }

    if (ok) {
        text += tr("Compile succeeded.");
        if (!m_compiler->lastBytecodePath().isEmpty()) {
            text += QLatin1Char('\n');
            text += tr("Wrote %1").arg(m_compiler->lastBytecodePath());
        }
    } else {
        text += tr("Compile failed (exit %1).").arg(exitCode);
    }

    m_compilerOutput->setPlainText(text);
}

void MainWindow::onCompileFailedToStart(const QString &message)
{
    m_compilerOutput->setPlainText(message);
}
