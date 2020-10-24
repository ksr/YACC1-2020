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

#define MEMORY_SIZE 0x10000
#define REGISTERS 8
#define PC 0
#define SP 1

union Register {
    unsigned char byte[2];
    unsigned short word;
} registers[REGISTERS];

unsigned char port[16];

unsigned char memory[MEMORY_SIZE];

unsigned char acc;
char treg;
int pc;
int sp;
int out;
int in;
int carry;


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

void dump(int start, int end) {
    int i;

    printf("dump %d %d\n", start, end);
    for (i = start; i < end; i++) {
        if ((i & 0x0f) == 0)
            printf("\n %04x ", i);
        printf("%02x ", memory[i]);
    }
    printf("\n\n");
}

/*
 * 
 */


int main(int argc, char** argv) {
    int i, ins;
    int reg;
    int portaddr;
    unsigned char hi, lo;

    sp = 0;
    pc = 0xf000;
    in = 1;
    registers[PC].word = pc;

  
    load_file("../Assembler/yacc1test.img");
    dump(0xf000, 0xf0ff);

    while (1) {
        ins = memory[registers[PC].word];
        //printf("Process opcode [%02x] at [%04x]\n", ins, registers[PC].word);
        registers[PC].word++;
        switch (ins) {
            case 0x00:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0x01:
                out = 1;
                break;
            case 0x02:
                out = 0;
                break;
            case 0x03:
                printf("halt opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0x04:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                memory[registers[SP].word] = registers[PC].byte[1];
                registers[SP].word--;
                memory[registers[SP].word] = registers[PC].byte[0];
                registers[SP].word--;

                registers[PC].byte[1] = hi;
                registers[PC].byte[0] = lo;
                //printf("jsr opcode pc[%04x]\n", registers[PC].word);
                break;
            case 0x05:
                registers[SP].word++;
                registers[PC].byte[0] = memory[registers[SP].word];
                registers[SP].word++;
                registers[PC].byte[1] = memory[registers[SP].word];
                break;
            case 0x06:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0x07:
                reg = memory[registers[PC].word] & 0x07;
                memory[registers[PC].word]++;

                memory[registers[SP].word] = registers[reg].byte[1];
                registers[SP].word--;
                memory[registers[SP].word] = registers[reg].byte[0];
                registers[SP].word--;
                break;
            case 0x08:
                reg = memory[registers[PC].word] & 0x07;
                registers[PC].word++;
                registers[SP].word++;
                registers[reg].byte[0] = memory[registers[SP].word];
                registers[SP].word++;
                registers[reg].byte[1] = memory[registers[SP].word];
                break;
            case 0x09:
                memory[registers[SP].word] = acc;
                registers[SP].word--;
                break;
            case 0x0a:
                registers[SP].word++;
                acc = memory[registers[SP].word];
                break;
            case 0x0b:
                treg = acc;
                break;
            case 0x0c:
                acc = treg;
                break;
            case 0x0d:
                treg = memory[registers[PC].word];
                registers[PC].word++;
                break;
            case 0x0e:
                acc = memory[registers[PC].word];
                registers[PC].word++;
                break;
            case 0x0f:
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
                registers[reg].byte[0] = memory[registers[PC].word];
                registers[PC].word++;
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
                registers[reg].byte[1] = memory[registers[PC].word];
                registers[PC].word++;
                registers[reg].byte[0] = memory[registers[PC].word];
                registers[PC].word++;
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
                acc = registers[reg].byte[0];
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
                acc = registers[reg].byte[1];
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
                registers[reg].byte[0] = acc;
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
                registers[reg].byte[1] = acc;
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
                if (portaddr == 2)
                    putchar(port[portaddr]);

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
                port[portaddr] = memory[registers[PC].word];
                registers[PC].word++;


                if ((port[0] == 0x40) && (portaddr == 1))
                    putchar(port[portaddr]);
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
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
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
                if (portaddr == 2){
                    printf("input\n");
                    acc = getchar(); // get a char
                }
                break;
            case 0xa0:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;

                registers[PC].byte[1] = hi;
                registers[PC].byte[0] = lo;
                break;
            case 0xa1:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (acc == 0) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xa2:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (acc != 0) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xa3:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (in == 1) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xa4:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (in == 0) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xa5:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (carry == 0) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xa6:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (carry == 1) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xa7:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (acc < treg) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xa8:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (acc == treg) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xa9:
                //printf("a9 %d %d ",acc,treg);
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (acc > treg) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xaa:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                if (acc != treg) {
                    registers[PC].byte[1] = hi;
                    registers[PC].byte[0] = lo;
                }
                break;
            case 0xab:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0xac:
                hi = memory[registers[PC].word];
                registers[PC].word++;
                lo = memory[registers[PC].word];
                registers[PC].word++;
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0xad:

                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0xae:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0xaf:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0xb0:
                acc += memory[registers[PC].word];
                registers[PC].word++;
                break;
            case 0xb1:
                //printf("s1 %d ",acc);
                acc = acc - memory[registers[PC].word];
                //printf("s2 %d ",acc);
                registers[PC].word++;
                break;
            case 0xb2:
                acc |= memory[registers[PC].word];
                registers[PC].word++;
                break;
            case 0xb3:
                acc &= memory[registers[PC].word];
                registers[PC].word++;
                break;
            case 0xb4:
                acc ^= memory[registers[PC].word];
                registers[PC].word++;
                break;
            case 0xb5:
                acc = ~memory[registers[PC].word];
                break;
            case 0xb6:
                acc = acc << 1;

                break;
            case 0xb7:
                acc = acc >> 1;

                break;
            case 0xb8:
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
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0xbe:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;
            case 0xbf:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                exit(0);
                break;

            case 0xc0:
            case 0xc1:
            case 0xc2:
            case 0xc3:
            case 0xc4:
            case 0xc5:
            case 0xc6:
            case 0xc7:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
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
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
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
                memory[registers[reg].word] = memory[registers[PC].word];
                registers[PC].word++;
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
                registers[PC].byte[1] = memory[registers[reg].word];
                registers[reg].word++;
                registers[PC].byte[0] = memory[(registers[reg].word) + 1];
                registers[reg].word++;
                break;
            case 0xe0:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe1:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe2:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe3:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe4:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe5:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe6:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe7:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe8:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xe9:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xea:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xeb:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xec:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xed:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xee:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xef:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf0:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf1:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf2:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf3:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf4:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf5:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf6:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf7:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf8:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xf9:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xfa:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xfb:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xfc:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xfd:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xfe:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
            case 0xff:
                printf("bad opcode [%02x] pc[%04x]\n", ins, pc);
                break;
        }

    }

    for (int i = 0; i < 256; i++) {
        printf("case %s%02x:\nbreak;\n", "0x", i);
    }

    return (EXIT_SUCCESS);
}

