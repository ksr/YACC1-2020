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

void moveRegtoTmp(int reg) {
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-RD-LO");
    setSignal("-REG-RD-HI");
    setRdId(reg);
    setSignal("-TMP-REG-LD0");
    writeCurrentLine();
    clearSignal("-TMP-REG-LD0");
    writeCurrentLine();
}

void branch(int mode, int invert, int actest) {
    //load the next two PC bytes format HHLL into branch register
    putMemAtRegOnBus(PC);
    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI"); // if branch reg +ve edge triggered next write not required
    writeCurrentLine();
    setSignal("-REG-FUNC-RD");
    setRdId(PC);
    setSignal("-REG-UP"); //Are conditions correct for REG UP - Index board bits set?
    writeCurrentLine();
    clearSignal("-REG-UP"); // if addr  +ve edge triggered next write not required
    writeCurrentLine();
    putMemAtRegOnBus(0);
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO"); // if branch reg +ve edge triggered next write not required
    //writeCurrentLine(); reg up is rising edge
    setSignal("-REG-FUNC-RD");
    setRdId(PC);
    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");
    writeCurrentLine(); // clear reg-func-rd?

    initCurrentLine(); //KEN - AUG 25 remove test, try existing code
    setSignal("-BRANCH-RD-LO"); //output branch register
    setSignal("-BRANCH-RD-HI");
    //setSignal("-REG-FUNC-LD");//?
    //writeCurrentLine(); //Ken Probably not needed
    if (actest == 1)
        setSignal("-AC-RD");
    setSignal("-ALU-FUNC");
    setAlu(mode);
    //if (mode == 4) exit(0);
    if (invert == INVERT)
        setSignal("-AC-LD-INV");
    setSignal("BR-TEST");
    setLdId(PC);
    setSignal("-REG-FUNC-LD");
    writeCurrentLine();
    setSignal("REG-LD-LO"); //load branch register
    setSignal("REG-LD-HI"); //FIX on new card ???
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    clearSignal("REG-LD-HI"); //Fix on new card  ???
    writeCurrentLine();
}

void branchInstructions() {

    startInstruction(BR);
    loadNextInstruction();
    initCurrentLine();
    branch(ALUBR, NOINVERT, 0);
    endInstruction();
    showCntlMemory(BR);

    startInstruction(BRINH);
    loadNextInstruction();
    initCurrentLine();
    branch(ALUBRIN, NOINVERT, 0);
    endInstruction();
    showCntlMemory(BRINH);

    startInstruction(BRINL);
    loadNextInstruction();
    initCurrentLine();
    branch(ALUBRIN, INVERT, 0);
    endInstruction();
    showCntlMemory(BRINL);

    startInstruction(BRZ);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(ALUBRZ, NOINVERT, 1);
    endInstruction();
    showCntlMemory(BRZ);

    startInstruction(BRNZ);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(ALUBRZ, INVERT, 1);
    endInstruction();
    showCntlMemory(BRNZ);

    //don't branch but setup branch register for JSR
    startInstruction(NBR);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(ALUBR, INVERT, 0);
    endInstruction();
    showCntlMemory(NBR);

    // Jump to subroutine must be called after NBR
    startInstruction(JSR);
    loadNextInstruction();
    initCurrentLine();

    moveRegtoTmp(PC);
    putBustoRegMem(SP, "-TMP-REG-RD0");

    setRdId(SP);
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-DN");
    writeCurrentLine();
    clearSignal("-REG-DN");
    writeCurrentLine();

    setSignal("-HL-SWAP");
    moveRegtoTmp(PC);
    putBustoRegMem(SP, "-TMP-REG-RD0");
    setRdId(SP);
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-DN");
    writeCurrentLine();
    clearSignal("-REG-DN");
    writeCurrentLine();
    setSignal("-BRANCH-RD-LO"); //output branch register
    setSignal("-BRANCH-RD-HI");
    
    
    setSignal("-ALU-FUNC");
    setAlu(ALUBR);
    setSignal("BR-TEST");
    setLdId(PC);
    setSignal("-REG-FUNC-LD");
    writeCurrentLine();
    setSignal("REG-LD-LO"); //load branch register
    setSignal("REG-LD-HI"); //FIX on new card ???
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    clearSignal("REG-LD-HI"); //Fix on new card  ???
    writeCurrentLine();
    endInstruction();
    showCntlMemory(JSR);

}

