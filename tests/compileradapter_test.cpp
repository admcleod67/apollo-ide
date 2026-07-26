#include "compileradapter.h"

#include <QFile>
#include <QFileInfo>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

class CompilerAdapterTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void compileSuccessWritesTbc();
    void compileFailureDoesNotWriteTbc();
    void missingCompilerFailsToStart();

private:
    QString m_successCompiler;
    QString m_failCompiler;
};

void CompilerAdapterTest::initTestCase()
{
    m_successCompiler = QStringLiteral(APOLLO_TEST_FIXTURES_DIR)
                        + QStringLiteral("/fake_apolloc_success.sh");
    m_failCompiler = QStringLiteral(APOLLO_TEST_FIXTURES_DIR)
                     + QStringLiteral("/fake_apolloc_fail.sh");

    QVERIFY(QFileInfo::exists(m_successCompiler));
    QVERIFY(QFileInfo::exists(m_failCompiler));
    QVERIFY(QFileInfo(m_successCompiler).isExecutable());
    QVERIFY(QFileInfo(m_failCompiler).isExecutable());
}

void CompilerAdapterTest::compileSuccessWritesTbc()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString sourcePath = dir.filePath(QStringLiteral("hello.pas"));
    const QString tbcPath = dir.filePath(QStringLiteral("hello.tbc"));
    {
        QFile source(sourcePath);
        QVERIFY(source.open(QIODevice::WriteOnly | QIODevice::Text));
        source.write("program Hello; begin end.\n");
    }

    CompilerAdapter adapter;
    adapter.setCompilerPath(m_successCompiler);

    QSignalSpy startedSpy(&adapter, &CompilerAdapter::compileStarted);
    QSignalSpy finishedSpy(&adapter, &CompilerAdapter::compileFinished);
    QSignalSpy failedSpy(&adapter, &CompilerAdapter::compileFailedToStart);

    adapter.compileFile(sourcePath);
    QVERIFY(finishedSpy.wait(5000));
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);

    const QList<QVariant> args = finishedSpy.takeFirst();
    QVERIFY(args.at(0).toBool());
    QVERIFY(args.at(1).toString().contains(QStringLiteral("PRINT")));
    QCOMPARE(args.at(3).toInt(), 0);

    QCOMPARE(adapter.lastBytecodePath(), tbcPath);
    QVERIFY(QFileInfo::exists(tbcPath));

    QFile tbc(tbcPath);
    QVERIFY(tbc.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString contents = QString::fromUtf8(tbc.readAll());
    QVERIFY(contents.contains(QStringLiteral("PRINT")));
}

void CompilerAdapterTest::compileFailureDoesNotWriteTbc()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString sourcePath = dir.filePath(QStringLiteral("bad.pas"));
    const QString tbcPath = dir.filePath(QStringLiteral("bad.tbc"));
    {
        QFile source(sourcePath);
        QVERIFY(source.open(QIODevice::WriteOnly | QIODevice::Text));
        source.write("not pascal\n");
    }

    CompilerAdapter adapter;
    adapter.setCompilerPath(m_failCompiler);

    QSignalSpy finishedSpy(&adapter, &CompilerAdapter::compileFinished);
    QSignalSpy failedSpy(&adapter, &CompilerAdapter::compileFailedToStart);

    adapter.compileFile(sourcePath);
    QVERIFY(finishedSpy.wait(5000));
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);

    const QList<QVariant> args = finishedSpy.takeFirst();
    QVERIFY(!args.at(0).toBool());
    QVERIFY(args.at(2).toString().contains(QStringLiteral("intentional compile failure")));
    QCOMPARE(args.at(3).toInt(), 1);

    QVERIFY(adapter.lastBytecodePath().isEmpty());
    QVERIFY(!QFileInfo::exists(tbcPath));
}

void CompilerAdapterTest::missingCompilerFailsToStart()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString sourcePath = dir.filePath(QStringLiteral("hello.pas"));
    {
        QFile source(sourcePath);
        QVERIFY(source.open(QIODevice::WriteOnly | QIODevice::Text));
        source.write("program Hello; begin end.\n");
    }

    CompilerAdapter adapter;
    adapter.setCompilerPath(dir.filePath(QStringLiteral("no-such-apolloc")));

    QSignalSpy finishedSpy(&adapter, &CompilerAdapter::compileFinished);
    QSignalSpy failedSpy(&adapter, &CompilerAdapter::compileFailedToStart);

    adapter.compileFile(sourcePath);
    QVERIFY(failedSpy.wait(5000));
    QCOMPARE(failedSpy.count(), 1);
    QVERIFY(failedSpy.takeFirst().at(0).toString().contains(QStringLiteral("Failed to start")));
    QCOMPARE(finishedSpy.count(), 0);
    QVERIFY(adapter.lastBytecodePath().isEmpty());
}

QTEST_MAIN(CompilerAdapterTest)
#include "compileradapter_test.moc"
