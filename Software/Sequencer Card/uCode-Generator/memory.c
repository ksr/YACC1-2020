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

void doMemory() {
    int ins, reg;

    // Load imm via register //KEN why tmp reg if needed shluld be tmp 1?
    for (reg = 0; reg < 8; reg++) {
        ins = LDIVR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        putMemAtRegOnBus(PC);
        setSignal("-TMP-REG-LD1");

        writeCurrentLine();
        clearSignal("-TMP-REG-LD1");
        writeCurrentLine();
        initCurrentLine(); // or just clear specific lines? memrd is active from putMemAtREG...
        

        putBustoRegMem(reg,"-TMP-REG-RD1");

        incrementReg(PC);
        
        endInstruction();
        showCntlMemory(ins);
    }
}