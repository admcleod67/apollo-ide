#include "mainwindow.h"

#include "compileradapter.h"
#include "editorwidget.h"
#include "settingsdialog.h"
#include "toolchainsettings.h"
#include "vmadapter.h"

#include <QAction>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTabWidget>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_editor(new EditorWidget(this))
    , m_outputTabs(new QTabWidget(this))
    , m_compilerOutput(new QPlainTextEdit(this))
    , m_runtimeOutput(new QPlainTextEdit(this))
    , m_compiler(new CompilerAdapter(this))
    , m_vm(new VmAdapter(this))
{
    resize(960, 640);

    m_compilerOutput->setReadOnly(true);
    m_compilerOutput->setPlaceholderText(tr("Compiler output"));
    m_runtimeOutput->setReadOnly(true);
    m_runtimeOutput->setPlaceholderText(tr("Runtime output"));

    m_outputTabs->addTab(m_compilerOutput, tr("Compiler"));
    m_outputTabs->addTab(m_runtimeOutput, tr("Runtime"));

    auto *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_editor);
    splitter->addWidget(m_outputTabs);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    createActions();
    createMenus();
    createToolBar();
    applyToolchainPaths();

    connect(m_editor, &EditorWidget::documentChanged, this, &MainWindow::updateWindowTitle);
    connect(m_compiler, &CompilerAdapter::compileStarted, this, &MainWindow::onCompileStarted);
    connect(m_compiler, &CompilerAdapter::compileFinished, this, &MainWindow::onCompileFinished);
    connect(m_compiler, &CompilerAdapter::compileFailedToStart, this,
            &MainWindow::onCompileFailedToStart);
    connect(m_vm, &VmAdapter::runStarted, this, &MainWindow::onRunStarted);
    connect(m_vm, &VmAdapter::runFinished, this, &MainWindow::onRunFinished);
    connect(m_vm, &VmAdapter::runFailedToStart, this, &MainWindow::onRunFailedToStart);

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

void MainWindow::createActions()
{
    m_newAction = new QAction(tr("&New"), this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    connect(m_newAction, &QAction::triggered, this, &MainWindow::newFile);

    m_openAction = new QAction(tr("&Open..."), this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);

    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    m_saveAsAction = new QAction(tr("Save &As..."), this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);

    m_findAction = new QAction(tr("&Find..."), this);
    m_findAction->setShortcut(QKeySequence::Find);
    connect(m_findAction, &QAction::triggered, this, &MainWindow::findInDocument);

    m_preferencesAction = new QAction(tr("&Preferences..."), this);
    m_preferencesAction->setMenuRole(QAction::PreferencesRole);
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::openPreferences);

    m_quitAction = new QAction(tr("&Quit"), this);
    m_quitAction->setShortcut(QKeySequence::Quit);
    m_quitAction->setMenuRole(QAction::QuitRole);
    connect(m_quitAction, &QAction::triggered, this, &QWidget::close);

    m_compileAction = new QAction(tr("&Compile"), this);
    m_compileAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    m_compileAction->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    connect(m_compileAction, &QAction::triggered, this, &MainWindow::compileFile);

    m_runAction = new QAction(tr("&Run"), this);
    m_runAction->setShortcut(QKeySequence(Qt::Key_F5));
    m_runAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_runAction, &QAction::triggered, this, &MainWindow::runFile);
}

void MainWindow::createMenus()
{
    auto *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_saveAction);
    fileMenu->addAction(m_saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_preferencesAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_quitAction);

    auto *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(m_findAction);

    auto *buildMenu = menuBar()->addMenu(tr("&Build"));
    buildMenu->addAction(m_compileAction);

    auto *runMenu = menuBar()->addMenu(tr("&Run"));
    runMenu->addAction(m_runAction);
}

void MainWindow::createToolBar()
{
    auto *toolBar = addToolBar(tr("Main"));
    toolBar->setMovable(false);
    toolBar->addAction(m_newAction);
    toolBar->addAction(m_openAction);
    toolBar->addAction(m_saveAction);
    toolBar->addSeparator();
    toolBar->addAction(m_compileAction);
    toolBar->addAction(m_runAction);
}

void MainWindow::applyToolchainPaths()
{
    m_compiler->setCompilerPath(ToolchainSettings::resolveCompilerPath());
    m_vm->setVmPath(ToolchainSettings::resolveVmPath());
    updateToolchainStatus();
}

void MainWindow::updateToolchainStatus()
{
    const QString compiler = m_compiler->compilerPath();
    const QString vm = m_vm->vmPath();
    const QString text =
        tr("Compiler: %1 | VM: %2")
            .arg(QFileInfo(compiler).fileName(), QFileInfo(vm).fileName());
    statusBar()->showMessage(text);
    statusBar()->setToolTip(tr("Compiler: %1\nVM: %2").arg(compiler, vm));
}

void MainWindow::openPreferences()
{
    SettingsDialog dialog(this);
    dialog.setCompilerPath(ToolchainSettings::storedCompilerPath());
    dialog.setVmPath(ToolchainSettings::storedVmPath());
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    ToolchainSettings::save(dialog.compilerPath(), dialog.vmPath());
    applyToolchainPaths();
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

bool MainWindow::prepareSourceForBuild()
{
    if (m_editor->isDirty() || m_editor->filePath().isEmpty()) {
        if (!saveDocument()) {
            return false;
        }
    }

    if (m_editor->filePath().isEmpty()) {
        QMessageBox::information(this, tr("Apollo IDE"),
                                 tr("Save the source file before compiling."));
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

void MainWindow::findInDocument()
{
    m_editor->showFind();
}

void MainWindow::compileFile()
{
    if (m_compiler->isRunning() || m_vm->isRunning()) {
        return;
    }

    m_runAfterCompile = false;
    if (!prepareSourceForBuild()) {
        return;
    }

    m_compiler->compileFile(m_editor->filePath());
}

void MainWindow::runFile()
{
    if (m_compiler->isRunning() || m_vm->isRunning()) {
        return;
    }

    if (!prepareSourceForBuild()) {
        return;
    }

    m_runAfterCompile = true;
    m_compiler->compileFile(m_editor->filePath());
}

void MainWindow::onCompileStarted()
{
    m_outputTabs->setCurrentWidget(m_compilerOutput);
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
    m_outputTabs->setCurrentWidget(m_compilerOutput);

    const bool shouldRun = m_runAfterCompile;
    m_runAfterCompile = false;

    if (shouldRun && ok && !m_compiler->lastBytecodePath().isEmpty()) {
        m_vm->runBytecode(m_compiler->lastBytecodePath());
    }
}

void MainWindow::onCompileFailedToStart(const QString &message)
{
    m_runAfterCompile = false;
    m_outputTabs->setCurrentWidget(m_compilerOutput);
    m_compilerOutput->setPlainText(message);
}

void MainWindow::onRunStarted()
{
    m_outputTabs->setCurrentWidget(m_runtimeOutput);
    m_runtimeOutput->setPlainText(tr("Running…"));
}

void MainWindow::onRunFinished(bool ok, const QString &stdoutText, const QString &stderrText,
                               int exitCode)
{
    QString text = stdoutText;
    if (!text.isEmpty() && !text.endsWith(QLatin1Char('\n'))) {
        text += QLatin1Char('\n');
    }
    if (!stderrText.isEmpty()) {
        text += stderrText;
        if (!text.endsWith(QLatin1Char('\n'))) {
            text += QLatin1Char('\n');
        }
    }

    if (ok) {
        text += tr("Run finished.");
    } else {
        text += tr("Run failed (exit %1).").arg(exitCode);
    }

    m_runtimeOutput->setPlainText(text);
    m_outputTabs->setCurrentWidget(m_runtimeOutput);
}

void MainWindow::onRunFailedToStart(const QString &message)
{
    m_outputTabs->setCurrentWidget(m_runtimeOutput);
    m_runtimeOutput->setPlainText(message);
}
