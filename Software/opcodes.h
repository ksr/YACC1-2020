/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   opcodes.h
 * Author: ken
 *
 * Created on January 2, 2017, 11:13 AM
 */

#ifndef OPCODES_H
#define OPCODES_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#define        START    0x00
#define        ON	0x01
#define        OFF	0x02
#define        YHALT	0x03
#define        JSR      0X04
#define        RET      0x05
#define        JSRUR    0x06
#define        PUSHR    0x07
#define        POPR     0x08
#define        PUSH     0x09
#define        POP      0x0A
#define        MVAT     0x0B
#define        MVTA     0x0C
#define        LDTI	0x0D
#define        LDAI	0x0E
#define        MOVRR    0x0F
#define        MVIB	0x10
#define        MVIW	0x18
#define        MVRLA	0x20
#define        MVRHA	0x28
#define        MVARL	0x30
#define        MVARH	0x38
#define        LDAVR	0x40
#define        STAVR	0x48
#define        INCR	0x50
#define        DECR	0x58
#define        OUTA	0x60
#define        OUTI	0x70
#define        OUTVR	0x80
#define        INP	0x90
#define        BR	0xA0
#define        BRZ	0xA1
#define        BRNZ	0xA2
#define        BRINH	0xA3
#define        BRINL	0xA4

//#define      BRNC     0xa5
#define        OPCODE_A5 0xA5

#define        BRC      0xA6
#define        BRLT     0xA7
#define        BREQ     0xA8
#define        BRGT     0xA9
#define        BRNEQ    0xAA
#define        BR16Z    0xAB
#define        BR16NZ   0xAC

//#define      0xad
#define        OPCODE_AD 0xAD
//#define      0xae
#define        OPCODE_AE 0xAE

#define        BRDEV    0xAF
#define        ADDI	0xB0
#define        SUBI	0xB1
#define        ORI	0xB2
#define        ANDI	0xB3
#define        XORI	0xB4
#define        INVA	0xB5
#define        SHL	0xB6
#define        SHR	0xB7
#define        ADDT	0xB8
#define        SUBT	0xB9
#define        ORT	0xBA
#define        ANDT	0xBB
#define        XORT	0xBC
#define        RSHL	0xBD
#define        RSHR	0xBE
#define        PSHR     0xBF
#define        LDTVR	0xC0
#define        STTVR	0xC8
#define        LDIVR    0xD0
#define        BRVR     0xD8
#define        CSHL     0xE0
#define        CSHR     0xE1
#define        ADDIC    0xE2
#define        ADDTC    0xE3
#define        LDA      0xE4
#define        STA      0xE5
#define        LDT      0xE6
#define        STT      0xE7
#define        STR      0xE8
#define        LDR      0xF0

//#define 0xf8
#define OPCODE_F8 0xF8
//#define 0xf9
#define OPCODE_F9 0xF9
//#define 0xfa
#define OPCODE_FA 0xFA


#define        INTE     0XFB
#define        INTD     0XFC
#define        IRET     0XFD
#define        IADDR    0xFE
#define        INT      0xFF
#endif /* OPCODES_H */

