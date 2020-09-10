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

void ioInstructions() {
    int reg, ins;

    // OUT ON
    startInstruction(ON);
    loadNextInstruction();
    initCurrentLine();
    setSignal("OUT-ON");
    writeCurrentLine();
    endInstruction();
    showCntlMemory(ON);

    // OUT OFF
    startInstruction(OFF);
    loadNextInstruction();
    initCurrentLine();
    setSignal("OUT-OFF");
    writeCurrentLine();
    endInstruction();
    showCntlMemory(OFF);


    //OUT immediate 
    for (reg = 0; reg <= 15; reg++) {
        ins = OUTI | (reg & 0x0f);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        putMemAtRegOnBus(PC);


        setIo(reg);
        writeCurrentLine();
        setSignal("-IO-ADDR-LD"); //if io addr is latched on rising edge then setio and io-addr-ld can be combined Is ioaddrld even needed
        writeCurrentLine();
        clearSignal("-IO-ADDR-LD");
        writeCurrentLine();
        setSignal("-IO-WR");
        writeCurrentLine();
        clearSignal("-IO-WR");
        writeCurrentLine();

        initCurrentLine();
        
        incrementReg(PC);

        endInstruction();
        showCntlMemory(ins);
    }

    //OUT ACCUM 
    for (reg = 0; reg <= 15; reg++) {
        ins = OUTA | (reg & 0x0f);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        setSignal("-AC-RD");
        setSignal("-ALU-FUNC");

        setIo(reg);
        writeCurrentLine();
        setSignal("-IO-ADDR-LD"); //if io addr is latched on rising edge then setio and io-addr-ld can be combined Is ioaddrld even needed
        writeCurrentLine();
        clearSignal("-IO-ADDR-LD");
        writeCurrentLine();
        setSignal("-IO-WR");
        writeCurrentLine();
        clearSignal("-IO-WR");
        writeCurrentLine(); // clear reg-func-rd?

        endInstruction();
        showCntlMemory(ins);
    }

    for (reg = 0; reg <= 15; reg++) {
        ins = INP | (reg & 0x0f);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        setSignal("-ALU-FUNC");
        setAlu(ALUDATA);
        setIo(reg);
        setSignal("-IO-ADDR-LD"); //if io addr is latched on rising edge then setio and io-addr-ld can be combined Is ioaddrld even needed
        writeCurrentLine();
        clearSignal("-IO-ADDR-LD");
        writeCurrentLine();
        
        setSignal("-IO-RD");
        writeCurrentLine();
        
        setSignal("-AC-LD");
        writeCurrentLine();
        clearSignal("-AC-LD");
        writeCurrentLine();
        endInstruction();
        showCntlMemory(ins);
    }
}
