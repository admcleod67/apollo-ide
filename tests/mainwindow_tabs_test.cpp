#include "editorwidget.h"
#include "mainwindow.h"

#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

class MainWindowTabsTest : public QObject
{
    Q_OBJECT

private slots:
    void startsWithOneUntitledEditorTab();
    void newCreatesAdditionalUntitledTabs();
    void openPathReusesExistingTab();
    void openTwoFilesCreatesTwoTabs();
    void closeCleanTabDecreasesCount();
    void closeLastTabLeavesOneUntitled();
    void tabBuffersAreIndependent();
    void saveCurrentDocumentDoesNotClearOtherTabDirty();
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

void MainWindowTabsTest::newCreatesAdditionalUntitledTabs()
{
    MainWindow window;
    window.newUntitledTab();
    QCOMPARE(window.editorTabCount(), 2);

    EditorWidget *editor = window.currentEditor();
    QVERIFY(editor != nullptr);
    QCOMPARE(editor->filePath(), QString());
    QVERIFY(editor->toPlainText().isEmpty());
}

void MainWindowTabsTest::openPathReusesExistingTab()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath(QStringLiteral("once.pas"));
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("program Once;\nbegin\nend.\n");
    }

    MainWindow window;
    QVERIFY(window.openPath(path));
    QCOMPARE(window.editorTabCount(), 1);
    EditorWidget *first = window.currentEditor();
    QVERIFY(first != nullptr);
    QCOMPARE(QFileInfo(first->filePath()).absoluteFilePath(), QFileInfo(path).absoluteFilePath());

    QVERIFY(window.openPath(path));
    QCOMPARE(window.editorTabCount(), 1);
    QCOMPARE(window.currentEditor(), first);
}

void MainWindowTabsTest::openTwoFilesCreatesTwoTabs()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString pathA = dir.filePath(QStringLiteral("a.pas"));
    const QString pathB = dir.filePath(QStringLiteral("b.pas"));
    {
        QFile file(pathA);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("program A;\n");
    }
    {
        QFile file(pathB);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("program B;\n");
    }

    MainWindow window;
    QVERIFY(window.openPath(pathA));
    QVERIFY(window.openPath(pathB));
    QCOMPARE(window.editorTabCount(), 2);

    EditorWidget *editorB = window.currentEditor();
    QVERIFY(editorB != nullptr);
    QCOMPARE(editorB->toPlainText(), QStringLiteral("program B;\n"));

    QVERIFY(window.openPath(pathA));
    EditorWidget *editorA = window.currentEditor();
    QVERIFY(editorA != nullptr);
    QCOMPARE(editorA->toPlainText(), QStringLiteral("program A;\n"));
    QVERIFY(editorA != editorB);
}

void MainWindowTabsTest::closeCleanTabDecreasesCount()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString pathA = dir.filePath(QStringLiteral("a.pas"));
    const QString pathB = dir.filePath(QStringLiteral("b.pas"));
    {
        QFile file(pathA);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("a\n");
    }
    {
        QFile file(pathB);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("b\n");
    }

    MainWindow window;
    QVERIFY(window.openPath(pathA));
    QVERIFY(window.openPath(pathB));
    QCOMPARE(window.editorTabCount(), 2);

    QVERIFY(window.closeTabAt(window.editorTabCount() - 1));
    QCOMPARE(window.editorTabCount(), 1);
}

void MainWindowTabsTest::closeLastTabLeavesOneUntitled()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath(QStringLiteral("only.pas"));
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("program Only;\n");
    }

    MainWindow window;
    QVERIFY(window.openPath(path));
    QCOMPARE(window.editorTabCount(), 1);
    QVERIFY(window.closeTabAt(0));
    QCOMPARE(window.editorTabCount(), 1);

    EditorWidget *editor = window.currentEditor();
    QVERIFY(editor != nullptr);
    QCOMPARE(editor->filePath(), QString());
    QVERIFY(editor->toPlainText().isEmpty());
}

void MainWindowTabsTest::tabBuffersAreIndependent()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString pathA = dir.filePath(QStringLiteral("a.pas"));
    const QString pathB = dir.filePath(QStringLiteral("b.pas"));
    {
        QFile file(pathA);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("program A;\n");
    }
    {
        QFile file(pathB);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("program B;\n");
    }

    MainWindow window;
    QVERIFY(window.openPath(pathA));
    QVERIFY(window.openPath(pathB));

    QVERIFY(window.openPath(pathA));
    EditorWidget *editorA = window.currentEditor();
    QVERIFY(editorA != nullptr);
    editorA->setContent(QStringLiteral("program AEdited;\n"), true);

    QVERIFY(window.openPath(pathB));
    EditorWidget *editorB = window.currentEditor();
    QVERIFY(editorB != nullptr);
    QCOMPARE(editorB->toPlainText(), QStringLiteral("program B;\n"));

    QVERIFY(window.openPath(pathA));
    QCOMPARE(window.currentEditor()->toPlainText(), QStringLiteral("program AEdited;\n"));
}

void MainWindowTabsTest::saveCurrentDocumentDoesNotClearOtherTabDirty()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString pathA = dir.filePath(QStringLiteral("a.pas"));
    const QString pathB = dir.filePath(QStringLiteral("b.pas"));
    {
        QFile file(pathA);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("program A;\n");
    }
    {
        QFile file(pathB);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("program B;\n");
    }

    MainWindow window;
    QVERIFY(window.openPath(pathA));
    QVERIFY(window.openPath(pathB));

    QVERIFY(window.openPath(pathA));
    EditorWidget *editorA = window.currentEditor();
    editorA->setContent(QStringLiteral("program A2;\n"), true);
    QVERIFY(editorA->isDirty());

    QVERIFY(window.openPath(pathB));
    EditorWidget *editorB = window.currentEditor();
    editorB->setContent(QStringLiteral("program B2;\n"), true);
    QVERIFY(editorB->isDirty());

    QVERIFY(window.openPath(pathA));
    QVERIFY(window.saveCurrentDocument());
    QVERIFY(!window.currentEditor()->isDirty());

    QVERIFY(window.openPath(pathB));
    QVERIFY(window.currentEditor()->isDirty());
    QCOMPARE(window.currentEditor()->toPlainText(), QStringLiteral("program B2;\n"));
}

QTEST_MAIN(MainWindowTabsTest)
#include "mainwindow_tabs_test.moc"
