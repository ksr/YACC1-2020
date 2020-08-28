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

void accumulatorInstructions(){
    int ins,reg;
    
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


//LDA MEM via REG to Accum
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

    //Store Accum at  MEM via REG
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

        putBustoReg(reg);


        endInstruction();
        showCntlMemory(ins);
    }
}