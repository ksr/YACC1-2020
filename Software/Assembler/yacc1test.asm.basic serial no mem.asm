
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

        OUTI  P0,(UARTA3!UARTCS)
        OUTI  P1,080H

        OUTI  P0,(UARTA0!UARTCS)
        OUTI  P1,12

        OUTI  P0,(UARTA1!UARTCS)
        OUTI  P1,00

        OUTI  P0,(UARTA3!UARTCS)
        OUTI  P1,03H

        OUTI  P0,UARTCS

        MVIB  R1,'A'
        MVIB  R2,26

uloop:
        MVRLA R1
        OUTA  P1
        
        DECR  R2
        INCR   R1

        MVIW R3,0FFFh
loop1:  DECR R3
        MVRHA R3
        BRNZ loop1

        MVRLA R2
        BRNZ uloop

aloop:
        OUTI  P1,'1'
        BR  aloop
