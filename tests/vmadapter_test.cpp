#include "vmadapter.h"

#include <QFile>
#include <QFileInfo>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

class VmAdapterTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void runSuccessCapturesStdout();
    void runFailureCapturesStderr();
    void missingVmFailsToStart();

private:
    QString m_successVm;
    QString m_failVm;
};

void VmAdapterTest::initTestCase()
{
    m_successVm = QStringLiteral(APOLLO_TEST_FIXTURES_DIR)
                  + QStringLiteral("/fake_gemini_vm_success.sh");
    m_failVm = QStringLiteral(APOLLO_TEST_FIXTURES_DIR)
               + QStringLiteral("/fake_gemini_vm_fail.sh");

    QVERIFY(QFileInfo::exists(m_successVm));
    QVERIFY(QFileInfo::exists(m_failVm));
    QVERIFY(QFileInfo(m_successVm).isExecutable());
    QVERIFY(QFileInfo(m_failVm).isExecutable());
}

void VmAdapterTest::runSuccessCapturesStdout()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString tbcPath = dir.filePath(QStringLiteral("hello.tbc"));
    {
        QFile tbc(tbcPath);
        QVERIFY(tbc.open(QIODevice::WriteOnly | QIODevice::Text));
        tbc.write("LINE 1\nPRINT \"hi\"\n");
    }

    VmAdapter adapter;
    adapter.setVmPath(m_successVm);

    QSignalSpy startedSpy(&adapter, &VmAdapter::runStarted);
    QSignalSpy finishedSpy(&adapter, &VmAdapter::runFinished);
    QSignalSpy failedSpy(&adapter, &VmAdapter::runFailedToStart);

    adapter.runBytecode(tbcPath);
    QVERIFY(finishedSpy.wait(5000));
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);

    const QList<QVariant> args = finishedSpy.takeFirst();
    QVERIFY(args.at(0).toBool());
    QVERIFY(args.at(1).toString().contains(QStringLiteral("Hello, world")));
    QCOMPARE(args.at(3).toInt(), 0);
}

void VmAdapterTest::runFailureCapturesStderr()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString tbcPath = dir.filePath(QStringLiteral("bad.tbc"));
    {
        QFile tbc(tbcPath);
        QVERIFY(tbc.open(QIODevice::WriteOnly | QIODevice::Text));
        tbc.write("LINE 1\n");
    }

    VmAdapter adapter;
    adapter.setVmPath(m_failVm);

    QSignalSpy finishedSpy(&adapter, &VmAdapter::runFinished);
    QSignalSpy failedSpy(&adapter, &VmAdapter::runFailedToStart);

    adapter.runBytecode(tbcPath);
    QVERIFY(finishedSpy.wait(5000));
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);

    const QList<QVariant> args = finishedSpy.takeFirst();
    QVERIFY(!args.at(0).toBool());
    QVERIFY(args.at(2).toString().contains(QStringLiteral("intentional runtime failure")));
    QCOMPARE(args.at(3).toInt(), 1);
}

void VmAdapterTest::missingVmFailsToStart()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString tbcPath = dir.filePath(QStringLiteral("hello.tbc"));
    {
        QFile tbc(tbcPath);
        QVERIFY(tbc.open(QIODevice::WriteOnly | QIODevice::Text));
        tbc.write("LINE 1\n");
    }

    VmAdapter adapter;
    adapter.setVmPath(dir.filePath(QStringLiteral("no-such-gemini-vm")));

    QSignalSpy finishedSpy(&adapter, &VmAdapter::runFinished);
    QSignalSpy failedSpy(&adapter, &VmAdapter::runFailedToStart);

    adapter.runBytecode(tbcPath);
    QVERIFY(failedSpy.wait(5000));
    QCOMPARE(failedSpy.count(), 1);
    QVERIFY(failedSpy.takeFirst().at(0).toString().contains(QStringLiteral("Failed to start")));
    QCOMPARE(finishedSpy.count(), 0);
}

QTEST_MAIN(VmAdapterTest)
#include "vmadapter_test.moc"
