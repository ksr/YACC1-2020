/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.c
 * Author: ksr77
 *
 * Created on January 11, 2017, 11:33 AM
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ADDR_REGS 4
#define PC 0
#define SP 1
#define IREG0 2
#define IREG1 3

#define DATA_REGS 8
#define MEM_SIZE 65536

#define MEM_ROM 0x01
#define READ_BREAK 0x02
#define WRITE_BREAK 0x04

#define VERBOSE_BEFORE 0x01
#define VERBOSE_AFTER 0x02

int verboseBefore = 0;
int verboseAfter = 0;


unsigned short addrReg[ADDR_REGS] = {0};
unsigned short dataReg[DATA_REGS] = {0};
unsigned char memory[MEM_SIZE] = {0};
unsigned char memoryAttributes[MEM_SIZE] = {0};
unsigned char dataRegAttributes[DATA_REGS] = {0};


unsigned char alu = 0;
unsigned char input = 0;
unsigned char cf = 0;

int singleStep;
int verbose = 0;
char *memoryFileName = "";
char *romFileName = "";
char *directory = "";

#define PROG_SIZE 100
unsigned char prog[PROG_SIZE] = {0x00, 0xfe, 0x80, 0xfe, 0xfd, 0xb2, 0x31, 0x00, 0x01, 0xff, 0x02};

/*
 *
 */




struct insInfo {
    int dec;
    char *name;
    int len;
} ins[256] = {
    0, "A", 0
    ,
    1, "A", 0
    ,
    2, "A", 0
    ,
    3, "A", 0
    ,
    4, "A", 0
    ,
    5, "A", 0
    ,
    6, "A", 0
    ,
    7, "A", 0
    ,
    8, "A", 0
    ,
    9, "A", 0
    ,
    10, "A", 0
    ,
    11, "A", 0
    ,
    12, "A", 0
    ,
    13, "A", 0
    ,
    14, "A", 0
    ,
    15, "A", 0
    ,
    16, "MOVBI", 2
    ,
    17, "MOVBI", 2
    ,
    18, "MOVBI", 2
    ,
    19, "MOVBI", 2
    ,
    20, "MOVBI", 2
    ,
    21, "MOVBI", 2
    ,
    22, "MOVBI", 2
    ,
    23, "MOVBI", 2
    ,
    24, "MOVWI", 3
    ,
    25, "MOVWI", 3
    ,
    26, "MOVWI", 3
    ,
    27, "MOVWI", 3
    ,
    28, "MOVWI", 3
    ,
    29, "MOVWI", 3
    ,
    30, "MOVWI", 3
    ,
    31, "MOVWI", 3
    ,
    32, "MOVBD", 3
    ,
    33, "MOVBD", 3
    ,
    34, "MOVBD", 3
    ,
    35, "MOVBD", 3
    ,
    36, "MOVBD", 3
    ,
    37, "MOVBD", 3
    ,
    38, "MOVBD", 3
    ,
    39, "MOVBD", 3
    ,
    40, "MOVWD", 3
    ,
    41, "MOVWD", 3
    ,
    42, "MOVWD", 3
    ,
    43, "MOVWD", 3
    ,
    44, "MOVWD", 3
    ,
    45, "MOVWD", 3
    ,
    46, "MOVWD", 3
    ,
    47, "MOVWD", 3
    ,
    48, "INC", 1
    ,
    49, "INC", 1
    ,
    50, "INC", 1
    ,
    51, "INC", 1
    ,
    52, "INC", 1
    ,
    53, "INC", 1
    ,
    54, "INC", 1
    ,
    55, "INC", 1
    ,
    56, "DEC", 1
    ,
    57, "DEC", 1
    ,
    58, "DEC", 1
    ,
    59, "DEC", 1
    ,
    60, "DEC", 1
    ,
    61, "DEC", 1
    ,
    62, "DEC", 1
    ,
    63, "DEC", 1
    ,
    64, "MOVE", 2
    ,
    65, "MOVRRA", 2
    ,
    66, "MOVRAR", 2
    ,
    67, "MOVRARA", 2
    ,
    68, "A", 0
    ,
    69, "A", 0
    ,
    70, "A", 0
    ,
    71, "A", 0
    ,
    72, "A", 0
    ,
    73, "A", 0
    ,
    74, "A", 0
    ,
    75, "A", 0
    ,
    76, "A", 0
    ,
    77, "A", 0
    ,
    78, "A", 0
    ,
    79, "A", 0
    ,
    80, "LDAI", 2
    ,
    81, "LDAD", 3
    ,
    82, "LDAR", 2
    ,
    83, "LDARL", 2
    ,
    84, "LDARH", 2
    ,
    85, "STAD", 3
    ,
    86, "STAR", 2
    ,
    87, "STARL", 2
    ,
    88, "STARH", 2
    ,
    89, "A", 0
    ,
    90, "A", 0
    ,
    91, "A", 0
    ,
    92, "A", 0
    ,
    93, "A", 0
    ,
    94, "A", 0
    ,
    95, "A", 0
    ,
    96, "BR", 3
    ,
    97, "BZ", 3
    ,
    98, "BNZ", 3
    ,
    99, "BIZ", 3
    ,
    100, "BINZ", 3
    ,
    101, "BCZ", 3
    ,
    102, "BCNZ", 3
    ,
    103, "A", 0
    ,
    104, "BRRZ", 4
    ,
    105, "BRRZ", 4
    ,
    106, "BRRZ", 4
    ,
    107, "BRRZ", 4
    ,
    108, "BRRZ", 4
    ,
    109, "BRRZ", 4
    ,
    110, "BRRZ", 4
    ,
    111, "BRRZ", 4
    ,
    112, "ADDI", 2
    ,
    113, "SUBI", 2
    ,
    114, "ANDI", 2
    ,
    115, "ORI", 2
    ,
    116, "XORI", 2
    ,
    117, "NOT", 1
    ,
    118, "A", 0
    ,
    119, "A", 0
    ,
    120, "ADDR", 2
    ,
    121, "SUBR", 2
    ,
    122, "ANDR", 2
    ,
    123, "ORR", 2
    ,
    124, "XORR", 2
    ,
    125, "A", 0
    ,
    126, "A", 0
    ,
    127, "A", 0
    ,
    128, "MOVMB", 3
    ,
    129, "MOVMB", 3
    ,
    130, "MOVMB", 3
    ,
    131, "MOVMB", 3
    ,
    132, "MOVMB", 3
    ,
    133, "MOVMB", 3
    ,
    134, "MOVMB", 3
    ,
    135, "MOVMB", 3
    ,
    136, "MOVMW", 3
    ,
    137, "MOVMW", 3
    ,
    138, "MOVMW", 3
    ,
    139, "MOVMW", 3
    ,
    140, "MOVMW", 3
    ,
    141, "MOVMW", 3
    ,
    142, "MOVMW", 3
    ,
    143, "MOVMW", 3
    ,
    144, "A", 0
    ,
    145, "A", 0
    ,
    146, "A", 0
    ,
    147, "A", 0
    ,
    148, "A", 0
    ,
    149, "A", 0
    ,
    150, "A", 0
    ,
    151, "A", 0
    ,
    152, "A", 0
    ,
    153, "A", 0
    ,
    154, "A", 0
    ,
    155, "A", 0
    ,
    156, "A", 0
    ,
    157, "A", 0
    ,
    158, "A", 0
    ,
    159, "A", 0
    ,
    160, "A", 0
    ,
    161, "A", 0
    ,
    162, "A", 0
    ,
    163, "A", 0
    ,
    164, "A", 0
    ,
    165, "A", 0
    ,
    166, "A", 0
    ,
    167, "A", 0
    ,
    168, "A", 0
    ,
    169, "A", 0
    ,
    170, "A", 0
    ,
    171, "A", 0
    ,
    172, "A", 0
    ,
    173, "A", 0
    ,
    174, "A", 0
    ,
    175, "A", 0
    ,
    176, "A", 0
    ,
    177, "A", 0
    ,
    178, "A", 0
    ,
    179, "A", 0
    ,
    180, "A", 0
    ,
    181, "A", 0
    ,
    182, "A", 0
    ,
    183, "A", 0
    ,
    184, "A", 0
    ,
    185, "A", 0
    ,
    186, "A", 0
    ,
    187, "A", 0
    ,
    188, "A", 0
    ,
    189, "A", 0
    ,
    190, "A", 0
    ,
    191, "A", 0
    ,
    192, "A", 0
    ,
    193, "A", 0
    ,
    194, "A", 0
    ,
    195, "A", 0
    ,
    196, "A", 0
    ,
    197, "A", 0
    ,
    198, "A", 0
    ,
    199, "A", 0
    ,
    200, "A", 0
    ,
    201, "A", 0
    ,
    202, "A", 0
    ,
    203, "A", 0
    ,
    204, "A", 0
    ,
    205, "A", 0
    ,
    206, "A", 0
    ,
    207, "A", 0
    ,
    208, "A", 0
    ,
    209, "A", 0
    ,
    210, "A", 0
    ,
    211, "A", 0
    ,
    212, "A", 0
    ,
    213, "A", 0
    ,
    214, "A", 0
    ,
    215, "A", 0
    ,
    216, "A", 0
    ,
    217, "A", 0
    ,
    218, "A", 0
    ,
    219, "A", 0
    ,
    220, "A", 0
    ,
    221, "A", 0
    ,
    222, "A", 0
    ,
    223, "A", 0
    ,
    224, "A", 0
    ,
    225, "A", 0
    ,
    226, "A", 0
    ,
    227, "A", 0
    ,
    228, "A", 0
    ,
    229, "A", 0
    ,
    230, "A", 0
    ,
    231, "A", 0
    ,
    232, "A", 0
    ,
    233, "A", 0
    ,
    234, "A", 0
    ,
    235, "A", 0
    ,
    236, "A", 0
    ,
    237, "A", 0
    ,
    238, "A", 0
    ,
    239, "A", 0
    ,
    240, "A", 0
    ,
    241, "A", 0
    ,
    242, "A", 0
    ,
    243, "A", 0
    ,
    244, "A", 0
    ,
    245, "A", 0
    ,
    246, "A", 0
    ,
    247, "A", 0
    ,
    248, "A", 0
    ,
    249, "A", 0
    ,
    250, "A", 0
    ,
    251, "A", 0
    ,
    252, "A", 0
    ,
    253, "A", 0
    ,
    254, "A", 0
    ,
    255, "A", 0,


};


void unimplemented(int opcode) {
    printf("unimplemented opcode [%02x]\n", opcode);
    exit(0);
}

void dumpRegs() {
    for (int i = 0; i < DATA_REGS; i++)
        printf("R[%d]=%02x ", i, dataReg[i]);
    printf("alu=%02x", alu);
    printf("\n");
}

void init() {
    int i;

    for (i = 0; i < ADDR_REGS; i++) {
        addrReg[i] = 0;
    }
    for (i = 0; i < DATA_REGS; i++) {
        dataReg[i] = 0;
    }
    for (i = 0; i < MEM_SIZE; i++) {
        memory[i] = 0;
    }
    alu = 0;
}

void loadMem() {
    int i;
    for (i = 0; i < PROG_SIZE; i++) {
        memory[i] = prog[i];
    }

}

unsigned char readMemByte(int address) {
    if (verboseAfter) printf("readMemByte memory[%04x]=%02x\n", address, memory[address]);
    return (memory[address]);
}

void writeMemByte(int address, unsigned char value) {
    if (verboseBefore) printf("writeMemByte memory[%04x]=%02x\n", address, memory[address]);
    memory[address] = value;
    if (verboseAfter) printf("writeMemByte memory[%04x]=%02x\n", address, memory[address]);
}

unsigned short readMemWord(int address) {
    unsigned short data;
    data = 0;
    data = memory[address] << 8;
    data |= memory[address + 1];
    if (verboseAfter) printf("readMemWord memory[%04x]=%04x\n", address, data);
    return (data);
}

void writeMemWord(int address, unsigned short value) {
    unsigned short data;

    data = 0;
    data = memory[address] << 8;
    data |= memory[address + 1];
    if (verboseBefore) printf("writeMemWord memory[%04x]=%04x\n", address, data);

    memory[address] = (value >> 8) & 0x0ff;
    memory[address + 1] = value & 0x0ff;

    data = 0;
    data = memory[address] << 8;
    data |= memory[address + 1];
    if (verboseAfter) printf("writeMemWord memory[%04x]=%04x\n", address, data);
}

unsigned char readMemAtRegByte(int reg) {
    unsigned char data;
    data = memory[dataReg[reg]];
    if (verboseAfter) printf("readMemAtRegByte reg[%02x]=%02x\n", reg, data);
    return (data);
}

unsigned short readMemAtRegWord(int reg) {
    unsigned short data;
    data = 0;
    data = memory[dataReg[reg]] << 8;
    data |= memory[dataReg[reg] + 1];
    if (verboseAfter) printf("readMemAtRegWord reg[%02x]=%04x\n", reg, data);
    return (data);
}

void writeMemAtRegByte(int reg, unsigned char value) {
    unsigned short data;
    data = 0;
    data = memory[dataReg[reg]];
    if (verboseBefore) printf("writeMemAtRegByte reg[%02x]=%02x\n", reg, data);
    memory[dataReg[reg]] = value;
    data = memory[dataReg[reg]];
    if (verboseAfter) printf("writeMemAtRegByte reg[%02x]=%02x\n", reg, data);
}

void writeMemAtRegWord(int reg, unsigned short value) {
    unsigned short data;
    data = 0;
    data = memory[dataReg[reg]] << 8;
    data |= memory[dataReg[reg] + 1];
    if (verboseBefore) printf("writeMemAtRegWord reg[%02x]=%04x\n", reg, data);
    memory[dataReg[reg]] = (value >> 8) & 0x0ff;
    memory[dataReg[reg] + 1] = value & 0x0ff;
    data = 0;
    data = memory[dataReg[reg]] << 8;
    data |= memory[dataReg[reg] + 1];
    if (verboseAfter) printf("writeMemAtRegWord reg[%02x]=%04x\n", reg, data);
}

void writeReg(int reg, unsigned short value) {
    if (verboseBefore) printf("writeReg Reg[%02x]=%04x\n", reg, dataReg[reg]);
    dataReg[reg] = value;
    if (verboseAfter) printf("writeReg Reg[%02x]=%04x\n", reg, dataReg[reg]);
}

unsigned short readReg(int unsigned reg) {
    if (verboseAfter) printf("readReg Reg[%02x]=%04x\n", reg, dataReg[reg]);
    return (dataReg[reg]);
}

void writeRegLow(int reg, unsigned char value) {
    unsigned short data;
    if (verboseBefore) printf("writeRegLow Reg[%02x]=%04x\n", reg, dataReg[reg]);
    data = dataReg[reg] & 0xff00;
    data |= value;
    dataReg[reg] = data;
    if (verboseAfter) printf("writeRegLow Reg[%02x]=%04x\n", reg, dataReg[reg]);
}

unsigned char readRegLow(int unsigned reg) {
    if (verboseAfter) printf("readRegLow Reg[%02x]=%04x\n", reg, dataReg[reg]&0xff);
    return (dataReg[reg] & 0xff);
}

void writeRegHi(int reg, unsigned char value) {
    unsigned short data;
    if (verboseBefore) printf("writeRegHi Reg[%02x]=%04x\n", reg, dataReg[reg]);
    data = dataReg[reg] & 0x00ff;
    data |= (value << 8);
    dataReg[reg] = data;
    if (verboseBefore) printf("writeRegHi Reg[%02x]=%04x\n", reg, dataReg[reg]);
}

unsigned char readRegHi(int unsigned reg) {
    if (verboseAfter) printf("readRegHi Reg[%02x]=%04x\n", reg, (dataReg[reg] >> 8)& 0xff);
    return ((dataReg[reg] >> 8) & 0xff);
}

unsigned char nextCharVia(int reg) {
    unsigned char opc;
    opc = memory[addrReg[reg]]; //expand to subroutine calls;
    addrReg[reg]++;
    return (opc);
}

unsigned short nextShortVia(int reg) {
    unsigned short opc;
    opc = (memory[addrReg[reg]] << 8) & 0xff00; //expand to subroutine calls;
    addrReg[reg]++;
    opc |= ((memory[addrReg[reg]]) & 0x00ff);
    addrReg[reg]++;
    return (opc);
}

void branch() {
    unsigned short data;

    data = nextShortVia(PC);
    addrReg[PC] = data;
    printf("branch address %04x %04x\n", data, addrReg[PC]);
}

void branchSkip() {
    addrReg[PC] += 2;
}

void test() {
    unsigned char i;

    memory[0] = 100;
    memory[1] = 200;

    i = nextCharVia(PC);
    printf("%d\n", i);
    i = nextCharVia(PC);
    printf("%d\n", i);
    printf("%d\n", addrReg[PC]);
    exit(0);
}



int execute(unsigned char opcode) {
    int operand;
    int reg;
    int regSrc, regDst;
    unsigned short data;
    unsigned short address;

    if (verbose)
        printf("OPcode=[%02x] %s\n", opcode, ins[opcode].name);
    switch (opcode) {
        case 0x00: // Start state, do not use as an opcode
            break;
        case 0x01:
            unimplemented(opcode);
            break;
        case 0x02:
            unimplemented(opcode);
            break;
        case 0x03:
            unimplemented(opcode);
            break;
        case 0x04:
            unimplemented(opcode);
            break;
        case 0x05:
            unimplemented(opcode);
            break;
        case 0x06:
            unimplemented(opcode);
            break;
        case 0x07:
            unimplemented(opcode);
            break;
        case 0x08:
            unimplemented(opcode);
            break;
        case 0x09:
            unimplemented(opcode);
            break;
        case 0x0a:
            unimplemented(opcode);
            break;
        case 0x0b:
            unimplemented(opcode);
            break;
        case 0x0c:
            unimplemented(opcode);
            break;
        case 0x0d:
            unimplemented(opcode);
            break;
        case 0x0e:
            unimplemented(opcode);
            break;
        case 0x0f:
            unimplemented(opcode);
            break;
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
            reg = opcode & 0x07;
            data = nextCharVia(PC);
            writeRegLow(reg, data);
            break;
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f:
            reg = opcode & 0x07;
            data = nextShortVia(PC);
            writeReg(reg, data);
            break;
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
            reg = opcode & 0x07;
            address = nextShortVia(PC);
            writeRegLow(reg, readMemByte(address));
            break;
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            reg = opcode & 0x07;
            address = nextShortVia(PC);
            writeReg(reg, readMemWord(address));
            break;
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
            writeReg(reg, readReg(reg) + 1);
            break;
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
            writeReg(reg, readReg(reg) - 1);
            break;
        case 0x40:
            reg = nextCharVia(PC);
            regSrc = reg & 0x07;
            regDst = (reg >> 4) & 0x07;
            writeReg(regDst, readReg(regSrc));
            break;
        case 0x41:
            reg = nextCharVia(PC);
            regSrc = reg & 0x07;
            regDst = (reg >> 4) & 0x07;
            writeReg(regDst, readMemAtRegWord(regSrc));
            break;
        case 0x42:
            reg = nextCharVia(PC);
            regSrc = reg & 0x07;
            regDst = (reg >> 4) & 0x07;
            writeMemAtRegWord(regDst, readReg(regSrc));
            break;
        case 0x43:
            reg = nextCharVia(PC);
            regSrc = reg & 0x07;
            regDst = (reg >> 4) & 0x07;
            writeMemAtRegWord(regDst, readMemAtRegWord(regSrc));
            break;
        case 0x44:
            unimplemented(opcode);
            break;
        case 0x45:
            unimplemented(opcode);
            break;
        case 0x46:
            unimplemented(opcode);
            break;
        case 0x47:
            unimplemented(opcode);
            break;
        case 0x48:
            unimplemented(opcode);
            break;
        case 0x49:
            unimplemented(opcode);
            break;
        case 0x4a:
            unimplemented(opcode);
            break;
        case 0x4b:
            unimplemented(opcode);
            break;
        case 0x4c:
            unimplemented(opcode);
            break;
        case 0x4d:
            unimplemented(opcode);
            break;
        case 0x4e:
            unimplemented(opcode);
            break;
        case 0x4f:
            unimplemented(opcode);
            break;
        case 0x50:
            alu = nextCharVia(PC);
            break;
        case 0x51:
            address = nextShortVia(PC);
            alu = readMemByte(address);
            break;
        case 0x52:
            reg = nextCharVia(PC);
            alu = readMemAtRegByte(reg);
            break;
        case 0x53:
            reg = nextCharVia(PC);
            alu = readRegLow(reg);
            break;
        case 0x54:
            reg = nextCharVia(PC);
            alu = readRegHi(reg);
            break;
        case 0x55:
            address = nextShortVia(PC);
            writeMemByte(address, alu);
            break;
        case 0x56:
            reg = nextCharVia(PC);
            writeMemAtRegByte(reg, alu);
            break;
        case 0x57:
            reg = nextCharVia(PC);
            writeRegLow(reg, alu);
            break;
        case 0x58:
            reg = nextCharVia(PC);
            writeRegHi(reg, alu);
            break;
        case 0x59:
            unimplemented(opcode);
            break;
        case 0x5a:
            unimplemented(opcode);
            break;
        case 0x5b:
            unimplemented(opcode);
            break;
        case 0x5c:
            unimplemented(opcode);
            break;
        case 0x5d:
            unimplemented(opcode);
            break;
        case 0x5e:
            unimplemented(opcode);
            break;
        case 0x5f:
            unimplemented(opcode);
            break;
        case 0x60:
            address = nextShortVia(PC);
            addrReg[PC] = address;
            break;
        case 0x61:
            address = nextShortVia(PC);
            if (alu == 0)
                addrReg[PC] = address;
            break;
        case 0x62:
            address = nextShortVia(PC);
            if (alu != 0)
                addrReg[PC] = address;
            break;
        case 0x63:
            address = nextShortVia(PC);
            if (input == 0)
                addrReg[PC] = address;
            break;
        case 0x64:
            address = nextShortVia(PC);
            if (input != 0)
                addrReg[PC] = address;
            break;
        case 0x65:
            address = nextShortVia(PC);
            if (cf == 0)
                addrReg[PC] = address;
            break;
        case 0x66:
            address = nextShortVia(PC);
            if (cf != 0)
                addrReg[PC] = address;
            break;
        case 0x67:
            unimplemented(opcode);
            break;
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
            reg = opcode & 0x07;
            address = nextShortVia(PC);
            if (dataReg[reg] == 0)
                addrReg[PC] = address;
            break;
        case 0x70:
            alu += nextCharVia(PC);
            break;
        case 0x71:
            alu -= nextCharVia(PC);
            break;
        case 0x72:
            alu &= nextCharVia(PC);
            break;
        case 0x73:
            alu |= nextCharVia(PC);
            break;
        case 0x74:
            alu ^= nextCharVia(PC);
            break;
        case 0x75:
            alu = ~alu;
            break;
        case 0x76:
            unimplemented(opcode);
            break;
        case 0x77:
            unimplemented(opcode);
            break;
        case 0x78:
            reg = nextCharVia(PC);
            alu += memory[dataReg[reg]];
            break;
        case 0x79:
            reg = nextCharVia(PC);
            alu -= memory[dataReg[reg]];
            break;
        case 0x7a:
            reg = nextCharVia(PC);
            alu &= memory[dataReg[reg]];
            break;
        case 0x7b:
            reg = nextCharVia(PC);
            alu |= memory[dataReg[reg]];
            break;
        case 0x7c:
            reg = nextCharVia(PC);
            alu ^= memory[dataReg[reg]];
            break;
        case 0x7d:
            unimplemented(opcode);
            break;
        case 0x7e:
            unimplemented(opcode);
            break;
        case 0x7f:
            unimplemented(opcode);
            break;
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
            reg = opcode & 0x07;
            address = nextShortVia(PC);
            writeMemByte(address, readRegLow(reg));
            break;
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
            reg = opcode & 0x07;
            address = nextShortVia(PC);
            writeMemWord(address, readReg(reg));
            break;
        case 0x90:
            unimplemented(opcode);
            break;
        case 0x91:
            unimplemented(opcode);
            break;
        case 0x92:
            unimplemented(opcode);
            break;
        case 0x93:
            unimplemented(opcode);
            break;
        case 0x94:
            unimplemented(opcode);
            break;
        case 0x95:
            unimplemented(opcode);
            break;
        case 0x96:
            unimplemented(opcode);
            break;
        case 0x97:
            unimplemented(opcode);
            break;
        case 0x98:
            unimplemented(opcode);
            break;
        case 0x99:
            unimplemented(opcode);
            break;
        case 0x9a:
            unimplemented(opcode);
            break;
        case 0x9b:
            unimplemented(opcode);
            break;
        case 0x9c:
            unimplemented(opcode);
            break;
        case 0x9d:
            unimplemented(opcode);
            break;
        case 0x9e:
            unimplemented(opcode);
            break;
        case 0x9f:
            unimplemented(opcode);
            break;
        case 0xa0:
            unimplemented(opcode);
            break;
        case 0xa1:
            unimplemented(opcode);
            break;
        case 0xa2:
            unimplemented(opcode);
            break;
        case 0xa3:
            unimplemented(opcode);
            break;
        case 0xa4:
            unimplemented(opcode);
            break;
        case 0xa5:
            unimplemented(opcode);
            break;
        case 0xa6:
            unimplemented(opcode);
            break;
        case 0xa7:
            unimplemented(opcode);
            break;
        case 0xa8:
            unimplemented(opcode);
            break;
        case 0xa9:
            unimplemented(opcode);
            break;
        case 0xaa:
            unimplemented(opcode);
            break;
        case 0xab:
            unimplemented(opcode);
            break;
        case 0xac:
            unimplemented(opcode);
            break;
        case 0xad:
            unimplemented(opcode);
            break;
        case 0xae:
            unimplemented(opcode);
            break;
        case 0xaf:
            unimplemented(opcode);
            break;
        case 0xb0:
            unimplemented(opcode);
            break;
        case 0xb1:
            unimplemented(opcode);
            break;
        case 0xb2:
            unimplemented(opcode);
            break;
        case 0xb3:
            unimplemented(opcode);
            break;
        case 0xb4:
            unimplemented(opcode);
            break;
        case 0xb5:
            unimplemented(opcode);
            break;
        case 0xb6:
            unimplemented(opcode);
            break;
        case 0xb7:
            unimplemented(opcode);
            break;
        case 0xb8:
            unimplemented(opcode);
            break;
        case 0xb9:
            unimplemented(opcode);
            break;
        case 0xba:
            unimplemented(opcode);
            break;
        case 0xbb:
            unimplemented(opcode);
            break;
        case 0xbc:
            unimplemented(opcode);
            break;
        case 0xbd:
            unimplemented(opcode);
            break;
        case 0xbe:
            unimplemented(opcode);
            break;
        case 0xbf:
            unimplemented(opcode);
            break;
        case 0xc0:
            unimplemented(opcode);
            break;
        case 0xc1:
            unimplemented(opcode);
            break;
        case 0xc2:
            unimplemented(opcode);
            break;
        case 0xc3:
            unimplemented(opcode);
            break;
        case 0xc4:
            unimplemented(opcode);
            break;
        case 0xc5:
            unimplemented(opcode);
            break;
        case 0xc6:
            unimplemented(opcode);
            break;
        case 0xc7:
            unimplemented(opcode);
            break;
        case 0xc8:
            unimplemented(opcode);
            break;
        case 0xc9:
            unimplemented(opcode);
            break;
        case 0xca:
            unimplemented(opcode);
            break;
        case 0xcb:
            unimplemented(opcode);
            break;
        case 0xcc:
            unimplemented(opcode);
            break;
        case 0xcd:
            unimplemented(opcode);
            break;
        case 0xce:
            unimplemented(opcode);
            break;
        case 0xcf:
            unimplemented(opcode);
            break;
        case 0xd0:
            unimplemented(opcode);
            break;
        case 0xd1:
            unimplemented(opcode);
            break;
        case 0xd2:
            unimplemented(opcode);
            break;
        case 0xd3:
            unimplemented(opcode);
            break;
        case 0xd4:
            unimplemented(opcode);
            break;
        case 0xd5:
            unimplemented(opcode);
            break;
        case 0xd6:
            unimplemented(opcode);
            break;
        case 0xd7:
            unimplemented(opcode);
            break;
        case 0xd8:
            unimplemented(opcode);
            break;
        case 0xd9:
            unimplemented(opcode);
            break;
        case 0xda:
            unimplemented(opcode);
            break;
        case 0xdb:
            unimplemented(opcode);
            break;
        case 0xdc:
            unimplemented(opcode);
            break;
        case 0xdd:
            unimplemented(opcode);
            break;
        case 0xde:
            unimplemented(opcode);
            break;
        case 0xdf:
            unimplemented(opcode);
            break;
        case 0xe0:
            unimplemented(opcode);
            break;
        case 0xe1:
            unimplemented(opcode);
            break;
        case 0xe2:
            unimplemented(opcode);
            break;
        case 0xe3:
            unimplemented(opcode);
            break;
        case 0xe4:
            unimplemented(opcode);
            break;
        case 0xe5:
            unimplemented(opcode);
            break;
        case 0xe6:
            unimplemented(opcode);
            break;
        case 0xe7:
            unimplemented(opcode);
            break;
        case 0xe8:
            unimplemented(opcode);
            break;
        case 0xe9:
            unimplemented(opcode);
            break;
        case 0xea:
            unimplemented(opcode);
            break;
        case 0xeb:
            unimplemented(opcode);
            break;
        case 0xec:
            unimplemented(opcode);
            break;
        case 0xed:
            unimplemented(opcode);
            break;
        case 0xee:
            unimplemented(opcode);
            break;
        case 0xef:
            unimplemented(opcode);
            break;
        case 0xf0:
            unimplemented(opcode);
            break;
        case 0xf1:
            unimplemented(opcode);
            break;
        case 0xf2:
            unimplemented(opcode);
            break;
        case 0xf3:
            unimplemented(opcode);
            break;
        case 0xf4:
            unimplemented(opcode);
            break;
        case 0xf5:
            unimplemented(opcode);
            break;
        case 0xf6:
            unimplemented(opcode);
            break;
        case 0xf7:
            unimplemented(opcode);
            break;
        case 0xf8:
            unimplemented(opcode);
            break;
        case 0xf9:
            unimplemented(opcode);
            break;
        case 0xfa:
            unimplemented(opcode);
            break;
        case 0xfb:
            unimplemented(opcode);
            break;
        case 0xfc:
            unimplemented(opcode);
            break;
        case 0xfd:
            unimplemented(opcode);
            break;
        case 0xfe:
            dumpRegs();
            break;
        case 0xff:
            return (-1);
            break;

            /* OLD

                    case 0:
                        break;

                    case 1:
                        operand = nextVia(PC);
                        printf("Operand=%02x\n", operand);
                        break;

                    case 0x10: //load data reg immeadiate
                    case 0x11:
                    case 0x12:
                    case 0x13:
                    case 0x14:
                    case 0x15:
                    case 0x16:
                    case 0x17:
                        reg = opcode & 0x07;
                        data = nextVia(PC) << 8;
                        data |= nextVia(PC);
                        dataReg[reg] = data;
                        break;

                    case 0x20: //load addr reg immeadiate
                    case 0x21:
                    case 0x22:
                    case 0x23:
                    case 0x24:
                    case 0x25:
                    case 0x26:
                    case 0x27:
                        reg = opcode & 0x07;
                        data = nextVia(PC) << 8;
                        data |= nextVia(PC);
                        addrReg[reg] = data;
                        break;

                    case 0xb0: //alu == 0
                        if (alu == 0)
                            branch();
                        else
                            branchSkip();
                        break;

                    case 0xb1: // alu !=0
                        if (alu != 0) {
                            printf("branch taken\n");
                            branch();
                        } else {
                            printf("branch not taken\n");
                            branchSkip();
                        }
                        break;

                    case 0xb2: // if alu == imm
                        data = nextVia(PC);
                        if (alu == data) {
                            printf("branch taken\n");
                            branch();
                        } else {
                            printf("branch not taken\n");
                            branchSkip();
                        }
                        break;
                    case 0xb3: // if alu < imm
                        data = nextVia(PC);
                        if (alu < data)
                            branch();
                        else
                            branchSkip();
                        break;
                    case 0xb4: // if alu > imm
                        data = nextVia(PC);
                        if (alu > data)
                            branch();
                        else
                            branchSkip();
                        break;

                    case 0xc0: // mv alu -> reg lo
                    case 0xc1:
                    case 0xc2:
                    case 0xc3:
                    case 0xc4:
                    case 0xc5:
                    case 0xc6:
                    case 0xc7:
                        dataReg[reg] = (dataReg[reg] & 0xff00) | alu;
                        break;
                    case 0xc8: // mv reg lo -> alu
                    case 0xc9:
                    case 0xca:
                    case 0xcb:
                    case 0xcc:
                    case 0xcd:
                    case 0xce:
                    case 0xcf:
                        alu = dataReg[reg] & 0x00ff;
                        break;

                    case 0xd0: // mv alu -> reg hi
                    case 0xd1:
                    case 0xd2:
                    case 0xd3:
                    case 0xd4:
                    case 0xd5:
                    case 0xd6:
                    case 0xd7:
                        dataReg[reg] = (dataReg[reg] & 0x00ff) | (alu << 8);
                        break;
                    case 0xd8: // mv reg hi -> alu
                    case 0xd9:
                    case 0xda:
                    case 0xdb:
                    case 0xdc:
                    case 0xdd:
                    case 0xde:
                    case 0xdf:
                        alu = (dataReg[reg] & 0xff00) >> 8;
                        break;



                    case 0xe0: //add imm
                        alu = alu + nextVia(PC);
                        break;
                    case 0xe1: //sub imm
                        alu = alu - nextVia(PC);
                        break;
                    case 0xe2: // and imm
                        alu = alu | nextVia(PC);
                        break;
                    case 0xe3: // or imm
                        alu = alu & nextVia(PC);
                        break;
                    case 0xe4: // xor imm
                        alu = alu ^ nextVia(PC);
                        break;
                    case 0xe5: // shr
                        alu = alu >> 1;
                        break;
                    case 0xe6: //shl
                        alu = alu << 1;
                        break;
                    case 0xe7: // ld alu imm
                        alu = nextVia(PC);
                        break;

                    case 0xf0: //push pc
                        memory[addrReg[SP]] = (addrReg[PC] & 0xff00) >> 8;
                        addrReg[SP]--;
                        memory[addrReg[SP]] = addrReg[PC] & 0x00ff;
                        addrReg[SP]--;
                        break;
                    case 0xf1: // pop pc
                        addrReg[SP]++;
                        data = memory[addrReg[SP]];
                        addrReg[SP]++;
                        data |= (memory[addrReg[SP]] << 8);
                        break;

                    case 0xfd:
                        putchar(alu);
                        break;
                    case 0xfe:
                        alu = getchar();
                        printf("ALU=%02x\n", alu);
                        break;
                    case 0xff:
                        printf("HALT\n");
                        exit(0);
             */
    }
}

int main(int argc, char** argv) {
    unsigned char opcode;
    int c;
    char str[100];

    while ((c = getopt(argc, argv, "v:sm:r:d:")) != -1)
        switch (c) {

            case 's':
                singleStep = 1;
                break;
            case 'v':
                verbose = 1;
                int tmp = atoi(optarg);
                if (tmp & VERBOSE_BEFORE)
                    verboseBefore = 1;
                if (tmp & VERBOSE_AFTER)
                    verboseAfter = 1;

                break;
            case 'r':
                romFileName = optarg;
                break;
            case 'm':
                memoryFileName = optarg;
                break;
            case 'd':
                directory = optarg;
                break;
            case '?':
                if (optopt == 'c')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
                return 1;
            default:
                abort();
        }
    init();
    printf("verbose=[%d] verboseBefore=[%d] verboseAfter=[%d]\n", verbose, verboseBefore, verboseAfter);
    printf("singleStep=[%d] ramfile=[%s] romfile=[%s] directory=[%s]\n",
            singleStep, memoryFileName, romFileName, directory);


    if (load_file(strcat(directory, memoryFileName)) == -1)
        loadMem();




    //test();

    while (1) {
        if(verbose)
            printf("PC=%02x\n", addrReg[PC]);
        opcode = nextCharVia(PC);
        if (singleStep) {
            dumpRegs();
            gets(&str);
        if (execute(opcode) == -1)
            exit(0);
        
        }
    }

    printf("counter expired\n");
    return (EXIT_SUCCESS);
}

