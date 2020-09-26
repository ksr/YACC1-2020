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
#include"signal.h"
#include "code.h"




#define PORTS_PER_CHIP 2
#define PINS_PER_PORT 8

#define BYTES_PER_LINE 8
#define LINES_PER_INSTRUCTION 32
#define INSTRUCTION_SIZE BYTES_PER_LINE * LINES_PER_INSTRUCTION
#define INSTRUCTIONS 256
#define MEMORY_SIZE BYTES_PER_LINE*LINES_PER_INSTRUCTION*INSTRUCTIONS

#define INSTRUCTIONS_TO_OUTPUT 10   // KEN sould be 255

#define SIGNALS1 64

unsigned char cntlMemory[MEMORY_SIZE] = {}; // RAM Image

void getSignalName(int instruction) {
    printf("%d\n", signals[instruction].chip);
    
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

void loadLine(int line, bool *signalBuffer, unsigned char *instruction) {
    int start, pos;
    unsigned char mask;

    start = line*BYTES_PER_LINE;
    for (pos = start; pos < start + BYTES_PER_LINE; pos++) {
        mask = 0x01;
        for (int i = 0; i < 8; i++) {
            if ((mask & instruction[pos]) != 0)
                signalBuffer[(pos - start) + i] = true;
            else
                signalBuffer[(pos - start) + i] = false;
            mask = mask << 1;
        }
    }
}

void flagActive(bool *buff1, bool *buff2, bool *buff3) {
    int i;

    for (i = 0; i < SIGNALS1; i++)
        if (buff1[i] != buff2[i])
            buff3[i] = true;
        else
            buff3[i] = false;
}

bool processInstruction(int instruction) {

    unsigned char workingInstruction[INSTRUCTION_SIZE];
    bool startingSignals[SIGNALS1];
    bool currentSignals[SIGNALS1];
    bool activeSignals[SIGNALS1];

    int start;
    int pos;
    bool validInstruction = false;

    printf("Process instruction [%d] - ", instruction);

    for (int i = 0; i < SIGNALS1; i++) {
        activeSignals[i] = false;
        startingSignals[i] = false;
        currentSignals[i] = false;
    }

    start = instruction*INSTRUCTION_SIZE;

    for (int line = 0; line < LINES_PER_INSTRUCTION; line++) {
        for (int insbyte = 0; insbyte < BYTES_PER_LINE; insbyte++) {
            pos = (line * BYTES_PER_LINE) + insbyte;
            workingInstruction[pos] = cntlMemory[start + pos];
        }
    }
    for (pos = 0; pos < INSTRUCTION_SIZE; pos++)
        if (workingInstruction[pos] != 0)
            validInstruction = true;

    if (validInstruction) {
        printf("Valid  instruction\n");

        loadLine(0, startingSignals, workingInstruction);
        for (int line = 1; line < LINES_PER_INSTRUCTION; line++) {
            loadLine(line, currentSignals, workingInstruction);
            flagActive(startingSignals, currentSignals, activeSignals);
        }
        for (int i = 0; i < SIGNALS1; i++)
            if (activeSignals[i] == true)
                printf("active signal [%d]\n", i);
    } else {
        printf("\n");
    }
    return (validInstruction);

}

/*
 * 
 */
int main(int argc, char** argv) {
    int i;

    printf("Start Disassembler\n");
    printf("Read in file\n");
    readSource("test.123");

    for (i = 0; i < INSTRUCTIONS; i++) {
        if (processInstruction(i)) {
            //printf("Instruction [%d] found\n", i);
        }
    }
    return (EXIT_SUCCESS);
}

