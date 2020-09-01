/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: kennethrother
 *
 * Created on August 19, 2020, 5:03 PM
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "yaccsignal.h"
//#include "code.h"

#define PORTS_PER_CHIP 2
#define PINS_PER_PORT 8

#define BYTES_PER_LINE 8
#define LINES_PER_INSTRUCTION 32
#define INSTRUCTION_SIZE BYTES_PER_LINE * LINES_PER_INSTRUCTION
#define INSTRUCTIONS 256
#define MEMORY_SIZE BYTES_PER_LINE*LINES_PER_INSTRUCTION*INSTRUCTIONS

#define INSTRUCTIONS_TO_DUMP 256   // KEN sould be 255

#define SIGNALS 64

unsigned char cntlMemory[MEMORY_SIZE] = {}; // RAM Image

char signalHistory[LINES_PER_INSTRUCTION][SIGNALS] = {' ',};

char * getSignalName(int instruction) {

    return (signals[instruction].name);
}

void readSource(char *filename) {

    FILE *binaryInstructions;
    int n = 0, count = 0;

    printf("read source\n");
    printf("Open file [%s]\n", filename);

    binaryInstructions = fopen(filename, "rb");
    printf("file opened\n");
    printf("buf size %d\n", MEMORY_SIZE);

    while (!feof(binaryInstructions)) {
        n = fread(cntlMemory, 1, MEMORY_SIZE, binaryInstructions);
        count += n;
        printf("n = %d\n", n);
    }
    printf("%d bytes read from library.\n", count);
}

/* 
 * Print out instruction directly from complete 64k memory array
 * format bytes (8 bytes/line, 32 lines
 */

void printInstruction(int instruction) {
    printf("Print Instruction [%d]\n", instruction);
    for (int line = 0; line < LINES_PER_INSTRUCTION; line++) {
        printf("Line %03d ", line);
        for (int insbyte = 0; insbyte < BYTES_PER_LINE; insbyte++) {
            printf("%02x ", cntlMemory[(instruction * INSTRUCTION_SIZE)+(line * BYTES_PER_LINE) + insbyte]);
        }
        printf("\n");
    }
    printf("\n");
}

void loadInstructionSignals(unsigned char *instruction, bool validLines[]) {
    unsigned char mask;

    for (int line = 0; line < LINES_PER_INSTRUCTION; line++) {
        for (int insbyte = 0; insbyte < BYTES_PER_LINE; insbyte++) {
            for (int i = 0; i < 8; i++) {
                signalHistory[line][(insbyte * BYTES_PER_LINE) + i] = '-';
            }
        }
    }

    for (int line = 0; line < LINES_PER_INSTRUCTION; line++) {
        if (validLines[line] == true) {
            for (int insbyte = 0; insbyte < BYTES_PER_LINE; insbyte++) {
                mask = 0x80;
                for (int bit = 7; bit >= 0; bit--) {
                    //printf("ins [%02x]\n", instruction[(line * BYTES_PER_LINE) + (insbyte)]);
                    if ((mask & (instruction[(line * BYTES_PER_LINE) + (insbyte)])) != 0) {
                        signalHistory[line][(insbyte * BYTES_PER_LINE) + bit] = 'h';
                    } else {
                        signalHistory[line][(insbyte * BYTES_PER_LINE) + bit] = 'l';
                    }
                    mask = mask >> 1;
                }

            }
        }
    }
}

void printInstructionSignals() {
    for (int line = 0; line < LINES_PER_INSTRUCTION; line++) {
        printf("line %02d ", line);
        for (int pos = 0; pos < BYTES_PER_LINE; pos++) {
            for (int i = 0; i < 8; i++) {
                printf("%c", signalHistory[line][(pos * BYTES_PER_LINE) + i]);
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

void printActiveSignals(bool * buffer) {
    for (int i = 0; i < SIGNALS; i++)
        if (buffer[i] == true)
            printf("active signal [%d] [%s]\n", i, getSignalName(i));
    printf("\n");
}

void printSignalsHistory(bool * activeSignals, bool validLine[]) {
    for (int i = 0; i < SIGNALS; i++)
        if (activeSignals[i] == true) {
            printf("active signal [%s] - [", getSignalName(i));
            for (int t = 0; t < LINES_PER_INSTRUCTION; t++) {
                if (validLine[t] == true) {
                    printf("%c", signalHistory[t][i]);
                }
            }
            printf("]\n");
        }
}

printWave(int instruction, bool * activeSignals, bool validLine[]) {
    char previous;

    printf("\n");
    printf("{ \"signal\" : [");
    for (int i = 0; i < SIGNALS; i++)
        if (activeSignals[i] == true) {
            previous = ' ';
            printf("{ \"name\": \"%s\", \"wave\": \"", getSignalName(i));
            for (int t = 0; t < LINES_PER_INSTRUCTION; t++) {
                if (validLine[t] == true) {
                    if (signalHistory[t][i] != previous)
                        printf("%c", signalHistory[t][i]);
                    else
                        printf("%c", '.');
                }
                previous = signalHistory[t][i];
            }
            printf("\"},\n");
        }
    printf("]}\n");
}

bool processInstruction(int instruction) {

    unsigned char workingInstruction[INSTRUCTION_SIZE];
    bool activeSignals[SIGNALS];
    bool validLine[LINES_PER_INSTRUCTION];
    int start;
    int pos;
    bool validInstruction = false;

    printf("\nProcess instruction [%d]\n", instruction);

    for (int i = 0; i < SIGNALS; i++) {
        activeSignals[i] = false;
    }

    /*
     Calculate if this instruction contains signals 
     and if yes which lines of instruction are valid (not all zeros)
     */
    for (int line = 0; line < LINES_PER_INSTRUCTION; line++) {
        validLine[line] = false;
        for (int insbyte = 0; insbyte < BYTES_PER_LINE; insbyte++)
            if (cntlMemory[(instruction * INSTRUCTION_SIZE)+(line * BYTES_PER_LINE) + insbyte] != 0) {
                validLine[line] = true;
                validInstruction = true;
            }
    }

    printInstruction(instruction);

    for (int i = 0; i < INSTRUCTION_SIZE; i++)
        workingInstruction[i] = cntlMemory[(instruction * INSTRUCTION_SIZE) + i];

    loadInstructionSignals(workingInstruction, validLine);

    //printInstructionSignals();




    if (validInstruction) {
        for (int line = 1; line < LINES_PER_INSTRUCTION; line++) {
            if (validLine[line]) {
                //printf("process line %d \n", line);
                for (int sig = 0; sig < SIGNALS; sig++) {
                    if (signalHistory[0][sig] != signalHistory[line][sig]) {
                        activeSignals[sig] = true;
                        //printf("sig %d\n ", sig);
                    }
                }
            }
        }

        printActiveSignals(activeSignals);

        printSignalsHistory(activeSignals, validLine);

        printWave(instruction, activeSignals, validLine);

    } else {
        printf("---\n");
    }

    //if (instruction == 10)
    //    exit(0);

    return (validInstruction);
}

/*
 * 
 */
int main(int argc, char** argv) {
    int i;

    printf("Start Disassembler\n");
    printf("Read in file\n");
    readSource("../Sequencer\ card/ucode-Generator/test.123");

    for (i = 0; i < INSTRUCTIONS; i++) {
        if (processInstruction(i)) {
            //printf("Instruction [%d] found\n", i);
        }
    }
    processInstruction(0xD0);
    processInstruction(0x18);
    return (EXIT_SUCCESS);
}

