/* CGen VM test */

#include <stdio.h>
#include <cgen-vm/vm.h>
#include <cgen-vm/instr.h>

#define JMP 0
#define NOP 1
#define EXIT 2
#define ADD 3
#define MOV 4
#define RMOD 5
#define SUB 6
#define MUL 7
#define DIV 8
#define AND 9
#define OR 10
#define XOR 11
#define NOT 12
#define SHR 13
#define SHL 14
#define SMEM 15
#define GMEM 16
#define CMP 17
#define INT 18
#define MOD 19

#define REG 0x80
#define IMM 0x40

#define R0(r) r << 5
#define R1(r) r << 2

#define MiB(x) (1024 * 1024 * x)

#define VM_CYCLE_MAX MiB(1)
#define VM_DEBUG_CYCLE 50

void debug_registers(CGEN_VM* instance)
{
    printf("\n------ CGen VM Register Information ------\n\n");
    printf("Register mode: %u\n\n", instance->register_mode);
    for(int i = 0; i < 8; i++)
    {
        printf("R8 #%d: %u\n", i, instance->registers_8bit[i]);
    }
    printf("\n");
    for(int i = 0; i < 8; i++)
    {
        printf("R16 #%d: %u\n", i, instance->registers_16bit[i]);
    }
    printf("\n");
    for(int i = 0; i < 8; i++)
    {
        printf("R32 #%d: %u\n", i, instance->registers_32bit[i]);
    }
    printf("\n");
    for(int i = 0; i < 8; i++)
    {
        printf("R64 #%d: %u\n", i, instance->registers_64bit[i]);
    }
    printf("\n------ CGen Register Information End ------\n\n");
}

int vm_debug_exec(unsigned char* p, unsigned long long psize, const char* test_name)
{
    int res = 0;
    int cycle = 1;
    printf("---- Initialize CGen %s ----\n\n", test_name);
    CGEN_VM vm;
    res = CGEN_VM_init(p, psize, 0, psize, &vm);
    if(res < 0)
    {
        printf("Error while initializing CGen %s\n", test_name);
        return 1;
    }
    while(1)
    {
        printf("CGen VM Cycle: #%d\n", cycle);
        res = CGEN_VM_exec_instr(&vm);
        printf("CGen cycle result: %d\n\n", res);
        if((cycle % VM_DEBUG_CYCLE) == 0)
        {
            debug_registers(&vm);
        }
        if(res < 0)
        {
            printf("Error while executing %s\n", test_name);
            break;
        }
        if(res == 0)
        {
            break;
        }
        if(cycle >= VM_CYCLE_MAX)
        {
            break;
        }
        cycle += 1;
    }
    printf("---- CGen %s Done ----\n\n", test_name);
}

int main()
{
    int res = 0;
    int cycle = 1;
    unsigned char p0[] = {
        MOV | REG | IMM, R0(7), 0x01, // Without this, the JMP instruction won't work
        RMOD | IMM, 0x01, // rmod 1
        JMP | IMM, 0x05, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00,
        EXIT, // Doesn't exit
    };
    vm_test(p0, 15, "Test 1 : Infinite Loop");
    
    return 0;
}