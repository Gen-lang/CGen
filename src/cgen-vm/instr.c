#include <stddef.h>
#include <stdint.h>
#include <cgen-vm/instr.h>
#include <cgen-vm/base.h>

static inline uint8_t CGen_VM_to_8bit(uint8_t* x)
{
    return x[0];
}

static inline uint16_t CGen_VM_to_16bit(uint8_t* x)
{
    return (uint16_t)(x[0]) | ((uint16_t)(x[1])) << 8;
}

uint32_t CGen_VM_to_32bit(uint8_t* x)
{
    uint32_t y = 0;
    for(uint8_t i = 0; i < 4; i++)
    {
        y |= ((uint32_t)x[i]) << (uint32_t)(i << 3);
    }
    return y;
}

uint64_t CGen_VM_to_64bit(uint8_t* x)
{
    uint64_t y = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        y |= ((uint64_t)x[i]) << (uint64_t)(i << 3);
    }
    return y;
}

int CGen_VM_exec_instr(CGen_VM* vm)
{
    uint64_t* pc = &(vm->regs_64bit[5]);
    if( ( *pc ) >= vm->mem_size)
    {
        return -1;
    }
    uint8_t opcode = vm->mem[ *pc ] & 0x3F;
    uint8_t reg_flag = vm->mem[ *pc ] & 0x80;
    uint8_t imm_flag = vm->mem[ *pc ] & 0x40;

    void* reg0;
    void* reg1;
    uint8_t* imm0;
    {
        uint8_t imm0_base[8] = {0,0,0,0, 0,0,0,0};
        imm0 = imm0_base;
    }
    if(vm->reg_mode == (uint8_t)0)
    {
        reg0 = (void*)vm->regs_8bit;
        reg1 = (void*)vm->regs_8bit;
    }
    else if(vm->reg_mode == (uint8_t)1)
    {
        reg0 = (void*)vm->regs_64bit;
        reg1 = (void*)vm->regs_64bit;
    }
    else if(vm->reg_mode == (uint8_t)2)
    {
        reg0 = (void*)vm->regs_16bit;
        reg1 = (void*)vm->regs_16bit;
    }
    else if(vm->reg_mode == (uint8_t)3)
    {
        reg0 = (void*)vm->regs_32bit;
        reg1 = (void*)vm->regs_32bit;
    }
    else
    {
        reg0 = (void*)vm->regs_8bit;
        reg1 = (void*)vm->regs_8bit;
    }

    if(reg_flag != 0)
    {
        *pc += 1;
        if(( *pc ) >= vm->mem_size)
        {
            return -1;
        }
        if(vm->reg_mode == 0)
        {
            reg0 += (size_t)((vm->mem[ *pc ] & 0xF0) >> 4);
            reg1 += (size_t)(vm->mem[ *pc ] & 0x0F);
        }
        else if(vm->reg_mode == 1)
        {
            reg0 += (size_t)((vm->mem[ *pc ] & 0xF0) >> 1);
            reg1 += (size_t)((vm->mem[ *pc ] & 0x0F) << 3);
        }
        else if(vm->reg_mode == 2)
        {
            reg0 += (size_t)((vm->mem[ *pc ] & 0xF0) >> 3);
            reg1 += (size_t)((vm->mem[ *pc ] & 0x0F) << 1);
        }
        else if(vm->reg_mode == 3)
        {
            reg0 += (size_t)((vm->mem[ *pc ] & 0xF0) >> 2);
            reg1 += (size_t)((vm->mem[ *pc ] & 0x0F) << 2);
        }
        else
        {
            reg0 += (size_t)((vm->mem[ *pc ] & 0xF0) >> 4);
            reg1 += (size_t)(vm->mem[ *pc ] & 0x0F);
        }
        if(imm_flag == 0)
        {
            *pc += 1;
        }
    }
    if(imm_flag != 0)
    {
        *pc += 1;
        uint64_t required = ( *pc ) - 1;
        if(opcode == (uint8_t)5)
        {
            required += 1;
        }
        else
        {
            if(vm->reg_mode == 0)
            {
                required += 1;
            }
            else if(vm->reg_mode == 1)
            {
                required += 8;
            }
            else if(vm->reg_mode == 2)
            {
                required += 2;
            }
            else if(vm->reg_mode == 3)
            {
                required += 4;
            }
            else
            {
                required += 1;
            }
        }
        if(required >= vm->mem_size)
        {
            return -1;
        }
        uint8_t ci = 0;
        for(;( *pc ) < (required + 1);)
        {
            imm0[ci] = vm->mem[ *pc ];
            ci += 1;
            *pc += 1;
        }
    }
    if(imm_flag == 0 && reg_flag == 0)
    {
        *pc += 1;
    }
    switch(opcode)
    {
    case 0:
        if(vm->reg_mode != 1)
        {
            return -1;
        }
        if(vm->regs_8bit[7] == 0)
        {
            return 1;
        }
        if(imm_flag)
        {
            *pc = CGen_VM_to_64bit(imm0);
        }
        else
        {
            *pc = *(uint64_t*)reg0;
        }
        break;
    case 1:
        return 1;
    case 2:
        return 0;
    case 3:
        #define instr_reg reg(reg0) = reg(reg0) + reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) + imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 4:
        #define instr_reg reg(reg0) = reg(reg1)
        #define instr_imm reg(reg0) = imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 5:
        if(imm_flag != 0)
        {
            vm->reg_mode = CGen_VM_to_8bit(imm0);
        }
        else
        {
            vm->reg_mode = *(uint8_t*)reg0;
        }
        break;
    case 6:
        #define instr_reg reg(reg0) = reg(reg0) - reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) - imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 7:
        #define instr_reg reg(reg0) = reg(reg0) * reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) * imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 8:
        #define instr_reg reg(reg0) = reg(reg0) / reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) / imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 9:
        #define instr_reg reg(reg0) = reg(reg0) & reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) & imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 10:
        #define instr_reg reg(reg0) = reg(reg0) | reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) | imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 11:
        #define instr_reg reg(reg0) = reg(reg0) ^ reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) ^ imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 12:
        #define instr_reg reg(reg0) = ~reg(reg0)
        #define instr_imm reg(reg0) = ~imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 13:
        #define instr_reg reg(reg0) = reg(reg0) >> reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) >> imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 14:
        #define instr_reg reg(reg0) = reg(reg0) << reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) << imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 15:
        #define instr_imm for(uint8_t i = 0; i < sz; i++){ vm->mem[reg(reg0) + i] = ((uint8_t*)imm0)[i]; }
        #define instr_reg for(uint8_t i = 0; i < sz; i++){ vm->mem[reg(reg0) + i] = ((uint8_t*)reg1)[i]; }
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
            #define sz 1
            if(vm->mem_size < (reg(reg0) + sz))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
            #define sz 8
            if(vm->mem_size < (reg(reg0) + sz))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
            #define sz 2
            if(vm->mem_size < (reg(reg0) + sz))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
            #define sz 4
            if(vm->mem_size < (reg(reg0) + sz))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
            #define sz 1
            if(vm->mem_size < (reg(reg0) + sz))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 16:
        #define instr_imm for(uint8_t i = 0; i < sz; i++){ ((uint8_t*)reg0)[i] = vm->mem[imm(imm0) + i]; }
        #define instr_reg for(uint8_t i = 0; i < sz; i++){ ((uint8_t*)reg0)[i] = vm->mem[reg(reg1) + i]; }
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
            #define sz 1
            if(vm->mem_size < (reg(reg0) + sz))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
            #define sz 8
            if(vm->mem_size < (reg(reg0) + sz))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
            #define sz 2
            if(vm->mem_size < (reg(reg0) + sz))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
            #define sz 4
            if(vm->mem_size < (reg(reg0) + sz))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
            #define sz 1
            if(vm->mem_size < (reg(reg0) + sz))
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
        #undef instr_reg
        #undef instr_imm
        break;
    case 17:
        #define instr_reg if(cd == 0) { if(reg(reg0) == reg(reg1)){ *cr = 1; } else { *cr = 0; } } else if(cd == 1) { if(reg(reg0) < reg(reg1)){ *cr = 1; } else { *cr = 0; } } else if(cd == 2) { if(reg(reg0) > reg(reg1)){ *cr = 1; } else { *cr = 0; } }
        #define instr_imm if(cd == 0) { if(reg(reg0) == imm(imm0)){ *cr = 1; } else { *cr = 0; } } else if(cd == 1) { if(reg(reg0) < imm(imm0)){ *cr = 1; } else { *cr = 0; } } else if(cd == 2) { if(reg(reg0) > imm(imm0)){ *cr = 1; } else { *cr = 0; } }
        {
        uint8_t cd = vm->regs_8bit[6];
        uint8_t* cr = &(vm->regs_8bit[7]);
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        }
        break;
    case 18:
        {
        int(*interrupt)(CGen_VM*) = vm->interrupts[vm->regs_8bit[0]];
        return interrupt(vm);
        }
    case 19:
        #define instr_reg reg(reg0) = reg(reg0) % reg(reg1)
        #define instr_imm reg(reg0) = reg(reg0) % imm(imm0)
        if(vm->reg_mode == 0)
        {
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        else if(vm->reg_mode == 1)
        {
            #define reg(x) (*(uint64_t*)x)
            #define imm(x) (CGen_VM_to_64bit(x))
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
        else if(vm->reg_mode == 2)
        {
            #define reg(x) (*(uint16_t*)x)
            #define imm(x) (CGen_VM_to_16bit(x))
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
        else if(vm->reg_mode == 3)
        {
            #define reg(x) (*(uint32_t*)x)
            #define imm(x) (CGen_VM_to_32bit(x))
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
            #define reg(x) (*(uint8_t*)x)
            #define imm(x) (CGen_VM_to_8bit(x))
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
        #undef instr_reg
        #undef instr_imm
        break;
    }
    return 1;
}
