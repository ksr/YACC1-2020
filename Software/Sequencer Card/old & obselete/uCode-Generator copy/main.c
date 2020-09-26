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
#include"signal.h"
#include "code.h"
#include "../../opcodes.h"
#include "CodeGen.h"

#define ALUDATA 0
#define ALUSUB 1
#define ALUAND 2
#define ALUOR  3
#define ALUXOR 4
#define ALUSHIFT 5
#define ALUZERO 6
#define ALUADD 7

#define ALUBR 0
#define ALUBRZ 4
#define ALUBRIN 5

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
    for (int i = 0; strlen(signals[i].name) != 0; i++)
        clearSignal(signals[i].name);
}

/*
 * Called at the start of processing a new instruction
 */
void startInstruction(int instruction) {
    startUcodeBlock(instruction);
    //clearCurrentLine(); should not be required
    initCurrentLine();
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
}

void setRdId(int reg) {
    setRegBit("REG-RD-ID0", reg & 0x0001);
    setRegBit("REG-RD-ID1", reg & 0x0010);
    setRegBit("REG-RD-ID2", reg & 0x0100);
    setRegBit("REG-RD-ID3", reg & 0x1000);
}

void setLdId(int reg) {
    setRegBit("REG-LD-ID0", reg & 0x0001);
    setRegBit("REG-LD-ID1", reg & 0x0010);
    setRegBit("REG-LD-ID2", reg & 0x0100);
    setRegBit("REG-LD-ID3", reg & 0x0100);
}

void setAlu(int aluBits) {
    setRegBit("ALU0", aluBits & 0x0001);
    setRegBit("ALU1", aluBits & 0x0010);
    setRegBit("ALU2", aluBits & 0x0100);
    // what about alu3
}

/* 
 * Code Generation Utility Routines
 */

void loadNextInstruction() {
    //Load Reg 0 in ADDR Reg 0
    //Put data from MEM at (ADDR REG 0) on bus
    //load instruction register
    //increment Reg 0

    initCurrentLine();
    putMemAtRegOnBus(0);
    setSignal("LD-INS-REG"); // rising or falling
    writeCurrentLine();
    clearSignal("LD-INS-REG");
    writeCurrentLine();
    setRdId(0);
    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");
    writeCurrentLine();
    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("LD-INS-REG"); // if reg is edge triggered, clearing can be part of next ins
    clearSignal("-MEM-RD");
    clearSignal("-REG-UP");
    writeCurrentLine();
}

void loadBranchRegister() {
    //load the next two PC bytes format HHLL into branch register
    setSignal("ADDR-REG-FUNC-RD"); //sp/pc sel on old card try without
    setSignal("ADDR-REG-ADDR-RD");
    setSignal("-MEM-RD");
    writeCurrentLine();
    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI"); // if branch reg +ve edge triggered next write not required
    setSignal("ADDR-REG-UP");
    writeCurrentLine();
    clearSignal("ADDR-REG-UP"); // if addr  +ve edge triggered next write not required
    writeCurrentLine();
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO"); // if branch reg +ve edge triggered next write not required
    setSignal("ADDR-REG-UP");
    writeCurrentLine();
    clearSignal("ADDR-REG-UP");
    writeCurrentLine();
}

void putMemAtRegOnBus(int reg) {
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-RD-LO");
    setRdId(reg);
    writeCurrentLine();
    setSignal("-ADDR-REG-LD0");
    writeCurrentLine();
    clearSignal("-ADDR-REG-LD0");
    writeCurrentLine();
    clearSignal("-REG-FUNC-RD");
    clearSignal("-REG-RD-LO");
    writeCurrentLine();
    setSignal("-ADDR-REG-RD0");
    setSignal("-MEM-RD");
    writeCurrentLine();
}

int main(int argc, char** argv) {
    int reg;
    int ins;

    basicTest(); //basic sequencer test led on, led off, reset counter
    exit(0);

    clearCntlMemory();
    //clearCurrentLine();

    startInstruction(0); // what is this for??? microcode to fetch 1st instructon?
    loadNextInstruction();
    endInstruction(); // this is a test (seems to work)
    showCntlMemory(0);

    // OUT ON
    startInstruction(ON);
    setSignal("OUT-ON");
    writeCurrentLine();
    loadNextInstruction();
    endInstruction();
    showCntlMemory(ON);

    // OUT OFF
    startInstruction(OFF);
    setSignal("OUT-OFF");
    writeCurrentLine();
    loadNextInstruction();
    endInstruction();
    showCntlMemory(OFF);

    // OUT ON
    startInstruction(HALT);
    setSignal("HALT");
    writeCurrentLine();
    loadNextInstruction();
    endInstruction();
    showCntlMemory(HALT);

    //load register low immediate 8 bit
    for (reg = 0; reg < 8; reg++) {
        ins = MVI | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        setSignal("ADDR-REG-FUNC-RD"); //sp/pc sel on old card try without
        setSignal("ADDR-REG-ADDR-RD"); // output pc address onto address bus
        setSignal("REG-FUNC-LD"); //enable register card for loading, 
        setSignal("1-BYTE-OPERAND-SEL");
        setSignal("-MEM-RD"); //output memory (location for address bus) to data bus
        setLdId(reg); //set the ID bits for the register
        writeCurrentLine();
        setSignal("REG-LD-LO"); //load register
        writeCurrentLine();
        clearSignal("REG-LD-LO");
        setSignal("ADDR-REG-UP"); //increment pc
        writeCurrentLine();
        clearSignal("ADDR-REG-UP");
        writeCurrentLine();

        endInstruction();
        showCntlMemory(ins);
    }
    /*
        // Branch
        startInstruction(BRANCH);
        loadNextInstruction();
       initCurrentLine();

        loadBranchRegister();
        initCurrentLine();
        setSignal("BRANCH-RD-LO"); //output branch register
        setSignal("BRANCH-RD-HI");
        setSignal("-REG-FUNC-LD"); //sp/pc select on old card, try without
        writeCurrentLine();

        // ALU 0-2 condition bits all set to 0 therefore selected condition (0) is HIGH forcing a branch

        setSignal("-REG-LD-LO"); //load branch register
        setSignal("-REG-LD-HI"); //FIX on new card ???
        writeCurrentLine();
        clearSignal("-REG-LD-LO");
        clearSignal("-REG-LD-HI"); //Fix on new card  ???
        writeCurrentLine();

        endInstruction();
        showCntlMemory(BRANCH);

        // Branch IN == 1 // This needs to be fixed, input will be part of regular branch logic
        startInstruction(BRANCH_IN_TRUE);
        loadNextInstruction();
        initCurrentLine();

        loadBranchRegister();
        initCurrentLine();
        setSignal("BRANCH-RD-LO"); //output branch register
        setSignal("BRANCH-RD-HI");
        setSignal("-REG-FUNC-LD"); //sp/pc sel on old card try without
        setSignal("TEST-IN"); //set test condition // this will be set via ALU settings
        writeCurrentLine();
        setSignal("-REG-LD-LO");
        setSignal("-REG-LD-HI");
        writeCurrentLine();
        clearSignal("-REG-LD-LO");
        clearSignal("-REG-LD-HI");
        writeCurrentLine();

        endInstruction();
        showCntlMemory(BRANCH_IN_TRUE);

        // Branch IN == 0 // This needs to be fixed, input will be part of regular branch logic
        startInstruction(BRANCH_IN_FALSE);
        loadNextInstruction();
        initCurrentLine();

        // Code missing 

        endInstruction();
        showCntlMemory(BRANCH_IN_FALSE);


        // LOAD Accumulator Immediate
        startInstruction(LD_ACC_I);
        loadNextInstruction();
        initCurrentLine();

        setSignal("ALU-FUNC");
        putMemAtRegOnBus(0);
        setSignal("-REG-FUNC-RD"); //sp/pc sel on old card try without
        setSignal("-REG-RD-LO"); //sp/pc sel on old card try without
        setSignal("-MEM-RD");
        writeCurrentLine();
        setSignal("AC-LD");
        writeCurrentLine();
        clearSignal("AC-LD");
        setSignal("-REG-UP");
        writeCurrentLine();
        clearSignal("-REG-UP");
        writeCurrentLine();
        endInstruction();
        showCntlMemory(LD_ACC_I);

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

        // Branch Accumulator != 0
        startInstruction(BRANCH_ACC_NZ);
        loadNextInstruction();
        initCurrentLine();

        loadBranchRegister();
        initCurrentLine();
        setSignal("BRANCH-RD-LO"); // OUTPUT the BRANCH register
        setSignal("BRANCH-RD-HI");
        setSignal("ADDR-REG-FUNC-RD"); //sp/pc sel on old card try without
        setSignal("ALU2"); // set branch condition
        setSignal("AC-LD-INV"); // ???wrong this is now testing ac = 255, inv causes all sigs to 0
        setSignal("AC-RD"); //output accumulator to compare logic for branch test
        writeCurrentLine();
        setSignal("ADDR-REG-LD-LO"); //load branch register (if condition met)
        //setSignal("ADDR-REG-LD-HI"); // FIX with new SP card
        writeCurrentLine();
        clearSignal("ADDR-REG-LD-LO");
        //clearSignal("ADDR-REG-LD-HI"); //FIC with new SP card
        writeCurrentLine();

        endInstruction();
        showCntlMemory(BRANCH_ACC_NZ);

        // OUT Accumulator, addr pointed to by SP
        startInstruction(ACC_OUT);
        loadNextInstruction();
        initCurrentLine();

        setSignal("ADDR-REG-ADDR-RD");
        setSignal("ALU-FUNC");
        setSignal("AC-RD");
        writeCurrentLine();
        setSignal("I/O-WR");
        writeCurrentLine();
        clearSignal("I/O-RD");
        writeCurrentLine();

        endInstruction();
        showCntlMemory(ACC_OUT);

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

        //load register immediate 8 bit
        for (reg = 0; reg < 8; reg++) {
            ins = LD_REG_I | (reg & 0x07);
            startInstruction(ins);
            loadNextInstruction();
            initCurrentLine();

            setSignal("ADDR-REG-FUNC-RD"); //sp/pc sel on old card try without
            setSignal("ADDR-REG-ADDR-RD"); // output pc address onto address bus
            setSignal("REG-FUNC-LD"); //enable register card for loading, 
            setSignal("1-BYTE-OPERAND-SEL");
            setSignal("-MEM-RD"); //output memory (location for address bus) to data bus
            setLdId(reg); //set the ID bits for the register
            writeCurrentLine();
            setSignal("REG-LD-LO"); //load register
            writeCurrentLine();
            clearSignal("REG-LD-LO");
            setSignal("ADDR-REG-UP"); //increment pc
            writeCurrentLine();
            clearSignal("ADDR-REG-UP");
            writeCurrentLine();

            endInstruction();
            showCntlMemory(ins);
        }

        // move register low to accumulator
        for (int reg = 0; reg < 8; reg++) {
            ins = MV_REG_ACC | (reg & 0x07);
            startInstruction(ins);
            loadNextInstruction();
            initCurrentLine();

            setSignal("REG-FUNC-RD"); //enable register card for loading, 
            setSignal("1-BYTE-OPERAND-SEL");
            setRdId(reg); //set the ID bits for the register
            setSignal("REG-RD-LO"); // Output register onto data bus
            setSignal("ALU-FUNC");
            writeCurrentLine();
            setSignal("AC-LD");
            writeCurrentLine();
            clearSignal("AC-LD");
            writeCurrentLine();

            endInstruction();
            showCntlMemory(ins);
        }

        // move accumulator to register low
        for (int reg = 0; reg < 8; reg++) {
            ins = MV_ACC_REG | (reg & 0x07);
            startInstruction(ins);
            loadNextInstruction();
            initCurrentLine();

            setSignal("REG-FUNC-LD");
            setSignal("1-BYTE-OPERAND-SEL");
            setLdId(reg);
            setSignal("ALU-FUNC");
            setSignal("AC-RD");
            writeCurrentLine();
            setSignal("REG-LD-LO");
            writeCurrentLine();
            clearSignal("REG-LD-LO");
            writeCurrentLine();

            endInstruction();
            showCntlMemory(ins);
        }

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
     */
    dumpCntlMemory();
    printf("Done\n");
    return (EXIT_SUCCESS);
}

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