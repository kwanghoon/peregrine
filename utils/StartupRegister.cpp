#include "StartupRegister.h"
#include <QProcess>
#include <string>

using namespace std;

#ifdef _WIN32
bool utils::isRegisteredAsStartupApp(QFileInfo execFile, QString taskName)
{
    QProcess proc;
    proc.start(QString("schtasks /query /tn %1").arg(taskName));
    if (!proc.waitForFinished())
    {
        throw std::runtime_error("Fail returned from QProcess::waitForFinished function");
    }
    return 0 == proc.exitCode();
}

bool utils::registerAsStartupApp(QFileInfo execFile, QString taskName)
{
    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    QString command = QString("schtasks /create /sc onlogon /rl highest /f /tn %1 /tr \"%2\"")
        .arg(taskName).arg(execFile.absoluteFilePath());
    proc.start(command);
    proc.waitForFinished();

    if (proc.exitCode() != 0)
    {
        string output = proc.readAll().toStdString();
        return false;
    }
    return true;
}

bool utils::deregisterAsStartupApp(QString taskName)
{
    QProcess proc;
    QString command = QString("schtasks /delete /f /tn %1").arg(taskName);
    proc.start(command);
    proc.waitForFinished();
    return 0 == proc.exitCode();
}

#else
namespace utils
{
    bool isRegisteredAsStartupApp(QFileInfo execFile)
    {
        return false;
    }

    bool registerAsStartupApp(QFileInfo execFile)
    {
        return false;
    }

    bool deregisterAsStartupApp(QString taskName)
    {
        return false;
    }
}
#endif