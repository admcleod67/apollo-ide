#include "toolchainsettings.h"

#include <QCoreApplication>
#include <QSettings>
#include <QTemporaryDir>
#include <QtTest>

class ToolchainSettingsTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void defaultsWhenEmpty();
    void storedSettingsUsedWhenNoEnv();
    void envOverridesStoredSettings();
    void saveAndLoadRoundTrip();

private:
    QTemporaryDir m_dir;
    QString m_previousCompiler;
    QString m_previousVm;
    bool m_hadCompiler = false;
    bool m_hadVm = false;
};

void ToolchainSettingsTest::init()
{
    QVERIFY(m_dir.isValid());
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_dir.path());
    QCoreApplication::setOrganizationName(QStringLiteral("ApolloTest"));
    QCoreApplication::setApplicationName(QStringLiteral("ApolloIdeSettingsTest"));

    m_hadCompiler = qEnvironmentVariableIsSet(ToolchainSettings::kCompilerEnv);
    m_hadVm = qEnvironmentVariableIsSet(ToolchainSettings::kVmEnv);
    m_previousCompiler = QString::fromLocal8Bit(qgetenv(ToolchainSettings::kCompilerEnv));
    m_previousVm = QString::fromLocal8Bit(qgetenv(ToolchainSettings::kVmEnv));
    qunsetenv(ToolchainSettings::kCompilerEnv);
    qunsetenv(ToolchainSettings::kVmEnv);

    QSettings().clear();
}

void ToolchainSettingsTest::cleanup()
{
    if (m_hadCompiler) {
        qputenv(ToolchainSettings::kCompilerEnv, m_previousCompiler.toLocal8Bit());
    } else {
        qunsetenv(ToolchainSettings::kCompilerEnv);
    }
    if (m_hadVm) {
        qputenv(ToolchainSettings::kVmEnv, m_previousVm.toLocal8Bit());
    } else {
        qunsetenv(ToolchainSettings::kVmEnv);
    }
}

void ToolchainSettingsTest::defaultsWhenEmpty()
{
    QCOMPARE(ToolchainSettings::resolveCompilerPath(), QStringLiteral("apolloc"));
    QCOMPARE(ToolchainSettings::resolveVmPath(), QStringLiteral("gemini-vm"));
}

void ToolchainSettingsTest::storedSettingsUsedWhenNoEnv()
{
    ToolchainSettings::save(QStringLiteral("/opt/apolloc"), QStringLiteral("/opt/gemini-vm"));
    QCOMPARE(ToolchainSettings::resolveCompilerPath(), QStringLiteral("/opt/apolloc"));
    QCOMPARE(ToolchainSettings::resolveVmPath(), QStringLiteral("/opt/gemini-vm"));
}

void ToolchainSettingsTest::envOverridesStoredSettings()
{
    ToolchainSettings::save(QStringLiteral("/opt/apolloc"), QStringLiteral("/opt/gemini-vm"));
    qputenv(ToolchainSettings::kCompilerEnv, "/env/apolloc");
    qputenv(ToolchainSettings::kVmEnv, "/env/gemini-vm");
    QCOMPARE(ToolchainSettings::resolveCompilerPath(), QStringLiteral("/env/apolloc"));
    QCOMPARE(ToolchainSettings::resolveVmPath(), QStringLiteral("/env/gemini-vm"));
}

void ToolchainSettingsTest::saveAndLoadRoundTrip()
{
    ToolchainSettings::save(QStringLiteral("/saved/apolloc"), QStringLiteral("/saved/gemini-vm"));
    QCOMPARE(ToolchainSettings::storedCompilerPath(), QStringLiteral("/saved/apolloc"));
    QCOMPARE(ToolchainSettings::storedVmPath(), QStringLiteral("/saved/gemini-vm"));
}

QTEST_MAIN(ToolchainSettingsTest)
#include "toolchainsettings_test.moc"
