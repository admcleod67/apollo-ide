#pragma once

#include <QString>

class ToolchainSettings
{
public:
    static constexpr auto kCompilerEnv = "APOLLO_COMPILER";
    static constexpr auto kVmEnv = "APOLLO_VM";

    static QString storedCompilerPath();
    static QString storedVmPath();
    static void save(const QString &compilerPath, const QString &vmPath);

    /** Env overrides stored settings; empty stored falls back to default binary name. */
    static QString resolveCompilerPath();
    static QString resolveVmPath();

private:
    static QString resolvePath(const char *envKey, const QString &stored, const QString &fallback);
};
