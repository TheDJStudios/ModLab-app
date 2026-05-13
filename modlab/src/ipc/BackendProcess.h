#pragma once
#include <QObject>
#include <QProcess>
#include <QString>

// BackendProcess wraps every QProcess call to main.py.
// Each action is a separate short-lived process.
// Progress lines from stdout are parsed and emitted as signals.
//
// Protocol (stdout lines from Python):
//   STATUS:<packkey>:<message>
//   PROGRESS:<packkey>:<0-100>
//   DONE:<packkey>
//   ERROR:<packkey>:<message>
//
// The Python backend needs small additions to emit these —
// see scripts/patch_backend.md for the exact changes.
class BackendProcess : public QObject {
    Q_OBJECT
public:
    explicit BackendProcess(const QString& pythonPath,
                            const QString& scriptPath,
                            QObject* parent = nullptr);

    void runPack(const QString& packKey);
    void installPack(const QString& packKey);
    void deletePack(const QString& packKey);
    void makePack(const QString& name,
                  const QString& version,
                  const QString& loader);

    bool isBusy() const { return m_process && m_process->state() != QProcess::NotRunning; }

signals:
    void statusLine(const QString& packKey, const QString& message);
    void progressUpdate(const QString& packKey, int percent);
    void operationDone(const QString& packKey);
    void operationError(const QString& packKey, const QString& message);
    void rawOutput(const QString& line);   // for a future log panel

private slots:
    void onReadyRead();
    void onFinished(int exitCode, QProcess::ExitStatus status);

private:
    void startProcess(const QStringList& args);
    void parseLine(const QString& line);

    QString   m_python;
    QString   m_script;
    QProcess* m_process = nullptr;
    QString   m_currentPack;
};
