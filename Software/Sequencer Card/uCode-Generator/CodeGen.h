/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CodeGen.h
 * Author: kennethrother
 *
 * Created on August 7, 2020, 10:05 AM
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CODEGEN_H */


#define PORTS_PER_CHIP 2
#define PINS_PER_PORT 8

#define BYTES_PER_LINE 8
#define LINES_PER_INSTRUCTION 32
#define INSTRUCTION_SIZE BYTES_PER_LINE * LINES_PER_INSTRUCTION
#define INSTRUCTIONS 256
#define MEMORY_SIZE BYTES_PER_LINE*LINES_PER_INSTRUCTION*INSTRUCTIONS

#define INSTRUCTIONS_TO_OUTPUT 256   // KEN sould be 255

#define ALUDATA 0
#define ALUSUB 1
#define ALUAND 2
#define ALUOR  3
#define ALUXOR 4
#define ALUSHIFT 5
#define ALUZERO 6
#define ALUADD 7

// compare options
#define ALUBR 0
#define ALUGT 1
#define ALUEQ 2
#define ALULT 3
#define ALUBRZ 4
#define ALUBRIN 5
#define ALU16Z 6
#define ALUCS 7

#define INVERT 1
#define NOINVERT 0

#define PC 0 //program counter is currently locked to reg 0
#define SP 1 //stack pointer is currently locked to reg 1 (only needed if JSR/RET is used)