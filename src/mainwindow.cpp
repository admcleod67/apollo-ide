#include "mainwindow.h"

#include "compileradapter.h"
#include "editorwidget.h"
#include "settingsdialog.h"
#include "toolchainsettings.h"
#include "vmadapter.h"

#include <QAction>
#include <QCloseEvent>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTabBar>
#include <QTabWidget>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_editorTabs(new QTabWidget(this))
    , m_outputTabs(new QTabWidget(this))
    , m_compilerOutput(new QPlainTextEdit(this))
    , m_runtimeOutput(new QPlainTextEdit(this))
    , m_compiler(new CompilerAdapter(this))
    , m_vm(new VmAdapter(this))
{
    resize(960, 640);

    m_editorTabs->setDocumentMode(true);
    m_editorTabs->setTabsClosable(true);
    m_editorTabs->tabBar()->installEventFilter(this);
    addEditorTab();

    m_compilerOutput->setReadOnly(true);
    m_compilerOutput->setPlaceholderText(tr("Compiler output"));
    m_runtimeOutput->setReadOnly(true);
    m_runtimeOutput->setPlaceholderText(tr("Runtime output"));

    m_outputTabs->addTab(m_compilerOutput, tr("Compiler"));
    m_outputTabs->addTab(m_runtimeOutput, tr("Runtime"));

    auto *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_editorTabs);
    splitter->addWidget(m_outputTabs);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    createActions();
    createMenus();
    createToolBar();
    applyToolchainPaths();

    connect(m_editorTabs, &QTabWidget::currentChanged, this, &MainWindow::onEditorTabChanged);
    connect(m_editorTabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeEditorTab);
    connect(m_compiler, &CompilerAdapter::compileStarted, this, &MainWindow::onCompileStarted);
    connect(m_compiler, &CompilerAdapter::compileFinished, this, &MainWindow::onCompileFinished);
    connect(m_compiler, &CompilerAdapter::compileFailedToStart, this,
            &MainWindow::onCompileFailedToStart);
    connect(m_vm, &VmAdapter::runStarted, this, &MainWindow::onRunStarted);
    connect(m_vm, &VmAdapter::runFinished, this, &MainWindow::onRunFinished);
    connect(m_vm, &VmAdapter::runFailedToStart, this, &MainWindow::onRunFailedToStart);

    updateBuildActionsEnabled();
    updateWindowTitle();
}

EditorWidget *MainWindow::currentEditor() const
{
    return qobject_cast<EditorWidget *>(m_editorTabs->currentWidget());
}

int MainWindow::editorTabCount() const
{
    return m_editorTabs->count();
}

QString MainWindow::editorTabText(int index) const
{
    if (index < 0 || index >= m_editorTabs->count()) {
        return {};
    }
    return m_editorTabs->tabText(index);
}

void MainWindow::newUntitledTab()
{
    addEditorTab();
}

bool MainWindow::openPath(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }

    const QString absolutePath = QFileInfo(path).absoluteFilePath();
    if (EditorWidget *existing = findEditorForPath(absolutePath)) {
        m_editorTabs->setCurrentWidget(existing);
        return true;
    }

    EditorWidget *editor = currentEditor();
    const bool reuseCurrent = editor != nullptr && editor->filePath().isEmpty() && !editor->isDirty()
        && editor->toPlainText().isEmpty();

    if (!reuseCurrent) {
        editor = addEditorTab();
    }

    if (!editor->loadFile(absolutePath)) {
        QMessageBox::warning(this, tr("Apollo IDE"),
                             tr("Could not open file:\n%1").arg(absolutePath));
        return false;
    }

    updateTabLabel(editor);
    updateWindowTitle();
    return true;
}

bool MainWindow::closeTabAt(int index)
{
    return closeEditorTab(index);
}

EditorWidget *MainWindow::addEditorTab()
{
    auto *editor = new EditorWidget(m_editorTabs);
    const int index = m_editorTabs->addTab(editor, tr("Untitled"));
    m_editorTabs->setCurrentIndex(index);
    connect(editor, &EditorWidget::documentChanged, this, &MainWindow::onEditorDocumentChanged);
    return editor;
}

EditorWidget *MainWindow::findEditorForPath(const QString &absolutePath) const
{
    const QString target = QDir::cleanPath(absolutePath);
    for (int i = 0; i < m_editorTabs->count(); ++i) {
        auto *editor = qobject_cast<EditorWidget *>(m_editorTabs->widget(i));
        if (editor == nullptr || editor->filePath().isEmpty()) {
            continue;
        }
        if (QDir::cleanPath(QFileInfo(editor->filePath()).absoluteFilePath()) == target) {
            return editor;
        }
    }
    return nullptr;
}

void MainWindow::onEditorTabChanged(int /*index*/)
{
    if (EditorWidget *editor = currentEditor()) {
        editor->setFocus(Qt::OtherFocusReason);
    }
    updateWindowTitle();
}

void MainWindow::onEditorDocumentChanged()
{
    auto *editor = qobject_cast<EditorWidget *>(sender());
    if (editor != nullptr) {
        updateTabLabel(editor);
    }
    updateWindowTitle();
}

void MainWindow::updateTabLabel(EditorWidget *editor)
{
    if (editor == nullptr) {
        return;
    }
    const int index = m_editorTabs->indexOf(editor);
    if (index < 0) {
        return;
    }

    QString label = tr("Untitled");
    if (!editor->filePath().isEmpty()) {
        label = QFileInfo(editor->filePath()).fileName();
    }
    if (editor->isDirty()) {
        label.prepend(QLatin1Char('*'));
    }
    m_editorTabs->setTabText(index, label);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_editorTabs->tabBar() && event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::MiddleButton) {
            const int index = m_editorTabs->tabBar()->tabAt(mouseEvent->pos());
            if (index >= 0) {
                closeEditorTab(index);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for (int i = 0; i < m_editorTabs->count(); ++i) {
        auto *editor = qobject_cast<EditorWidget *>(m_editorTabs->widget(i));
        if (editor == nullptr || !editor->isDirty()) {
            continue;
        }
        m_editorTabs->setCurrentWidget(editor);
        if (!maybeSave(editor)) {
            event->ignore();
            return;
        }
    }
    event->accept();
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

    m_closeAction = new QAction(tr("&Close"), this);
    m_closeAction->setShortcut(QKeySequence::Close);
    connect(m_closeAction, &QAction::triggered, this, &MainWindow::closeCurrentTab);

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
    fileMenu->addAction(m_closeAction);
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
    m_vm->setModulesPath(ToolchainSettings::resolveModulesPath());
    updateToolchainStatus();
}

void MainWindow::updateToolchainStatus()
{
    const QString compiler = m_compiler->compilerPath();
    const QString vm = m_vm->vmPath();
    const QString modules = m_vm->modulesPath();
    const QString modulesLabel =
        modules.isEmpty() ? tr("(unset)") : QFileInfo(modules).fileName();
    const QString text =
        tr("Compiler: %1 | VM: %2 | Modules: %3")
            .arg(QFileInfo(compiler).fileName(), QFileInfo(vm).fileName(), modulesLabel);
    statusBar()->showMessage(text);
    statusBar()->setToolTip(
        tr("Compiler: %1\nVM: %2\nModules: %3")
            .arg(compiler, vm, modules.isEmpty() ? tr("(unset)") : modules));
}

void MainWindow::openPreferences()
{
    SettingsDialog dialog(this);
    dialog.setCompilerPath(ToolchainSettings::storedCompilerPath());
    dialog.setVmPath(ToolchainSettings::storedVmPath());
    dialog.setModulesPath(ToolchainSettings::storedModulesPath());
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    ToolchainSettings::save(dialog.compilerPath(), dialog.vmPath(), dialog.modulesPath());
    applyToolchainPaths();
}

void MainWindow::updateWindowTitle()
{
    EditorWidget *editor = currentEditor();
    if (editor == nullptr) {
        setWindowTitle(tr("Apollo IDE"));
        return;
    }

    QString name = tr("Untitled");
    if (!editor->filePath().isEmpty()) {
        name = QFileInfo(editor->filePath()).fileName();
    }

    if (editor->isDirty()) {
        name.prepend(QLatin1Char('*'));
    }

    setWindowTitle(tr("%1 — Apollo IDE").arg(name));
}

bool MainWindow::maybeSave(EditorWidget *editor)
{
    if (editor == nullptr || !editor->isDirty()) {
        return true;
    }

    m_editorTabs->setCurrentWidget(editor);

    const QString name = editor->filePath().isEmpty()
        ? tr("Untitled")
        : QFileInfo(editor->filePath()).fileName();

    const auto result = QMessageBox::warning(
        this,
        tr("Apollo IDE"),
        tr("The document \"%1\" has been modified.\nDo you want to save your changes?").arg(name),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    switch (result) {
    case QMessageBox::Save:
        return saveDocument(editor);
    case QMessageBox::Discard:
        return true;
    default:
        return false;
    }
}

bool MainWindow::saveDocument(EditorWidget *editor)
{
    if (editor == nullptr) {
        return false;
    }

    if (editor->filePath().isEmpty()) {
        return saveDocumentAs(editor);
    }

    if (!editor->save()) {
        QMessageBox::warning(this, tr("Apollo IDE"),
                             tr("Could not save file:\n%1").arg(editor->filePath()));
        return false;
    }
    updateTabLabel(editor);
    return true;
}

bool MainWindow::saveDocumentAs(EditorWidget *editor)
{
    if (editor == nullptr) {
        return false;
    }

    m_editorTabs->setCurrentWidget(editor);

    const QString path = QFileDialog::getSaveFileName(
        this,
        tr("Save As"),
        editor->filePath(),
        tr("Pascal sources (*.pas);;All files (*)"));

    if (path.isEmpty()) {
        return false;
    }

    if (!editor->saveToFile(path)) {
        QMessageBox::warning(this, tr("Apollo IDE"), tr("Could not save file:\n%1").arg(path));
        return false;
    }
    updateTabLabel(editor);
    return true;
}

bool MainWindow::closeEditorTab(int index)
{
    if (index < 0 || index >= m_editorTabs->count()) {
        return false;
    }

    auto *editor = qobject_cast<EditorWidget *>(m_editorTabs->widget(index));
    if (editor == nullptr) {
        return false;
    }

    if (!maybeSave(editor)) {
        return false;
    }

    m_editorTabs->removeTab(index);
    editor->deleteLater();

    if (m_editorTabs->count() == 0) {
        addEditorTab();
    }

    updateWindowTitle();
    return true;
}

void MainWindow::closeCurrentTab()
{
    closeEditorTab(m_editorTabs->currentIndex());
}

void MainWindow::updateBuildActionsEnabled()
{
    const bool busy = m_compiler->isRunning() || m_vm->isRunning();
    const bool hasEditor = currentEditor() != nullptr;
    m_compileAction->setEnabled(hasEditor && !busy);
    m_runAction->setEnabled(hasEditor && !busy);
}

void MainWindow::clearPendingBuild()
{
    m_runAfterCompile = false;
    m_pendingBuildSourcePath.clear();
}

bool MainWindow::prepareSourceForBuild(EditorWidget *editor)
{
    if (editor == nullptr) {
        return false;
    }

    if (editor->isDirty() || editor->filePath().isEmpty()) {
        if (!saveDocument(editor)) {
            return false;
        }
    }

    if (editor->filePath().isEmpty()) {
        QMessageBox::information(this, tr("Apollo IDE"),
                                 tr("Save the source file before compiling."));
        return false;
    }

    return true;
}

bool MainWindow::saveCurrentDocument()
{
    return saveDocument(currentEditor());
}

void MainWindow::newFile()
{
    addEditorTab();
}

void MainWindow::openFile()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        QString(),
        tr("Pascal sources (*.pas);;All files (*)"));

    if (path.isEmpty()) {
        return;
    }

    openPath(path);
}

void MainWindow::saveFile()
{
    saveCurrentDocument();
}

void MainWindow::saveFileAs()
{
    saveDocumentAs(currentEditor());
}

void MainWindow::findInDocument()
{
    if (EditorWidget *editor = currentEditor()) {
        editor->showFind();
    }
}

void MainWindow::compileFile()
{
    if (m_compiler->isRunning() || m_vm->isRunning()) {
        return;
    }

    EditorWidget *editor = currentEditor();
    if (editor == nullptr) {
        return;
    }

    m_runAfterCompile = false;
    if (!prepareSourceForBuild(editor)) {
        return;
    }

    m_pendingBuildSourcePath = editor->filePath();
    m_compiler->compileFile(m_pendingBuildSourcePath);
    updateBuildActionsEnabled();
}

void MainWindow::runFile()
{
    if (m_compiler->isRunning() || m_vm->isRunning()) {
        return;
    }

    EditorWidget *editor = currentEditor();
    if (editor == nullptr) {
        return;
    }

    if (!prepareSourceForBuild(editor)) {
        return;
    }

    m_runAfterCompile = true;
    m_pendingBuildSourcePath = editor->filePath();
    m_compiler->compileFile(m_pendingBuildSourcePath);
    updateBuildActionsEnabled();
}

void MainWindow::onCompileStarted()
{
    m_outputTabs->setCurrentWidget(m_compilerOutput);
    m_compilerOutput->setPlainText(tr("Compiling…"));
    updateBuildActionsEnabled();
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
    const QString bytecodePath = m_compiler->lastBytecodePath();
    clearPendingBuild();
    updateBuildActionsEnabled();

    // Run the bytecode from the pinned compile, not whichever tab is current.
    if (shouldRun && ok && !bytecodePath.isEmpty()) {
        m_vm->runBytecode(bytecodePath);
        updateBuildActionsEnabled();
    }
}

void MainWindow::onCompileFailedToStart(const QString &message)
{
    clearPendingBuild();
    m_outputTabs->setCurrentWidget(m_compilerOutput);
    m_compilerOutput->setPlainText(message);
    updateBuildActionsEnabled();
}

void MainWindow::onRunStarted()
{
    m_outputTabs->setCurrentWidget(m_runtimeOutput);
    m_runtimeOutput->setPlainText(tr("Running…"));
    updateBuildActionsEnabled();
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
    updateBuildActionsEnabled();
}

void MainWindow::onRunFailedToStart(const QString &message)
{
    m_outputTabs->setCurrentWidget(m_runtimeOutput);
    m_runtimeOutput->setPlainText(message);
    updateBuildActionsEnabled();
}
