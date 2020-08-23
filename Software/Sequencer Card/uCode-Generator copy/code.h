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

#ifndef CODES_H
#define CODES_H

#ifdef __cplusplus
extern "C" {
#endif

    struct signal {
        char *name;
        int chip;
        int port;
        int bit;
    };
    
    

#ifdef __cplusplus
}
#endif

#endif /* CODES_H */

void clearCurrentLine();
void putMemAtRegOnBus(int reg);
void cmd(char *str, int reg);
void bitOn();
void bitOff();
void writeCurrentLine();
void dumpCntlMemory();
void startInstruction(int instruction);
void startUcodeBlock(int index);
void clearCntlMemory();
void showCntlMemory(int instruction);
void basicTest();
void showCurrentLine();