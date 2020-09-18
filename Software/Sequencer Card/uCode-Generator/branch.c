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

#define SOURCE_NONE 0
#define SOURCE_AC 1
#define SOURCE_TMP 2

void branch(int reg, int mode, int invert, int source) {
    //load the next two PC bytes format HHLL into branch register
    putMemAtRegOnBus(reg);

    writeCurrentLine();
    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI"); // if branch reg +ve edge triggered next write not required
    //writeCurrentLine(); step 4

    incrementReg(reg);
    //writeCurrentLine(); increment ends in writeline step 2 cleanup

    putMemAtRegOnBus(reg);

    writeCurrentLine();
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO"); // if branch reg +ve edge triggered next write not required
    //writeCurrentLine();  step 4 //reg up is rising edge??? are you sure

    incrementReg(reg);
    // writeCurrentLine(); increment ends in writeline step 2 cleanup
    clearSignal("-MEM-RD");
    writeCurrentLine();
    //initCurrentLine(); //KEN - AUG 25 remove test, try existing code
    //setSignal("-BRANCH-RD-LO"); //output branch register maybe move down after brtest sr set
    //setSignal("-BRANCH-RD-HI");
    //setSignal("-REG-FUNC-LD");//?
    //writeCurrentLine(); //Ken Probably not needed
    if (source == SOURCE_AC)
        setSignal("-AC-RD");
    if (source == SOURCE_TMP) // problem will conflict with data transfer of branch reg to PC
        setSignal("-TMP-REG-RD0");
    setSignal("-ALU-FUNC");
    setAlu(mode);
    //if (mode == 4) exit(0);
    if (invert == INVERT)
        setSignal("-AC-LD-INV");
    writeCurrentLine();
    setSignal("BR-TEST");
    writeCurrentLine(); // added 10
    clearSignal("BR-TEST"); // added 10
    if (source == SOURCE_TMP) // problem will conflict with data transfer of branch reg to PC added 10
        clearSignal("-TMP-REG-RD0"); //added 10
    writeCurrentLine(); // added 10
    setSignal("-BRANCH-RD-LO"); //output branch register maybe move down after brtest sr set
    setSignal("-BRANCH-RD-HI");
    //setSignal("-REG-FUNC-LD");//?
    writeCurrentLine(); //Ken Probably not needed
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
    branch(PC, ALUBR, NOINVERT, SOURCE_NONE);
    endInstruction();
    showCntlMemory(BR);

    startInstruction(BRINH);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALUBRIN, NOINVERT, SOURCE_NONE);
    endInstruction();
    showCntlMemory(BRINH);

    startInstruction(BRINL);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALUBRIN, INVERT, SOURCE_NONE);
    endInstruction();
    showCntlMemory(BRINL);

    startInstruction(BRZ);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(PC, ALUBRZ, NOINVERT, SOURCE_AC);
    endInstruction();
    showCntlMemory(BRZ);

    startInstruction(BRNZ);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(PC, ALUBRZ, INVERT, SOURCE_AC);
    endInstruction();
    showCntlMemory(BRNZ);

    startInstruction(BRC);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(PC, ALUCS, NOINVERT, SOURCE_NONE);
    endInstruction();
    showCntlMemory(BRC);

    for (int reg = 0; reg < 8; reg++) {
        startInstruction(BRVR | reg);
        loadNextInstruction();
        initCurrentLine();
        //setSignal("-AC-RD");
        branch(reg, ALUBR, NOINVERT, SOURCE_NONE);
        endInstruction();
        showCntlMemory(BRVR | reg);
    }

    startInstruction(BRLT);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALULT, NOINVERT, SOURCE_TMP);
    endInstruction();
    showCntlMemory(BRLT);

    startInstruction(BREQ);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALUEQ, NOINVERT, SOURCE_TMP);
    endInstruction();
    showCntlMemory(BREQ);

    startInstruction(BRGT);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALUGT, NOINVERT, SOURCE_TMP);
    endInstruction();
    showCntlMemory(BRGT);

    // JSR
    startInstruction(JSR); //try as one instruction *** possible two writeCurrentLine needed see below
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);
    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI"); // if branch reg +ve edge triggered next write not required
    //writeCurrentLine(); step 6 //maybe not needed

    incrementReg(PC);

    putMemAtRegOnBus(PC);
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO"); // if branch reg +ve edge triggered next write not required
    //writeCurrentLine(); step 6//reg up is rising edge??? are you sure
    clearSignal("-MEM-RD");
    incrementReg(PC);
    incrementReg(PC);

#define noret 1
#ifdef noret
    setSignal("-REG-FUNC-RD");
    setRdId(PC);
    //writeCurrentLine(); //???
    setSignal("-HL-SWAP");
    putBustoRegMem(SP, "-REG-RD-HI");

    decrementReg(SP);

    setSignal("-REG-FUNC-RD");
    setRdId(PC);
    clearSignal("-HL-SWAP");

    putBustoRegMem(SP, "-REG-RD-LO");

    decrementReg(SP);
#endif
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
    //setSignal("-HL-SWAP");
    writeCurrentLine();
    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI"); // if branch reg +ve edge triggered next write not required
    writeCurrentLine(); //maybe not needed
    clearSignal("-MEM-RD");
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
    endInstruction();
    showCntlMemory(RET);

}
