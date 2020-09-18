
UARTA0:       EQU 000h
UARTA1:       EQU 008h
UARTA2:       EQU 010h
UARTA3:       EQU 018h
UARTA4:       EQU 020h
UARTA5:       EQU 028h
UARTA6:       EQU 030h
UARTA7:       EQU 038h

SWITCHLED:    EQU 001H
LCDENABLE:    EQU 002H
LCDREGISTER:  EQU 004H
UARTCS:       EQU 040H
TIL311:       EQU 080H

CNTL-PORT:    EQU "P0"
DATAPORT:     EQU "P1"



         LDAI 081h
         RSHR
         LDAI 081h
         RSHL

         
         ORG 0f000h
         BR eprom
         ORG 0f003h
eprom:
;
; Setup Stack
;
         MVIW R1,1000h
;
; SERIAL OUT SETUP
;
         OUTI  P0,(UARTA3!UARTCS)
         OUTI  P1,080H

         OUTI  P0,(UARTA0!UARTCS)
         OUTI  P1,12

         OUTI  P0,(UARTA1!UARTCS)
         OUTI  P1,00

         OUTI  P0,(UARTA3!UARTCS)
         OUTI  P1,03H

;        JSR blink


;
; Main
;
         MVIW R2,hello
         JSR stringout
         JSR cmptest
         JSR shrtest
         JSR shltest
         JSR rshrtest
         JSR rshltest




;
; Tests
;

;
; shift tests
;
shltest:
         MVIW   R2,SHIFT_LEFT
         JSR    stringout
         MVIB   R2,10
shlloop:
         JSR blink
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         SHL
         OUTA  P1

         DECR R2
         MVRLA R2
         BRNZ shlloop
         RET

shrtest:
          MVIW   R2,SHIFT_RIGHT
          JSR    stringout
          MVIB   R2,10
shrloop:
          JSR blink
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          SHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ shrloop
          RET

rshltest:
         MVIW   R2,RSHIFT_LEFT
         JSR    stringout
         MVIB   R2,10
rshlloop:
         JSR blink
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         SHL
         OUTA  P1

         DECR R2
         MVRLA R2
         BRNZ rshlloop
         RET

rshrtest:
          MVIW   R2,RSHIFT_RIGHT
          JSR    stringout
          MVIB   R2,10
rshrloop:
          JSR blink
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          SHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ rshrloop
          RET
;
; Compare Tests/compare input switches to 0x55 10 times
;
cmptest:
          MVIW   R2,COMPARE_STRING
          JSR    stringout
          MVIB   R2,10
          LDTI   055H
cmploop:
          JSR blink
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          BRGT OUTGT
          BREQ OUTEQ
          BRLT OUTLT
;Should not happen
          LDAI '?'
          BR cmpres
OUTGT:
          LDAI 'G'
          BR cmpres
OUTEQ:
          LDAI 'E'
          BR cmpres
OUTLT:
          LDAI 'L'
          BR cmpres

cmpres:
          JSR uartout
          DECR R2
          MVRLA R2
          BRNZ cmploop
          RET

;
; reading switches into accumulator
;
switchin:
        OUTI  P0,(SWITCHLED)
        INP   P1
        RET
;
;
; output accumulator to LEDS & Hex displays
;
ledout:
        OUTI  P0,(SWITCHLED!TIL311)
        OUTA  P1
        RET
;
; Output null string pointed to by R2 to UART then send CR and LF
;
stringout:
sloop:
        LDAVR R2
        BRZ sloopdone
        JSR uartout
        INCR R2
        BR sloop
;
; send CR and LF
;
sloopdone:
        LDAI 0ah
        JSR uartout
        LDAI 0dh
        JSR uartout
        RET
;
; output accumulator to UART
;
uartout:
        OUTI  P0,UARTCS
        OUTA  P1

        MVIW R3,01FFh
uartdelay:
        DECR R3
        MVRHA R3
        BRNZ uartdelay
        RET
;
; toggle input switch
;
switchtoggle:

offw:   BRINL offw
        MVIW R3,01FFh
delaya:
        DECR R3
        MVRHA R3
        BRNZ delaya
        ON

onw:   BRINH onw
       MVIW R3,01FFh
delayb:
        DECR R3
        MVRHA R3
        BRNZ delayb
        OFF
        RET
;
; blink LED
;
blink:
        ON
;
        MVIW R3,04FFh
onloop:
        DECR R3
        MVRHA R3
        BRNZ onloop

        OFF
        MVIW R3,04FFh
offloop:
        DECR R3
        MVRHA R3
        BRNZ offloop
        RET

codes:  DB 'G','H','I','J','H','K','L','A','B','C','D','E'
hello:  DB 0ah,0dh,"YACC 2020: hello world  ",0
COMPARE_STRING: DB "Compare Tests",0
SHIFT_LEFT: DB "Shift Left",0
SHIFT_RIGHT: DB "Shift Right",0
RSHIFT_LEFT: DB "Ring Shift Left",0
RSHIFT_RIGHT: DB "Ring Shift Right",0,0,0

;
;LCD
;
xlcdtest:
        OUTI P0,(LCDENABLE)

        MVIW R3,1fFFh
xdelay0:
        DECR R3
        MVRHA R3
        BRNZ xdelay0

        OUTI P1,3CH

        MVIW R3,1fFFh
xdelay1:
        DECR R3
        MVRHA R3
        BRNZ xdelay1

        OUTI P1,01H

        MVIW R3,1fFFh
xdelay2:
        DECR R3
        MVRHA R3
        BRNZ xdelay2

        OUTI P1,0FH

        MVIW R3,1fFFh
xdelay3:
        DECR R3
        MVRHA R3
        BRNZ xdelay3

        OUTI P0,(LCDENABLE!LCDREGISTER)

        MVIW R3,1fFFh
xdelay4:
        DECR R3
        MVRHA R3
        BRNZ xdelay4

        OUTI P1,'A'

xdelay5:
        DECR R3
        MVRHA R3
        BRNZ xdelay5

        OUTI P1,'B'



xaddtest:
      OUTI  P0,(SWITCHLED)
      INP   P1
      ADDI  001H
      OUTA  P1

xortest:
      OUTI  P0,(SWITCHLED)
      INP   P1
      ORI  055H
      OUTA  P1

xandtest:
      OUTI  P0,(SWITCHLED)
      INP   P1
      ANDI  055H
      OUTA  P1

xxortest:
     OUTI  P0,(SWITCHLED)
     INP   P1
     XORI  055H
     OUTA  P1
