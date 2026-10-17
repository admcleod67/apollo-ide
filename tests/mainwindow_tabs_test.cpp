#include "editorwidget.h"
#include "mainwindow.h"

#include <QtTest>

class MainWindowTabsTest : public QObject
{
    Q_OBJECT

private slots:
    void startsWithOneUntitledEditorTab();
};

void MainWindowTabsTest::startsWithOneUntitledEditorTab()
{
    MainWindow window;
    QCOMPARE(window.editorTabCount(), 1);

    EditorWidget *editor = window.currentEditor();
    QVERIFY(editor != nullptr);
    QCOMPARE(editor->filePath(), QString());
    QVERIFY(!editor->isDirty());
    QVERIFY(editor->toPlainText().isEmpty());
}

QTEST_MAIN(MainWindowTabsTest)
#include "mainwindow_tabs_test.moc"
