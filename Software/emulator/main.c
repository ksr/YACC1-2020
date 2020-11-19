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
#include <termios.h>
#include <unistd.h>

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINTF(...)  printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#define MEMORY_SIZE 0x10000
#define REGISTERS 12
#define PC 0
#define SP 1
#define IR 2

union Register {
    unsigned char byte[2];
    unsigned short word;
} registers[REGISTERS];

unsigned char port[16];

unsigned char memory[MEMORY_SIZE];

unsigned char acc;
unsigned char treg;
int out;
int in;
int jsrdepth;
int pushpopdepth;
int pushpoprdepth;
bool carry;
int firstSwitchRead = 1;


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

/* this writes a part of memory[] to an intel hex file */
void save_file(char *command);

/* this is used by load_file to get each line of intex hex */
int parse_hex_line(char *theline, int bytes[], int *addr, int *num, int *code);

/* this does the dirty work of writing an intel hex file */
/* caution, static buffering is used, so it is necessary */
/* to call it with end=1 when finsihed to flush the buffer */
/* and close the file */
void hexout(FILE *fhex, int byte, int memory_location, int end);

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
    char line[1000];
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
            if (status == 2); /* begin of file */
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

    //printf("dump %d %d\n", start, end);
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
    if ((address == 0x0204) && (value == 0)) {
        printf("argh\n");
        regdump();
    }
    memory[address] = value;
}

unsigned char register_read_lo(int registernum) {
    if (registernum > 7) {
        printf("reg error\n");
        exit(0);
    }
    return (registers[registernum].byte[0]);
}

unsigned char register_read_hi(int registernum) {
    if (registernum > 7) {
        printf("reg error\n");
        exit(0);
    }
    return (registers[registernum].byte[1]);
}

unsigned short register_read_word(int registernum) {
    if (registernum > 7) {
        printf("reg error\n");
        exit(0);
    }
    return (registers[registernum].word);
}

void register_write_lo(int registernum, unsigned char val) {
    if (registernum > 7) {
        printf("reg error\n");
        exit(0);
    }
    registers[registernum].byte[0] = val;
}

void register_write_hi(int registernum, unsigned char val) {
    if (registernum > 7) {
        printf("reg error\n");
        exit(0);
    }
    registers[registernum].byte[1] = val;
}

void register_write_word(int registernum, unsigned short val) {
    if (registernum > 7) {
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
#define RUN 0
#define HALT 1
#define SINGLESTEP 2
#define JUMPOVER 3
int stepmode = 0;
int jsrdepth = 0;
int jsrover = 0;

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

int mygetchar() {
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
        if (c == 0x0d)
            c = 0x0a;
        return (c);
    }

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

    //while (1) {
    //    myputchar(mygetchar());
    //    printf(" cc=%d",cc++);
    // }

    in = 0;
    registers[PC].word = 0xf000;
    int singleStep = 0;
    int breakAddress = 0;
    pushpopdepth = 0;
    pushpoprdepth = 0;

    int regtrigger = 0;

    load_file("../Assembler/basic.img");
    load_file("../Assembler/monitor.img");
    dump(0xf000, 0xf0ff);

    while (1) {

        ins = memory_read(register_read_word(PC));
        if ((registers[7].word == 0x1002) && (registers[PC].word == 0xeaff))
            debug_mode(SINGLESTEP);
        if (stepmode != 0) {
            if (stepmode == SINGLESTEP) {
                for (int t = 3; t < jsrdepth; t++)
                    printf("->");
                printf("singlestep [%02x] at [%04x] acc=[%02x] R3=[%04x] R7=[%04x]\n", ins, registers[PC].word, acc, register_read_word(3), register_read_word(7));
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
            case 0x01:
                out_on();
                break;
            case 0x02:
                out_off();
                break;
            case 0x03:

                printf("\nHalt[%02x] at [%04x] acc=[%02x] R3=[%04x] R7=[%04x]\n", ins, registers[PC].word - 1, acc, register_read_word(3), register_read_word(7));
                dump(0x0200, 0x021f);
                dump(0x0f80, 0x0f8f);
                dump(0x0400, 0x040F);
                dump(0x1000, 0x1020);
                debug_mode(HALT);
                break;
            case 0x04:
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
            case 0x05:
                jsrdepth--;
                DEBUG_PRINTF("RET pc=%04x sp=%04x jsrd=%d ", (register_read_word(PC) - 1), register_read_word(SP), jsrdepth);
                //dump(0x0200, 0x020f);
                register_inc(SP);
                register_write_lo(PC, memory_read(register_read_word(SP)));
                register_inc(SP);
                register_write_hi(PC, memory_read(register_read_word(SP)));


                DEBUG_PRINTF(" return to %02x%02x\n", register_read_hi(PC), register_read_lo(PC));

                break;
            case 0x06:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0x07:
                reg = memory_read(register_read_word(PC)) & 0x07;
                register_inc(PC);
                //DEBUG_PRINTF("PUSHR reg=%d pc=%04x pre-sp=%04x\n", reg, (register_read_word(PC) - 1), register_read_word(SP));
                memory_write(register_read_word(SP), register_read_hi(reg));
                register_dec(SP);
                memory_write(register_read_word(SP), register_read_lo(reg));
                register_dec(SP);

                break;
            case 0x08:
                reg = (memory_read(register_read_word(PC)) >> 4) & 0x07;
                register_inc(PC);
                //DEBUG_PRINTF("POPR reg=%d pc=%04x pre-sp=%04x\n", reg, (register_read_word(PC) - 1), register_read_word(SP));
                register_inc(SP);
                register_write_lo(reg, memory_read(register_read_word(SP)));
                register_inc(SP);
                register_write_hi(reg, memory_read(register_read_word(SP)));
                break;
            case 0x09:
                pushpopdepth++;
                //DEBUG_PRINTF("PUSH pc=%04x pre-sp=%04x ppd=%d\n", (register_read_word(PC) - 1), register_read_word(SP), pushpopdepth);
                memory_write(register_read_word(SP), acc_read());
                register_dec(SP);
                break;
            case 0x0a:
                pushpopdepth--;
                //DEBUG_PRINTF("POP pc=%04x pre-sp=%04x ppd=%d\n", (register_read_word(PC) - 1), register_read_word(SP), pushpopdepth);
                register_inc(SP);
                acc_write(memory_read(register_read_word(SP)));
                break;
            case 0x0b:
                treg = acc;
                break;
            case 0x0c:
                acc = treg;
                break;
            case 0x0d:
                treg = memory_read(register_read_word(PC));
                register_inc(PC);
                break;
            case 0x0e:
                acc = memory_read(register_read_word(PC));
                register_inc(PC);
                break;
            case 0x0f:
                reg = memory_read(register_read_word(PC));
                register_inc(PC);
                src = reg & 0x0f;
                dest = (reg >> 4) & 0x0f;
                register_write_word(dest, register_read_word(src));
                break;
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
                reg = ins & 0x07;
                register_write_lo(reg, memory_read(register_read_word(PC)));
                register_inc(PC);
                break;
            case 0x18:
            case 0x19:
            case 0x1a:
            case 0x1b:
            case 0x1c:
            case 0x1d:
            case 0x1e:
            case 0x1f:
                reg = ins & 0x07;
                register_write_hi(reg, memory_read(register_read_word(PC)));
                register_inc(PC);
                register_write_lo(reg, memory_read(register_read_word(PC)));
                register_inc(PC);
                break;
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
                reg = ins & 0x07;
                acc_write(register_read_lo(reg));
                break;
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
                reg = ins & 0x07;
                acc_write(register_read_hi(reg));
                break;
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
                reg = ins & 0x07;
                register_write_lo(reg, acc_read());
                break;
            case 0x38:
            case 0x39:
            case 0x3a:
            case 0x3b:
            case 0x3c:
            case 0x3d:
            case 0x3e:
            case 0x3f:
                reg = ins & 0x07;
                register_write_hi(reg, acc_read());
                break;
            case 0x40:
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
                reg = ins & 0x07;
                acc = memory[registers[reg].word];
            case 0x48:
            case 0x49:
            case 0x4a:
            case 0x4b:
            case 0x4c:
            case 0x4d:
            case 0x4e:
            case 0x4f:
                reg = ins & 0x07;
                memory[registers[reg].word] = acc;
                break;
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
                reg = ins & 0x07;
                registers[reg].word++;
                break;
            case 0x58:
            case 0x59:
            case 0x5a:
            case 0x5b:
            case 0x5c:
            case 0x5d:
            case 0x5e:
            case 0x5f:
                reg = ins & 0x07;
                registers[reg].word--;
                break;
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6a:
            case 0x6b:
            case 0x6c:
            case 0x6d:
            case 0x6e:
            case 0x6f:
                portaddr = ins & 0x0f;
                port[portaddr] = acc;
                if (portaddr == 2) {
                    myputchar(port[portaddr]);
                    DEBUG_PRINTF(" ");
                }
                break;
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7a:
            case 0x7b:
            case 0x7c:
            case 0x7d:
            case 0x7e:
            case 0x7f:
                portaddr = ins & 0x0f;
                port[portaddr] = memory_read(register_read_word(PC));
                register_inc(PC);


                if ((port[0] == 0x40) && (portaddr == 1))
                    myputchar(port[portaddr]);
                break;
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8a:
            case 0x8b:
            case 0x8c:
            case 0x8d:
            case 0x8e:
            case 0x8f:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0x90:
            case 0x91:
            case 0x92:
            case 0x93:
            case 0x94:
            case 0x95:
            case 0x96:
            case 0x97:
            case 0x98:
            case 0x99:
            case 0x9a:
            case 0x9b:
            case 0x9c:
            case 0x9d:
            case 0x9e:
            case 0x9f:
                portaddr = ins & 0x0f;
                if ((port[0] == 1) && (firstSwitchRead == 1) && (portaddr == 1)) {
                    firstSwitchRead = 0;
                    acc = 0xff;
                }
                if (portaddr == 2) {
                    acc = mygetchar(); // get a char
                }
                break;
            case 0xa0:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);

                register_write_hi(PC, hi);
                register_write_lo(PC, lo);
                break;
            case 0xa1:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc == 0) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xa2:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc != 0) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xa3:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (in == 1) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xa4:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (in == 0) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xa5:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (!carry) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xa6:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (carry) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xa7:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc < treg) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xa8:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc == treg) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xa9:
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
            case 0xaa:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                if (acc != treg) {
                    register_write_hi(PC, hi);
                    register_write_lo(PC, lo);
                }
                break;
            case 0xab:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                exit(0);
                break;
            case 0xac:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                exit(0);
                break;
            case 0xad:

                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                exit(0);
                break;
            case 0xae:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                exit(0);
                break;
            case 0xaf:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                // do not branch
                //register_write_hi(PC, hi);
                //register_write_lo(PC, lo);
                break;
            case 0xb0:
                if ((acc + memory_read(register_read_word(PC))) > 255) {
                    carry = true;
                } else {
                    carry = false;
                }
                acc += memory_read(register_read_word(PC));
                register_inc(PC);
                break;
            case 0xb1:
                //printf("s1 %d ",acc);
                acc = acc - memory_read(register_read_word(PC));
                //printf("s2 %d ",acc);
                register_inc(PC);
                break;
            case 0xb2:
                acc |= memory_read(register_read_word(PC));
                register_inc(PC);
                break;
            case 0xb3:
                acc &= memory_read(register_read_word(PC));
                register_inc(PC);
                break;
            case 0xb4:
                acc ^= memory_read(register_read_word(PC));
                register_inc(PC);
                break;
            case 0xb5:
                acc = ~acc;
                break;
            case 0xb6:
                acc = acc << 1;

                break;
            case 0xb7:
                acc = acc >> 1;

                break;
            case 0xb8:
                if ((acc + treg) > 255) {
                    carry = true;
                } else {
                    carry = false;
                }
                acc = acc + treg;
                break;
            case 0xb9:
                acc = acc - treg;
                break;
            case 0xba:
                acc = acc | treg;
                break;
            case 0xbb:
                acc = acc & treg;
                break;
            case 0xbc:
                acc = acc ^ treg;
                break;
            case 0xbd:
                if (acc & 0x80)
                    tmpCarry = true;
                else
                    tmpCarry = false;

                acc = acc << 1;
                if (tmpCarry)
                    acc = acc | 0x01;
                break;
            case 0xbe:
                if (acc & 0x01)
                    tmpCarry = true;
                else
                    tmpCarry = false;

                acc = acc >> 1;
                if (tmpCarry)
                    acc = acc | 0x80;
                break;
            case 0xbf:
                if (acc & 0x80)
                    tmpCarry = true;
                else
                    tmpCarry = false;
                acc = acc >> 1;
                if (tmpCarry)
                    acc = acc | 0x80;
                break;

            case 0xc0:
            case 0xc1:
            case 0xc2:
            case 0xc3:
            case 0xc4:
            case 0xc5:
            case 0xc6:
            case 0xc7:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                exit(0);
                break;
            case 0xc8:
            case 0xc9:
            case 0xca:
            case 0xcb:
            case 0xcc:
            case 0xcd:
            case 0xce:
            case 0xcf:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                exit(0);
                break;

            case 0xd0:
            case 0xd1:
            case 0xd2:
            case 0xd3:
            case 0xd4:
            case 0xd5:
            case 0xd6:
            case 0xd7:
                reg = ins & 0x07;
                memory[registers[reg].word] = memory_read(register_read_word(PC));
                register_inc(PC);
                break;
            case 0xd8:
            case 0xd9:
            case 0xda:
            case 0xdb:
            case 0xdc:
            case 0xdd:
            case 0xde:
            case 0xdf:
                reg = ins & 0x07;
                register_write_hi(reg, memory_read(register_read_word(reg)));
                register_inc(reg);
                register_write_lo(reg, memory_read(register_read_word(reg)));
                register_inc(reg);
                break;
            case 0xe0:
                if (acc & 0x80)
                    tmpCarry = true;
                else
                    tmpCarry = false;

                acc = acc << 1;
                if (carry)
                    acc = acc | 0x01;
                carry = tmpCarry;
                break;
            case 0xe1:
                if (acc & 0x01)
                    tmpCarry = true;
                else
                    tmpCarry = false;

                acc = acc >> 1;
                if (carry)
                    acc = acc | 0x80;
                carry = tmpCarry;
                break;
            case 0xe2:
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
            case 0xe3:
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
            case 0xe4:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0xe5:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0xe6:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0xe7:
                acc = 0xff;
                break;
            case 0xe8:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0xe9:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0xea:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0xeb:
                printf("bad opcode [%02x] pc[%04x]\n", ins, register_read_word(PC));
                break;
            case 0xec:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                acc = memory_read(register_read_word(IR));
                break;
            case 0xed:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                memory_write(register_read_word(IR), acc);
                break;
            case 0xee:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                treg = memory_read(register_read_word(IR));
                break;
            case 0xef:
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                memory_write(register_read_word(IR), treg);
                break;
            case 0xf0:
            case 0xf1:
            case 0xf2:
            case 0xf3:
            case 0xf4:
            case 0xf5:
            case 0xf6:
            case 0xf7:
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
            case 0xf9:
            case 0xfa:
            case 0xfb:
            case 0xfc:
            case 0xfd:
            case 0xfe:
            case 0xff:
                reg = ins & 0x07;
                hi = memory_read(register_read_word(PC));
                register_inc(PC);
                lo = memory_read(register_read_word(PC));
                register_inc(PC);
                register_write_lo(IR, lo);
                register_write_hi(IR, hi);
                memory_write(register_read_word(IR), register_read_hi(reg));
                register_inc(IR);
                memory_write(register_read_word(IR), register_read_lo(reg));
                register_inc(IR);
                break;
        }
    }
    return (EXIT_SUCCESS);
}

