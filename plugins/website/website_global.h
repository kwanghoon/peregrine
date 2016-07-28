#ifndef WEBSITE_GLOBAL_H
#define WEBSITE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WEBSITE_LIBRARY)
#  define WEBSITESHARED_EXPORT Q_DECL_EXPORT
#else
#  define WEBSITESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // WEBSITE_GLOBAL_H
