#ifndef __CGEN_INCLUDE_API_DEF
#define __CGEN_INCLUDE_API_DEF
#ifdef CGEN_VM_WIN32
#ifdef CGEN_VM_WIN32_EXPORT
#define CGEN_VM_DEF __declspec(dllexport)
#else
#define CGEN_VM_DEF __declspec(dllimport)
#endif
#else
#define CGEN_VM_DEF
#endif
#endif
