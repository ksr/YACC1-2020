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

#define        ON	01
#define        OFF	02
#define        HALT	03
#define        TBR      05
#define        TBRON    06
#define        TBROFF   07
#define        LDT	0D
#define        LDA	0E
#define        MVI	10
#define        MVI	18
#define        MVRLA	20
#define        MVRHA	28
#define        MVARL	30
#define        MVARH	38
#define        LDAVR	40
#define        STAVR	48
#define        INCR	50
#define        DECR	58
#define        OUTA	60
#define        OUTI	70
#define        OUTVR	80
#define        INP	90
#define        BR	0xA0
#define        BRZ	A1
#define        BRNZ	A2
#define        BRINH	A3
#define        BRINL	A4
#define        ADDI	B0
#define        SUBI	B1
#define        ORI	B2
#define        ANDI	B3
#define        XORI	B4
#define        INVA	B5
#define        SHL	B6
#define        SHR	B7
#define        ADDT	B8
#define        SUBT	B9
#define        ORT	BA
#define        ANDT	BB
#define        XORT	BC
#define        LDTVR	C0
#define        STTVR	C8

#endif /* OPCODES_H */

