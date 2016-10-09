// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#ifndef MANVIEW_GLOBAL_H
#define MANVIEW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MANVIEW_LIBRARY)
#  define MANVIEWSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MANVIEWSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MANVIEW_GLOBAL_H
