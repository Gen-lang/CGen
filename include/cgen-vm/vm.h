#ifndef __CGEN_INCLUDE_VM
#define __CGEN_INCLUDE_VM
#include <stdint.h>
#include "base.h"
#include "api-def.h"

CGEN_VM_DEF int CGen_VM_init(uint8_t* p, uint64_t p_size, uint64_t p_start, uint64_t mem_size, CGen_VM* vm);

CGEN_VM_DEF void CGen_VM_free(CGen_VM* vm);

#endif
