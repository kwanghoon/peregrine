#include "actionuihelper.h"
#include <QLabel>
#include <QImageReader>

namespace ActionUIHelper
{
    void loadActionImage(QLabel* actionLabel, const QString& imagePath, const QString& label)
    {
        const QSize kActionImageSize{ 120, 60 };

        if (imagePath.isEmpty())
        {
            actionLabel->setText(label);
            actionLabel->resize(kActionImageSize);
            actionLabel->setAlignment(Qt::AlignCenter);
            return;
        }

        QImageReader reader(imagePath);
        reader.setAutoTransform(true);
        auto pixmap = QPixmap::fromImage(reader.read());

        actionLabel->resize(kActionImageSize);
        actionLabel->setAlignment(Qt::AlignCenter);
        actionLabel->setPixmap(pixmap.scaled(kActionImageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}