#include <cgen-vm/base.h>
#include <cgen-vm/instr.h>
#include <stdlib.h>
#include <stdio.h>

/* Opcodes:
 * JMP  = 0
 * NOP  = 1
 * EXIT = 2
 * ADD  = 3
 * MOV  = 4
 * RMOD = 5
 * SUB  = 6
 * MUL  = 7
 * DIV  = 8
 * AND  = 9
 * OR   = 10
 * XOR  = 11
 * NOT  = 12
 * SHR  = 13
 * SHL  = 14
 * SMEM = 15
 * GMEM = 16
 * CMP  = 17
 * INT  = 18
 * MOD  = 19
 * 
 */ 

static inline unsigned char CGEN_VM_to_8bit(unsigned char* x)
{
    return x[0];
}

static inline uint16_t CGEN_VM_to_16bit(unsigned char* x)
{
    return (uint16_t)x[0] | (uint16_t)x[1] << (1 << 2);
}

static inline uint32_t CGEN_VM_to_32bit(unsigned char* x)
{
    uint32_t y = 0;
    for(int i = 0; i < 4; i++)
    {
        y |= x[i] << (i << 2);
    }
    return y;
}

static inline unsigned long long CGEN_VM_to_64bit(unsigned char* x)
{
    unsigned long long y = 0;
    for(int i = 0; i < 8; i++)
    {
        y |= (unsigned long long)(x[i]) << (i << 2);
    }
    return y;
}

int CGEN_VM_exec_instr(CGEN_VM* instance)
{
    unsigned long long* pc = &(instance->registers_64bit[5]);
    unsigned char byte0 = instance->memory[*pc];
    unsigned char reg_flag = byte0 & 0x80;
    unsigned char imm_flag = byte0 & 0x40;
    unsigned char opcode = byte0 & 0x3F;
    void* reg0;
    void* reg1;
    void* imm0;
    {
        unsigned char v[8];
        imm0 = v;
    }
    if(instance->register_mode == 0)
    {
        reg0 = (void*)instance->registers_8bit;
        reg1 = (void*)instance->registers_8bit;
    }
    else if(instance->register_mode == 1)
    {
        reg0 = (void*)instance->registers_64bit;
        reg1 = (void*)instance->registers_64bit;
    }
    else if(instance->register_mode == 2)
    {
        reg0 = (void*)instance->registers_16bit;
        reg1 = (void*)instance->registers_16bit;
    }
    else if(instance->register_mode == 3)
    {
        reg0 = (void*)instance->registers_32bit;
        reg1 = (void*)instance->registers_32bit;
    }
    else
    {
        reg0 = (void*)instance->registers_8bit;
        reg1 = (void*)instance->registers_8bit;
    }
    if(reg_flag != 0)
    {
        *pc += 1;
        if(*pc >= instance->memory_size)
        {
            return -1;
        }
        unsigned char byte1 = instance->memory[*pc];
        if(instance->register_mode == 0)
        {
            reg0 += ((byte1 & 0b11100000) >> 5);
            reg1 += ((byte1 & 0b00011100) >> 2);
        }
        else if(instance->register_mode == 1)
        {
            reg0 += (((unsigned long long)(byte1 & 0b11100000)) >> 2);
            reg1 += (((unsigned long long)(byte1 & 0b00011100)) << 1);
        }
        else if(instance->register_mode == 2)
        {
            reg0 += (((unsigned long long)(byte1 & 0b11100000)) >> 4);
            reg1 += (((unsigned long long)(byte1 & 0b00011100)) >> 1);
        }
        else if(instance->register_mode == 3)
        {
            reg0 += (((unsigned long long)(byte1 & 0b11100000)) >> 3);
            reg1 += (unsigned long long)(byte1 & 0b00011100);
        }
        else
        {
            reg0 += ((byte1 & 0b11100000) >> 5);
            reg1 += ((byte1 & 0b00011100) >> 2);
        }
        if(imm_flag == 0)
        {
            *pc += 1;
        }
    }
    if(imm_flag != 0)
    {
        *pc += 1;
        unsigned long long required = *pc - 1;
        if(instance->register_mode == 0)
        {
            required += 1;
        }
        else if(instance->register_mode == 1)
        {
            required += 8;
        }
        else if(instance->register_mode == 2)
        {
            required += 2;
        }
        else if(instance->register_mode == 3)
        {
            required += 4;
        }
        else
        {
            required += 1;
        }
        if(required >= instance->memory_size)
        {
            return -1;
        }
        int ci = 0;
        for(;*pc < (required + 1);)
        {
            ((unsigned char*)imm0)[ci] = instance->memory[*pc];
            ci += 1;
            *pc += 1;
        }
    }
    if(imm_flag == 0 && reg_flag == 0)
    {
        *pc += 1;
    }
    /* JMP */
    if(opcode == 0x0)
    {
        /* Only 64 bit register mode */
        if(instance->register_mode != 1)
        {
            return -1;
        }
        if(instance->registers_8bit[7] == 0)
        {
            return 1;
        }
        if(imm_flag != 0)
        {
            *pc = CGEN_VM_to_64bit(imm0);
        }
        else
        {
            *pc = *(unsigned long long*)reg0;
        }
        return 1;
    }
    /* NOP */
    else if(opcode == 0x1)
    {
        return 1;
    }
    /* EXIT */
    else if(opcode == 0x2)
    {
        return 0;
    }
    /* ADD */
    else if(opcode == 0x3)
    {
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                reg(reg0) = reg(reg0) + imm(imm0);
            }
            else
            {
                reg(reg0) = reg(reg0) + reg(reg1);
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                reg(reg0) = reg(reg0) + imm(imm0);
	        }
            else
            {
                reg(reg0) = reg(reg0) + reg(reg1);
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                reg(reg0) = reg(reg0) + imm(imm0);
	        }
            else
            {
                reg(reg0) = reg(reg0) + reg(reg1);
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                reg(reg0) = reg(reg0) + imm(imm0);
	        }
            else
            {
                reg(reg0) = reg(reg0) + reg(reg1);
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                reg(reg0) = reg(reg0) + imm(imm0);
	        }
            else
            {
                reg(reg0) = reg(reg0) + reg(reg1);
            }
            #undef reg
            #undef imm
        }
        return 1;
    }
    /* MOV */
    else if(opcode == 0x4)
    {
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                reg(reg0) = imm(imm0);
            }
            else
            {
                reg(reg0) = reg(reg1);
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                reg(reg0) = imm(imm0);
	        }
            else
            {
                reg(reg0) = reg(reg1);
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                reg(reg0) = imm(imm0);
	        }
            else
            {
                reg(reg0) = reg(reg1);
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                reg(reg0) = imm(imm0);
	        }
            else
            {
                reg(reg0) = reg(reg1);
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                reg(reg0) = imm(imm0);
	        }
            else
            {
                reg(reg0) = reg(reg1);
            }
            #undef reg
            #undef imm
        }
        return 1;
    }
    /* RMOD */
    else if(opcode == 0x5)
    {
        if(imm_flag != 0)
        {
            instance->register_mode = CGEN_VM_to_8bit(imm0);
        }
        else
        {
            instance->register_mode = *(unsigned char*)reg0;
        }
        return 1;
    }
    /* SUB */
    else if(opcode == 0x6)
    {
        #define instr_imm reg(reg0) = reg(reg0) - imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) - reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* MUL */
    else if(opcode == 0x7)
    {
        #define instr_imm reg(reg0) = reg(reg0) * imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) * reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* DIV */
    else if(opcode == 0x8)
    {
        #define instr_imm reg(reg0) = reg(reg0) / imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) / reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* AND */
    else if(opcode == 0x9)
    {
        #define instr_imm reg(reg0) = reg(reg0) & imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) & reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* OR */
    else if(opcode == 0xA)
    {
        #define instr_imm reg(reg0) = reg(reg0) | imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) | reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* XOR */
    else if(opcode == 0xB)
    {
        #define instr_imm reg(reg0) = reg(reg0) ^ imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) ^ reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* NOT */
    else if(opcode == 0xC)
    {
        #define instr_imm reg(reg0) = ~imm(imm0)
        #define instr_reg reg(reg0) = ~reg(reg0)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* SHR */
    else if(opcode == 0xD)
    {
        #define instr_imm reg(reg0) = reg(reg0) >> imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) >> reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* SHL */
    else if(opcode == 0xE)
    {
        #define instr_imm reg(reg0) = reg(reg0) << imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) << reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* SMEM */
    else if(opcode == 0xF)
    {
        #define instr_imm for(int i = 0; i < sz; i++){ instance->memory[reg(reg0) + i] = ((unsigned char*)imm0)[i]; }
        #define instr_reg for(int i = 0; i < sz; i++){ instance->memory[reg(reg0) + i] = ((unsigned char*)reg1)[i]; }
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            #define sz 1
            if(instance->memory_size < (reg(reg0) + 1))
            {
                return -1;
            }
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
            #define sz 8
            if(instance->memory_size < (reg(reg0) + 8))
            {
                return -1;
            }
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
            #define sz 2
            if(instance->memory_size < (reg(reg0) + 2))
            {
                return -1;
            }
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
            #define sz 4
            if(instance->memory_size < (reg(reg0) + 4))
            {
                return -1;
            }
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
            #define sz 1
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* GMEM */
    else if(opcode == 0x10)
    {
        #define instr_imm for(int i = 0; i < sz; i++){ ((unsigned char*)reg0)[i] = instance->memory[imm(imm0) + i]; }
        #define instr_reg for(int i = 0; i < sz; i++){ ((unsigned char*)reg0)[i] = instance->memory[reg(reg1) + i]; }
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            #define sz 1
            if(instance->memory_size < (reg(reg0) + 1))
            {
                return -1;
            }
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
            #define sz 8
            if(instance->memory_size < (reg(reg0) + 8))
            {
                return -1;
            }
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
            #define sz 2
            if(instance->memory_size < (reg(reg0) + 2))
            {
                return -1;
            }
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
            #define sz 4
            if(instance->memory_size < (reg(reg0) + 4))
            {
                return -1;
            }
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
            #define sz 1
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef sz
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* CMP */
    else if(opcode == 0x11)
    {
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            unsigned char cd = instance->registers_8bit[6];
            unsigned char* cr = &(instance->registers_8bit[7]);
            if(imm_flag != 0)
            {
                if(cd == 0)
                {
                    if(reg(reg0) == imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            else
            {
                if(cd == 0)
                {
                    if(reg(reg0) == reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        unsigned char cd = instance->registers_8bit[6];
            unsigned char* cr = &(instance->registers_8bit[7]);
            if(imm_flag != 0)
            {
                if(cd == 0)
                {
                    if(reg(reg0) == imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            else
            {
                if(cd == 0)
                {
                    if(reg(reg0) == reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        unsigned char cd = instance->registers_8bit[6];
            unsigned char* cr = &(instance->registers_8bit[7]);
            if(imm_flag != 0)
            {
                if(cd == 0)
                {
                    if(reg(reg0) == imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            else
            {
                if(cd == 0)
                {
                    if(reg(reg0) == reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        unsigned char cd = instance->registers_8bit[6];
            unsigned char* cr = &(instance->registers_8bit[7]);
            if(imm_flag != 0)
            {
                if(cd == 0)
                {
                    if(reg(reg0) == imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            else
            {
                if(cd == 0)
                {
                    if(reg(reg0) == reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        unsigned char cd = instance->registers_8bit[6];
            unsigned char* cr = &(instance->registers_8bit[7]);
            if(imm_flag != 0)
            {
                if(cd == 0)
                {
                    if(reg(reg0) == imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > imm(imm0))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            else
            {
                if(cd == 0)
                {
                    if(reg(reg0) == reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 1)
                {
                    if(reg(reg0) < reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
                else if(cd == 2)
                {
                    if(reg(reg0) > reg(reg1))
                    {
                        *cr = 1;
                    }
                    else
                    {
                        *cr = 0;
                    }
                }
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    /* INT */
    else if(opcode == 0x12)
    {
        if(instance->register_mode != 0)
        {
            return -1;
        }
        return instance->interrupts[instance->registers_8bit[0]](instance);
    }
    /* MOD */
    else if(opcode == 0x13)
    {
        #define instr_imm reg(reg0) = reg(reg0) % imm(imm0)
        #define instr_reg reg(reg0) = reg(reg0) % reg(reg1)
        if(instance->register_mode == 0)
        {
	        #define reg(x) (*(unsigned char*)x)
	        #define imm(x) CGEN_VM_to_8bit(x)
            if(imm_flag != 0)
            {
                instr_imm;
            }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
	    else if(instance->register_mode == 1)
	    {
            #define reg(x) (*(unsigned long long*)x)
            #define imm(x) CGEN_VM_to_64bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 2)
	    {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) CGEN_VM_to_16bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else if(instance->register_mode == 3)
	    {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) CGEN_VM_to_32bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
	    }
        else
        {
            #define reg(x) (*(unsigned char*)x)
            #define imm(x) CGEN_VM_to_8bit(x)
	        if(imm_flag != 0)
	        {
                instr_imm;
	        }
            else
            {
                instr_reg;
            }
            #undef reg
            #undef imm
        }
        #undef instr_imm
        #undef instr_reg
        return 1;
    }
    return 0;
}
