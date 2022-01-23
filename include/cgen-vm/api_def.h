#ifndef __CGEN_VM_API_DEF
#define __CGEN_VM_API_DEF

#ifdef CGEN_VM_WIN32_BUILD
#ifndef CGEN_VM_WIN32_EXPORT
#define CGEN_VM_METHOD __declspec(dllexport)
#else
#define CGEN_VM_METHOD __declspec(dllimport)
#endif
#else
#define CGEN_VM_METHOD
#endif

#endif
