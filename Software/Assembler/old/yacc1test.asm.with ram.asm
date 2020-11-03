
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

        OUTI  P0,(UARTA3!UARTCS)
        OUTI  P1,080H

        OUTI  P0,(UARTA0!UARTCS)
        OUTI  P1,12

        OUTI  P0,(UARTA1!UARTCS)
        OUTI  P1,00

        OUTI  P0,(UARTA3!UARTCS)
        OUTI  P1,03H

        OUTI  P0,UARTCS

        MVIW  R1,0010h
        LDIVR R1,'A'
        INCR  R1
        LDIVR R1,'C'
        INCR  R1
        LDIVR R1,'E'
        INCR  R1
        LDIVR R1,'G'
        INCR  R1
        LDIVR R1,'I'

        MVIW  R1,0010h
        MVIB  R2,5

uloop:
        LDAVR R1
        ADDI  1
        OUTA  P1

        DECR  R2
        INCR   R1

        MVIW R3,02FFh
loop1:  DECR R3
        MVRHA R3
        BRNZ loop1

        MVRLA R2
        BRNZ uloop

onoffloop:
        ON
;
        MVIW R3,02FFh
onloop:
        DECR R3
        MVRHA R3
        BRNZ onloop

        OFF
        MVIW R3,02FFh
offloop:
        DECR R3
        MVRHA R3
        BRNZ offloop

        br onoffloop
        on
codes:  DB 'A','B','C','D','E'
        LDAI 2
