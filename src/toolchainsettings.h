#pragma once

#include <QString>

class ToolchainSettings
{
public:
    static constexpr auto kCompilerEnv = "APOLLO_COMPILER";
    static constexpr auto kVmEnv = "APOLLO_VM";
    static constexpr auto kModulesEnv = "APOLLO_MODULES";

    static QString storedCompilerPath();
    static QString storedVmPath();
    static QString storedModulesPath();
    static void save(const QString &compilerPath, const QString &vmPath, const QString &modulesPath);

    /** Env overrides stored settings; empty stored falls back to default binary name. */
    static QString resolveCompilerPath();
    static QString resolveVmPath();
    /** Env overrides stored settings; empty when unset (no default directory). */
    static QString resolveModulesPath();

private:
    static QString resolvePath(const char *envKey, const QString &stored, const QString &fallback);
};
