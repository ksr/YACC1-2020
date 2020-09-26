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

#define INSTRUCTIONS_TO_OUTPUT 16
