#ifndef HJ_config_h__
#define HJ_config_h__

#include "macro.h"

#if (defined(_WINDOWS) || defined(WIN32) || defined(_WIN32))
#ifdef SHARED_LIBS_IMPORT
#define HJ_EXPORT __declspec(dllimport)
#else 
#define HJ_EXPORT __declspec(dllexport)
#endif
#endif  // #ifdef _WINDOWS

#endif // HJ_config_h__
