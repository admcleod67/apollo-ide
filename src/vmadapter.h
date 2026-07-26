#pragma once

#include <QObject>
#include <QString>

class QProcess;

class VmAdapter : public QObject
{
    Q_OBJECT

public:
    explicit VmAdapter(QObject *parent = nullptr);
    ~VmAdapter() override;

    void setVmPath(const QString &path);
    [[nodiscard]] QString vmPath() const;
    [[nodiscard]] bool isRunning() const;

    void runBytecode(const QString &tbcPath);

signals:
    void runStarted();
    void runFinished(bool ok, const QString &stdoutText, const QString &stderrText, int exitCode);
    void runFailedToStart(const QString &message);

private:
    void handleProcessFinished(int exitCode, int exitStatus);

    QProcess *m_process = nullptr;
    QString m_vmPath;
};
