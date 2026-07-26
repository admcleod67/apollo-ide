#pragma once

#include <QMainWindow>

class CompilerAdapter;
class EditorWidget;
class QCloseEvent;
class QPlainTextEdit;
class QTabWidget;
class VmAdapter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void createMenus();
    void updateWindowTitle();
    bool maybeSave();
    bool saveDocument();
    bool saveDocumentAs();
    bool prepareSourceForBuild();

    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void compileFile();
    void runFile();

    void onCompileStarted();
    void onCompileFinished(bool ok, const QString &stdoutText, const QString &stderrText,
                           int exitCode);
    void onCompileFailedToStart(const QString &message);

    void onRunStarted();
    void onRunFinished(bool ok, const QString &stdoutText, const QString &stderrText, int exitCode);
    void onRunFailedToStart(const QString &message);

    EditorWidget *m_editor = nullptr;
    QTabWidget *m_outputTabs = nullptr;
    QPlainTextEdit *m_compilerOutput = nullptr;
    QPlainTextEdit *m_runtimeOutput = nullptr;
    CompilerAdapter *m_compiler = nullptr;
    VmAdapter *m_vm = nullptr;
    bool m_runAfterCompile = false;
};
