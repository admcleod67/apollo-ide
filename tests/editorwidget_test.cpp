#include "editorwidget.h"

#include <QTemporaryDir>
#include <QtTest>

class EditorWidgetTest : public QObject
{
    Q_OBJECT

private slots:
    void newFileIsCleanAndUntitled();
    void setContentMarksDirty();
    void saveAndLoadRoundTrip();
    void saveClearsDirtyAndEditDirtiesAgain();
    void newFileClearsPathAndContent();
    void saveWithoutPathFails();
    void lineNumberAreaWidthGrowsWithLineCount();
};

void EditorWidgetTest::newFileIsCleanAndUntitled()
{
    EditorWidget editor;
    QCOMPARE(editor.filePath(), QString());
    QVERIFY(!editor.isDirty());
    QVERIFY(editor.toPlainText().isEmpty());
}

void EditorWidgetTest::setContentMarksDirty()
{
    EditorWidget editor;
    editor.setContent(QStringLiteral("program Hello;"), true);
    QVERIFY(editor.isDirty());
    QCOMPARE(editor.toPlainText(), QStringLiteral("program Hello;"));
}

void EditorWidgetTest::saveAndLoadRoundTrip()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath(QStringLiteral("hello.pas"));
    const QString source = QStringLiteral("program Hello;\nbegin\nend.\n");

    EditorWidget writer;
    writer.setContent(source, true);
    QVERIFY(writer.saveToFile(path));
    QCOMPARE(writer.filePath(), path);
    QVERIFY(!writer.isDirty());

    EditorWidget reader;
    QVERIFY(reader.loadFile(path));
    QCOMPARE(reader.filePath(), path);
    QCOMPARE(reader.toPlainText(), source);
    QVERIFY(!reader.isDirty());
}

void EditorWidgetTest::saveClearsDirtyAndEditDirtiesAgain()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath(QStringLiteral("sample.pas"));

    EditorWidget editor;
    editor.setContent(QStringLiteral("a"), true);
    QVERIFY(editor.isDirty());
    QVERIFY(editor.saveToFile(path));
    QVERIFY(!editor.isDirty());

    editor.setContent(QStringLiteral("ab"), true);
    QVERIFY(editor.isDirty());
    QVERIFY(editor.save());
    QVERIFY(!editor.isDirty());
    QCOMPARE(editor.filePath(), path);
}

void EditorWidgetTest::newFileClearsPathAndContent()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath(QStringLiteral("old.pas"));

    EditorWidget editor;
    editor.setContent(QStringLiteral("old content"), true);
    QVERIFY(editor.saveToFile(path));
    QCOMPARE(editor.filePath(), path);

    editor.newFile();
    QCOMPARE(editor.filePath(), QString());
    QVERIFY(editor.toPlainText().isEmpty());
    QVERIFY(!editor.isDirty());
}

void EditorWidgetTest::saveWithoutPathFails()
{
    EditorWidget editor;
    editor.setContent(QStringLiteral("x"), true);
    QVERIFY(!editor.save());
    QVERIFY(editor.isDirty());
}

void EditorWidgetTest::lineNumberAreaWidthGrowsWithLineCount()
{
    EditorWidget editor;
    const int singleDigitWidth = editor.lineNumberAreaWidth();

    QString manyLines;
    for (int i = 0; i < 100; ++i) {
        manyLines += QStringLiteral("line %1\n").arg(i);
    }
    editor.setContent(manyLines, true);

    QVERIFY(editor.lineNumberAreaWidth() > singleDigitWidth);
}

QTEST_MAIN(EditorWidgetTest)
#include "editorwidget_test.moc"
