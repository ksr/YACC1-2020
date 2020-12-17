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
    setSignal("-BRANCH-RD"); //output branch register maybe move down after brtest sr set
    //setSignal("-BRANCH-RD-HI");
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
    branch(PC, ALUIN, NOINVERT, SOURCE_NONE);
    endInstruction();
    showCntlMemory(BRINH);

    startInstruction(BRINL);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALUIN, INVERT, SOURCE_NONE);
    endInstruction();
    showCntlMemory(BRINL);

    startInstruction(BRZ);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(PC, ALUZ, NOINVERT, SOURCE_AC);
    endInstruction();
    showCntlMemory(BRZ);

    startInstruction(BRNZ);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(PC, ALUZ, INVERT, SOURCE_AC);
    endInstruction();
    showCntlMemory(BRNZ);

    startInstruction(BR16Z);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(PC, ALU16Z, NOINVERT, SOURCE_AC);
    endInstruction();
    showCntlMemory(BR16Z);

    startInstruction(BR16NZ);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(PC, ALU16Z, INVERT, SOURCE_AC);
    endInstruction();
    showCntlMemory(BR16NZ);

    startInstruction(BRC);
    loadNextInstruction();
    initCurrentLine();
    //setSignal("-AC-RD");
    branch(PC, ALUCS, NOINVERT, SOURCE_NONE);
    endInstruction();
    showCntlMemory(BRC);

    startInstruction(BRDEV);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALUBR, NOINVERT, SOURCE_NONE);
    endInstruction();
    showCntlMemory(BRDEV);

    for (int reg = 0; reg < 8; reg++) {
        // When reg != pc should reg autoincrement ?
        startInstruction(BRVR | reg);
        loadNextInstruction();
        initCurrentLine();
        //setSignal("-AC-RD");
        branch(reg, ALUBR, NOINVERT, SOURCE_NONE);
        endInstruction();
        showCntlMemory(BRVR | reg);
    }

    /* not implemented 
        for (int reg = 0; reg < 8; reg++) {
            startInstruction(BRUR | reg);
            loadNextInstruction();
            initCurrentLine();

            setRdId(reg);
            setSignal("-REG-FUNC-RD");
            writeCurrentLine();

            setSignal("-REG-RD-HI"); //load branch register
            writeCurrentLine();
            setSignal("BRANCH-LD-HI");
            writeCurrentLine();
            clearSignal("BRANCH-LD-HI");
            writeCurrentLine();
            clearSignal("-REG-RD-HI");
            writeCurrentLine();

            setSignal("-REG-RD-LO"); //load branch register
            writeCurrentLine();
            setSignal("BRANCH-LD-LO");
            writeCurrentLine();
            clearSignal("BRANCH-LD-LO");
            writeCurrentLine();
            clearSignal("-REG-RD-LO"); //load branch register 
            writeCurrentLine();

            endInstruction();
            showCntlMemory(BRUR | reg);
  
        }
     */

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

    startInstruction(BRNEQ);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALUEQ, INVERT, SOURCE_TMP);
    endInstruction();
    showCntlMemory(BRNEQ);


    startInstruction(BRGT);
    loadNextInstruction();
    initCurrentLine();
    branch(PC, ALUGT, NOINVERT, SOURCE_TMP);
    endInstruction();
    showCntlMemory(BRGT);

    // ORIG JSR one increment PC removed
    // JSR
    //
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
    //incrementReg(PC);

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
    setSignal("-BRANCH-RD"); //output branch register
    //setSignal("-BRANCH-RD-HI");

    setSignal("-ALU-FUNC");
    setAlu(ALUBR);
    setSignal("BR-TEST");
    setLdId(PC);
    setSignal("-REG-FUNC-LD");
    writeCurrentLine();

    clearSignal("BR-TEST"); //ken added oct 10, probably needed
    writeCurrentLine();

    setSignal("REG-LD-LO"); //load branch register
    setSignal("REG-LD-HI"); //FIX on new card ???
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    clearSignal("REG-LD-HI"); //Fix on new card  ???
    writeCurrentLine();

    endInstruction();
    showCntlMemory(JSR);



#ifdef NEWJSR    
    //
    // JSR
    //
    startInstruction(JSR); //try as one instruction *** possible two writeCurrentLine needed see below
    loadNextInstruction();
    initCurrentLine();

    setAddrId(PC);
    setSignal("-VMA");
    setSignal("-MEM-RD");
    writeCurrentLine();

    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI");
    writeCurrentLine();

    setRdId(PC);
    setSignal("-REG-FUNC-RD");

    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");
    writeCurrentLine();

    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO"); // if branch reg +ve edge triggered next write not required
    clearSignal("-MEM-RD");
    writeCurrentLine();

    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");
    writeCurrentLine();

    setRdId(PC);
    //writeCurrentLine(); //???
    setSignal("-HL-SWAP");
    putBustoRegMem(SP, "-REG-RD-HI");

    setRdId(SP);
    writeCurrentLine();
    setSignal("-REG-DN");
    writeCurrentLine();
    clearSignal("-REG-DN");
    writeCurrentLine();

    setRdId(PC);
    clearSignal("-HL-SWAP");
    putBustoRegMem(SP, "-REG-RD-LO");

    setRdId(SP);
    setSignal("-REG-FUNC-RD");
    writeCurrentLine();
    setSignal("-REG-DN");
    writeCurrentLine();
    clearSignal("-REG-DN");

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
    writeCurrentLine(); //ken oct 5  maybe not needed 

    endInstruction();
    showCntlMemory(JSR);
#endif

    //
    // JSRUR
    //
#define JSRURDEF    
#ifdef JSRURDEF    
    startInstruction(JSRUR);

    loadNextInstruction();

    //#define customloadnextinstruction
#ifdef customloadnextinstruction//load next instruciton
    //initCurrentLine(); // added oct 8 // not needed done in start instruction
    //putMemAtRegOnBus(PC);
    setAddrId(PC);
    setSignal("-VMA");
    setSignal("-MEM-RD");
    setRdId(PC);
    setSignal("-REG-FUNC-RD");
    writeCurrentLine(); // not needed if if reg loads on rising edge?
    setSignal("LD-INS-REG"); // rising or falling?
    writeCurrentLine();
    clearSignal("LD-INS-REG");
    writeCurrentLine(); // -reg-up is rising edge

    //incrementReg(PC); // load next instruction

    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");
    writeCurrentLine();
#endif
    //load  operand register with register that point to address to sub to

    setSignal("-MEM-RD");

    writeCurrentLine();
    setSignal("OPERAND-CLK");
    writeCurrentLine();
    setSignal("-REG-FUNC-RD");
    clearSignal("OPERAND-CLK");
    writeCurrentLine();
    clearSignal("-MEM-RD");
    //writeCurrentLine();

    // increment PC past  operand
    setSignal("-REG-UP");
    writeCurrentLine();
    clearSignal("-REG-UP");

    setSignal("-2-BYTE-OPERAND-SEL");
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-RD-HI");
    setSignal("-HL-SWAP");
    writeCurrentLine();

    setSignal("BRANCH-LD-HI");
    writeCurrentLine();
    clearSignal("BRANCH-LD-HI");
    writeCurrentLine();

    clearSignal("-REG-RD-HI");
    clearSignal("-HL-SWAP");
    setSignal("-REG-RD-LO");
    writeCurrentLine();
    setSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("BRANCH-LD-LO");
    writeCurrentLine();
    clearSignal("-REG-RD-LO");
    clearSignal("-2-BYTE-OPERAND-SEL");
    writeCurrentLine();

    // save pc to stack        
    setSignal("-HL-SWAP");
    setAddrId(SP);
    setRdId(PC);
    setSignal("-REG-RD-HI");
    writeCurrentLine();
    setSignal("-MEM-WR");
    writeCurrentLine();
    clearSignal("-MEM-WR");
    writeCurrentLine();

    setRdId(SP);
    clearSignal("-REG-RD-HI");
    setSignal("-REG-FUNC-RD");
    writeCurrentLine();

    //decrementReg(SP);
    setSignal("-REG-DN");
    writeCurrentLine();
    clearSignal("-REG-DN");
    writeCurrentLine();

    setSignal("-REG-FUNC-RD");
    setRdId(PC);
    clearSignal("-HL-SWAP");

    //putBustoRegMem(SP, "-REG-RD-LO");
    setSignal("-REG-RD-LO");
    writeCurrentLine();
    setSignal("-MEM-WR");
    writeCurrentLine();
    clearSignal("-MEM-WR");
    writeCurrentLine();
    clearSignal("-REG-RD-LO");
    setRdId(SP);
    writeCurrentLine();

    //decrementReg(SP);

    setSignal("-REG-DN");
    writeCurrentLine();
    clearSignal("-REG-DN");
    clearSignal("-REG-FUNC-RD"); //suspect , glitch? maybe move below writeline
    writeCurrentLine();


    setSignal("-BRANCH-RD"); //output branch register
    //setSignal("-BRANCH-RD-HI");

    setSignal("-ALU-FUNC");
    setAlu(ALUBR);
    setSignal("BR-TEST");
    setLdId(PC);
    setSignal("-REG-FUNC-LD");

    writeCurrentLine();

    clearSignal("BR-TEST"); //ken added oct 10, probably needed

    setSignal("REG-LD-LO"); //load branch register
    setSignal("REG-LD-HI"); //FIX on new card ???
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    clearSignal("REG-LD-HI"); //Fix on new card  ???
    //writeCurrentLine(); //KEN OCT 6 removed untested

    endInstruction();
    showCntlMemory(JSRUR);
#endif

    //
    //RET
    //
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

    setSignal("-BRANCH-RD"); //output branch register
    //setSignal("-BRANCH-RD-HI");
    setSignal("-ALU-FUNC");
    setAlu(ALUBR);
    setSignal("BR-TEST");
    setLdId(PC);
    setSignal("-REG-FUNC-LD");
    writeCurrentLine();
    setSignal("REG-LD-LO"); //load branch register
    setSignal("REG-LD-HI"); //FIX on new card ???
    writeCurrentLine();

    clearSignal("BR-TEST"); //ken added oct 10, this is why branch after return fails

    clearSignal("REG-LD-LO");
    clearSignal("REG-LD-HI"); //Fix on new card  ???
    writeCurrentLine();
    endInstruction();
    showCntlMemory(RET);

    //
    //push pop
    //
    startInstruction(PUSH); //try as one instruction *** possible two writeCurrentLine needed see below
    loadNextInstruction();
    initCurrentLine();

    setSignal("-ALU-FUNC");
    setAlu(ALUDATA);
    writeCurrentLine();

    putBustoRegMem(SP, "-AC-RD");

    decrementReg(SP);
    endInstruction();
    showCntlMemory(PUSH);



    startInstruction(POP); //try as one instruction *** possible two writeCurrentLine needed see below
    loadNextInstruction();
    initCurrentLine();

    incrementReg(SP);
    putMemAtRegOnBus(SP);

    setSignal("-ALU-FUNC");
    writeCurrentLine();
    setSignal("-AC-LD");
    writeCurrentLine();
    clearSignal("-AC-LD");
    writeCurrentLine();

    endInstruction();
    showCntlMemory(POP);

    //
    //pushr popr
    //
    startInstruction(PUSHR); //try as one instruction *** possible two writeCurrentLine needed see below
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);
    setSignal("OPERAND-CLK");
    writeCurrentLine();
    clearSignal("OPERAND-CLK");
    writeCurrentLine();
    clearSignal("-MEM-RD");
    //writeCurrentLine();
    incrementReg(PC);

    setSignal("-2-BYTE-OPERAND-SEL");
    setSignal("-REG-FUNC-RD");
    setSignal("-HL-SWAP");
    setSignal("-REG-RD-HI");
    writeCurrentLine();
    setSignal("-TMP-REG-LD1");
    writeCurrentLine();
    clearSignal("-TMP-REG-LD1");
    writeCurrentLine();
    clearSignal("-2-BYTE-OPERAND-SEL");
    writeCurrentLine();
    putBustoRegMem(SP, "-TMP-REG-RD1");
    decrementReg(SP);

    clearSignal("-HL-SWAP");
    setSignal("-2-BYTE-OPERAND-SEL");
    setSignal("-REG-FUNC-RD");
    setSignal("-REG-RD-LO");
    writeCurrentLine();
    setSignal("-TMP-REG-LD1");
    writeCurrentLine();
    clearSignal("-TMP-REG-LD1");
    writeCurrentLine();
    clearSignal("-2-BYTE-OPERAND-SEL");
    writeCurrentLine();
    putBustoRegMem(SP, "-TMP-REG-RD1");
    decrementReg(SP);

    endInstruction();
    showCntlMemory(PUSHR);


    startInstruction(POPR); //try as one instruction *** possible two writeCurrentLine needed see below
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

    incrementReg(SP);
    putMemAtRegOnBus(SP);
    setSignal("-2-BYTE-OPERAND-SEL");
    writeCurrentLine();
    setSignal("-REG-FUNC-LD");

    writeCurrentLine();
    setSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    writeCurrentLine();
    clearSignal("-2-BYTE-OPERAND-SEL");
    writeCurrentLine();

    incrementReg(SP);
    setSignal("-2-BYTE-OPERAND-SEL");
    writeCurrentLine();
    putMemAtRegOnBus(SP);
    setSignal("-REG-FUNC-LD");
    setSignal("-HL-SWAP");
    writeCurrentLine();
    setSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("REG-LD-HI");
    writeCurrentLine();
    clearSignal("-REG-FUNC-LD");
    writeCurrentLine();

    endInstruction();
    showCntlMemory(POPR);

    //
    // IADDR - Set interrupt vector
    //
    startInstruction(IADDR);
    loadNextInstruction();
    initCurrentLine();

    putMemAtRegOnBus(PC);
    writeCurrentLine();
    setSignal("INT-LD-HI");
    writeCurrentLine();
    clearSignal("INT-LD-HI"); // if branch reg +ve edge triggered next write not required
    writeCurrentLine();

    incrementReg(PC);
    //writeCurrentLine(); increment ends in writeline step 2 cleanup

    putMemAtRegOnBus(PC);

    writeCurrentLine();
    setSignal("INT-LD-LO");
    writeCurrentLine();
    clearSignal("INT-LD-LO"); // if branch reg +ve edge triggered next write not required
    //writeCurrentLine();  step 4 //reg up is rising edge??? are you sure
    writeCurrentLine();
    incrementReg(PC);
    // writeCurrentLine(); increment ends in writeline step 2 cleanup
    clearSignal("-MEM-RD");
    writeCurrentLine();

    endInstruction();
    showCntlMemory(IADDR);


    //
    // Interrupt handler 
    //
    startInstruction(INT);
    //putMemAtRegOnBus(PC);
    //setSignal("LD-INS-REG"); // rising or falling?
    //writeCurrentLine();
    //clearSignal("LD-INS-REG");
    //writeCurrentLine(); // -reg-up is rising edge
    //incrementReg(PC);
    //clearSignal("-MEM-RD"); //STEP 9 DO NOT LEAVE DATA BUS DRIVEN WITH INSTRUCTION FETCHED FROM MEM  
    
    loadNextInstruction();
    initCurrentLine();

    setSignal("INT-START");
    writeCurrentLine();
    clearSignal("INT-START");
    writeCurrentLine();
    decrementReg(PC);   // 
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

    setSignal("-INT-JMP"); //output isr address register
    clearSignal("-REG-FUNC-RD");
    setSignal("-ALU-FUNC");
    setAlu(ALUBR);
    setSignal("BR-TEST");
    setLdId(PC);
    setSignal("-REG-FUNC-LD");
    writeCurrentLine();

    clearSignal("BR-TEST"); //ken added oct 10, probably needed
    writeCurrentLine();

    setSignal("REG-LD-LO"); //load branch register
    setSignal("REG-LD-HI"); //FIX on new card ???
    writeCurrentLine();
    clearSignal("REG-LD-LO");
    clearSignal("REG-LD-HI"); //Fix on new card  ???
    writeCurrentLine();

    endInstruction();
    showCntlMemory(INT);

    //
    //IRET - return from interrupt service routine
    //
    startInstruction(IRET); //try as one instruction
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

    setSignal("-BRANCH-RD"); //output branch register
    //setSignal("-BRANCH-RD-HI");
    setSignal("-ALU-FUNC");
    setAlu(ALUBR);
    setSignal("BR-TEST");
    setLdId(PC);
    setSignal("-REG-FUNC-LD");
    writeCurrentLine();
    setSignal("REG-LD-LO"); //load branch register
    setSignal("REG-LD-HI"); //FIX on new card ???
    writeCurrentLine();

    clearSignal("BR-TEST"); //ken added oct 10, this is why branch after return fails

    clearSignal("REG-LD-LO");
    clearSignal("REG-LD-HI"); //Fix on new card  ???
    writeCurrentLine();
    setSignal("INT-EN");
    writeCurrentLine();
    clearSignal("INT-EN");
    writeCurrentLine();
    endInstruction();
    showCntlMemory(IRET);

    //
    // enable interrupts
    //
    startInstruction(INTE); //try as one instruction
    loadNextInstruction();
    initCurrentLine();

    setSignal("INT-EN");
    writeCurrentLine();
    clearSignal("INT-EN");
    writeCurrentLine();
    endInstruction();
    showCntlMemory(INTE);
 
    //
    // Disable interrupts 
    //
    startInstruction(INTD); //try as one instruction
    loadNextInstruction();
    initCurrentLine();

    setSignal("INT-START");
    writeCurrentLine();
    clearSignal("INT-START");
    writeCurrentLine();
    endInstruction();
    showCntlMemory(INTD);
}
