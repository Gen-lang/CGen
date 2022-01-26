#include <stdio.h>
#include <cgen-vm/base.h>
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

#define R0(r) r << 4
#define R1(r) r

#define MiB(x) (1024 * 1024 * x)
#define KiB(x) (1024 * x)

#define VM_CYCLE_MAX KiB(1)
#define VM_DEBUG_CYCLE 1

void debug_registers(CGen_VM* instance)
{
    printf("\n------ CGen VM Register Information ------\n\n");
    printf("Register mode: %u\n\n", instance->reg_mode);
    for(int i = 0; i < 16; i++)
    {
        printf("R8 #%d: %u\n", i, instance->regs_8bit[i]);
    }
    printf("\n");
    for(int i = 0; i < 16; i++)
    {
        printf("R16 #%d: %u\n", i, instance->regs_16bit[i]);
    }
    printf("\n");
    for(int i = 0; i < 16; i++)
    {
        printf("R32 #%d: %u\n", i, instance->regs_32bit[i]);
    }
    printf("\n");
    for(int i = 0; i < 16; i++)
    {
        printf("R64 #%d: %llu\n", i, instance->regs_64bit[i]);
    }
    printf("\n------ CGen Register Information End ------\n\n");
}

int vm_int_pru(CGen_VM* vm)
{
    printf("%llu\n", vm->regs_64bit[vm->regs_8bit[1]]);
    return 1;
}

int vm_debug_exec(uint8_t* p, uint64_t psize, const char* test_name)
{
    int res = 0;
    int cycle = 1;
    printf("---- Initialize CGen %s ----\n\n", test_name);
    CGen_VM vm;
    res = CGen_VM_init(p, psize, 0, psize, &vm);
    if(res < 0)
    {
        printf("Error while initializing CGen %s\n", test_name);
        return -1;
    }
    vm.interrupts[0] = &vm_int_pru;
    while(1)
    {
        printf("CGen VM Cycle: #%d\n", cycle);
        res = CGen_VM_exec_instr(&vm);
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
    CGen_VM_free(&vm);
    printf("---- CGen %s Done ----\n\n", test_name);
    return 0;
}

int main()
{
    unsigned char p0[] = {
        /*
         * Assembly version:
         * mov r8_cr, 0x01
         * rmod 0x01
         * jmp 0x05
         * exit
         */
        MOV | REG | IMM, R0(7), 0x01, // mov r8_cr, 0x01
        RMOD | IMM, 0x01, // rmod 0x01
        JMP | IMM, 0x05,  // jmp 0x05
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        EXIT, // exit
    };
    vm_debug_exec(p0, 15, "Test 1 : Infinite Loop");

    unsigned char p1[] = {
        /*
         * Assembly version:
         * rmod 0x01
         * mov r64_ax, 0xFF
         * rmod 0x00
         * mov r8_a, 0
         * mov r8_b, 4
         * int
         * exit
         */
        RMOD | IMM, 0x01, // rmod 0x01
        MOV | REG | IMM, R0(4), // mov r64_ax, 0xFF
        0xFF, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        RMOD | IMM, 0,
        MOV | REG | IMM, R0(0), 0,
        MOV | REG | IMM, R0(1), 4, // mov r8_b, 4
        INT, // int
        EXIT, // exit
    };
    vm_debug_exec(p1, 27, "Test 2 : Interrupt");

    return 0;
}
