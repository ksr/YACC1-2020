/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: kennethrother
 *
 * Created on June 30, 2020, 9:44 AM
 */

#include <stdio.h>
#include <stdlib.h>
//#include "stdio.h"

#define HI 1
#define LO 0

#define ON 1
#define OFF 0


void cmd(char *,int);
void ret(char *,int);

void setLdId(int);
void setRdId(int);

void dataRDLO(int);
void dataWRLO(int);
void dataRDHI(int);
void dataWRHI(int);

void loadReg(int,int,int);
void readReg(int,int,int);


int main()
{
	loadReg(1,LO,100);
	readReg(1,LO,100);
	loadReg(1,HI,101);
	readReg(1,HI,101);

}


void loadReg(int reg,int hl, int val){
	cmd("REG-FUNC-LD",1);
	setLdId(reg);
        if(hl == LO)
		dataWRLO(val);
	else
		dataWRHI(val);
	cmd("REG-FUNC-LD",0);
}	

void readReg(int reg,int hl, int val){
	cmd("REG-FUNC-RD",1);
	setRdId(reg);
        if(hl == LO)
	        dataRDLO(val);
	 else
        	dataRDHI(val);
	cmd("REG-FUNC-RD",0);
}	
void dataRDLO(int val){
	cmd("BUS-RD",0);
	cmd("REG-RD-LO",1);
	ret("RDATAL",val);
	cmd("REG-RD-LO",0);
}
void dataRDHI(int val){
	cmd("BUS-RD",0);
	cmd("REG-RD-HI",1);
	ret("RDATAH",val);
	cmd("REG-RD-HI",0);
}

void dataWRLO(int val){
	cmd("BUS-WR",0);
	cmd("WDATA",val);
	cmd("REG-LD-LO",1);
	cmd("REG-LD-LO",0);
}

void dataWRHI(int val){
	cmd("BUS-WR",0);
	cmd("WDATA",val<<8);
	cmd("REG-LD-HI",1);
	cmd("REG-LD-HI",0);
}
	
void setRdId(int reg) {
	cmd("REG-BRD-RD-ID",reg & 0x1000);
	cmd("REG-RD-ID0",reg & 0x0001);
	cmd("REG-RD-ID1",reg & 0x0010);
	cmd("REG-RD-ID2",reg & 0x0100);
}

void setLdId(int reg) {
	cmd("REG-BRD-LD-ID",reg & 0x1000);
	cmd("REG-LD-ID0",reg & 0x0001);
	cmd("REG-LD-ID1",reg & 0x0010);
	cmd("REG-LD-ID2",reg & 0x0100);
}

void cmd(char *cmd, int param) {
	printf("%s:%x#\n",cmd,param);
}

void ret(char *cmd, int param) {
	printf("%s:0#%x!\n",cmd,param);
}


