#include "vmadapter.h"

#include <QFileInfo>
#include <QProcess>

VmAdapter::VmAdapter(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_vmPath(QStringLiteral("gemini-vm"))
{
    connect(m_process, &QProcess::finished, this,
            [this](int exitCode, QProcess::ExitStatus status) {
                handleProcessFinished(exitCode, static_cast<int>(status));
            });
    connect(m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error != QProcess::FailedToStart) {
            return;
        }
        const QString message =
            tr("Failed to start VM '%1': %2").arg(m_vmPath, m_process->errorString());
        emit runFailedToStart(message);
    });
}

VmAdapter::~VmAdapter()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void VmAdapter::setVmPath(const QString &path)
{
    m_vmPath = path;
}

QString VmAdapter::vmPath() const
{
    return m_vmPath;
}

bool VmAdapter::isRunning() const
{
    return m_process->state() != QProcess::NotRunning;
}

void VmAdapter::runBytecode(const QString &tbcPath)
{
    if (isRunning()) {
        return;
    }

    const QFileInfo info(tbcPath);
    m_process->setProgram(m_vmPath);
    m_process->setArguments({tbcPath});
    m_process->setWorkingDirectory(info.absolutePath());
    m_process->setProcessChannelMode(QProcess::SeparateChannels);

    emit runStarted();
    m_process->start();
}

void VmAdapter::handleProcessFinished(int exitCode, int exitStatus)
{
    const QString stdoutText = QString::fromUtf8(m_process->readAllStandardOutput());
    const QString stderrText = QString::fromUtf8(m_process->readAllStandardError());

    const bool crashed = exitStatus != static_cast<int>(QProcess::NormalExit);
    const bool ok = !crashed && exitCode == 0;
    emit runFinished(ok, stdoutText, stderrText, exitCode);
}
