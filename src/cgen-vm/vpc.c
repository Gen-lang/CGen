#include <cgen-vm/base.h>
#include <cgen-vm/vm.h>
#include <stdlib.h>
#include <stddef.h>

int smalloc(void** ptr, size_t size)
{
    *ptr = malloc(size);
    if(*ptr == NULL)
    {
        return -1;
    }
    return 0;
}

int srealloc(void* ptr, size_t size)
{
    ptr = realloc(ptr, size);
    if(ptr == NULL)
    {
        return -1;
    }
    return 0;
}

int CGEN_VM_init(unsigned char* program,
                       unsigned long long program_size,
                       unsigned long long program_addr,
                       unsigned long long memory_size,
                       CGEN_VM* vpc)
{
    if(program_size == 0)
    {
        return -1;
    }
    if(memory_size < (program_size + program_addr))
    {
        return -1;
    }
    CGEN_VM instance;
    if(smalloc(&instance.memory, (size_t)memory_size) < 0)
    {
        return -1;
    }
    if(smalloc(&instance.registers_8bit, 8) < 0)
    {
        free(instance.memory);
        return -1;
    }
    if(smalloc(&instance.registers_16bit, 16) < 0)
    {
        free(instance.memory);
        free(instance.registers_8bit);
        return -1;
    }
    if(smalloc(&instance.registers_32bit, 32) < 0)
    {
        free(instance.memory);
        free(instance.registers_8bit);
        free(instance.registers_16bit);
        return 0;
    }
    if(smalloc(&instance.registers_64bit, 64) < 0)
    {
        free(instance.memory);
        free(instance.registers_8bit);
        free(instance.registers_16bit);
        free(instance.registers_32bit);
        return -1;
    }
    unsigned long long ci = 0;
    for(unsigned long long i = program_addr; i < (program_size + program_addr); i++)
    {
        instance.memory[i] = program[ci];
        ci += 1;
    }
    instance.memory_size = memory_size;
    instance.program_memory_address = program_addr;
    instance.registers_64bit[5] = program_addr;
    instance.register_mode = 0;
    *vpc = instance;
    return 0;
}
void CGEN_VM_free(CGEN_VM* instance)
{
    free(instance->memory);
    free(instance->registers_8bit);
    free(instance->registers_64bit);
    #ifdef CGEN_VM_ZERO_INSTANCE
    instance->memory_size = 0;
    instance->program_memory_address = 0;
    instance->register_mode = 0;
    #endif
}