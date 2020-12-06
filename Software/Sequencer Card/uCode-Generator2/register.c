/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../opcodes.h"
#include "code.h"
#include "CodeGen.h"

void registerOnlyInstructions() {
    int reg;
    int ins;

    //Decrement Register
    for (reg = 0; reg < 8; reg++) {
        ins = DECR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        decrementReg(reg);

        endInstruction();
        showCntlMemory(ins);
    }

    //Increment Register
    for (reg = 0; reg < 8; reg++) {
        ins = INCR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        incrementReg(reg);

        endInstruction();
        showCntlMemory(ins);
    }

    //load register low immediate 8 bit
    for (reg = 0; reg < 8; reg++) {
        ins = MVIB | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        putMemAtRegOnBus(PC);

        setSignal("-REG-FUNC-LD");
        setLdId(reg);
        writeCurrentLine();
        setSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");
        incrementReg(PC);

        endInstruction();
        showCntlMemory(ins);
    }

    //load tmp reg 0 immediate 
    ins = LDTI;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);

    setSignal("-TMP-REG-LD0");
    writeCurrentLine();
    clearSignal("-TMP-REG-LD0");
    writeCurrentLine();
    incrementReg(PC);

    endInstruction();
    showCntlMemory(ins);

    //load register low & hi immediate 16 bit
    for (reg = 0; reg < 8; reg++) {
        ins = MVIW | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        putMemAtRegOnBus(PC);

        setSignal("-REG-FUNC-LD");
        setLdId(reg);
        setSignal("-HL-SWAP");
        writeCurrentLine();
        setSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");
        clearSignal("-HL-SWAP");

        incrementReg(PC);

        putMemAtRegOnBus(PC);

        setSignal("-REG-FUNC-LD");
        setLdId(reg);

        writeCurrentLine();
        setSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("REG-LD-LO");
        writeCurrentLine();


        clearSignal("-REG-FUNC-LD");
        incrementReg(PC);

        endInstruction();
        showCntlMemory(ins);
    }
    //
    ins = MOVRR;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);
    setSignal("OPERAND-CLK");
    writeCurrentLine();
    clearSignal("OPERAND-CLK");
    writeCurrentLine();
    clearSignal("-MEM-RD");
    writeCurrentLine();
    incrementReg(PC);

    setSignal("-2-BYTE-OPERAND-SEL");
    setSignal("-REG-FUNC-LD");
    setSignal("-REG-FUNC-RD");
    writeCurrentLine();

    setSignal("-REG-RD-LO");
    setSignal("-REG-RD-HI");
    writeCurrentLine();

    setSignal("REG-LD-LO");
    setSignal("REG-LD-HI");
    writeCurrentLine();

    clearSignal("REG-LD-LO");
    clearSignal("REG-LD-HI");
    writeCurrentLine();

    clearSignal("-REG-RD-LO");
    clearSignal("-REG-RD-HI");
    writeCurrentLine();

    endInstruction();
    showCntlMemory(ins);

    for (reg = 0; reg < 8; reg++) {
        ins = LDR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        putMemAtRegOnBus(PC);
        setSignal("-REG-FUNC-LD");
        setLdId(IR);
        setSignal("-HL-SWAP");
        setSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");
        clearSignal("-HL-SWAP");
        writeCurrentLine();
        incrementReg(PC);

        putMemAtRegOnBus(PC);
        setSignal("-REG-FUNC-LD");
        setLdId(IR);
        writeCurrentLine();
        setSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");
        clearSignal("-MEM-RD");
        writeCurrentLine();
        incrementReg(PC);

        putMemAtRegOnBus(IR);
        setSignal("-REG-FUNC-LD");
        setLdId(reg);
        setSignal("-HL-SWAP");
        writeCurrentLine();
        setSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");
        clearSignal("-HL-SWAP");
        writeCurrentLine();
        setRdId(IR);
        writeCurrentLine();
        incrementReg(IR);

        putMemAtRegOnBus(IR);
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

    for (reg = 0; reg < 8; reg++) {
        ins = STR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        putMemAtRegOnBus(PC);
        setSignal("-REG-FUNC-LD");
        setLdId(IR);
        setSignal("-HL-SWAP");
        writeCurrentLine();
        setSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("REG-LD-HI");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");
        clearSignal("-HL-SWAP");
        writeCurrentLine();
        incrementReg(PC);

        putMemAtRegOnBus(PC);
        setSignal("-REG-FUNC-LD");
        setLdId(IR);
        writeCurrentLine();
        setSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("-REG-FUNC-LD");
        clearSignal("-MEM-RD");
        writeCurrentLine();
        incrementReg(PC);

        setSignal("-REG-FUNC-RD");
        setRdId(reg);
        setSignal("-HL-SWAP");
        writeCurrentLine();
        putBustoRegMem(IR, "-REG-RD-HI");
        writeCurrentLine();
        setRdId(IR);
        writeCurrentLine();
        incrementReg(IR);

        clearSignal("-HL-SWAP");
        setSignal("-REG-FUNC-RD");
        setRdId(reg);
        writeCurrentLine();
        putBustoRegMem(IR, "-REG-RD-LO");
        writeCurrentLine();
       
        endInstruction();
        showCntlMemory(ins);
    }

    ins = LDA;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);
    setSignal("-REG-FUNC-LD");
    setLdId(IR);
    setSignal("-HL-SWAP");
    setSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    clearSignal("-HL-SWAP");
    writeCurrentLine();
    incrementReg(PC);

    putMemAtRegOnBus(PC);
    setSignal("-REG-FUNC-LD");
    setLdId(IR);
    writeCurrentLine();
    setSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    clearSignal("-MEM-RD");
    writeCurrentLine();
    incrementReg(PC);

    putMemAtRegOnBus(IR);
    setSignal("-ALU-FUNC");
    setAlu(ALUDATA);
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();

    endInstruction();
    showCntlMemory(ins);

    ins = LDT;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);
    setSignal("-REG-FUNC-LD");
    setLdId(IR);
    setSignal("-HL-SWAP");
    setSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    clearSignal("-HL-SWAP");
    writeCurrentLine();
    incrementReg(PC);

    putMemAtRegOnBus(PC);
    setSignal("-REG-FUNC-LD");
    setLdId(IR);
    writeCurrentLine();
    setSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    clearSignal("-MEM-RD");
    writeCurrentLine();
    incrementReg(PC);

    putMemAtRegOnBus(IR);
    setSignal("-TMP-REG-LD0");
    writeCurrentLine();
    clearSignal("-TMP-REG-LD0");
    writeCurrentLine();

    endInstruction();
    showCntlMemory(ins);

    ins = STA;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);
    setSignal("-REG-FUNC-LD");
    setLdId(IR);
    setSignal("-HL-SWAP");
    setSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    clearSignal("-HL-SWAP");
    writeCurrentLine();
    incrementReg(PC);

    putMemAtRegOnBus(PC);
    setSignal("-REG-FUNC-LD");
    setLdId(IR);
    writeCurrentLine();
    setSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    clearSignal("-MEM-RD");
    writeCurrentLine();
    incrementReg(PC);

    setSignal("-ALU-FUNC");
    setAlu(ALUDATA);
    writeCurrentLine();

    putBustoRegMem(IR, "-AC-RD");

    endInstruction();
    showCntlMemory(ins);


    ins = STT;
    startInstruction(ins);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);
    setSignal("-REG-FUNC-LD");
    setLdId(IR);
    setSignal("-HL-SWAP");
    setSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    clearSignal("-HL-SWAP");
    writeCurrentLine();
    incrementReg(PC);

    //putBustoRegMem(IR, "-REG-RD-LO");
    putMemAtRegOnBus(PC);
    setSignal("-REG-FUNC-LD");
    setLdId(IR);
    writeCurrentLine();
    setSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    clearSignal("-MEM-RD");
    writeCurrentLine();
    incrementReg(PC);

    putBustoRegMem(IR, "-TMP-REG-RD0");

    endInstruction();
    showCntlMemory(ins);
}
