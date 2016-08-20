#pragma once

#include <QFileInfo>

namespace utils
{
	bool isRegisteredAsStartupApp(QFileInfo execFile, QString taskName);
	
    bool registerAsStartupApp(QFileInfo execFile, QString taskName);
	
	bool deregisterAsStartupApp(QString taskName);
}