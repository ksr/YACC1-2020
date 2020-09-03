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
    
    incrementReg(PC);
    
    putMemAtRegOnBus(0);
    
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO"); // if branch reg +ve edge triggered next write not required
    //writeCurrentLine(); reg up is rising edge??? are you sure
    
    incrementReg(PC);
    
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

    // JSR
    startInstruction(JSR); //try as one instruction *** possible two writeCurrentLine needed see below
    loadNextInstruction();
    initCurrentLine();
    
    putMemAtRegOnBus(PC);
    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI"); // if branch reg +ve edge triggered next write not required
    writeCurrentLine(); //maybe not needed
    
    incrementReg(PC);
    
    putMemAtRegOnBus(PC);
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO"); // if branch reg +ve edge triggered next write not required
    writeCurrentLine(); //reg up is rising edge??? are you sure
    
    incrementReg(PC);
    
    setSignal("-REG-FUNC-RD");
    setRdId(PC);
    //writeCurrentLine(); //???
    putBustoRegMem(SP, "-REG-RD-HI");

    decrementReg(SP);
    
    setSignal("-REG-FUNC-RD");
    setRdId(PC);
    setSignal("-HL-SWAP");
    
    putBustoRegMem(SP, "-REG-RD-LO");
    
    decrementReg(SP);
    
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

    
    startInstruction(RET); //try as one instruction
    loadNextInstruction();
    initCurrentLine();
      
    incrementReg(SP);
    putMemAtRegOnBus(SP);
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO"); // if branch reg +ve edge triggered next write not required
    writeCurrentLine(); //reg up is rising edge??? are you sure
    
    incrementReg(SP);
    putMemAtRegOnBus(SP);
     setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI"); // if branch reg +ve edge triggered next write not required
    writeCurrentLine(); //maybe not needed
    
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
    endInstruction();
    showCntlMemory(RET);
}

