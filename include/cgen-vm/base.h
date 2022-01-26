#ifndef __CGEN_INCLUDE_BASE
#define __CGEN_INCLUDE_BASE

#include <stdint.h>

typedef struct CGEN_VM_STRUCT
{
    uint8_t* mem; // VM memory
    uint64_t mem_size; // VM memory size

    uint8_t reg_mode; // VM register mode

    uint8_t* regs_8bit; // VM 8 bit registers
    uint16_t* regs_16bit; // VM 16 bit registers
    uint32_t* regs_32bit; // VM 32 bit registers
    uint64_t* regs_64bit; // VM 64 bit registers

    int(**interrupts)(struct CGEN_VM_STRUCT*); // VM interrupts
} CGen_VM;

#endif
