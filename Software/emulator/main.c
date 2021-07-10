/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: kennethrother
 *
 * Created on October 23, 2020, 1:04 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "../opcodes.h"

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINTF(...)  printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#define MEMORY_SIZE 0x10000
#define REGISTERS 9
#define PC 0
#define SP 1
#define PORTS 16

#define IR 8 //HACK

#define REG0 0
#define REG1 1
#define REG2 2
#define REG3 3
#define REG4 4
#define REG5 5
#define REG6 6
#define REG7 7

#define PORT0 0
#define PORT1 1
#define PORT2 2
#define PORT3 3
#define PORT4 4
#define PORT5 5
#define PORT6 6
#define PORT7 7
#define PORT8 8
#define PORT9 9
#define PORT10 10
#define PORT11 11
#define PORT12 12
#define PORT13 13
#define PORT14 14
#define PORT15 15

union Register {
    unsigned char byte[2];
    unsigned short word;
} registers[REGISTERS];

unsigned char port[PORTS];

unsigned char memory[MEMORY_SIZE];

unsigned char acc;
unsigned char treg;
int out;
int in;
int jsrdepth;
int pushpopdepth;
int pushpoprdepth;
bool carry;
int firstSwitchRead = 1; // What is this


/* Intel HEX read/write functions, Paul Stoffregen, paul@ece.orst.edu */
/* This code is in the public domain.  Please retain my name and */
/* email address in distributed copies, and let me know about any bugs */

/* I, Paul Stoffregen, give no warranty, expressed or implied for */
/* this software and/or documentation provided, including, without */
/* limitation, warranty of merchantability and fitness for a */
/* particular purpose. */

/* some ansi prototypes.. maybe ought to make a .h file */

/* this loads an intel hex file into the memory[] array */
void load_file(char *filename);

/* this is used by load_file to get each line of intex hex */
int parse_hex_line(char *theline, int bytes[], int *addr, int *num, int *code);

/* parses a line of intel hex code, stores the data in bytes[] */
/* and the beginning address in addr, and returns a 1 if the */
/* line was valid, or a 0 if an error occured.  The variable */

/* num gets the number of bytes that were stored into bytes[] */

int parse_hex_line(theline, bytes, addr, num, code)
char *theline;
int *addr, *num, *code, bytes[];
{
    int sum, len, cksum;
    char *ptr;

    *num = 0;
    if (theline[0] != ':') return 0;
    if (strlen(theline) < 11) return 0;
    ptr = theline + 1;
    if (!sscanf(ptr, "%02x", &len)) return 0;
    ptr += 2;
    if (strlen(theline) < (11 + (len * 2))) return 0;
    if (!sscanf(ptr, "%04x", addr)) return 0;
    ptr += 4;
    /* printf("Line: length=%d Addr=%d\n", len, *addr); */
    if (!sscanf(ptr, "%02x", code)) return 0;
    ptr += 2;
    sum = (len & 255) + ((*addr >> 8) & 255) + (*addr & 255) + (*code & 255);
    while (*num != len) {
        if (!sscanf(ptr, "%02x", &bytes[*num])) return 0;
        ptr += 2;
        sum += bytes[*num] & 255;
        (*num)++;
        if (*num >= 256) return 0;
    }
    if (!sscanf(ptr, "%02x", &cksum)) return 0;
    if (((sum & 255) + (cksum & 255)) & 255) return 0; /* checksum error */
    return 1;
}

/* loads an intel hex file into the global memory[] array */

/* filename is a string of the file to be opened */

void load_file(filename)
char *filename;
{
    unsigned char line[1000];
    FILE *fin;
    int addr, n, status, bytes[256];
    int i, total = 0, lineno = 1;
    int minaddr = 65536, maxaddr = 0;

    if (strlen(filename) == 0) {
        printf("   Can't load a file without the filename.");
        printf("  '?' for help\n");
        return;
    }
    fin = fopen(filename, "r");
    if (fin == NULL) {
        printf("   Can't open file '%s' for reading.\n", filename);
        return;
    }
    while (!feof(fin) && !ferror(fin)) {
        line[0] = '\0';
        fgets(line, 1000, fin);
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (line[strlen(line) - 1] == '\r') line[strlen(line) - 1] = '\0';
        if (parse_hex_line(line, bytes, &addr, &n, &status)) {
            if (status == 0) { /* data */
                for (i = 0; i <= (n - 1); i++) {
                    memory[addr] = bytes[i] & 255;
                    total++;
                    if (addr < minaddr) minaddr = addr;
                    if (addr > maxaddr) maxaddr = addr;
                    addr++;
                }
            }
            if (status == 1) { /* end of file */
                fclose(fin);
                printf("   Loaded %d bytes between:", total);
                printf(" %04X to %04X\n", minaddr, maxaddr);
                return;
            }
            if (status == 2)
                ; /* begin of file */
        } else {
            printf("   Error: '%s', line: %d\n", filename, lineno);
        }
        lineno++;
    }
}

void regdump() {
    int i;

    printf("reg dump - ");
    for (i = 0; i < REGISTERS; i++)
        printf("%d=[%04x]", i, registers[i]);
    printf("\n");
}

void dump(int start, int end) {
    int i;

    printf("dump %04x %04x\n", start, end);
    for (i = start; i < end; i++) {
        if ((i & 0x0f) == 0)
            printf("\n %04x ", i);
        printf("%02x ", memory[i]);
    }
    printf("\n");
}

unsigned char memory_read(int address) {
    return (memory[address]);
}

void memory_write(int address, unsigned char value) {
    if (address > 0xdfff) {
        printf("Rom Write\n");
        regdump();
        exit(0);
    }
    /*   if ((address == 0x0204) && (value == 0)) {
           printf("argh\n");
           regdump();
       }
     */
    memory[address] = value;
}

unsigned char register_read_lo(int registernum) {
    if (registernum > REGISTERS) { //HACK FIX HARD CODE 8
        printf("reg error\n");
        exit(0);
    }
    return (registers[registernum].byte[0]);
}

unsigned char register_read_hi(int registernum) {
    if (registernum > REGISTERS) {
        printf("reg error\n");
        exit(0);
    }
    return (registers[registernum].byte[1]);
}

unsigned short register_read_word(int registernum) {
    if (registernum > REGISTERS) {
        printf("reg error\n");
        exit(0);
    }
    return (registers[registernum].word);
}

void register_write_lo(int registernum, unsigned char val) {
    if (registernum > REGISTERS) {
        printf("reg error\n");
        exit(0);
    }
    registers[registernum].byte[0] = val;
}

void register_write_hi(int registernum, unsigned char val) {
    if (registernum > REGISTERS) {
        printf("reg error\n");
        exit(0);
    }
    registers[registernum].byte[1] = val;
}

void register_write_word(int registernum, unsigned short val) {
    if (registernum > REGISTERS) {
        printf("reg error\n");
        exit(0);
    }
    registers[registernum].word = val;
}

void register_inc(int registernum) {
    registers[registernum].word++;
}

void register_dec(int registernum) {
    registers[registernum].word--;
}

unsigned char acc_read() {
    return acc;
}

void acc_write(unsigned char val) {
    acc = val;
}

unsigned char tmp_read() {
    return treg;
}

void tmp_write(unsigned char val) {
    treg = val;
}

void out_on() {
    out = 1;
    //printf("out on\n");
}

void out_off() {
    out = 0;
    //printf("out off\n");
}

void badOpcode(int ins) {
    printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
    exit(0);
}
#define RUN 0
#define HALT 1
#define SINGLESTEP 2
#define JUMPOVER 3
int stepmode = 0;
int jsrdepth = 0;
int jsrover = 0;

char mygetchar();

void debug_mode(int mode) {
    char c;
    while (1) {
        c = toupper(mygetchar());
        if (c == 'C') {
            return;
        } else if (c == 'S') {
            stepmode = SINGLESTEP;
            jsrover = 0;
            return;
        } else if (c == 'R') {
            stepmode = RUN;
            return;
        } else if (c == 'J') {
            stepmode = JUMPOVER;
            jsrover = jsrdepth;
            return;
        } else if (c == 'D') {
            dump(0x0200, 0x021f);
            dump(0x0f80, 0x0f8f);
            dump(0x0400, 0x040F);
            dump(0x1000, 0x10FF);
            regdump();
        } else {
            return;
        }
    }
}


struct termios orig_termios;

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    //atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);
    //raw.c_oflag &= ~(OPOST);
    //raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_lflag &= ~(ICANON | IEXTEN | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char mygetchar() {
    enableRawMode();
    char c;

    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        /*    if (iscntrl(c)) {
              printf("%d\n", c);
            } else {
              printf("%d ('%c')\n", c, c);
            }
         * */
        //printf("%02x\n",c);
        if (c == 0x0d) {
            //printf("conv %02x\n", c);
            c = 0x0a;
            //printf("conv %02x\n", c);
        }
        return (c);
    }
    //return (0); //ksr hack
}

void myputchar(char c) {
    write(STDOUT_FILENO, &c, 1);
    //printf("me\n");
}

int main(int argc, char** argv) {
    int i, ins;
    int reg;
    int portaddr;
    unsigned char hi, lo;
    int src, dest;
    bool tmpCarry;
    char c;

    enableRawMode();
    int cc;

    /* ech testwhile (1) {
        myputchar(mygetchar());
        printf(" cc=%d",cc++);
     }
     */

    in = 0;
    registers[PC].word = 0xf000;
    int singleStep = 0;
    int breakAddress = 0;
    pushpopdepth = 0;
    pushpoprdepth = 0;
    int regtrigger = 0;

    load_file("../Assembler/basic.img");
    load_file("../Assembler/monitor.img");
    dump(0xf700, 0xf7ff);

    while (1) {

        ins = memory_read(register_read_word(PC));

        // trigger condition, for instance test PC value or a reg value
        if (registers[PC].word == 0x0000) {
            debug_mode(SINGLESTEP);
        }


        if (stepmode != 0) {
            if (stepmode == SINGLESTEP) {
                for (int t = 3; t < jsrdepth; t++)
                    printf("->");
                printf("singlestep [%02x] at [%04x] acc=[%02x] tmp=[%02x] R3=[%04x] R7=[%04x]\n", ins, registers[PC].word, acc, treg, register_read_word(3), register_read_word(7));
                debug_mode(SINGLESTEP);
            }

            if ((stepmode == JUMPOVER) && (jsrover <= jsrdepth)) {
                for (int t = 3; t < jsrdepth; t++)
                    printf("->");
                printf("singlestep [%02x] at [%04x] acc=[%02x] R3=[%04x] R7=[%04x]\n", ins, registers[PC].word, acc, register_read_word(3), register_read_word(7));
                debug_mode(SINGLESTEP);
            }
        }

        if ((register_read_word(PC) >= 0xe000) && (register_read_word(PC) <= 0xEFFF)) {
            for (int t = 3; t < jsrdepth; t++)
                DEBUG_PRINTF("->");
            DEBUG_PRINTF("Process opcode [%02x] at [%04x] acc=[%02x] R3=[%04x] R7=[%04x]\n", ins, registers[PC].word, acc, register_read_word(3), register_read_word(7));
        }

        register_inc(PC);
        switch (ins) {
            case 0x00:
                printf("Bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                exit(0);
                break;

            case ON:
                out_on();
                break;

            case OFF:
                out_off();
                break;

            case 0x03:

                printf("\nHalt[%02x] at [%04x] acc=[%02x] tmp=[%02x] R3=[%04x] R7=[%04x]\n", ins, registers[PC].word - 1, acc, treg, register_read_word(3), register_read_word(7));
                dump(0x0200, 0x021f);
                dump(0x0f80, 0x0f8f);
                dump(0x0400, 0x040F);
                dump(0x1000, 0x1020);
                debug_mode(HALT);
                break;

            case JSR:
                jsrdepth++;
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);

                DEBUG_PRINTF("JSR pc=%04x sp=%04x jsrd=%d %02x%02x\n", (register_read_word(PC) - 3), register_read_word(SP), jsrdepth, hi, lo);
                //dump(0xdump0200, 0x020f);
                memory_write(register_read_word(SP), register_read_hi(PC));
                register_dec(SP);
                memory_write(register_read_word(SP), register_read_lo(PC));
                register_dec(SP);

                register_write_hi(PC, hi);
                register_write_lo(PC, lo);

                //printf("jsr opcode pc[%04x]\n", registers[PC].word);
                break;

            case RET:
                jsrdepth--;
                DEBUG_PRINTF("RET pc=%04x sp=%04x jsrd=%d ", (register_read_word(PC) - 1), register_read_word(SP), jsrdepth);
                //dump(0x0200, 0x020f);
                register_inc(SP);
                register_write_lo(PC, memory_read(register_read_word(SP)));
                register_inc(SP);
                register_write_hi(PC, memory_read(register_read_word(SP)));


                DEBUG_PRINTF(" return to %02x%02x\n", register_read_hi(PC), register_read_lo(PC));

                break;

            case JSRUR:
                jsrdepth++;
                reg = memory_read(register_read_word(PC)) & 0x07;
                register_inc(PC);

                DEBUG_PRINTF("JSR pc=%04x sp=%04x jsrd=%d %02x%02x\n", (register_read_word(PC) - 3), register_read_word(SP), jsrdepth, hi, lo);
                //dump(0xdump0200, 0x020f);
                memory_write(register_read_word(SP), register_read_hi(PC));
                register_dec(SP);
                memory_write(register_read_word(SP), register_read_lo(PC));
                register_dec(SP);

                register_write_hi(PC, register_read_lo(reg));
                register_write_lo(PC, register_read_hi(reg));

                //printf("jsr opcode pc[%04x]\n", registers[PC].word);
                //printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;

            case PUSHR:
                reg = memory_read(register_read_word(PC)) & 0x07;
                register_inc(PC);
                //DEBUG_PRINTF("PUSHR reg=%d pc=%04x pre-sp=%04x\n", reg, (register_read_word(PC) - 1), register_read_word(SP));
                memory_write(register_read_word(SP), register_read_hi(reg));
                register_dec(SP);
                memory_write(register_read_word(SP), register_read_lo(reg));
                register_dec(SP);
                break;

            case POPR:
                reg = (memory_read(register_read_word(PC)) >> 4) & 0x07;
                register_inc(PC);
                //DEBUG_PRINTF("POPR reg=%d pc=%04x pre-sp=%04x\n", reg, (register_read_word(PC) - 1), register_read_word(SP));
                register_inc(SP);
                register_write_lo(reg, memory_read(register_read_word(SP)));
                register_inc(SP);
                register_write_hi(reg, memory_read(register_read_word(SP)));
                break;

            case PUSH:
                pushpopdepth++;
                //DEBUG_PRINTF("PUSH pc=%04x pre-sp=%04x ppd=%d\n", (register_read_word(PC) - 1), register_read_word(SP), pushpopdepth);
                memory_write(register_read_word(SP), acc_read());
                register_dec(SP);
                break;

            case POP:
                pushpopdepth--;
                //DEBUG_PRINTF("POP pc=%04x pre-sp=%04x ppd=%d\n", (register_read_word(PC) - 1), register_read_word(SP), pushpopdepth);
                register_inc(SP);
                acc_write(memory_read(register_read_word(SP)));
                break;

            case MVAT:
                treg = acc;
                break;

            case MVTA:
                acc = treg;
                break;

            case LDTI:
                treg = memory_read(register_read_word(PC));
                register_inc(PC);
                break;

            case LDAI:
                acc = memory_read(register_read_word(PC));
                register_inc(PC);
                break;

            case MOVRR:
                reg = memory_read(register_read_word(PC));
                register_inc(PC);
                src = reg & 0x0f;
                dest = (reg >> 4) & 0x0f;
                register_write_word(dest, register_read_word(src));
                break;

            case MVIB + REG0:
            case MVIB + REG1:
            case MVIB + REG2:
            case MVIB + REG3:
            case MVIB + REG4:
            case MVIB + REG5:
            case MVIB + REG6:
            case MVIB + REG7:
                reg = ins & 0x07;
                register_write_lo(reg, memory_read(register_read_word(PC)));
                register_inc(PC);
                break;

            case MVIW + REG0:
            case MVIW + REG1:
            case MVIW + REG2:
            case MVIW + REG3:
            case MVIW + REG4:
            case MVIW + REG5:
            case MVIW + REG6:
            case MVIW + REG7:
                reg = ins & 0x07;
                register_write_hi(reg, memory_read(register_read_word(PC)));
                register_inc(PC);
                register_write_lo(reg, memory_read(register_read_word(PC)));
                register_inc(PC);
                break;

            case MVRLA + REG0:
            case MVRLA + REG1:
            case MVRLA + REG2:
            case MVRLA + REG3:
            case MVRLA + REG4:
            case MVRLA + REG5:
            case MVRLA + REG6:
            case MVRLA + REG7:
                reg = ins & 0x07;
                acc_write(register_read_lo(reg));
                break;

            case MVRHA + REG0:
            case MVRHA + REG1:
            case MVRHA + REG2:
            case MVRHA + REG3:
            case MVRHA + REG4:
            case MVRHA + REG5:
            case MVRHA + REG6:
            case MVRHA + REG7:
                reg = ins & 0x07;
                acc_write(register_read_hi(reg));
                break;

            case MVARL + REG0:
            case MVARL + REG1:
            case MVARL + REG2:
            case MVARL + REG3:
            case MVARL + REG4:
            case MVARL + REG5:
            case MVARL + REG6:
            case MVARL + REG7:
                reg = ins & 0x07;
                register_write_lo(reg, acc_read());
                break;

            case MVARH + REG0:
            case MVARH + REG1:
            case MVARH + REG2:
            case MVARH + REG3:
            case MVARH + REG4:
            case MVARH + REG5:
            case MVARH + REG6:
            case MVARH + REG7:
                reg = ins & 0x07;
                register_write_hi(reg, acc_read());
                break;

            case LDAVR + REG0:
            case LDAVR + REG1:
            case LDAVR + REG2:
            case LDAVR + REG3:
            case LDAVR + REG4:
            case LDAVR + REG5:
            case LDAVR + REG6:
            case LDAVR + REG7:
                reg = ins & 0x07;
                acc = memory[registers[reg].word];
                break;

            case STAVR + REG0:
            case STAVR + REG1:
            case STAVR + REG2:
            case STAVR + REG3:
            case STAVR + REG4:
            case STAVR + REG5:
            case STAVR + REG6:
            case STAVR + REG7:
                reg = ins & 0x07;
                memory[registers[reg].word] = acc;
                break;

            case INCR + REG0:
            case INCR + REG1:
            case INCR + REG2:
            case INCR + REG3:
            case INCR + REG4:
            case INCR + REG5:
            case INCR + REG6:
            case INCR + REG7:
                reg = ins & 0x07;
                registers[reg].word++;
                break;

            case DECR + REG0:
            case DECR + REG1:
            case DECR + REG2:
            case DECR + REG3:
            case DECR + REG4:
            case DECR + REG5:
            case DECR + REG6:
            case DECR + REG7:
                reg = ins & 0x07;
                registers[reg].word--;
                break;

            case OUTA + PORT0:
            case OUTA + PORT1:
            case OUTA + PORT2:
            case OUTA + PORT3:
            case OUTA + PORT4:
            case OUTA + PORT5:
            case OUTA + PORT6:
            case OUTA + PORT7:
            case OUTA + PORT8:
            case OUTA + PORT9:
            case OUTA + PORT10:
            case OUTA + PORT11:
            case OUTA + PORT12:
            case OUTA + PORT13:
            case OUTA + PORT14:
            case OUTA + PORT15:
                portaddr = ins & 0x0f;
                port[portaddr] = acc;
                if (portaddr == 2) {
                    myputchar(port[portaddr]);
                    DEBUG_PRINTF(" ");
                }
                break;

            case OUTI + PORT0:
            case OUTI + PORT1:
            case OUTI + PORT2:
            case OUTI + PORT3:
            case OUTI + PORT4:
            case OUTI + PORT5:
            case OUTI + PORT6:
            case OUTI + PORT7:
            case OUTI + PORT8:
            case OUTI + PORT9:
            case OUTI + PORT10:
            case OUTI + PORT11:
            case OUTI + PORT12:
            case OUTI + PORT13:
            case OUTI + PORT14:
            case OUTI + PORT15:
                portaddr = ins & 0x0f;
                port[portaddr] = memory_read(register_read_word(PC));
                register_inc(PC);


                if ((port[0] == 0x40) && (portaddr == 1))
                    myputchar(port[portaddr]);
                break;

            case OUTVR + PORT0:
            case OUTVR + PORT1:
            case OUTVR + PORT2:
            case OUTVR + PORT3:
            case OUTVR + PORT4:
            case OUTVR + PORT5:
            case OUTVR + PORT6:
            case OUTVR + PORT7:
            case OUTVR + PORT8:
            case OUTVR + PORT9:
            case OUTVR + PORT10:
            case OUTVR + PORT11:
            case OUTVR + PORT12:
            case OUTVR + PORT13:
            case OUTVR + PORT14:
            case OUTVR + PORT15:
                badOpcode(ins);
                break;

            case INP + PORT0:
            case INP + PORT1:
            case INP + PORT2:
            case INP + PORT3:
            case INP + PORT4:
            case INP + PORT5:
            case INP + PORT6:
            case INP + PORT7:
            case INP + PORT8:
            case INP + PORT9:
            case INP + PORT10:
            case INP + PORT11:
            case INP + PORT12:
            case INP + PORT13:
            case INP + PORT14:
            case INP + PORT15:
                portaddr = ins & 0x0f;
                if ((port[0] == 1) && (firstSwitchRead == 1) && (portaddr == 1)) {
                    firstSwitchRead = 0;
                    acc = 0xff;
                }
                if (portaddr == 2) {
                    acc = mygetchar(); // get a char
                    //printf("in char %x\n", acc);
                }
                break;

                
            case BR:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);

                register_write_hi(PC, hi);
                register_write_lo(PC, lo);
                break;

            case BRZ:

                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc == 0) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case BRNZ:

                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc != 0) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case BRINH:

                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (in == 1) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;

            case BRINL:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (in == 0) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;

            case 0xa5: /* not implemented yet4*/
                badOpcode(ins);

                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (!carry) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;

            case BRC:

                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (carry) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;

            case BRLT:

                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc < treg) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case BREQ:

                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc == treg) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;

            case BRGT:
                //printf("a9 %d %d ",acc,treg);
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc > treg) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;

            case BRNEQ:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc != treg) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;

            case BR16Z:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                badOpcode(ins);
                break;

            case BR16NZ:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);

                badOpcode(ins);
                break;

            case 0xad:
                badOpcode(ins);
                break;

            case 0xae:
                badOpcode(ins);
                break;

            case BRDEV:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                // do not branch
                //register_write_hi(PC, hi);
                //register_write_lo(PC, lo);
                break;

            case ADDI:
                if ((acc + memory_read(register_read_word(PC))) > 255) {
                    carry = true;
                } else {
                    carry = false;
                }
                acc += memory_read(register_read_word(PC));
                register_inc(PC);
                break;

            case SUBI: // borrow flag?
                //printf("s1 %d ",acc);
                acc = acc - memory_read(register_read_word(PC));
                //printf("s2 %d ",acc);
                register_inc(PC);
                break;

            case ORI:
                acc |= memory_read(register_read_word(PC));
                register_inc(PC);
                break;

            case ANDI:
                acc &= memory_read(register_read_word(PC));
                register_inc(PC);
                break;

            case XORI:
                acc ^= memory_read(register_read_word(PC));
                register_inc(PC);
                break;

            case INVA:
                acc = ~acc;
                break;

            case SHL:
                acc = acc << 1;
                break;

            case SHR:
                acc = acc >> 1;
                break;

            case ADDT:
                if ((acc + treg) > 255) {
                    carry = true;
                } else {
                    carry = false;
                }
                acc = acc + treg;
                break;

            case SUBT: //borrow flag?
                acc = acc - treg;
                break;

            case ORT:
                acc = acc | treg;
                break;

            case ANDT:
                acc = acc & treg;
                break;

            case XORT:
                acc = acc ^ treg;
                break;

            case RSHL:
                if (acc & 0x80)
                    tmpCarry = true;
                else
                    tmpCarry = false;

                acc = acc << 1;
                if (tmpCarry)
                    acc = acc | 0x01;
                break;

            case RSHR:
                if (acc & 0x01)
                    tmpCarry = true;
                else
                    tmpCarry = false;

                acc = acc >> 1;
                if (tmpCarry)
                    acc = acc | 0x80;
                break;

            case PSHR:
                if (acc & 0x80)
                    tmpCarry = true;
                else
                    tmpCarry = false;
                acc = acc >> 1;
                if (tmpCarry)
                    acc = acc | 0x80;
                break;

            case LDTVR + REG0: //YIKES
            case LDTVR + REG1:
            case LDTVR + REG2:
            case LDTVR + REG3:
            case LDTVR + REG4:
            case LDTVR + REG5:
            case LDTVR + REG6:
            case LDTVR + REG7:
                badOpcode(ins);
                break;

            case STTVR + REG0:
            case STTVR + REG1:
            case STTVR + REG2:
            case STTVR + REG3:
            case STTVR + REG4:
            case STTVR + REG5:
            case STTVR + REG6:
            case STTVR + REG7:
                badOpcode(ins);
                break;

            case LDIVR + REG0:
            case LDIVR + REG1:
            case LDIVR + REG2:
            case LDIVR + REG3:
            case LDIVR + REG4:
            case LDIVR + REG5:
            case LDIVR + REG6:
            case LDIVR + REG7:
                reg = ins & 0x07;
                memory[registers[reg].word] = memory_read(register_read_word(PC));
                register_inc(PC);
                break;

            case BRVR + REG0:
            case BRVR + REG1:
            case BRVR + REG2:
            case BRVR + REG3:
            case BRVR + REG4:
            case BRVR + REG5:
            case BRVR + REG6:
            case BRVR + REG7:
                reg = ins & 0x07;
                register_write_hi(reg, memory_read(register_read_word(reg)));
                register_inc(reg);
                register_write_lo(reg, memory_read(register_read_word(reg)));
                register_inc(reg);
                break;
            case CSHL:
                if (acc & 0x80)
                    tmpCarry = true;
                else
                    tmpCarry = false;

                acc = acc << 1;
                if (carry)
                    acc = acc | 0x01;
                carry = tmpCarry;
                break;
            case CSHR:
                if (acc & 0x01)
                    tmpCarry = true;
                else
                    tmpCarry = false;

                acc = acc >> 1;
                if (carry)
                    acc = acc | 0x80;
                carry = tmpCarry;
                break;
            case ADDIC:
                if ((acc + carry + memory_read(register_read_word(PC))) > 255) {
                    tmpCarry = true;
                } else {
                    tmpCarry = false;
                }
                acc = acc + memory_read(register_read_word(PC)) + carry;
                if (tmpCarry)
                    carry = true;
                else
                    carry = false;
                register_inc(PC);
                break;
            case ADDTC:
                if ((acc + carry + treg) > 255) {
                    tmpCarry = true;
                } else {
                    tmpCarry = false;
                }
                acc = acc + treg + carry;
                if (tmpCarry)
                    carry = true;
                else
                    carry = false;
                break;
            case LDA:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                acc = memory_read(register_read_word(IR));
                break;
            case STA:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                memory_write(register_read_word(IR), acc);
                break;
            case LDT:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                treg = memory_read(register_read_word(IR));
                break;
            case STT:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                memory_write(register_read_word(IR), treg);
                break;

            case STR + REG0:
            case STR + REG1:
            case STR + REG2:
            case STR + REG3:
            case STR + REG4:
            case STR + REG5:
            case STR + REG6:
            case STR + REG7:
                reg = ins & 0x07;
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                
                memory_write(register_read_word(IR),register_read_hi(reg));
                register_inc(IR);
                memory_write(register_read_word(IR),register_read_lo(reg));
                register_inc(IR);
                /*
                register_write_hi(reg, memory_read(register_read_word(IR)));
                register_inc(IR);
                register_write_lo(reg, memory_read(register_read_word(IR)));
                register_inc(IR);*/
                break;

            case LDR + REG0:
            case LDR + REG1:
            case LDR + REG2:
            case LDR + REG3:
            case LDR + REG4:
            case LDR + REG5:
            case LDR + REG6:
            case LDR + REG7:
                reg = ins & 0x07;
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                register_write_hi(reg, memory_read(register_read_word(IR)));
                register_inc(IR);
                register_write_lo(reg, memory_read(register_read_word(IR)));
                register_inc(IR);
                break;

            case 0xf8:
                badOpcode(ins);
                break;

            case 0xf9:
                badOpcode(ins);
                break;

            case 0xfa:
                badOpcode(ins);
                break;

            case INTE: //hack interrupt enable - do nothing
                break;

            case 0xfc:
                badOpcode(ins);
                break;

            case 0xfd:
                badOpcode(ins);
                break;

            case IADDR: //hack load interrupt address for now just skip
                register_inc(PC);
                register_inc(PC);
                break;

            case 0xff:
                badOpcode(ins);
                break;
        }
    }
    return (EXIT_SUCCESS);
}

