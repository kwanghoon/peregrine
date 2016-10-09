// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

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