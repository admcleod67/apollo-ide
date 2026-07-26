#pragma once

#include <QMainWindow>

class CompilerAdapter;
class EditorWidget;
class QCloseEvent;
class QPlainTextEdit;

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

    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void compileFile();

    void onCompileStarted();
    void onCompileFinished(bool ok, const QString &stdoutText, const QString &stderrText,
                           int exitCode);
    void onCompileFailedToStart(const QString &message);

    EditorWidget *m_editor = nullptr;
    QPlainTextEdit *m_compilerOutput = nullptr;
    CompilerAdapter *m_compiler = nullptr;
};
