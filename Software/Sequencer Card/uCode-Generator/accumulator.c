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

void aluOp(int func){
    setSignal("-ALU-FUNC");
    setAlu(func);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();
}

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

        putBustoRegMem(reg, "-AC-RD");

        endInstruction();
        showCntlMemory(ins);
    }

    //load Accum low immediate
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

    incrementReg(PC);
    
    
    endInstruction();
    showCntlMemory(ins);



    //Add to  Accum low immediate 8 bit
    ins = ADDI;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);

    aluOp(ALUADD);
   
    incrementReg(PC);

    endInstruction();
    showCntlMemory(ins);

    // Sub from Accum low immediate 8 bit
    ins = SUBI;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);

    aluOp(ALUSUB);
    
    incrementReg(PC);

    endInstruction();
    showCntlMemory(ins);

    //AND to  Accum low immediate 8 bit
    ins = ANDI;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);

    aluOp(ALUAND);

    incrementReg(PC);
    
    endInstruction();
    showCntlMemory(ins);

    //OR to  Accum low immediate 8 bit
    ins = ORI;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);

    aluOp(ALUOR);
    
    incrementReg(PC);

    endInstruction();
    showCntlMemory(ins);

    //XORI to  Accum low immediate 8 bit
    ins = XORI;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);

    aluOp(ALUXOR);
    
    incrementReg(PC);

    endInstruction();
    showCntlMemory(ins);
}