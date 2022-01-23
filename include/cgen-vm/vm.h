#ifndef __CGEN_VM_MAIN
#define __CGEN_VM_MAIN
#include "api_def.h"
#include "base.h"

CGEN_VM_METHOD extern int VPC64_3090_1X_init(unsigned char* program,
                                                   unsigned long long program_size,
                                                   unsigned long long program_addr,
                                                   unsigned long long memory_size,
                                                   CGEN_VM* instance);
CGEN_VM_METHOD extern void VPC64_3090_1X_free(CGEN_VM* instance);

#endif