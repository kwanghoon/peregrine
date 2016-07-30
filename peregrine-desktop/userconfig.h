#pragma once

#include <QPoint>
#include <QString>
#include <vector>

class UserConfig
{
public:
    struct ActionSlotAssignInfo
    {
        QPoint pos;
        QString actionId;
    };
    QString pluginDir;
    std::vector<ActionSlotAssignInfo> actionSlotAssignData;
};