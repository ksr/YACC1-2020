/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   signal.h
 * Author: kennethrother
 *
 * Created on August 7, 2020, 11:17 AM
 */

//#include "code.h"

#ifndef SIGNAL_H
#define SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* SIGNAL_H */

struct signal signals[] = {

    /*
        "CADDR0", 0, 0, 0,
        "CADDR1", 0, 0, 1,
        "CADDR2", 0, 0, 2,
        "CADDR3", 0, 0, 3,
        "CADDR4", 0, 0, 4,
        "CADDR5", 0, 0, 5,
        "CADDR6", 0, 0, 6,
        "CADDR7", 0, 0, 7,
        "CADDR8", 0, 1, 0,
        "CADDR9", 0, 1, 1,
        "CADDR10", 0, 1, 2,
        "CADDR11", 0, 1, 3,
        "CADDR12", 0, 1, 4,
*/

        "-REG-FUNC-RD", 1, 0, 0,
        "-REG-FUNC-LD", 1, 0, 1,

        "REG-RD-ID0", 1, 0, 2,
        "REG-RD-ID1", 1, 0, 3,
        "REG-RD-ID2", 1, 0, 4,
        "REG-RD-ID3", 1, 0, 5,

        "REG-LD-ID0", 1, 0, 6,
        "REG-LD-ID1", 1, 0, 7,
        "REG-LD-ID2", 1, 1, 0,
        "REG-LD-ID3", 1, 1, 1,

        "-REG-RD-LO", 1, 1, 2,
        "REG-LD-LO", 1, 1, 3, // this active low on the bus
        "-REG-RD-HI", 1, 1, 4,
        "REG-LD-HI", 1, 1, 5, //this  active low  on the bus

        "-REG-DN", 1, 1, 6,
        "-REG-UP", 1, 1, 7,

        "-MEM-RD", 2, 0, 0,
        "-MEM-WR", 2, 0, 1,
        "-IO-RD", 2, 0, 2,
        "-IO-WR", 2, 0, 3,

        "-TMP-REG-RD0", 2, 0, 4,
        "-TMP-REG-LD0", 2, 0, 5,
        "-TMP-REG-RD1", 2, 0, 6,
        "-TMP-REG-LD1", 2, 0, 7,

        "ADDR-REG-ID0", 2, 1, 0,
        "ADDR-REG-ID1", 2, 1, 1,
        "ADDR-REG-ID2", 2, 1, 2,
        "ADDR-REG-ID3", 2, 1, 3,

        "IOADDR0", 2, 1, 4,
        "IOADDR1", 2, 1, 5,
        "IOADDR2", 2, 1, 6,
        "IOADDR3", 2, 1, 7,

        "-IO-ADDR-LD", 3, 0, 0,
        "-VMA", 3, 0, 1,

        "-ALU-FUNC", 3, 0, 2,
        "ALU0", 3, 0, 3,
        "ALU1", 3, 0, 4,
        "ALU2", 3, 0, 5,
        "ALU3", 3, 0, 6,
        "-AC-LD-INV", 3, 0, 7,

        "-AC-RD", 3, 1, 0,
        "-AC-LD", 3, 1, 1,
        "-SR-LD", 3, 1, 2,
        "BR-TEST", 3, 1, 3,
        "-HL-SWAP", 3, 1, 4,
        "SOFT-HALT", 3, 1, 5,
        "OUT-OFF", 3, 1, 6,
        "OUT-ON", 3, 1, 7,


        "-INTA", 4, 0, 0,
        "-SRC-ADDR", 4, 0, 1,
        "INT-LD-HI", 4, 0, 2,
        "INT-START", 4, 0, 3,
        "INT-EN", 4, 0, 4,
        "LD-INS-REG", 4, 0, 5,
        "UCODE-COUNT-RESET", 4, 0, 6,
        "OPERAND-CLK", 4, 0, 7,

        "-BRANCH-RD", 4, 1, 0,
        "BRANCH-LD-LO", 4, 1, 1,
        "SPARE3", 4, 1, 2,
        "BRANCH-LD-HI", 4, 1, 3,
        "-INT-JMP", 4, 1, 4,
        "INT-LD-LO", 4, 1, 5,
        "-2-BYTE-OPERAND-SEL", 4, 1, 6,
        "-DEST-ADDR", 4, 1, 7,
/*
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
*/

        "", 0, 0,0,
    };
