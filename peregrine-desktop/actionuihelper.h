#pragma once

class QWidget;
class QString;
class QLabel;

namespace ActionUIHelper
{
    void loadActionImage(QLabel* actionLabel, const QString& imagePath, const QString& label);
}