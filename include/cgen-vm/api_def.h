#ifndef __64BIT_VPC_3090_1X_API_DEF
#define __64BIT_VPC_3090_1X_API_DEF

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
