#ifndef APIDLLEXPORTDEFINE_H
#define APIDLLEXPORTDEFINE_H

#ifdef WIN32
    #ifdef ENABLE_UNIFIED_ROBOT_EXPORT_API_DLL
    #define UNIFIED_ROBOT_DLL_EXPORT __declspec(dllexport)
    #else
    #define UNIFIED_ROBOT_DLL_EXPORT __declspec(dllimport)
    #endif
#else
    #ifdef ENABLE_UNIFIED_ROBOT_EXPORT_API_DLL
    #define UNIFIED_ROBOT_DLL_EXPORT __attribute__((visibility("default")))
    #else
    #define UNIFIED_ROBOT_DLL_EXPORT
    #endif
#endif

#endif // APIDLLEXPORTDEFINE_H
