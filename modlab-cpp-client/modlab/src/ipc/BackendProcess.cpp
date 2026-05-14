#include "BackendProcess.h"
#include <QStringList>

BackendProcess::BackendProcess(const QString& pythonPath,
                               const QString& scriptPath,
                               QObject* parent)
    : QObject(parent)
    , m_python(pythonPath)
    , m_script(scriptPath)
{}

// ---- Public action methods ----

void BackendProcess::runPack(const QString& packKey) {
    m_currentPack = packKey;
    startProcess({"--action", "run", "--pack", packKey});
}

void BackendProcess::installPack(const QString& packKey) {
    m_currentPack = packKey;
    startProcess({"--action", "install", "--pack", packKey});
}

void BackendProcess::deletePack(const QString& packKey) {
    m_currentPack = packKey;
    startProcess({"--action", "delete", "--pack", packKey});
}

void BackendProcess::makePack(const QString& name,
                              const QString& version,
                              const QString& loader) {
    m_currentPack = name.toLower();
    startProcess({
        "--action", "make",
        "--name",    name,
        "--version", version,
        "--loader",  loader
    });
}

// ---- Private helpers ----

void BackendProcess::startProcess(const QStringList& args) {
    // Kill any previous process before starting a new one
    if (m_process) {
        m_process->kill();
        m_process->deleteLater();
    }

    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &BackendProcess::onReadyRead);
    connect(m_process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &BackendProcess::onFinished);

    QStringList fullArgs = {m_script};
    fullArgs += args;
    m_process->start(m_python, fullArgs);
}

void BackendProcess::onReadyRead() {
    while (m_process->canReadLine()) {
        QString line = QString::fromUtf8(m_process->readLine()).trimmed();
        emit rawOutput(line);
        parseLine(line);
    }
}

void BackendProcess::parseLine(const QString& line) {
    // Expected formats (colon-delimited):
    //   STATUS:<pack>:<msg>
    //   PROGRESS:<pack>:<0-100>
    //   DONE:<pack>
    //   ERROR:<pack>:<msg>
    QStringList parts = line.split(':');
    if (parts.size() < 2) return;

    const QString tag  = parts[0].toUpper();
    const QString pack = parts[1];

    if (tag == "STATUS" && parts.size() >= 3) {
        emit statusLine(pack, parts.mid(2).join(':'));
    } else if (tag == "PROGRESS" && parts.size() >= 3) {
        bool ok;
        int pct = parts[2].toInt(&ok);
        if (ok) emit progressUpdate(pack, pct);
    } else if (tag == "DONE") {
        emit operationDone(pack);
    } else if (tag == "ERROR" && parts.size() >= 3) {
        emit operationError(pack, parts.mid(2).join(':'));
    }
}

void BackendProcess::onFinished(int exitCode, QProcess::ExitStatus) {
    if (exitCode != 0 && !m_currentPack.isEmpty())
        emit operationError(m_currentPack,
            QString("Process exited with code %1").arg(exitCode));
    m_process->deleteLater();
    m_process = nullptr;
}
