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

void registerOnlyInstructions() {
    int reg;
    int ins;
    
    //Decrement Register
    for (reg = 0; reg < 8; reg++) {
        ins = DECR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();


        setSignal("-REG-FUNC-RD");
        setRdId(reg);

        writeCurrentLine();
        setSignal("-REG-DN");
        writeCurrentLine();
        clearSignal("-REG-DN");
        writeCurrentLine();

        endInstruction();
        showCntlMemory(ins);
    }

    //Increment Register
    for (reg = 0; reg < 8; reg++) {
        ins = INCR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();


        setSignal("-REG-FUNC-RD");
        setRdId(reg);

        writeCurrentLine();
        setSignal("-REG-UP");
        writeCurrentLine();
        clearSignal("-REG-UP");
        writeCurrentLine();

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
        setRdId(PC);
        setSignal("-REG-FUNC-RD");
        setSignal("-REG-UP");
        writeCurrentLine();
        clearSignal("-REG-UP");
        writeCurrentLine(); // clear reg-func-rd?

        endInstruction();
        showCntlMemory(ins);
    }

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
        
        setRdId(PC);
        setSignal("-REG-FUNC-RD");
        setSignal("-REG-UP");
        writeCurrentLine();
        clearSignal("-REG-UP");
        writeCurrentLine();

        putMemAtRegOnBus(PC);

        setSignal("-REG-FUNC-LD");
        setLdId(reg);

        writeCurrentLine();
        setSignal("REG-LD-LO");
        writeCurrentLine();
        clearSignal("REG-LD-LO");
        writeCurrentLine();


        clearSignal("-REG-FUNC-LD");
        setRdId(PC);
        setSignal("-REG-FUNC-RD");
        setSignal("-REG-UP");
        writeCurrentLine();
        clearSignal("-REG-UP");
        writeCurrentLine(); // clear reg-func-rd?

        endInstruction();
        showCntlMemory(ins);
    }
}