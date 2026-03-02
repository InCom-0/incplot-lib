#pragma once

#if defined(INCPLOT_LIB_SHARED)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(INCPLOT_LIB_EXPORTS)
#define INCPLOT_LIB_API __declspec(dllexport)
#else
#define INCPLOT_LIB_API __declspec(dllimport)
#endif
#else
#define INCPLOT_LIB_API __attribute__((visibility("default")))
#endif

#else
// Static library
#define INCPLOT_LIB_API
#endif