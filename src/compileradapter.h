#pragma once

#include <QObject>
#include <QString>

class QProcess;

class CompilerAdapter : public QObject
{
    Q_OBJECT

public:
    explicit CompilerAdapter(QObject *parent = nullptr);
    ~CompilerAdapter() override;

    void setCompilerPath(const QString &path);
    [[nodiscard]] QString compilerPath() const;
    [[nodiscard]] QString lastBytecodePath() const;
    [[nodiscard]] bool isRunning() const;

    void compileFile(const QString &sourcePath);

signals:
    void compileStarted();
    void compileFinished(bool ok, const QString &stdoutText, const QString &stderrText,
                         int exitCode);
    void compileFailedToStart(const QString &message);

private:
    void handleProcessFinished(int exitCode, int exitStatus);
    [[nodiscard]] static QString bytecodePathForSource(const QString &sourcePath);
    bool writeBytecodeFile(const QString &path, const QByteArray &data);

    QProcess *m_process = nullptr;
    QString m_compilerPath;
    QString m_sourcePath;
    QString m_lastBytecodePath;
};
