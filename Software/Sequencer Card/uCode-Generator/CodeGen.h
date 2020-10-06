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

#define CARRY_SHIFT 0x08 //goes into ALU3

// compare options
#define ALUBR 0
#define ALUGT 1
#define ALUEQ 2
#define ALULT 3
#define ALUZ 4
#define ALUIN 5
#define ALU16Z 6
#define ALUCS 7

//Shift operations
#define SHIFT_LEFT 0x01
#define SHIFT_RIGHT 0x02
#define SHIFT_LOAD 0x03

#define SHIFT_ZERO 0x00
#define SHIFT_RING 0x04
#define SHIFT_PROP 0x08
#define SHIFT_CARRY 0x0C

#define INVERT 1
#define NOINVERT 0

#define PC 0 //program counter is currently locked to reg 0
#define SP 1 //stack pointer is currently locked to reg 1 (only needed if JSR/RET is used)