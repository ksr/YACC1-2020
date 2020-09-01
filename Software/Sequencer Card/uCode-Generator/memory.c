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

    for (reg = 0; reg < 8; reg++) {
        ins = LDIVR | (reg & 0x07);
        startInstruction(ins);
        loadNextInstruction();
        initCurrentLine();

        putMemAtRegOnBus(PC);
        setSignal("-TMP-REG-LD0");

        writeCurrentLine();
        clearSignal("-TMP-REG-LD0");
        writeCurrentLine();
        initCurrentLine(); // or just clear specific lines?


        putBustoRegMem(reg,"-TMP-REG-RD0");

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