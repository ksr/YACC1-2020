/* 
 * File:   main.c
 * Author: ken
 *
 * Created on October 20, 2016, 10:43 PM
 * 
 *Build sequencer microcode
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yaccsignal.h"
#include "code.h"
#include "../../opcodes.h"
#include "CodeGen.h"

void branchInstructions();
void registerOnlyInstructions();
void ioInstructions();
void accumulatorInstructions();
void doMemory();


//#define DEBUG 1

/*
 * Format "%AIXXXXXXXX....XXXXX-"
   "%"  Start character
   "A" Checksum (not including instruction number) (Ignore for now) - 2 hex assci chars
   "I" Instruction Number (0-255)- 2 hex ascii chars
   "XXXXXXXX....XXXXX" Data Bytes -  2 hex ascii chars
    "-" End Instruction data character
    "!" END ALL
 * 
 */

/* 
 * Search for signal in signals array and return index
 */
int findSignal(char *signal) {

    for (int i = 0; strlen(signals[i].name) != 0; i++)
        if (strcmp(signal, signals[i].name) == 0)
            return (i);
    return (-1);
}

/* 
 * set signal to active, if signal name starts with '-', meaning active low
 * signal is set low, otherwise set high
 */
void setSignal(char *signal) {

    int signalNum;

    if ((signalNum = findSignal(signal)) == -1) {
        printf("Error %s not found\n", signal);
        exit(1);
    }

    if (signals[signalNum].name[0] == '-')
        bitOff(signalNum);
    else
        bitOn(signalNum);
}

/* 
 * set signal to inactive, if signal name starts with '-', meaning active low
 * signal is set high, otherwise set low
 */
void clearSignal(char *signal) {

    int signalNum;

    if ((signalNum = findSignal(signal)) == -1) {
        printf("Error %s not found\n", signal);
        exit(1);
    }

    if (signals[signalNum].name[0] == '-')
        bitOn(signalNum);
    else
        bitOff(signalNum);
}

/* 
 * loop through all signal lines and set to default state
 */
void initCurrentLine() {
    clearCurrentLine();
    for (int i = 0; strlen(signals[i].name) != 0; i++) {

#ifdef DEBUG
        printf("initCurrent line [%d] [%s] \n", i, signals[i].name);
#endif
        clearSignal(signals[i].name);
        setAddrId(PC);
        setSignal("-VMA"); // Hack prevent ROM mapping from triggering
    }
}

/*
 * Called at the start of processing a new instruction
 */
void startInstruction(int instruction) {
    startUcodeBlock(instruction);
    //clearCurrentLine(); should not be required
    initCurrentLine();
    setAddrId(PC);
    setSignal("-VMA"); // Hack prevent ROM mapping from triggering
    if (instruction == 0) {
        setSignal("OUT-OFF");
    }
    writeCurrentLine(); // KEN maybe only need these 2 extra Lines 0&1 for Instruction 0 so maybe test
    writeCurrentLine();
}

/* 
 * Called at the completion of processing an instruction
 */
void endInstruction() {
    setSignal("UCODE-COUNT-RESET");
    writeCurrentLine();
    endUcodeBlock();
    //clearCurrentLine(); // Probably not needed since current line is inited in startInstruction
}

/* 
 * Utility routines for setting bits
 */

void setRegBit(char *signal, int regBit) {
    if (regBit)
        setSignal(signal);
    else
        clearSignal(signal);
}

void setAddrId(int reg) {
    setRegBit("ADDR-REG-ID0", reg & 0x0001);
    setRegBit("ADDR-REG-ID1", reg & 0x0002);
    setRegBit("ADDR-REG-ID2", reg & 0x0004);
    setRegBit("ADDR-REG-ID3", reg & 0x0008);
}

void setRdId(int reg) {
    setRegBit("REG-RD-ID0", reg & 0x0001);
    setRegBit("REG-RD-ID1", reg & 0x0002);
    setRegBit("REG-RD-ID2", reg & 0x0004);
    setRegBit("REG-RD-ID3", reg & 0x0008);
}

void setLdId(int reg) {
    setRegBit("REG-LD-ID0", reg & 0x0001);
    setRegBit("REG-LD-ID1", reg & 0x0002);
    setRegBit("REG-LD-ID2", reg & 0x0004);
    setRegBit("REG-LD-ID3", reg & 0x0008);
}

void setAlu(int aluBits) {
    //printf("setALU %d\n", aluBits);
    setRegBit("ALU0", aluBits & 0x0001);
    setRegBit("ALU1", aluBits & 0x0002);
    setRegBit("ALU2", aluBits & 0x0004);
    setRegBit("ALU3", aluBits & 0x0008);
}

void setIo(int ioBits) {
    //printf("setIO %d\n", ioBits);
    setRegBit("IOADDR0", ioBits & 0x0001);
    setRegBit("IOADDR1", ioBits & 0x0002);
    setRegBit("IOADDR2", ioBits & 0x0004);
    setRegBit("IOADDR3", ioBits & 0x0008);
}

/* 
 * Code Generation Utility Routines
 */

void loadNextInstruction() {
    //Load Reg 0 in ADDR Reg 0
    //Put data from MEM at (ADDR REG 0) on bus
    //load instruction register
    //increment Reg 0

    //initCurrentLine(); // not needed, load next instruction is always done at start of instruction and initCurrentLIne is called
    putMemAtRegOnBus(0);
    setSignal("LD-INS-REG"); // rising or falling?
    writeCurrentLine();
    clearSignal("LD-INS-REG");
    //writeCurrentLine(); // -reg-up is rising edge
    incrementReg(PC);
    
}

void incrementReg(int reg) {
    setRdId(reg);
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");
    writeCurrentLine();
}

void decrementReg(int reg) {
    setRdId(reg);
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-DN");
    writeCurrentLine();
    clearSignal("-REG-DN");
    writeCurrentLine();
}

void putMemAtRegOnBus(int reg) {
    setAddrId(reg);
    setSignal("-VMA");
    setSignal("-MEM-RD");
    writeCurrentLine(); // not needed if if reg loads on rising edge

}

void putBustoRegMem(int reg, char *source) { // Source is either Accumulator TMP registers, if index reg needs more work
    setAddrId(reg);
    setSignal("-VMA");
    setSignal(source);
    writeCurrentLine();
    setSignal("-MEM-WR");
    writeCurrentLine();
    clearSignal("-MEM-WR");
    writeCurrentLine();
    clearSignal(source);
    clearSignal("-VMA");
    writeCurrentLine();

}

int main(int argc, char** argv) {
    int reg;
    int ins;

    //basicTest(); //basic sequencer test led on, led off, reset counter
    //exit(0);

    clearCntlMemory();
    //clearCurrentLine();

    startInstruction(0); // what is this for??? microcode to fetch 1st instructon?
    loadNextInstruction();
    endInstruction(); // this is a test (seems to work)
    showCntlMemory(0);

    // HALT
    startInstruction(HALT);
    loadNextInstruction();
    initCurrentLine();
    setSignal("SOFT-HALT");
    writeCurrentLine();
    endInstruction();
    showCntlMemory(HALT);

    branchInstructions();

    registerOnlyInstructions();

    ioInstructions();

    accumulatorInstructions();

    doMemory();



#ifdef NOTYET

    // Code missing 



    // Add to Accumulator Immediate
    startInstruction(ADD_ACC_I);
    loadNextInstruction();
    initCurrentLine();
    setSignal("ALU-FUNC");
    setSignal("ALU0");
    setSignal("ALU1");
    setSignal("ALU2");
    //setSignal("ALU3");
    setSignal("ADDR-REG-FUNC-RD"); //sp/pc sel on old card try without
    setSignal("ADDR-REG-ADDR-RD"); //put PC address on the address bus
    setSignal("-MEM-RD"); //output memory
    writeCurrentLine();
    setSignal("AC-LD"); //latch data on data bus, from memory, into accumulator
    writeCurrentLine();
    clearSignal("AC-LD");
    setSignal("ADDR-REG-UP"); // advance PC to next location
    writeCurrentLine();
    clearSignal("ADDR-REG-UP");
    writeCurrentLine();

    endInstruction();
    showCntlMemory(ADD_ACC_I);



    // Load SP immediate
    startInstruction(LD_SP_I);
    loadNextInstruction();
    initCurrentLine();

    setSignal("ADDR-ID0"); // bypass branch logic for non PC operations
    // fix for new sp/pc card with address out latch mechanism
    writeCurrentLine();
    //load the next two PC bytes format HHLL SP register
    setSignal("ADDR-REG-FUNC-RD"); //sp/pc sel on old card try without ??? should be off
    setSignal("ADDR-REG-ADDR-RD");
    setSignal("-MEM-RD");
    writeCurrentLine();
    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI");
    ;
    setSignal("ADDR-REG-UP");
    writeCurrentLine();
    clearSignal("ADDR-REG-UP");
    writeCurrentLine();
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO");
    setSignal("ADDR-REG-UP");
    writeCurrentLine();
    clearSignal("ADDR-REG-UP");
    clearSignal("-MEM-RD");
    writeCurrentLine();
    setSignal("BRANCH-RD-LO"); //output branch register
    setSignal("BRANCH-RD-HI");
    writeCurrentLine();
    setSignal("ADDR-REG-LD-LO"); //load branch register
    //setSignal("ADDR-REG-LD-HI"); //FIX on new card
    writeCurrentLine();
    clearSignal("ADDR-REG-LD-LO");
    //clearSignal("ADDR-REG-LD-HI"); //Fix on new card
    writeCurrentLine();

    endInstruction();
    showCntlMemory(LD_SP_I);

    // Out Register (addr pointed to by sp)
    for (int reg = 0; reg < 8; reg++) {
        ins = OUT_REG | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        setSignal("REG-FUNC-RD");
        setSignal("1-BYTE-OPERAND-SEL");
        setRdId(reg);
        setSignal("REG-RD-LO"); // Output register onto data bus
        setSignal("ADDR-REG-ADDR-RD");
        writeCurrentLine();

        setSignal("I/O-WR");
        writeCurrentLine();
        clearSignal("I/O-RD");
        writeCurrentLine();

        endInstruction();
        showCntlMemory(ins);
    }
#endif
    dumpCntlMemory();
    printf("Done\n");
    return (EXIT_SUCCESS);
}

/* 
 * Write test program directly into microcode, this will 
 * test if uCode program counter is logic is working
 */
void basicTest() {
    //Basic Sequencer memory test OUT ON - OUT OFF - uPC Reset 

    clearCntlMemory();
    showCurrentLine();
    startInstruction(0);

    showCurrentLine();

    initCurrentLine();
    showCurrentLine();
    setSignal("OUT-ON");
    showCurrentLine();
    writeCurrentLine();
    showCurrentLine();

    initCurrentLine();
    setSignal("OUT-OFF");
    showCurrentLine();
    writeCurrentLine();

    initCurrentLine();
    setSignal("UCODE-COUNT-RESET");
    showCurrentLine();
    writeCurrentLine();
    endInstruction();

    dumpCntlMemory();
    printf("Done\n");

}