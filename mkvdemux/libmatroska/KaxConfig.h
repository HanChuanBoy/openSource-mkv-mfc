#ifndef LIBMATROSKA_CONFIG_H
#define LIBMATROSKA_CONFIG_H

//we allow the user to set the namespace for the  library for cases,where the name conflicts;

#ifdef NO_NAMESPACE 
#ifndef LIBMATROSKA_NAMESPACE
#define LIBMATROSKA_NAMESPACE libmatroska
#define START_LIBMATROSKA_NAMESPACE
#define END_LIBMATROSKA_NAMESPACE
#endif
#else
#ifndef LIBMATROSKA_NAMESPACE
#define LIBMATROSKA_NAMESPACE libmatroska
#define START_LIBMATROSKA_NAMESPACE namespace LIBMATROSKA_NAMESPACE {
#define END_LIBMATROSKA_NAMESPACE };
#endif
#endif

//there are special implementations for certain platforms. For example 
// on windows we use the Win32 file API. here we set the appropriate macros;
#if defined(_WIN32)||defined(WIN32)
#define LIBMATROSKA_WIN32
#else
#define LIBMATROSKA_UNIX
#endif


//we assume,that the user uses DLL by default.if he wants to use the static library,
//the MATROSKA_STATIC value has to be specified to the preprocessor
#ifdef LIBMATROSKA_WIN32
   //Either MATROSKA_STATIC or MATROSKA_EXPORT should be defined here;
   #ifdef LIBMATROSKA_EXPORT
     #define LIBMATROSKA_EXPORT
   #else
     #ifdef LIBMATROSKA_EXPORT
        #define LIBMATROSKA_EXPORT __declspec(dllimport)  //this should work on Visual C++ and GCC
        #define LIBMATROSKA_EXPORT __declspec(dllexport)
     #endif
#else 
     #define LIBMATROSKA_EXPORT
#endif

//The MATROSKA_DEBUG symbol is defined,when we are creating a debug build. In this
//case the debug logging code is compiled in;
#if (define(DEBUG)||define(_DEBUG))&&!defined(LIBMATROSKA_DEBUG)
#define LIBMATROSKA_DEBUG
#endif

#endif //LIBMATROSKA_CONFIG_H

