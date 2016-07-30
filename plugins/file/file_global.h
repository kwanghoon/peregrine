#ifndef FILE_GLOBAL_H
#define FILE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FILE_LIBRARY)
#  define fileSHARED_EXPORT Q_DECL_EXPORT
#else
#  define fileSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // FILE_GLOBAL_H
