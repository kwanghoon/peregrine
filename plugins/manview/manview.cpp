#include "manview.h"
#include <QDebug>
#include <QProcess>
#include <string>

const struct PG_FUNC_TABLE* g_funcTable;

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable, const PG_PLUGIN_CONFIGS* pluginConfigs)
{
    g_funcTable = funcTable;
    return 0;
}

int RunAction(const char* actionId, const char* data)
{
    QProcess proc;
    char command[512];

#   ifdef Q_OS_WIN
    sprintf(command, "where %s", data);
    proc.start(command);
    if (proc.waitForFinished())
    {
        if (proc.exitCode() != 0)
        {
            g_funcTable->fpInvokeQmlFunc("setManText", 
                QString("Cannot find command(%1).").arg(data).toStdString().c_str());
            return PG_BEHAVIOR_ON_RETURN::PG_REMAIN;
        }
    }
    else
    {
        g_funcTable->fpInvokeQmlFunc("setManText", "");
        return PG_BEHAVIOR_ON_RETURN::PG_REMAIN;
    }

    sprintf(command, "%s /?", data);
#   else
    sprintf(command, "%s --help", data);
#   endif
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
