/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   codes.h
 * Author: ksr77
 *
 * Created on October 21, 2016, 2:57 PM
 *
 * FIX IO pin mapping
 */

#ifndef CODE_H
#define CODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
    struct signal {
        char *name;
        int chip;
        int port;
        int bit;
    };
    
 */   

#ifdef __cplusplus
}
#endif

#endif /* CODE_H */

void clearCurrentLine();
void loadNextInstruction();
void initCurrentLine();
void putMemAtRegOnBus(int reg);
void putBustoRegMem(int,char *);
void cmd(char *str, int reg);
void bitOn();
void bitOff();
void writeCurrentLine();
void dumpCntlMemory();
void startInstruction(int instruction);
void endInstruction();
void startUcodeBlock(int index);
void endUcodeBlock();
void clearCntlMemory();
void showCntlMemory(int instruction);
void basicTest();
void showCurrentLine();
void setRdId(int);
void setLdId(int);
void setAddrId(int);
void setIo(int);
void setAlu(int);
void setSignal(char *);
void clearSignal(char *);
void incrementReg(int);
void decrementReg(int);