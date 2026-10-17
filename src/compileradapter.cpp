#include "compileradapter.h"

#include <QFileInfo>
#include <QProcess>

CompilerAdapter::CompilerAdapter(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_compilerPath(QStringLiteral("apolloc"))
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
            tr("Failed to start compiler '%1': %2")
                .arg(m_compilerPath, m_process->errorString());
        m_lastBytecodePath.clear();
        emit compileFailedToStart(message);
    });
}

CompilerAdapter::~CompilerAdapter()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void CompilerAdapter::setCompilerPath(const QString &path)
{
    m_compilerPath = path;
}

QString CompilerAdapter::compilerPath() const
{
    return m_compilerPath;
}

QString CompilerAdapter::lastBytecodePath() const
{
    return m_lastBytecodePath;
}

bool CompilerAdapter::isRunning() const
{
    return m_process->state() != QProcess::NotRunning;
}

void CompilerAdapter::compileFile(const QString &sourcePath)
{
    if (isRunning()) {
        return;
    }

    m_sourcePath = sourcePath;
    m_outputPath = bytecodePathForSource(sourcePath);
    m_lastBytecodePath.clear();

    const QFileInfo info(sourcePath);
    m_process->setProgram(m_compilerPath);
    m_process->setArguments({QStringLiteral("--tbc"), sourcePath, QStringLiteral("-o"), m_outputPath});
    m_process->setWorkingDirectory(info.absolutePath());
    m_process->setProcessChannelMode(QProcess::SeparateChannels);

    emit compileStarted();
    m_process->start();
}

void CompilerAdapter::handleProcessFinished(int exitCode, int exitStatus)
{
    const QString stdoutText = QString::fromUtf8(m_process->readAllStandardOutput());
    const QString stderrText = QString::fromUtf8(m_process->readAllStandardError());

    const bool crashed = exitStatus != static_cast<int>(QProcess::NormalExit);
    const bool exitOk = !crashed && exitCode == 0;

    if (exitOk) {
        if (!QFileInfo::exists(m_outputPath)) {
            m_lastBytecodePath.clear();
            const QString missing =
                tr("Compile succeeded but compiler did not write '%1'.").arg(m_outputPath);
            emit compileFinished(false, stdoutText,
                                 stderrText.isEmpty() ? missing
                                                      : stderrText + QLatin1Char('\n') + missing,
                                 exitCode);
            return;
        }
        m_lastBytecodePath = m_outputPath;
        emit compileFinished(true, stdoutText, stderrText, exitCode);
        return;
    }

    m_lastBytecodePath.clear();
    emit compileFinished(false, stdoutText, stderrText, exitCode);
}

QString CompilerAdapter::bytecodePathForSource(const QString &sourcePath)
{
    const QFileInfo info(sourcePath);
    return info.absolutePath() + QLatin1Char('/') + info.completeBaseName()
           + QStringLiteral(".tbc");
}
