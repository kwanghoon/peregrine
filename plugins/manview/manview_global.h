#ifndef MANVIEW_GLOBAL_H
#define MANVIEW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MANVIEW_LIBRARY)
#  define MANVIEWSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MANVIEWSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MANVIEW_GLOBAL_H
