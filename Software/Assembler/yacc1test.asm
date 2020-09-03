
UARTA0:       EQU 000h
UARTA1:       EQU 008h
UARTA2:       EQU 010h
UARTA3:       EQU 018h
UARTA4:       EQU 020h
UARTA5:       EQU 028h
UARTA6:       EQU 030h
UARTA7:       EQU 038h

SWITCHLED:   EQU 001H
LCDENABLE:   EQU 002H
LCDREGISTER: EQU 004H
UARTCS:      EQU 040H

CNTL-PORT:    EQU "P0"
DATAPORT:    EQU "P1"

        ORG 0f000h
        BR eprom
        ORG 0f003h

eprom:  ON
;
;Setup UART
;

        OUTI  P0,(SWITCHLED)
        OUTI  P1,0OH

        MVIB  R1,00h

uloop:
;        MVRLA R1
        INP   P1
        OUTA  P1
        INCR  R1
        BR uloop


        MVIW R3,02FFh
loop1:  DECR R3
        MVRHA R3
        BRNZ loop1

        MVRLA R1
        BRNZ uloop

onoffloop:
        ON
;
        MVIW R3,8fFFh
onloop:
        DECR R3
        MVRHA R3
        BRNZ onloop

        OFF
        MVIW R3,8fFFh
offloop:
        DECR R3
        MVRHA R3
        BRNZ offloop

        br onoffloop
