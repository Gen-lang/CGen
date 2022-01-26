#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <cgen-vm/base.h>
#include <cgen-vm/vm.h>

#ifdef CGEN_VM_ZERO
#include <string.h>
#endif

int CGen_VM_init(uint8_t* p, uint64_t p_size, uint64_t p_start, uint64_t mem_size, CGen_VM* vm)
{
    if((p_start + p_size) > mem_size)
    {
        return -1;
    }
    vm->mem = malloc((size_t)mem_size);
    vm->regs_8bit = malloc((size_t)16);
    vm->regs_16bit = malloc((size_t)32);
    vm->regs_32bit = malloc((size_t)64);
    vm->regs_64bit = malloc((size_t)128);
    vm->interrupts = malloc((size_t)256 * sizeof(int(**)(CGen_VM*)));
    if(vm->mem == NULL)
    {
        return -1;
    }
    if(vm->regs_8bit == NULL)
    {
        free(vm->mem);
        return -1;
    }
    if(vm->regs_16bit == NULL)
    {
        free(vm->mem);
        free(vm->regs_8bit);
        return -1;
    }
    if(vm->regs_32bit == NULL)
    {
        free(vm->mem);
        free(vm->regs_8bit);
        free(vm->regs_16bit);
        return -1;
    }
    if(vm->regs_64bit == NULL)
    {
        free(vm->mem);
        free(vm->regs_8bit);
        free(vm->regs_16bit);
        free(vm->regs_32bit);
        return -1;
    }
    if(vm->interrupts == NULL)
    {
        free(vm->mem);
        free(vm->regs_8bit);
        free(vm->regs_16bit);
        free(vm->regs_32bit);
        free(vm->regs_64bit);
        return -1;
    }
    uint64_t ci = 0;
    for(uint64_t i = p_start; i < (p_size + p_start); i++)
    {
        vm->mem[i] = p[ci];
        ci += 1;
    }
    #ifdef CGEN_VM_ZERO
    memset(vm->regs_8bit, (int)((unsigned char)0),16);
    memset(vm->regs_64bit, (int)((unsigned char)0),128);
    memset(vm->regs_32bit, (int)((unsigned char)0),64);
    memset(vm->regs_16bit, (int)((unsigned char)0),32);
    #endif
    vm->reg_mode = (uint8_t)0;
    vm->mem_size = mem_size;
    vm->regs_64bit[5] = p_start;
    return 0;
}

void CGen_VM_free(CGen_VM* vm)
{
    free(vm->mem);
    free(vm->regs_8bit);
    free(vm->regs_16bit);
    free(vm->regs_32bit);
    free(vm->regs_64bit);
    free(vm->interrupts);
    #ifdef CGEN_VM_ZERO
    vm->reg_mode = (uint8_t)0;
    vm->mem_size = (uint64_t)0;
    #endif
}
