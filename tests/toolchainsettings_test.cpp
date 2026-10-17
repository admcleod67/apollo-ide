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
    QString m_previousModules;
    bool m_hadCompiler = false;
    bool m_hadVm = false;
    bool m_hadModules = false;
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
    m_hadModules = qEnvironmentVariableIsSet(ToolchainSettings::kModulesEnv);
    m_previousCompiler = QString::fromLocal8Bit(qgetenv(ToolchainSettings::kCompilerEnv));
    m_previousVm = QString::fromLocal8Bit(qgetenv(ToolchainSettings::kVmEnv));
    m_previousModules = QString::fromLocal8Bit(qgetenv(ToolchainSettings::kModulesEnv));
    qunsetenv(ToolchainSettings::kCompilerEnv);
    qunsetenv(ToolchainSettings::kVmEnv);
    qunsetenv(ToolchainSettings::kModulesEnv);

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
    if (m_hadModules) {
        qputenv(ToolchainSettings::kModulesEnv, m_previousModules.toLocal8Bit());
    } else {
        qunsetenv(ToolchainSettings::kModulesEnv);
    }
}

void ToolchainSettingsTest::defaultsWhenEmpty()
{
    QCOMPARE(ToolchainSettings::resolveCompilerPath(), QStringLiteral("apolloc"));
    QCOMPARE(ToolchainSettings::resolveVmPath(), QStringLiteral("gemini-vm"));
    QVERIFY(ToolchainSettings::resolveModulesPath().isEmpty());
}

void ToolchainSettingsTest::storedSettingsUsedWhenNoEnv()
{
    ToolchainSettings::save(QStringLiteral("/opt/apolloc"), QStringLiteral("/opt/gemini-vm"),
                            QStringLiteral("/opt/modules"));
    QCOMPARE(ToolchainSettings::resolveCompilerPath(), QStringLiteral("/opt/apolloc"));
    QCOMPARE(ToolchainSettings::resolveVmPath(), QStringLiteral("/opt/gemini-vm"));
    QCOMPARE(ToolchainSettings::resolveModulesPath(), QStringLiteral("/opt/modules"));
}

void ToolchainSettingsTest::envOverridesStoredSettings()
{
    ToolchainSettings::save(QStringLiteral("/opt/apolloc"), QStringLiteral("/opt/gemini-vm"),
                            QStringLiteral("/opt/modules"));
    qputenv(ToolchainSettings::kCompilerEnv, "/env/apolloc");
    qputenv(ToolchainSettings::kVmEnv, "/env/gemini-vm");
    qputenv(ToolchainSettings::kModulesEnv, "/env/modules");
    QCOMPARE(ToolchainSettings::resolveCompilerPath(), QStringLiteral("/env/apolloc"));
    QCOMPARE(ToolchainSettings::resolveVmPath(), QStringLiteral("/env/gemini-vm"));
    QCOMPARE(ToolchainSettings::resolveModulesPath(), QStringLiteral("/env/modules"));
}

void ToolchainSettingsTest::saveAndLoadRoundTrip()
{
    ToolchainSettings::save(QStringLiteral("/saved/apolloc"), QStringLiteral("/saved/gemini-vm"),
                            QStringLiteral("/saved/modules"));
    QCOMPARE(ToolchainSettings::storedCompilerPath(), QStringLiteral("/saved/apolloc"));
    QCOMPARE(ToolchainSettings::storedVmPath(), QStringLiteral("/saved/gemini-vm"));
    QCOMPARE(ToolchainSettings::storedModulesPath(), QStringLiteral("/saved/modules"));
}

QTEST_MAIN(ToolchainSettingsTest)
#include "toolchainsettings_test.moc"
