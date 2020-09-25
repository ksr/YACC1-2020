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

void aluOp(int func) {
    setSignal("-ALU-FUNC");
    setAlu(func);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();
}

void shiftOp(int instruction, int mode) {

    startInstruction(instruction);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-ALU-FUNC");
    setAlu(SHIFT_LOAD);
    writeCurrentLine();
    setSignal("-SR-LD");
    writeCurrentLine();
    clearSignal("-SR-LD");
    writeCurrentLine();
    setAlu(mode);
    writeCurrentLine();
    setSignal("-SR-LD");
    writeCurrentLine();
    clearSignal("-SR-LD");
    writeCurrentLine();

    setAlu(ALUSHIFT);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine(); // clear reg-func-rd?

    endInstruction();
    showCntlMemory(instruction);
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

    //Move Accumulator to Register Low
    for (reg = 0; reg < 8; reg++) {
        ins = MVARL | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        setSignal("-ALU-FUNC");
        setAlu(ALUDATA);
        setSignal("-AC-RD");
        writeCurrentLine();

        setSignal("-REG-FUNC-LD");
        setLdId(reg);

        writeCurrentLine();
        setSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");

        writeCurrentLine();

        endInstruction();
        showCntlMemory(ins);
    }

    //Move Accumulator to Register High
    for (reg = 0; reg < 8; reg++) {
        ins = MVARH | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        setSignal("-ALU-FUNC");
        setAlu(ALUDATA);
        setSignal("-AC-RD");
        writeCurrentLine();

        setSignal("-REG-FUNC-LD");
        setSignal("-HL-SWAP"); // needs testing Sept 21 entering address in montitor not working
        setLdId(reg);

        writeCurrentLine();
        setSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");
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

//Invert accumulator 
    ins = INVA;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-ALU-FUNC");
    setAlu(ALUDATA);
    setSignal("-AC-RD");
    setSignal("-AC-LD-INV");
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();

   

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

    setSignal("-AC-RD");
    setSignal("-ALU-FUNC");
    writeCurrentLine();
    setSignal("-TMP-REG-LD1");
    writeCurrentLine();
    clearSignal("-TMP-REG-LD1");
    writeCurrentLine();
    clearSignal("-AC-RD");
    writeCurrentLine();

    putMemAtRegOnBus(PC);

    setAlu(ALUDATA);
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-MEM-RD");
    setSignal("-TMP-REG-RD1");
    writeCurrentLine();

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

     //AND to  Accum with tmp
    ins = ANDT;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-TMP-REG-RD0");
    writeCurrentLine();

    aluOp(ALUAND);

    endInstruction();
    showCntlMemory(ins);

    //OR to  Accum with tmp
    ins = ORT;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-TMP-REG-RD0");
    clearSignal("-MEM-RD");
    writeCurrentLine();

    aluOp(ALUOR);

    endInstruction();
    showCntlMemory(ins);

    //XOR to  Accum with tmp
    ins = XORT;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-TMP-REG-RD0");
    clearSignal("-MEM-RD");
    writeCurrentLine();

    aluOp(ALUXOR);

    endInstruction();
    showCntlMemory(ins);
    
    //ADD to  tmp to accum
    ins = ADDT;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-TMP-REG-RD0");
    clearSignal("-MEM-RD");
    writeCurrentLine();

    aluOp(ALUADD);

    endInstruction();
    showCntlMemory(ins);
    
    
    // sub tmp from accum
    // Sub from Accum low immediate 8 bit
    ins = SUBT;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-AC-RD");
    setSignal("-ALU-FUNC");
    writeCurrentLine();
    setSignal("-TMP-REG-LD1");
    writeCurrentLine();
    clearSignal("-TMP-REG-LD1");
    writeCurrentLine();
    clearSignal("-AC-RD");
    writeCurrentLine();

    //putMemAtRegOnBus(PC);
    setSignal("-TMP-REG-RD0");

    setAlu(ALUDATA);
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();
    //clearSignal("-MEM-RD");
    clearSignal("-TMP-REG-RD0");
    setSignal("-TMP-REG-RD1");
    writeCurrentLine();

    aluOp(ALUSUB);

    incrementReg(PC);

    endInstruction();
    showCntlMemory(ins);
    
    
    shiftOp(SHL, (SHIFT_LEFT | SHIFT_ZERO));
    shiftOp(SHR, (SHIFT_RIGHT | SHIFT_ZERO));
    shiftOp(RSHL, (SHIFT_LEFT | SHIFT_RING));
    shiftOp(RSHR, (SHIFT_RIGHT | SHIFT_RING));
    shiftOp(PSHR, (SHIFT_RIGHT | SHIFT_PROP));
    shiftOp(CSHL, (SHIFT_LEFT | SHIFT_CARRY));
    shiftOp(CSHR, (SHIFT_RIGHT | SHIFT_CARRY));

    ins = MVAT;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-ALU-FUNC");
    setAlu(ALUDATA);
    setSignal("-AC-RD");
    writeCurrentLine();
    setSignal("-TMP-REG-LD0");
    writeCurrentLine();
    clearSignal("-TMP-REG-LD0");
    writeCurrentLine();

    endInstruction();
    showCntlMemory(ins);

    ins = MVTA;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-TMP-REG-RD0");
    writeCurrentLine();
    setSignal("-ALU-FUNC");
    setAlu(ALUDATA);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();


    endInstruction();
    showCntlMemory(ins);

#ifdef notused    
    //Shift Accum Left LSB->MSB
    ins = SHL;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-ALU-FUNC");
    setAlu(SHIFT_LOAD);
    writeCurrentLine();
    setSignal("-SR-LD");
    writeCurrentLine();
    clearSignal("-SR-LD");
    writeCurrentLine();
    setAlu(SHIFT_LEFT | SHIFT_ZERO);
    writeCurrentLine();
    setSignal("-SR-LD");
    writeCurrentLine();
    clearSignal("-SR-LD");
    writeCurrentLine();

    setAlu(ALUSHIFT);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine(); // clear reg-func-rd?

    endInstruction();
    showCntlMemory(ins);

    //Shift Accum Left MSB->LSB
    ins = SHR;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    setSignal("-ALU-FUNC");
    setAlu(SHIFT_LOAD);
    writeCurrentLine();
    setSignal("-SR-LD");
    writeCurrentLine();
    clearSignal("-SR-LD");
    writeCurrentLine();
    setAlu(SHIFT_RIGHT | SHIFT_ZERO);
    writeCurrentLine();
    setSignal("-SR-LD");
    writeCurrentLine();
    clearSignal("-SR-LD");
    writeCurrentLine();

    setAlu(ALUSHIFT);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine(); // clear reg-func-rd?

    endInstruction();
    showCntlMemory(ins);
#endif    
}