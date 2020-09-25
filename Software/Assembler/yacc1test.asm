
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
         MVIW R1,2000h
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

         JSR blink

         MVIB R6,NOMODE
;
; Main
;
         MVIW R2,hello
         JSR stringout
         HALT

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
         JSR pushpop
         JSR ortest
         JSR orttest
;
; Tests
;

;
; OR test
;
ortest:
         MVIW   R2,ORTESTMSG
         JSR    stringout
         MVIB   R2,5
orloop:
         JSR blink
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         ORI  055H
         OUTA  P1

         DECR R2
         MVRLA R2
         BRNZ orloop
         RET

orttest:
        MVIW   R2,ORTTESTMSG
        JSR    stringout
        MVIB   R2,5
ortloop:
        JSR blink
        JSR switchtoggle
        OUTI P0,(SWITCHLED)
        INP P1
        MVAT
        JSR switchtoggle
        OUTI P0,(SWITCHLED)
        INP P1
        ORT
        OUTA  P1

        DECR R2
        MVRLA R2
        BRNZ ortloop
        RET
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
; H      - Display help menu
;
; T      - Test menu
;
; D AAAA - Show 16 bytes at address AAAA (16 byte aligned)
;          followed by CR disply next 16 bytes
;
; E AAAA - show contents of location AAAA (Output AAAA:XX)
;          if followed by ASCII-HEX modify location with new value (and redisplay)
;          if followed by CR display next location
;
; G AAAA - Jump to (and execute) starting at AAAA
;          code could end in BR to 0xf000h to restart monitor or RET if called via JSR

;
; Output Prompt
:

cmdloop:
      MVIW R2,PROMPT
      JSR stringout

      JSR uartin
      JSR toupper
      LDTI 'H'
      BRNEQ testexamine
      MVIW R2,helpmenu
      JSR stringout
      BR cmdloop

testexamine:
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
      MVIW R2,helpmenu
      JSR stringout
      BR cmdloop
;
continue:
;      MVIW R2,CONTMSG
;      JSR stringout

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
       MVIW R2,CRLF
       JSR stringout

examinecont:
      JSR showaddr
      JSR showbyte
      LDAI ' '
      JSR uartout

      JSR uartin
      LDTI 01bh
      BREQ examdone
      LDTI 0dh
      BREQ examnext
      JSR getnibblec
      shL
      shL
      shL
      shL
      push
      jsr getnibble
      MVAT
      Pop
      ORT
      STAVR R3

examnext:
      INCR R3
      LDAI 0ah
      JSR uartout
      LDAI 0dh
      JSR uartout
      BR examinecont
examdone:
      MVIW R2,CRLF
      JSR stringout
      BR cmdloop

dump:
      MVIW R2,DUMPMSG
      JSR stringout
      MVIB R6,DUMPMODE
      jsr getaddress
      MVIW R2,CRLF
      JSR stringout

dumpcont:
      jsr showaddr
      jsr show16
      BR cmdloop

go:
      MVIW R2,GOMSG
      JSR stringout
      jsr getaddress
      BRVR R3


getaddress:
;
; Read 4 char address and return in R3
;
            Push
            JSR getnibble
            SHL
            SHL
            shL
            shL
            ANDI 0f0h
            Push
            JSR getnibble
            ANDI 0FH
            MVAT
            Pop
            ORT
            MVARH R3

            JSR getnibble
            shL
            shl
            shl
            shl
            ANDI 0f0h
            push
            JSR getnibble
            ANDI 0FH
            MVAT
            pop
            ORT
            MVARL R3
            POP
            RET
;
; getnibble return in accumulator
;
getnibble:
          JSR uartin
getnibblec:
          LDTI '9'
          BRGT INAF
          SUBI '0'
          RET
INAF:     JSR toupper
          SUBI 'A'
          ADDI 10
          RET
;
; convert to uppercase
;
toupper:  LDTI 'Z'
          BRGT lower
          RET
lower:
          SUBI 020h
          RET
;
; display R3
;
showaddr:   Push
            MVRHA R3
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
            POP
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
showbyte:   PUSH
            LDAVR R3
            BR doshowbyte

showbytea:  Push

doshowbyte:
            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            LDAVR R3
            ANDI 0FH
            JSR shownibble
            POP
            RET
;
; Display nibble in accumulator (destructive)
;
shownibble:  PUSH
             LDTI 9
             BRGT AF
             ADDI '0'
             JSR uartout
             Pop
             RET

AF:          SUBI 10
             ADDI 'A'
             JSR uartout
             Pop
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
        Push
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
        POP
        RET
;
; output accumulator to UART
;
uartout:
        PUSH
        push
; doubt 2nd push pop is needed, to be tested
uartoutw:
        OUTI  P0,(UARTCS!UARTA5)
        INP   p1
        ANDI  040h
;
; i think this a brnz test -- that didn't work
        BRZ   uartoutw
        POP
        OUTI  P0,UARTCS
        OUTA  P1
;
; may not be needed
       Pop
       RET

;
; OLD
;
;        MVIW R7,01FFh
;uartdelay:
;        DECR R7
;        MVRHA R7
;        BRNZ uartdelay
;        POP
;        RET
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
        JSR uartout
        RET
;
; toggle input switch
;
switchtoggle:
        Push
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
        Pop
        RET
;
; blink LED
;
blink:
        Push
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
        Pop
        RET

codes:  DB 'G','H','I','J','H','K','L','A','B','C','D','E'

hello:  DB 0ah,0dh,"YACC 2020: hello world  ",0ah,0dh,0
COMPARE_STRING: DB "Compare Tests",0ah,0dh,0
SHIFT_LEFT: DB "Shift Left",0ah,0dh,0
SHIFT_RIGHT: DB "Shift Right",0ah,0dh,0
RSHIFT_LEFT: DB "Ring Shift Left",0ah,0dh,0
RSHIFT_RIGHT: DB "Ring Shift Right",0ah,0dh,0
PSHIFT_LEFT: DB "PROP Shift Left",0ah,0dh,0
CSHIFT_LEFT: DB "CARRY Shift Left",0ah,0dh,0
CSHIFT_RIGHT: DB "CARRY Shift Right",0ah,0dh,0
SUB: DB "SUBTRACT",0ah,0dh,0
ERROR: DB "UNRECOGINIZED COMMAND",0ah,0dh,0
CONTINUEERROR: DB "CONTINUE CMD ERROR",0ah,0dh,0
DUMPMSG: DB 0ah,0dh,"DUMP ADDRESS:",0
GOMSG: DB 0ah,0dh,"GO ADDRESS:",0
EXAMINEMSG: DB 0ah,0Dh,"EXAMINE ADDRESS:",0
CONTMSG: DB "CONTINUE MODE",0
accumtests: DB "accumulator test",0ah,0dh,0
PUSHPOPMSG: DB "Push Pop enter 3 numbers",0ah,0dh,0
ORTESTMSG: DB "OR tests",0ah,0dh,0
ORTTESTMSG: DB "OR Tmp register tests",0ah,0dh,0
PROMPT: DB ">>",0
CRLF: DB 0ah,0dh,0
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
helpmenu:
DB "H      - Display help menu",0ah,0dh
DB "T      - Test menu",0ah,0DH
DB "D AAAA - Show 16 bytes at address AAAA (16 byte aligned) followed by CR display next 16 bytes",0ah,0dh
DB "E AAAA - show contents of location AAAA (Output AAAA:XX)",0ah,0dh
DB "         if followed by ASCII-HEX modify location with new value (and redisplay)",0ah,0DH
DB "         if followed by CR display next location",0ah,0dh
DB "G AAAA - Jump to (and execute) starting at AAAA",0ah,0dh
DB "         code could end in BR to 0xf000h to restart monitor or RET if called via JSR",0ah,0dh,0
