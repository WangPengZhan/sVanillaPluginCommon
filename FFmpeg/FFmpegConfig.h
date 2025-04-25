#pragma once

// The way how the function is called
#if !defined(FFMPEG_CALL)
#    if defined(_WIN32)
#        define FFMPEG_CALL __stdcall
#    else
#        define FFMPEG_CALL
#    endif /* _WIN32 */
#endif     /* FFMPEG_CALL */

// The function exported symbols
#if defined _WIN32 || defined __CYGWIN__
#    define FFMPEG_DLL_IMPORT __declspec(dllimport)
#    define FFMPEG_DLL_EXPORT __declspec(dllexport)
#    define FFMPEG_DLL_LOCAL
#else
#    if __GNUC__ >= 4
#        define FFMPEG_DLL_IMPORT __attribute__((visibility("default")))
#        define FFMPEG_DLL_EXPORT __attribute__((visibility("default")))
#        define FFMPEG_DLL_LOCAL  __attribute__((visibility("hidden")))
#    else
#        define FFMPEG_DLL_IMPORT
#        define FFMPEG_DLL_EXPORT
#        define FFMPEG_DLL_LOCAL
#    endif
#endif

#ifdef FFMPEG_LOCAL
#    define FFMPEG_PORT
#    define FFMPEG_API FFMPEG_DLL_LOCAL
#else
#    ifdef FFMPEG_DLL_EXPORTS  // defined if we are building the DLL (instead of using it)
#        define FFMPEG_PORT FFMPEG_DLL_EXPORT
#        define FFMPEG_API  FFMPEG_DLL_EXPORT FFMPEG_CALL
#    else
#        define FFMPEG_PORT FFMPEG_DLL_IMPORT
#        define FFMPEG_API  FFMPEG_DLL_IMPORT FFMPEG_CALL
#    endif  // FFMPEG_DLL_EXPORTS
#endif
