/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "code.h"
#include "CodeGen.h"

#define START_CHAR '%'
#define END_INS_CHAR '-'
#define END_ALL '!'

#define DEBUG 1

struct signal signals[];

unsigned char cntlMemory[MEMORY_SIZE] = {}; // RAM Image
unsigned char currentLine[BYTES_PER_LINE] = {}; // 

int eolInfo[INSTRUCTIONS] = {0,};
int ucodeLine = 0;
int currentUcodeBlock = -1;

int showDetail = 1;

void writeAsciiByte(FILE *f, unsigned char c);

void clearCntlMemory() {
    for (int i = 0; i < MEMORY_SIZE; i++)
        cntlMemory[i] = 0;
}

void startUcodeBlock(int index) {
    currentUcodeBlock = index;
    ucodeLine = 0;
}

void endUcodeBlock() {
    printf("Instruction=%02x (hex) lines=%d (dec)\n", currentUcodeBlock, ucodeLine);
    eolInfo[currentUcodeBlock] = ucodeLine;
}

void clearCurrentLine() {
    for (int i = 0; i < BYTES_PER_LINE; i++)
        currentLine[i] = 0;
}

void writeCurrentLine() {
    int lineToWrite;

    lineToWrite = currentUcodeBlock * LINES_PER_INSTRUCTION * BYTES_PER_LINE + ucodeLine*BYTES_PER_LINE;
    for (int i = 0; i < BYTES_PER_LINE; i++)
        cntlMemory[lineToWrite + i] = currentLine[i];
    ucodeLine++;
    if (ucodeLine >= LINES_PER_INSTRUCTION) {
        printf("ucode lines overflow %02x",currentUcodeBlock);
        exit(1);
    }
#ifdef DEBUG
    printf("Write line [%d]\n",ucodeLine-1);
#endif 
}

void showCurrentLine() {
    printf("[%d]", ucodeLine);
    for (int i = 0; i < BYTES_PER_LINE; i++)
        printf("%02x ", currentLine[i]);
    printf("\n");
    //printf("BYTE[%i]=%x\n", i, currentLine[i]);
}

void bitOn(int signalNum) {

    int byteToChange;
    int bitToChange;
    unsigned char newByte;

    byteToChange = (signals[signalNum].chip - 1) * PORTS_PER_CHIP + signals[signalNum].port;
    bitToChange = signals[signalNum].bit;

    newByte = 0x01 << bitToChange;

    currentLine[byteToChange] = currentLine[byteToChange] | newByte;
#ifdef DEBUG
    printf("bitOn-[%d - %s] %d %d %x %x \n", signalNum, signals[signalNum].name, byteToChange, bitToChange, newByte, currentLine[byteToChange]);
#endif
}

void bitOff(int signalNum) {

    int byteToChange;
    int bitToChange;
    unsigned char newByte;

    byteToChange = (signals[signalNum].chip - 1) * PORTS_PER_CHIP + signals[signalNum].port;
    bitToChange = signals[signalNum].bit;

    newByte = 0x01 << bitToChange;
    newByte = 0xff ^ newByte;
    currentLine[byteToChange] = currentLine[byteToChange] & newByte;
#ifdef DEBUG
    printf("bitOff-[%d - %s] %d %d %x %x \n", signalNum, signals[signalNum].name, byteToChange, bitToChange, newByte, currentLine[byteToChange]);
#endif
}

void dumpCntlMemory() {
    unsigned char checksum;
    int i, j, k;
    FILE *dumper;
    dumper = fopen("test.123", "wb");
    //for(int i=0;i<MEMORY_SIZE;i++)
    fwrite(cntlMemory, 1, MEMORY_SIZE, dumper);
    fclose(dumper);

    FILE *dumper2;
    dumper2 = fopen("test.hex", "w");
    for (i = 0; i < INSTRUCTIONS_TO_OUTPUT; i++) {
        checksum = 0;
        for (j = 0; j < INSTRUCTION_SIZE; j++) {
            checksum += cntlMemory[i * INSTRUCTION_SIZE + j];
        }
        fputc(START_CHAR, dumper2);
        writeAsciiByte(dumper2, checksum);
        writeAsciiByte(dumper2, i);

        for (j = 0; j < INSTRUCTION_SIZE; j++) {
            writeAsciiByte(dumper2, cntlMemory[i * INSTRUCTION_SIZE + j]);

        }
        printf("\n");
        fputc(END_INS_CHAR, dumper2);
        fputc('\n', dumper2);
    }
    fputc(END_ALL, dumper2);
    fputc('\n', dumper2);
    //flush(dumper2)
    fclose(dumper2);

    dumper2 = fopen("test.hexz", "w");
    for (i = 0; i < INSTRUCTIONS_TO_OUTPUT; i++) {
        checksum = 0;
        for (j = 0; j < INSTRUCTION_SIZE; j++) {
            checksum += cntlMemory[i * INSTRUCTION_SIZE + j];
        }
        fputc(START_CHAR, dumper2);
        writeAsciiByte(dumper2, checksum);
        writeAsciiByte(dumper2, i);

        for (j = 0; j < eolInfo[i]; j++) {
            for (k = 0; k < BYTES_PER_LINE; k++) {
                writeAsciiByte(dumper2, cntlMemory[i * INSTRUCTION_SIZE + j * BYTES_PER_LINE + k]);
            }
        }
        if (j < LINES_PER_INSTRUCTION) {
            fputc('Z', dumper2);
        }
        printf("\n");
        fputc(END_INS_CHAR, dumper2);
        fputc('\n', dumper2);
    }
    fputc(END_ALL, dumper2);
    fputc('\n', dumper2);
    //flush(dumper2)
    fclose(dumper2);
}

void showCntlMemory(int instruction) {
    int lineStart;

    if (showDetail) {
        printf("Show Instruction=%02x\n", instruction);
        lineStart = BYTES_PER_LINE * LINES_PER_INSTRUCTION * instruction;
        for (int j = 0; j < LINES_PER_INSTRUCTION; j++) {
            printf("Line[%d] = ", j);
            for (int i = 0; i < BYTES_PER_LINE; i++) {
                printf("%02x ", cntlMemory[lineStart + j * BYTES_PER_LINE + i]);
            }
            printf("\n");
        }
    }
}

unsigned char outAsciiHex(unsigned char c) {
    if ((c >= 0) && (c <= 9))
        return ('0' + c);
    else
        return ('A' + (c - 10));
}

void writeAsciiByte(FILE *f, unsigned char c) {
    unsigned char hi, lo;
    char outData[1];

    hi = (c & 0xf0) >> 4;
    lo = c & 0x0f;

    fputc(outAsciiHex(hi), f);
    fputc(outAsciiHex(lo), f);
}