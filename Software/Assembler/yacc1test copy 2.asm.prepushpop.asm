
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

NOMODE:       EQU 0
EXAMINEMODE:  EQU 1
DUMPMODE:     EQU 2


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

        MVIB R6,NOMODE


;
; Main
;
         MVIW R2,hello
         JSR stringout

         JSR pushpop

         MVIW R2,hello
         JSR stringout

         MVIW R3,0f000h
         JSR showaddr
         JSR show16
         JSR cmdloop
         JSR subtest
         JSR cmptest
         JSR shrtest
         JSR shltest
         JSR rshrtest
         JSR rshltest
         JSR pshltest
         JSR cshltest
         JSR cshrtest
         JSR accumtest


;
; Tests
;

;
; push pop tests
;
pushpop:
         MVIW   R2,PUSHPOPMSG
         JSR    stringout

         MVIB   R2,3
ppenterloop:
         JSR blink
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         push

         DECR R2
         MVRLA R2
         BRNZ ppenterloop

         MVIB   R2,3
ppdisloop:
        JSR blink
        JSR switchtoggle
        OUTI P0,(SWITCHLED)
        POP
        OUTA  P1
        DECR R2
        MVRLA R2
        BRNZ ppdisloop

        RET
;
; accumulator register Tests
;
accumtest:
         MVIW   R2,accumtests
         JSR    stringout
         MVIB   R2,10
accloop:
         JSR blink
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         MVARH R3
         INCR R3
         MVRHA R3
         OUTA P1
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         MVARL R3
         INCR R3
         MVRLA R3
         OUTA P1

         DECR R2
         MVRLA R2
         BRNZ accloop
         RET
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
         RSHL
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
          RSHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ rshrloop
          RET

pshltest:
          MVIW   R2,PSHIFT_LEFT
          JSR    stringout
          MVIB   R2,10
pshlloop:
          JSR blink
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          RSHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ pshlloop
          RET

cshltest:
          MVIW   R2,CSHIFT_LEFT
          JSR    stringout
          MVIB   R2,10
cshlloop:
          JSR blink
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          RSHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ cshlloop
          RET

cshrtest:
          MVIW   R2,CSHIFT_RIGHT
          JSR    stringout
          MVIB   R2,10
cshrloop:
          JSR blink
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          RSHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ cshrloop
          RET
;
; subtraction tests
;
subtest:
          MVIW   R2,SUB
          JSR    stringout
          MVIB   R2,10
subloop:
          JSR blink
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          SUBI  1
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ subloop
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
; Get Command
;

; Single char
;
;
; Commands
;
; D AAAA - show 16 bytes at address AAAA
;          followed by CR disply next 16 bytes
;
; E AAAA - show contents of location AAAA (Output AAAA:XX)
;          if followed by ASCII-HEX modify location with new value (and redisplay)
;          if followed by CR display next location
;
; G AAAA - Jump to (and execute) starting at AAAA
;          code could end in BR to 0xf000h to restart monitor
;
;
; Output Prompt
:

cmdloop:
      LDAI '>'
      JSR uartout
      LDAI '>'
      JSR uartout

      JSR uartin
      JSR uartout
      LDTI 'E'
      BREQ examine
      LDTI 'D'
      BREQ dump
      LDTI 0Dh
      BREQ continue
      LDTI 'G'
      BREQ go
      MVIW R2,ERROR
      JSR stringout
      BR cmdloop
;
continue:
      MVIW R2,CONTMSG
      JSR stringout

       MVRLA R6
       LDTI EXAMINEMODE
       BREQ examinecont
       LDTI DUMPMODE
       BREQ dumpcont
;
;      ERROR
;
        MVIW   R2,CONTINUEERROR
stop:   BR stop

examine:
       MVIW R2,EXAMINEMSG
       JSR stringout
       MVIB R6,EXAMINEMODE
       jsr getaddress

examinecont:

dump:
      MVIW R2,DUMPMSG
      JSR stringout
      MVIB R6,DUMPMODE
      jsr getaddress

dumpcont:
      jsr showaddr
      jsr show16
      BR cmdloop


go:
      jsr getaddress


getaddress:
;
; Read 4 char address and return in R3
;
            JSR getnibble
            jsr ledout
            jsr switchtoggle

            SHL
            SHL
            shL
            shL

            jsr ledout
            jsr switchtoggle

            ANDI 0f0h

            jsr ledout
            jsr switchtoggle

            MVAT

            jsr ledout
            jsr switchtoggle

;
            JSR getnibble
            ANDI 0FH
            ORT
            MVAT
            JSR showbytea
            MVTA
            MVARH R3
            JSR getnibble
            shL
            shl
            shl
            shl
            ANDI 0f0h
            MVAT
            JSR getnibble
            ANDI 0FH
            ORT
            MVAT
            JSR showbytea
            MVTA
            MVARL R3
            RET
;
; getnibble return in accumulator
;
getnibble:
          JSR uartin
          JSR uartout
          LDTI '9'
          BRGT INAF
          SUBI '0'
          RET
INAF:     LDTI 'Z'
          BRGT lowercasein
          SUBI 'A'
;added this addi needs testing
          ADDI 10
          RET
lowercasein:
          SUBI 020h
          BR  INAF
;
; display R3
;
showaddr:   MVRHA R3
            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            MVRHA R3
            ANDI 0FH
            JSR shownibble
            MVRLA R3
            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            MVRLA R3
            ANDI 0FH
            JSR shownibble
            LDAI ':'
            JSR uartout
            LDAI ' '
            JSR uartout
            RET
;
; display 16 bytes point to by R3
;
show16:     JSR showbyte
            INCR R3
            LDAI ' '
            JSR uartout
            MVRLA R3
            ANDI 0FH
            BRNZ show16
            LDAI 0ah
            JSR uartout
            LDAI 0dh
            JSR uartout
            RET
;
; Output ASCII representation of a BYTE pointed to by R3
; or showbye in accumulator
; both destructive for accumulator
:
showbyte:   LDAVR R3
showbytea:  SHR
            SHR
            SHR
            SHR
            JSR shownibble
            LDAVR R3
            ANDI 0FH
            JSR shownibble
            RET
;
; Display nibble in accumulator (destructive)
;
shownibble:  MVARL R4
             LDTI 9
             BRGT AF
             ADDI '0'
             BR uartout
             RET
AF:          SUBI 10
             ADDI 'A'
             BR uartout
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
; output accumulator to LEDS & Hex displays (non distructive)
;
ledout:
        OUTI  P0,(SWITCHLED!TIL311)
        OUTA  P1
        RET
;
; Output null string pointed to by R2 to UART then send CR and LF
;
stringout:
        MVARL R4
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
        MVRLA R4
        RET
;
; output accumulator to UART
;
uartout:
        MVARL R4
        OUTI  P0,(UARTCS!UARTA5)
        INP   p1
        ANDI  020h
; i think this a brnz test -- that didn't work
;       BRZ   uartout
        MVRLA R4
        OUTI  P0,UARTCS
        OUTA  P1
; may not be needed
;       RET
        MVIW R7,01FFh
uartdelay:
        DECR R7
        MVRHA R7
        BRNZ uartdelay
        MVRLA R4
        RET
;
; input UART to accumulator
;
uartin:
        OUTI  P0,(UARTCS!UARTA5)
        INP   p1
        ANDI  01h
        BRZ   uartin
        OUTI  P0,(UARTCS)
        INP   P1
        RET
;
; toggle input switch
;
switchtoggle:
        MVARL R4
offw:   BRINL offw
        MVIW R7,01FFh
delaya:
        DECR R7
        MVRHA R7
        BRNZ delaya
        ON

onw:    BRINH onw
        MVIW R7,01FFh
delayb:
        DECR R7
        MVRHA R7
        BRNZ delayb
        OFF
        MVRLA R4
        RET
;
; blink LED
;
blink:
        MVARL R4
        ON
;
        MVIW R7,04FFh
onloop:
        DECR R7
        MVRHA R7
        BRNZ onloop

        OFF
        MVIW R7,04FFh
offloop:
        DECR R7
        MVRHA R7
        BRNZ offloop
        MVRLA R4
        RET

codes:  DB 'G','H','I','J','H','K','L','A','B','C','D','E'

hello:  DB 0ah,0dh,"YACC 2020: hello world  ",0
COMPARE_STRING: DB "Compare Tests",0
SHIFT_LEFT: DB "Shift Left",0
SHIFT_RIGHT: DB "Shift Right",0
RSHIFT_LEFT: DB "Ring Shift Left",0
RSHIFT_RIGHT: DB "Ring Shift Right",0
PSHIFT_LEFT: DB "PROP Shift Left",0
CSHIFT_LEFT: DB "CARRY Shift Left",0
CSHIFT_RIGHT: DB "CARRY Shift Right",0
SUB: DB "SUBTRACT",0
ERROR: DB "UNRECOGINIZED COMMAND",0
CONTINUEERROR: DB "CONTINUE CMD ERROR",0
DUMPMSG: DB "DUMP MODE",0
EXAMINEMSG: DB "EXAMINE MODE",0
CONTMSG: DB "CONTINUE MODE",0
accumtests: DB "accumulator test",0
PUSHPOPMSG: DB "Push Pop enter 3 numbers",0


;
; OLD
;
;
;LCD
;
;xlcdtest:
;        OUTI P0,(LCDENABLE)

;        MVIW R3,1fFFh
;xdelay0:
;        DECR R3
;        MVRHA R3
;        BRNZ xdelay0

;        OUTI P1,3CH

;        MVIW R3,1fFFh
;xdelay1:
;        DECR R3
;        MVRHA R3
;        BRNZ xdelay1

;        OUTI P1,01H

;        MVIW R3,1fFFh
;xdelay2:
;        DECR R3
;        MVRHA R3
;        BRNZ xdelay2

;        OUTI P1,0FH

;        MVIW R3,1fFFh
;xdelay3:
;        DECR R3
;        MVRHA R3
;        BRNZ xdelay3

;        OUTI P0,(LCDENABLE!LCDREGISTER)

;        MVIW R3,1fFFh
;xdelay4:
;        DECR R3
;        MVRHA R3
;        BRNZ xdelay4

;        OUTI P1,'A'

;xdelay5:
;        DECR R3
;        MVRHA R3
;        BRNZ xdelay5

;        OUTI P1,'B'



;xaddtest:
;      OUTI  P0,(SWITCHLED)
;      INP   P1
;      ADDI  001H
;      OUTA  P1

;xortest:
;      OUTI  P0,(SWITCHLED)
;      INP   P1
;      ORI  055H
;      OUTA  P1

;xandtest:
;      OUTI  P0,(SWITCHLED)
;      INP   P1
;      ANDI  055H
;      OUTA  P1

;xxortest:
;     OUTI  P0,(SWITCHLED)
;     INP   P1
;     XORI  055H
;     OUTA  P1
