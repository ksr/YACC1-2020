/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "yaccsignal.h"
#include "code.h"
#include "../../opcodes.h"
#include "CodeGen.h"

void accumulatorInstructions() {
    int ins, reg;

    //Move Register low to Accumulator
    for (reg = 0; reg < 8; reg++) {
        ins = MVRLA | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();


        setSignal("-REG-FUNC-RD");
        setRdId(reg);
        setSignal("-REG-RD-LO");
        setSignal("-ALU-FUNC");
        setAlu(ALUDATA);
        writeCurrentLine();
        setSignal("-AC-LD");
        writeCurrentLine();
        clearSignal("-AC-LD");
        writeCurrentLine();

        endInstruction();
        showCntlMemory(ins);
    }

    //Move Register high to Accumulator
    for (reg = 0; reg < 8; reg++) {
        ins = MVRHA | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();


        setSignal("-REG-FUNC-RD");
        setRdId(reg);
        setSignal("-REG-RD-HI");
        setSignal("-HL-SWAP");
        setSignal("-ALU-FUNC");
        setAlu(ALUDATA);
        writeCurrentLine();
        setSignal("-AC-LD");
        writeCurrentLine();
        clearSignal("-AC-LD");
        writeCurrentLine();

        endInstruction();
        showCntlMemory(ins);
    }


    //Load Accum From MEM pointed to by REG
    for (reg = 0; reg < 8; reg++) {
        ins = LDAVR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        putMemAtRegOnBus(reg);

        setSignal("-ALU-FUNC");
        setAlu(ALUDATA);
        writeCurrentLine();
        setSignal("-AC-LD");
        writeCurrentLine();
        clearSignal("-AC-LD");
        writeCurrentLine();

        endInstruction();
        showCntlMemory(ins);
    }

    //Store Accum at  MEM pointed to by REG
    for (reg = 0; reg < 8; reg++) {
        ins = STAVR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        setSignal("-ALU-FUNC");
        setAlu(ALUDATA);
        writeCurrentLine();
        setSignal("-AC-RD");
        writeCurrentLine();

        putBustoRegMem(reg);


        endInstruction();
        showCntlMemory(ins);
    }

    //load Accum low immediate 8 bit
    ins = LDAI;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);

    setSignal("-ALU-FUNC");
    setAlu(ALUDATA);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();

    setRdId(PC);
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");
    writeCurrentLine(); // clear reg-func-rd?

    endInstruction();
    showCntlMemory(ins);

    //Add to  Accum low immediate 8 bit
    ins = ADDI;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);

    setSignal("-ALU-FUNC");
    setAlu(ALUADD);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();

    setSignal("-REG-FUNC-RD");
    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");
    writeCurrentLine(); // clear reg-func-rd?

    endInstruction();
    showCntlMemory(ins);




}