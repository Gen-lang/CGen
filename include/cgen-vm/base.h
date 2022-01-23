#ifndef __CGEN_VM_BASE
#define __CGEN_VM_BASE
#include "api_def.h"
#include <stdint.h>

/* Registers
 *
 * Registers:
 *   Common 8bit registers:
 * 		r8_a   - 8bit register  				#1
 * 		r8_b   - 8bit register  				#2
 * 		r8_c   - 8bit register  				#3
 * 		r8_d   - 8bit register  				#4
 *
 *   2nd 8bit registers:
 *   	r8_a2  - 8bit register  				#5
 *   	r8_b2  - 8bit register  				#6
 * 
 * 	 Special 8bit registers:
 *   	r8_cd  - conditional register  			#7
 *   	r8_cr  - condition result register  	#8
 *   
 * 	 16 bit registers: 8
 * 	  Only has general purpose registers
 * 	 32 bit registers: 8
 * 	  Only has general purpose registers
 * 
 *   Common 64bit registers:
 *   	r64_a  - 64bit register 				#1
 *   	r64_b  - 64bit register 				#2
 *   	r64_c  - 64bit register 				#3
 *   	r64_d  - 64bit register 				#4
 *
 *   Special 64bit registers:
 *   	r64_pc - 64bit program counter 			#5
 *   	r64_ax - accumulator register 			#6
 *   
 *   Secondary 64bit registers:
 *   	r64_a2 - 64bit register					#7
 *   	r64_b2 - 64bit register					#8
 */


;

/* CGen VM structure */
typedef struct CGEN_VM_STRUCT
{
	unsigned char* memory; /* CGen VM memory */ 
	unsigned long long memory_size; /* CGen VM memory size */
	unsigned long long program_memory_address; /* CGen VM program memory address */
	unsigned char register_mode; /* CGen VM register mode | 0 = 8 bit mode, 1 = 64 bit mode, 2 = 16 bit mode, 3 = 32 bit mode */
	unsigned char* registers_8bit; /* CGen VM 8 bit registers */
	uint16_t* registers_16bit; /* CGen VM 16 bit registers */
	uint32_t* registers_32bit; /* CGen VM 32 bit registers */
	unsigned long long* registers_64bit; /* CGen VM 64 bit registers */
	int(*interrupts[256])(struct CGEN_VM_STRUCT*); /* CGen VM 256 Interrupts */
} CGEN_VM;

#endif
