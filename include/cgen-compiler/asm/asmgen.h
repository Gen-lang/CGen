#ifndef __CGEN_COMPILER_ASMGEN
#define __CGEN_COMPILER_ASMGEN

#define CGEN_JMP 0
#define CGEN_NOP 1
#define CGEN_EXIT 2
#define CGEN_ADD 3
#define CGEN_MOV 4
#define CGEN_RMOD 5
#define CGEN_SUB 6
#define CGEN_MUL 7
#define CGEN_DIV 8
#define CGEN_AND 9
#define CGEN_OR 10
#define CGEN_XOR 11
#define CGEN_NOT 12
#define CGEN_SHR 13
#define CGEN_SHL 14
#define CGEN_SMEM 15
#define CGEN_GMEM 16
#define CGEN_CMP 17
#define CGEN_INT 18
#define CGEN_MOD 19

#define CGEN_REG_FLAG 0x80
#define CGEN_IMM_FLAG 0x40

#define CGEN_R0(r) r << 5
#define CGEN_R1(r) r << 2

#endif