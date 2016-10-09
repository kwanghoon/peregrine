// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#ifndef WEBSITE_GLOBAL_H
#define WEBSITE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WEBSITE_LIBRARY)
#  define WEBSITESHARED_EXPORT Q_DECL_EXPORT
#else
#  define WEBSITESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // WEBSITE_GLOBAL_H
