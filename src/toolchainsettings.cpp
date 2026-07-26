#include "toolchainsettings.h"

#include <QSettings>
#include <QtGlobal>

namespace {

constexpr auto kCompilerKey = "toolchain/compilerPath";
constexpr auto kVmKey = "toolchain/vmPath";

} // namespace

QString ToolchainSettings::storedCompilerPath()
{
    return QSettings().value(QLatin1String(kCompilerKey)).toString();
}

QString ToolchainSettings::storedVmPath()
{
    return QSettings().value(QLatin1String(kVmKey)).toString();
}

void ToolchainSettings::save(const QString &compilerPath, const QString &vmPath)
{
    QSettings settings;
    settings.setValue(QLatin1String(kCompilerKey), compilerPath);
    settings.setValue(QLatin1String(kVmKey), vmPath);
    settings.sync();
}

QString ToolchainSettings::resolvePath(const char *envKey, const QString &stored,
                                       const QString &fallback)
{
    const QByteArray env = qgetenv(envKey);
    if (!env.isEmpty()) {
        return QString::fromLocal8Bit(env);
    }
    if (!stored.isEmpty()) {
        return stored;
    }
    return fallback;
}

QString ToolchainSettings::resolveCompilerPath()
{
    return resolvePath(kCompilerEnv, storedCompilerPath(), QStringLiteral("apolloc"));
}

QString ToolchainSettings::resolveVmPath()
{
    return resolvePath(kVmEnv, storedVmPath(), QStringLiteral("gemini-vm"));
}
