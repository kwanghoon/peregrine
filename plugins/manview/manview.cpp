#include "manview.h"
#include <QDebug>
#include <QProcess>
#include <string>

const struct PG_FUNC_TABLE* g_funcTable;

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable)
{
    g_funcTable = funcTable;
    return 0;
}

int RunAction(const char* actionId, const char* data)
{
    char command[512];
#   ifdef Q_OS_WIN
    sprintf(command, "%s /?", data);
#   else
    sprintf(command, "%s --help", data);
#   endif
    QProcess proc;
    proc.start(command);
    QString errorstr = proc.errorString();
    if (proc.waitForFinished())
    {
        auto ba = proc.readAllStandardOutput();
        g_funcTable->fpInvokeQmlFunc("setManText", 
            QString::fromLocal8Bit(ba).toStdString().c_str());
    }
    else
    {
        g_funcTable->fpInvokeQmlFunc("setManText", "");
    }
    return PG_BEHAVIOR_ON_RETURN::PG_REMAIN;
}
