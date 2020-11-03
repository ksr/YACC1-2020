
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

CNTL-PORT:   EQU "P0"
DATAPORT:    EQU "P1"

        ORG 0f000h
        BR eprom
        ORG 0f003h

eprom:
         MVIW R1,1000h
testloop:
         JSR onoffloop
         BR testloop
 ;
 ; SERIAL OUT
 ;

 uarttest:
         OUTI  P0,(UARTA3!UARTCS)
         OUTI  P1,080H

         OUTI  P0,(UARTA0!UARTCS)
         OUTI  P1,12

         OUTI  P0,(UARTA1!UARTCS)
         OUTI  P1,00

         OUTI  P0,(UARTA3!UARTCS)
         OUTI  P1,03H

         OUTI  P0,UARTCS
start:

offw:   BRINL offw
        MVIW R3,00FFh
delaya:
        DECR R3
        MVRHA R3
        BRNZ delaya
        on

onw:   BRINH onw
       MVIW R3,00FFh
delayb:
        DECR R3
        MVRHA R3
        BRNZ delayb
        off

        BRVR R2

;
; switches, LEDs, TIL311
;
basic:
        OUTI  P0,(SWITCHLED!TIL311)
        OUTI  P1,00H
        OUTI  P1,055H
        OUTI  P1,0AAH
        BR    start
;
; reading switches add 1 and > LEDS, add 2 > TIL 311
;
switchtest:
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
        BR    start
;
;LCD
;
lcdtest:
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

        BR start

addtest:

      OUTI  P0,(SWITCHLED)
      INP   P1
      ADDI  001H
      OUTA  P1
      BRC   con
      BR    start
con:  ON
      BR    start

ortest:
      OUTI  P0,(SWITCHLED)
      INP   P1
      ORI  055H
      OUTA  P1
      BRC   con
      BR    start

andtest:
      OUTI  P0,(SWITCHLED)
      INP   P1
      ANDI  055H
      OUTA  P1
      BRC   con
      BR    start

xortest:
     OUTI  P0,(SWITCHLED)
     INP   P1
     XORI  055H
     OUTA  P1
     BRC   con
     BR    start

shltest:
   OUTI  P0,(SWITCHLED)
   INP   P1
   XORI  055H
   OUTA  P1
   BRC   con
   BR    start

shrtest:
    OUTI  P0,(SWITCHLED)
    INP   P1
    XORI  055H
    OUTA  P1
    BRC   con
    BR    start

rshltest:
   OUTI  P0,(SWITCHLED)
   INP   P1
   XORI  001H
   OUTA  P1
   BRC   con
   BR    start

rshrtest:
  OUTI  P0,(SWITCHLED)
  INP   P1
  XORI  001H
  OUTA  P1
  BRC   con
  BR    start

;
; SERIAL OUT
;

uarttest:
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

uartOut:
        OUTA  P1

        MVIW R3,1fFFh
uartdelay:
        DECR R3
        MVRHA R3
        BRNZ uartdelay

        RET


onoffloop:
        ON
;
        MVIW R3,00FFh
onloop:
        DECR R3
        MVRHA R3
        BRNZ onloop

        OFF
        MVIW R3,00FFh
offloop:
        DECR R3
        MVRHA R3
        BRNZ offloop

        RET


jumps:
        DW basic,switchtest,lcdtest
        DW addtest,addtest,addtest,ortest,ortest,ortest,andtest,andtest,andtest
        DW xortest,xortest,xortest,shltest,shltest,shltest,shrtest,shrtest,shrtest,uarttest

codes:  DB 'G','H','I','J','H','K','L','A','B','C','D','E'
