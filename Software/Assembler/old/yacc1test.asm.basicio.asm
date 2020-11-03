
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
TIL311:      EQU 080H

CNTL-PORT:    EQU "P0"
DATAPORT:    EQU "P1"

        ORG 0f000h
        BR eprom
        ORG 0f003h

eprom:  ON

;
; switches, LEDs, TIL311
;
        OUTI  P0,(SWITCHLED!TIL311)
        OUTI  P1,00H
        OUTI  P1,055H
        OUTI  P1,0AAH
        OUTI  P1,00H
;
; reading switches add 1 and > LEDS, add 2 > TIL 311
;

uloop:
        OUTI  P0,(SWITCHLED)
        INP   P1
;
        ADDI  01H
        OUTI  P0,(SWITCHLED)
        OUTA  P1
;
        ADDI  01H
        OUTI  P0,(TIL311)
        OUTA  P1
;
        OFF
;
;LCD
;
        OUTI P0,(LCDENABLE)

        MVIW R3,1fFFh
delay0:
        DECR R3
        MVRHA R3
        BRNZ delay0

        OUTI P1,3CH

        MVIW R3,1fFFh
delay1:
        DECR R3
        MVRHA R3
        BRNZ delay1

        OUTI P1,01H

        MVIW R3,1fFFh
delay2:
        DECR R3
        MVRHA R3
        BRNZ delay2

        OUTI P1,0FH

        MVIW R3,1fFFh
delay3:
        DECR R3
        MVRHA R3
        BRNZ delay3

        OUTI P0,(LCDENABLE!LCDREGISTER)

        MVIW R3,1fFFh
delay4:
        DECR R3
        MVRHA R3
        BRNZ delay4

        OUTI P1,'A'

delay5:
                DECR R3
                MVRHA R3
                BRNZ delay5

                OUTI P1,'B'

;
; SERIAL OUT
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

        MVIW  R1,codes

        MVIB  R2,10

uartloop:
        LDAVR R1
        OUTA  P1
        DECR  R2
        INCR  R1

        MVIW R3,1fFFh
uartdelay:
        DECR R3
        MVRHA R3
        BRNZ uartdelay

        MVRLA R2
        BRNZ uartloop


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

codes:  DB 'G','H','I','J','H','K','L','A','B','C','D','E'
