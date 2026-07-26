#pragma once

#include <QMainWindow>

class EditorWidget;
class QCloseEvent;

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

    EditorWidget *m_editor = nullptr;
};
