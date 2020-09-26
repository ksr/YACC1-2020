/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   codes.h
 * Author: ksr77
 *
 * Created on October 21, 2016, 2:57 PM
 * 
 * FIX IO pin mapping
 */

#ifndef CODES_H
#define CODES_H

#ifdef __cplusplus
extern "C" {
#endif

    struct signal {
        char *name;
        int chip;
        int port;
        int bit;
    };
    struct signal signals[] = {

        "CADDR0",   0, 0, 0,
        "CADDR1",   0, 0, 1,
        "CADDR2",   0, 0, 2,
        "CADDR3",   0, 0, 3,
        "CADDR4",   0, 0, 4,
        "CADDR5",   0, 0, 5,
        "CADDR6",   0, 0, 6,
        "CADDR7",   0, 0, 7,
        "CADDR8",   0, 1, 0,
        "CADDR9",   0, 1, 1,
        "CADDR10",  0, 1, 2,
        "CADDR11",  0, 1, 3,
        "CADDR12",  0, 1, 4,


        "REG-FUNC-LD",      1, 0, 0,
        "REG-FUNC-RD",      1, 0, 1,
        "ADDR-ID0",         1, 0, 2,
        "ADDR-ID1",         1, 0, 3,
        "UNUESED",          1, 0, 4,
        "REG-BRD-RD-ID",    2, 0, 5,
        "REG-RD-ID0",       2, 0, 6,
        "REG-RD-ID1",       2, 0, 7,

        "REG-RD-ID2",       1, 1, 0,
        "REG-BRD-LD-ID",    1, 1, 1,
        "REG-LD-ID0",       1, 1, 2,
        "REG-LD-ID1",       1, 1, 3,
        "REG-LD-ID2",       1, 1, 4,
        "REG-RD-LO",        1, 1, 5,
        "REG-LD-LO",        1, 1, 6,
        "REG-RD-HI",        1, 1, 7,

        "REG-LD-HI",        2, 0, 0,
        "MEM-WR",           2, 0, 1,
        "MEM-RD",           2, 0, 2,
        "I/O-WR",           2, 0, 3,
        "I/O-RD",           2, 0, 4,
        "I/O-ADDR",         2, 0, 5,

        "INTA",             2, 0, 7,

        "TMP-REG-LD",       2, 1, 0,
        "TMP-REG-RD",       2, 1, 1,
        "HALT",             2, 1, 2,
        "ADDR-REG-FUNC-LD", 2, 1, 3,
        "ADDR-REG-FUNC-RD", 2, 1, 4,
        "ADDR-REG-DATA-RD-LO", 2, 1, 5,
        "ADDR-REG-DATA-RD-HI", 2, 1, 6,
        "ADDR-REG-ADDR-RD", 2, 1, 7,

        "ADDR-REG-LD-LO",   3, 0, 0,
        "ADDR-REG-LD-HI",   3, 0, 1,
        "ADDR-REG-DN",      3, 0, 2,
        "ADDR-REG-UP",      3, 0, 3,
        "ALU-FUNC",         3, 0, 4,
        "ALU0",             3, 0, 5,
        "ALU1",             3, 0, 6,
        "ALU2",             3, 0, 7,

        "ALU3",             3, 1, 0,
        "AC-LD-INV",        3, 1, 1,
        "AC-LD",            3, 1, 2,
        "AC-RD",            3, 1, 3,
        "SR-LD",            3, 1, 4,
        "BR-COND",          3, 1, 5,
        "TEST-IN",          3, 1, 5,
        "H/L-SWAP",         3, 1, 6,
        "OUT-ON",           3, 1, 7,

        "OUT-OFF",          4, 0, 0,
        "SOFT-RESET",       4, 0, 1,

        "INT-START",        4, 0, 3,
        "INT-EN",           4, 0, 4,
        "LD-INS-REG",       4, 0, 5,
        "UCODE-COUNT-RESET", 4, 0, 6,
        "OPERAND-CLK",      4, 0, 7,

        "BRANCH-RD-LO",     4, 1, 0,
        "BRANCH-LD-LO",     4, 1, 1,
        "BRANCH-RD-HI",      4, 1, 2,
        "BRANCH-LD-HI",     4, 1, 3,
        "INT-JMP",          4, 1, 4,
        "INT-LD",           4, 1, 5,
        "2-BYTE-OPERAND-SEL", 4, 1, 6,
        "1-BYTE-OPERAND-SEL", 4, 1, 7,

        "BIT100", 1, 0, 0,
        "BIT101", 1, 0, 1,
        "BIT102", 1, 0, 2,
        "BIT103", 1, 0, 3,
        "BIT104", 1, 0, 4,
        "BIT105", 1, 0, 5,
        "BIT106", 1, 0, 6,
        "BIT107", 1, 0, 7,
        "BIT110", 1, 1, 0,
        "BIT111", 1, 1, 1,
        "BIT112", 1, 1, 2,
        "BIT113", 1, 1, 3,
        "BIT114", 1, 1, 4,
        "BIT115", 1, 1, 5,
        "BIT116", 1, 1, 6,
        "BIT117", 1, 1, 7,
        "BIT200", 2, 0, 0,
        "BIT201", 2, 0, 1,
        "BIT202", 2, 0, 2,
        "BIT203", 2, 0, 3,
        "BIT204", 2, 0, 4,
        "BIT205", 2, 0, 5,
        "BIT206", 2, 0, 6,
        "BIT207", 2, 0, 7,
        "BIT210", 2, 1, 0,
        "BIT211", 2, 1, 1,
        "BIT212", 2, 1, 2,
        "BIT213", 2, 1, 3,
        "BIT214", 2, 1, 4,
        "BIT215", 2, 1, 5,
        "BIT216", 2, 1, 6,
        "BIT217", 2, 1, 7,
        "BIT300", 3, 0, 0,
        "BIT301", 3, 0, 1,
        "BIT302", 3, 0, 2,
        "BIT303", 3, 0, 3,
        "BIT304", 3, 0, 4,
        "BIT305", 3, 0, 5,
        "BIT306", 3, 0, 6,
        "BIT307", 3, 0, 7,
        "BIT310", 3, 1, 0,
        "BIT311", 3, 1, 1,
        "BIT312", 3, 1, 2,
        "BIT313", 3, 1, 3,
        "BIT314", 3, 1, 4,
        "BIT315", 3, 1, 5,
        "BIT316", 3, 1, 6,
        "BIT317", 3, 1, 7,
        "BIT400", 4, 0, 0,
        "BIT401", 4, 0, 1,
        "BIT402", 4, 0, 2,
        "BIT403", 4, 0, 3,
        "BIT404", 4, 0, 4,
        "BIT405", 4, 0, 5,
        "BIT406", 4, 0, 6,
        "BIT407", 4, 0, 7,
        "BIT410", 4, 1, 0,
        "BIT411", 4, 1, 1,
        "BIT412", 4, 1, 2,
        "BIT413", 4, 1, 3,
        "BIT414", 4, 1, 4,
        "BIT415", 4, 1, 5,
        "BIT416", 4, 1, 6,
        "BIT417", 4, 1, 7,


        "", 0, 0,
    };



#ifdef __cplusplus
}
#endif

#endif /* CODES_H */

