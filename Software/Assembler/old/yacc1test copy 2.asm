
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

TOKENIZER_ERROR: EQU 00
TOKENIZER_ENDOFINPUT: EQU 01
TOKENIZER_NUMBER: EQU 02
TOKENIZER_STRING: EQU 03
TOKENIZER_VARIABLE: EQU 04
TOKENIZER_LET: EQU 05
TOKENIZER_PRINT: EQU 06
TOKENIZER_IF: EQU 07
TOKENIZER_THEN: EQU 08
TOKENIZER_ELSE: EQU 09
TOKENIZER_FOR: EQU 10
TOKENIZER_TO: EQU 11
TOKENIZER_NEXT: EQU 12
TOKENIZER_GOTO: EQU 13
TOKENIZER_GOSUB: EQU 14
TOKENIZER_RETURN: EQU 15
TOKENIZER_CALL: EQU 16
TOKENIZER_REM: EQU 17
TOKENIZER_PEEK: EQU 18
TOKENIZER_POKE: EQU 19
TOKENIZER_END: EQU 20
TOKENIZER_COMMA: EQU 21
TOKENIZER_SEMICOLON: EQU 22
TOKENIZER_PLUS: EQU 23
TOKENIZER_MINUS: EQU 24
TOKENIZER_AND: EQU 25
TOKENIZER_OR: EQU 26
TOKENIZER_ASTR: EQU 27
TOKENIZER_SLASH: EQU 28
TOKENIZER_MOD: EQU 29
TOKENIZER_HASH: EQU 30
TOKENIZER_LEFTPAREN: EQU 31
TOKENIZER_RIGHTPAREN: EQU 32
TOKENIZER_LT: EQU 33
TOKENIZER_GT: EQU 34
TOKENIZER_EQ: EQU 35
TOKENIZER_CR: EQU 36
TOKENIZER_LINENUM: EQU 37

;
; MONITOR MODES
;
NOMODE:       EQU 0
EXAMINEMODE:  EQU 1
DUMPMODE:     EQU 2
BLOCKMODE:    EQU 3
FILLMODE:     EQU 4

         ORG 0f000h
         BR eprom
         ORG 0f003h
eprom:
;
; Setup Stack
;
         : EQU 00 R1,2000h
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

; Set intial monitor mode

         MVIB R6,NOMODE
;
; Main
;
          JSR lblink
          MVIW R2,hello
          JSR stringout
;
; additional proof of life
;
; show first 16 bytes of ROM & REGISTERS
;
         MVIW R3,0f000h
         JSR showaddr
         JSR show16
         JSR showregs
         MVIW R2,CRLF
         JSR stringout
;
; show test code addr to use with go command
;
         MVIW R3,tttt
         JSR showaddr
         MVIW R2,CRLF
         JSR stringout
;
; if INPUT high start the monitor
;
         BRINH cmdloop
;
; else run test/code below at completetion blink OUT LED forever
;
tttt:
        MVIW R2,TESTMSG
        JSR stringout

        LDAI 5
        MVIW R2,nblink
        JSRUR R2

alltests:
         jsr shltest
         JSR shrtest
         jsr rshltest
         jsr rshrtest
         jsr cshltest
         JSR cshrtest
         JSR pshltest
         JSR additest
         JSR addictest
         JSR subtest
         JSR cmptest
         JSR shrtest
         JSR shltest
         JSR rshrtest
         JSR rshltest
         JSR cshltest
         JSR cshrtest
         JSR accumtest
         JSR pushpoptest
         JSR ortest
         JSR orttest
         JSR additest
         JSR movrrtest

testsdone:
          JSR lblink
          BR testsdone

;
; Individual Tests
;

;
; Register to Register move test
;
movrrtest:
        MVIW   R2,MOVRRHELP
        JSR    stringout

        MVIW R3,1234h
        MVIW R4,5678h

        jsr showreg34

        MOVRR R3,R4

        jsr showreg34

        MVIW R3,4321h

        jsr showreg34

        MVIW R2,1234h
        MVIW R3,5678h

        jsr showreg23

        MOVRR R2,R3

        jsr showreg23


        MVIW R2,4321h

        jsr showreg23

        ret

;
; sho registers (hard coded)
;
showreg23:
        LDAI 0h
        JSR ledout
        MVRHA r2
        JSR TIL311out
        JSR switchtoggle
        LDAI 1h
        JSR ledout
        MVRLA r2
        JSR TIL311out
        JSR switchtoggle
        LDAI 2h
        JSR ledout
        MVRHA r3
        JSR TIL311out
        JSR switchtoggle
        LDAI 3h
        JSR ledout
        MVRLA r3
        JSR TIL311out
        JSR switchtoggle
        ret

showreg34:
        LDAI 0h
        JSR ledout
        MVRHA r3
        JSR TIL311out
        JSR switchtoggle
        LDAI 1h
        JSR ledout
        MVRLA r3
        JSR TIL311out
        JSR switchtoggle
        LDAI 2h
        JSR ledout
        MVRHA r4
        JSR TIL311out
        JSR switchtoggle
        LDAI 3h
        JSR ledout
        MVRLA r4
        JSR TIL311out
        JSR switchtoggle
        ret
;
; OR - OR accumulator immediate
;
ORHELP: DB "OR tests - or input switches with 0x55 (5x)",0ah,0dh,0
ortest:
         MVIW   R2,ORHELP
         JSR    stringout
         MVIB   R2,5
orloop:
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         ORI  055H
         OUTA  P1

         DECR R2
         MVRLA R2
         BRNZ orloop
         RET

;
; ADDI - ADD accumulator with immediate
;
additest:
       MVIW   R2,ADDIHELP
       JSR    stringout
       MVIB   R2,5
addiloop:
       JSR switchtoggle
       OUTI P0,(SWITCHLED)
       INP P1
       ADDI 02h
       OUTA  P1

       DECR R2
       MVRLA R2
       BRNZ addiloop
       RET

;
; ADDIC - ADD accumulator with immediate with carry
;
addictest:
      MVIW   R2,ADDICHELP
      JSR    stringout
      MVIB   R2,5
addicloop:
      JSR switchtoggle
      OUTI P0,(SWITCHLED)
      INP P1
      ADDIC 02h
      OUTA  P1

      DECR R2
      MVRLA R2
      BRNZ addicloop
      RET

;
; ORT - OR accumulator with tmp register
;
orttest:
        MVIW   R2,ORTHELP
        JSR    stringout
        MVIB   R2,5
ortloop:
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
; push pop tests - push 3 values onto stack, pop 3values from stack
;
pushpoptest:
         MVIW   R2,PUSHPOPHELP
         JSR    stringout

         MVIB   R2,3
ppenterloop:
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         push

         DECR R2
         MVRLA R2
         BRNZ ppenterloop

         MVIB   R2,3
ppdisloop:
        JSR switchtoggle
        OUTI P0,(SWITCHLED)
        POP
        OUTA  P1
        DECR R2
        MVRLA R2
        BRNZ ppdisloop

        RET
;
; accumulator register Tests - move values between accumulator and register HI 8 bits
;                              move values between accumulator and register LO 8 bits
;
accumtest:
         MVIW   R2,accumhelp
         JSR    stringout
         MVIB   R2,10
accloop:
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
; Shift left
;
SHIFT_LEFTHELP: DB "Shift Left - shift input switches (5x)",0ah,0dh,0
shltest:
         MVIW   R2,SHIFT_LEFTHELP
         JSR    stringout
         MVIB   R2,5
shlloop:
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         SHL
         OUTA  P1

         DECR R2
         MVRLA R2
         BRNZ shlloop
         RET
;
; shift Right
;
SHIFT_RIGHTHELP: DB "Shift Right - shift input switches (5x)",0ah,0dh,0
shrtest:
          MVIW   R2,SHIFT_RIGHTHELP
          JSR    stringout
          MVIB   R2,5
shrloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          SHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ shrloop
          RET
;
; ring shift left
;
rshltest:
         MVIW   R2,RSHIFT_LEFTHELP
         JSR    stringout
         MVIB   R2,10
rshlloop:
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         RSHL
         OUTA  P1

         DECR R2
         MVRLA R2
         BRNZ rshlloop
         RET
;
; ring shift right
;
rshrtest:
          MVIW   R2,RSHIFT_RIGHTHELP
          JSR    stringout
          MVIB   R2,10
rshrloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          RSHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ rshrloop
          RET
;
; shift left and propagate sign bit
;
pshltest:
          MVIW   R2,PSHIFT_LEFTHELP
          JSR    stringout
          MVIB   R2,10
pshlloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          PSHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ pshlloop
          RET
;
; ring shift left through carry bit
;
cshltest:
          MVIW   R2,CSHIFT_LEFTHELP
          JSR    stringout
          MVIB   R2,10
cshlloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          CSHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ cshlloop
          RET
;
; ring shift right through carry bit
;
cshrtest:
          MVIW   R2,CSHIFT_RIGHTHELP
          JSR    stringout
          MVIB   R2,10
cshrloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          CSHR
          OUTA  P1

          DECR R2
          MVRLA R2
          BRNZ cshrloop
          RET
;
; subtraction tests
;
subtest:
          MVIW   R2,SUBHELP
          JSR    stringout
          MVIB   R2,10
subloop:
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
          MVIW   R2,COMPAREHELP
          JSR    stringout
          MVIB   R2,10
          LDTI   055H
cmploop:
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
; Monitor
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
; B AAAA - Show 256 bytes at address AAAA (16 byte aligned)
;          followed by CR display next 256 bytes
;
; D AAAA - Show 16 bytes at address AAAA (16 byte aligned)
;          followed by CR display next 16 bytes
;
; E AAAA - show contents of location AAAA (Output AAAA:XX)
;          if followed by ASCII-HEX modify location with new value (and redisplay)
;          if followed by CR display next location
;
; F AAAA - fill block with 0
;          if followed by CR fill next location
;
; G AAAA - Jump to (and execute) starting at AAAA
;          code could end in BR to 0xf000h to restart monitor or RET if called via JSR
;
; R      - SHOW REGISTERS
;
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
      MVIW R2,CRLF
      JSR stringout
      MVIW R2,helpmenu
      JSR stringout
      BR cmdloop

testexamine:
      LDTI 'B'
      BREQ dumpblock
      LDTI 'D'
      BREQ dump
      LDTI 'E'
      BREQ examine
      LDTI 'F'
      BREQ fillblock
      LDTI 'G'
      BREQ go
      LDTI 'R'
      BREQ dumpreg
      LDTI 'T'
      BREQ tests
      LDTI 0Dh
      BREQ continue

      MVIW R2,CRLF
      JSR stringout
      MVIW R2,ERROR
      JSR stringout
      MVIW R2,helpmenu
      JSR stringout
      BR cmdloop
;
continue:
       MVRLA R6

       LDTI BLOCKMODE
       BREQ dumpblockcont

       LDTI DUMPMODE
       BREQ dumpcont

       LDTI EXAMINEMODE
       BREQ examinecont

       LDTI FILLMODE
       BREQ fillcont
       BR cmdloop
;
;      ERROR
;
       MVIW R2,CONTINUEERROR
       JSR stringout
       BR cmdloop

stop:   BR stop

dumpblock:
       MVIB R6,BLOCKMODE
       MVIW R2,DUMPBLOCKMSG
       JSR stringout
       jsr getaddress
       MVIW R2,CRLF
       JSR stringout

dumpblockcont:
       jsr show256
       BR cmdloop

dump:
       MVIB R6,DUMPMODE
       MVIW R2,DUMPMSG
       JSR stringout
       jsr getaddress
       MVIW R2,CRLF
       JSR stringout

dumpcont:
       jsr showaddr
       jsr show16
       BR cmdloop

examine:
       MVIB R6,EXAMINEMODE
       MVIW R2,EXAMINEMSG
       JSR stringout
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
      SHL
      SHL
      SHL
      SHL
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

fillblock:
       MVIB R6,FILLMODE
       MVIW R2,FILLMSG
       JSR stringout
       jsr getaddress
       MVIW R2,CRLF
       JSR stringout

fillcont:
      jsr showaddr
      MVIW R2,CRLF
      JSR stringout

morefill:
      LDAI 0
      STAVR R3
      INCR R3
      MVRLA R3
      ANDI  0FFH
      BRNZ morefill
      LDAI 0ah
      JSR uartout
      LDAI 0dh
      JSR uartout
      BR cmdloop


go:
      MVIW R2,GOMSG
      JSR stringout
      jsr getaddress
      BRVR R3

dumpreg:
      JSR showregs
      MVIB R6,NOMODE
      BR cmdloop

tests:
      MVIB R6,NOMODE
      MVIW R2,CRLF
      JSR stringout
      MVIW R3,testmenu
      MVIW R4,0000h

testsloop:

      INCR R3
      INCR R3
      LDAVR R3
      MVARH R2
      INCR  R3
      LDAVR R3
      MVARL R2
      INCR R3
      LDAVR R2
      LDTI '-'
      BREQ testsloopdone
      MVRLA R4
      JSR showbytea
      LDAI  '-'
      JSR uartout
      JSR stringout
      MVIW R2,CRLF
      JSR stringout
      INCR R4
      BR testsloop

testsloopdone:
; get test number
; multiple by 4 and add to test list base
; JSR via register holding info
;
      MVIW R2,gettestpromopt
      JSR stringout

      JSR getnibble
      SHL
      SHL
      SHL
      SHL
      push
      jsr getnibble
      MVAT
      Pop
      ORT
; calculate test address
      shl
      shl
      JSR TIL311out
      MVAT
      MVIW R3,testmenu
      MVRLA R3
      ADDT
      MVARL R3
      JSR TIL311out
      BRC menucarry
      BR dotest

menucarry:
      incr r3

dotest:
      LDAVR R3
      MVARH R4
      INCR  R3
      LDAVR R3
      MVARL R4
      JSRUR R4
      BR cmdloop

getaddress:
;
; Read 4 char address and return in R3
;
            Push
            JSR getnibble
            SHL
            SHL
            SHL
            SHL
            ANDI 0f0h
            Push
            JSR getnibble
            ANDI 0FH
            MVAT
            Pop
            ORT
            MVARH R3

            JSR getnibble
            SHL
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
; display R3 followed by a ":" and " "
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
;
;
showregs:
            MVIW R2,CRLF
            JSR stringout
            MOVRR r0,r3
            jsr showaddr
            MOVRR r1,r3
            jsr showaddr
            MOVRR r2,r3
            jsr showaddr
            MOVRR r3,r3
            jsr showaddr
            MOVRR r4,r3
            jsr showaddr
            MOVRR r5,r3
            jsr showaddr
            MOVRR r6,r3
            jsr showaddr
            MOVRR r7,r3
            jsr showaddr

            MVIW R2,CRLF
            JSR stringout
            RET
;
; display upto 16 bytes point to by R3, stops on a 16 byte boundry, increments R3
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
; display upto 256 bytes point to by R3, stops on a 256 byte boundry, increments R3
show256:
          jsr showaddr
          jsr show16
;          MVIW R2,CRLF
;          JSR stringout
          MVRLA R3
          ANDI  0FFH
          BRNZ show256
          LDAI 0ah
          JSR uartout
          LDAI 0dh
          JSR uartout
          RET
;
; Output ASCII representation of a BYTE pointed to by R3
; or use showbytea in accumulator
; both destructive for accumulator - no longer true with push/pop?
:
showbyte:   PUSH
            LDAVR R3

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
showbytea:  PUSH
            PUSH

            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            POP
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
; output accumulator to LEDS or Hex displays (non distructive)
;
ledout:
        OUTI  P0,(SWITCHLED)
        OUTA  P1
        RET
TIL311out:
        OUTI  P0,(TIL311)
        OUTA  P1
        RET
;
; Output null terminated string pointed to by R2 to UART then send CR and LF
; Advances R2 to end of string
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
charout:
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
; long delay (approx 5 seconds)
;
LONGDELAY:
        PUSH
        MVIW R7,0FFFFh
longdelayloop:
        DECR R7
        MVRHA R7
        BRNZ longdelayloop
        POP
        RET
;
; short delay (approx 1 second)
;
SHORTDELAY:
        PUSH
        MVIW R7,033FFh
shortdelayloop:
        DECR R7
        MVRHA R7
        BRNZ shortdelayloop
        POP
        RET

NODELAY:
      PUSH
      MVIW R7,00FFh
nodelayloop:
      DECR R7
      MVRHA R7
      BRNZ nodelayloop
      POP
      RET
;
; toggle input switch (with debounce)
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
; quick blink LED
;
blink:
        Push
        ON
        MVIW R7,03FFh
onloop:
        DECR R7
        MVRHA R7
        BRNZ onloop

        OFF
        MVIW R7,003FFh
offloop:
        DECR R7
        MVRHA R7
        BRNZ offloop
        Pop
        RET
;
; long blink LED
;
lblink:
        Push
        ON
        MVIW R7,018FFh
lonloop:
        DECR R7
        MVRHA R7
        BRNZ lonloop

        OFF
        MVIW R7,018FFh
loffloop:
        DECR R7
        MVRHA R7
        BRNZ loffloop
        Pop
        RET
;
; blink n times in accumulator
;
nblink:
        push
nblinkloop:
        JSR TIL311out
        BRZ nblinkdone
        JSR blink
        subi 1
        BR nblinkloop
nblinkdone:
        POP
        RET


;
; MONITOR STRINGS
;
hello:  DB 0ah,0dh,"YACC 2020: hello world  ",0ah,0dh,0
PROMPT: DB ">>",0
CRLF: DB 0ah,0dh,0
ERROR: DB "UNRECOGINIZED COMMAND",0ah,0dh,0
CONTINUEERROR: DB "CONTINUE CMD ERROR",0ah,0dh,0
DUMPMSG: DB 0ah,0dh,"DUMP ADDRESS:",0
DUMPBLOCKMSG: DB 0ah,0dh,"DUMP BLOCK ADDRESS:",0
FILLMSG: DB 0ah,0dh,"FILL BLOCK ADDRESS:",0
GOMSG: DB 0ah,0dh,"GO ADDRESS:",0
EXAMINEMSG: DB 0ah,0Dh,"EXAMINE ADDRESS:",0
CONTMSG: DB "CONTINUE MODE",0
gettestpromopt: DB "Enter Test number:",0
;
helpmenu:
DB "H      - Display help menu",0ah,0dh
DB "B AAAA - Show 256 bytes of memory at address AAAA (16 byte aligned)",0ah,0dh
DB "         followed by CR display next 256 bytes",0ah,0dh
DB "D AAAA - Show 16 bytes of memory at address AAAA (16 byte aligned)",0ah,0dh
DB "         followed by CR display next 16 bytes",0ah,0dh
DB "E AAAA - show contents of location AAAA (Output AAAA:XX)",0ah,0dh
DB "         if followed by ASCII-HEX modify location with new value (and redisplay)",0ah,0DH
DB "         if followed by CR display next location",0ah,0dh
DB "F AAAA   Fill contents 256 bytes of memory at address AAAA with 0(16 byte aligned) with 0",0ah,0dh
DB "         if followed by CR fill next 256 bytes",0ah,0dh
DB "G AAAA - Jump to (and execute) starting at AAAA",0ah,0dh
DB "         code could end in BR to 0xf000h to restart monitor or RET if called via JSR",0ah,0dh
DB "R      - Show registers",0ah,0dh
DB "T      - Test menu",0ah,0DH
DB 0
;
; TEST HELP MESSAGES
;
COMPAREHELP: DB "Compare Tests",0ah,0dh,0
RSHIFT_LEFTHELP: DB "Ring Shift Left",0ah,0dh,0
RSHIFT_RIGHTHELP: DB "Ring Shift Right",0ah,0dh,0
PSHIFT_LEFTHELP: DB "PROP Shift Left",0ah,0dh,0
CSHIFT_LEFTHELP: DB "CARRY Shift Left",0ah,0dh,0
CSHIFT_RIGHTHELP: DB "CARRY Shift Right",0ah,0dh,0
SUBHELP: DB "SUBTRACT",0ah,0dh,0
accumhelp: DB "accumulator test",0ah,0dh,0
PUSHPOPHELP: DB "Push Pop enter 3 numbers",0ah,0dh,0
ORTHELP: DB "OR Tmp register tests",0ah,0dh,0
ADDIHELP: DB "Add immediate 02h to input number",0ah,0dh,0
ADDICHELP: DB "Add immediate with carry 02h to input number",0ah,0dh,0
MOVRRHELP: DB "MOVERR TEST",0ah,0dh,0
TESTMSG: DB "Run test code",0ah,0dh,0

;
; TEST MENU
;
      DB "AAAAAAAAAAAAAAAAAAAAAAAAA"
testmenu:
      DW ortest,ormenu
      DW orttest,ortmenu
      DW pushpoptest,pushpopmenu
      DW accumtest,accummenu
      DW shltest,shlmenu
      DW shrtest,shrmenu
      DW rshltest,rshlmenu
      DW rshrtest,rshrmenu
      DW cshltest,cshlmenu
      DW cshrtest,cshrmenu
      DW pshltest,pshlmenu
      DW subtest,submenu
      DW cmptest,cmpmenu
      DW additest,addimenu
      DW addictest,addicmenu
      DW movrrtest,movrrmenu
      DW endmenu,endmenu

;
; TEST MENU STRINGS
;
testmenustrings:
ormenu: DB "OR",0
ortmenu: DB "ORT",0
pushpopmenu: DB "PUSH/POP",0
accummenu: DB "Accumulator <-> Register",0
shlmenu: DB "SHL",0
shrmenu: DB "SHR",0
rshlmenu: DB "RSHL",0
rshrmenu: DB "RSHR",0
cshlmenu: DB "CSHL",0
cshrmenu: DB "CSHR",0
pshlmenu: DB "PSHL",0
submenu: DB "SUB",0
cmpmenu: DB "Compare Branch",0
addimenu: DB "ADDI",0
addicmenu: DB "ADDI",0
movrrmenu: DB "MOVRR",0
endmenu: DB "-",0

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
      JSRUR R2

bas1: dw "unexpected token"

exe:
  on

exe_init:
  on

;
; compare and eat token in accumulator to current token in buffer
;
exe_accept:
    push
;
; get current token into accumulator
;
    jsr tok_token
    MVAT
    POP
    BREQ exe_accept_done
    mviw r2,bas1
exe_accept_done:
    jsr tok_next
    RET

;
; get variable value pointed to by tok_variable_num in token stream (this is not a value but a variable id)
; return value in r7
;
exe_varfactor:
; get variable number id (one byte for now a-z) into accumulator
;
  jsr tok_variable_num
;
; variable name in accumulator, return in r7
  jsr exe_get_variable
  RET

;
; return value in r7
;
exe_factor:
;
; get current token into accumulator
;
    jsr tok_token
;
    LDTI TOKENIZER_NUMBER
    BRNEQ exe_factor1
    JSR tok_num
    MVTA
    JSR exe_accept
    RET

exe_factor1:
    LDTI TOKENIZER_LEFTPAREN
    BRNEQ exe_factor2
    JSR exe_accept
    JSR exe_expr
    LDAI TOKENIZER_RIGHTPAREN
    JSR exe_accept
    RET

exe_factor2:
    jsr exe_varfactor
    ret

;
;
;
exe_term:
    jsr exe_factor

exe_term_loop:
    MOVRR r7,r6
    jsr tok_token
    LDAI TOKENIZER_ASTR
    BREQ exe_term_astr
    LDAI TOKENIZER_SLASH
    breq exe_term_slash
    ldai TOKENIZER_MOD
    breq exe_term_mod
    br exe_term_done

exe_term_astr:
    jsr tok_next
    jsr exe_factor
    BR exe_term_loop

exe_term_slash:
    jsr tok_next
    jsr exe_factor
    BR exe_term_loop

exe_term_mod:
    jsr tok_next
    jsr exe_factor
    BR exe_term_loop

exe_term_done:
    ret

exe_expr:
    jsr exe_term

exe_expr_loop:
    MOVRR r7,r6
    jsr tok_token

    LDAI TOKENIZER_PLUS
    BREQ exe_expr_plus
    LDAI TOKENIZER_MINUS
    breq exe_expr_minus
    LDAI TOKENIZER_AND
    breq exe_expr_and
    ldai TOKENIZER_OR
    breq exe_expr_or

    br exe_expr_done

exe_expr_plus:
    jsr tok_next
    jsr exe_term
    BR exe_expr_loop

exe_expr_minus:
    jsr tok_next
    jsr exe_term
    BR exe_expr_loop

exe_expr_and:
    jsr tok_next
    jsr exe_term
    BR exe_expr_loop

exe_expr_or:
    jsr tok_next
    jsr exe_term
    BR exe_expr_loop

exe_expr_done:
    ret

exe_relation:
    jsr exe_expr

exe_relation_loop:
    MOVRR r7,r6
    jsr tok_token

    ldai TOKENIZER_LT
    BREQ exe_relation_lt
    LDAI TOKENIZER_GT
    breq exe_relation_gt
    ldai TOKENIZER_EQ
    breq exe_relation_eq

    br exe_relation_done

exe_relation_lt:
    jsr tok_next
    jsr exe_expr
    BR exe_relation_loop

exe_relation_gt:
    jsr tok_next
    jsr exe_expr
    BR exe_relation_loop

exe_relation_eq:
    jsr tok_next
    jsr exe_expr
    BR exe_relation_loop

exe_relation_done:
    ret

exe_index_free:
  on

exe_index_find:
  on

exe_index_add:
  on

exe_jump_line_low:
  on

exe_jump_line:
  on

exe_goto_stmt:
  on

exe_print_stmt:
  LDAI TOKENIZER_PRINT
  jsr exe_accept

exe_print_stmt_loop:
  jsr tok_token
  LDAI TOKENIZER_STRING
  BRNEQ exe_print_stmt1
;
; this should return string to print in r2
;
  jsr tok_string
  jsr stringout
  jsr tok_next
  br exe_print_stmt_test

exe_print_stmt1:

  ldti TOKENIZER_COMMA
  BRNEQ exe_print_stmt2
  LDAI ' '
  JSR charout
  jsr tok_next
  br exe_print_stmt_test

exe_print_stmt2:
  ldti TOKENIZER_SEMICOLON
  BRNEQ exe_print_stmt3
  jsr tok_next
  br exe_print_stmt_test

exe_print_stmt3:
    ldai TOKENIZER_VARIABLE
    BREQ exe_print_stmt4
    ldti TOKENIZER_NUMBER
    breq exe_print_stmt4
    ldai  TOKENIZER_LEFTPAREN
    breq exe_print_stmt4

    br exe_print_stmt_done

exe_print_stmt4:
    jsr exe_expr
;   br exe_print_stmt_test // falls through

exe_print_stmt_test:
    jsr tok_token
    LDTI TOKENIZER_CR
    breq exe_print_stmt_done
    LDTI TOKENIZER_ENDOFINPUT
    breq exe_print_stmt_done
    br exe_print_stmt_loop

exe_print_stmt_done:
    mviw R2,CRLF
    jsr stringout
    jsr tok_next
    ret

exe_if_stmt:
  on

exe_let_statement:
  on

exe_gosub_statement:
  on

exe_return:
  on

exe_next_statement:
  on

exe_for_statement:
  on

exe_peek_statement:
  on

exe_poke_statement:
  on

exe_end_statement:
  on

exe_statement:
  on

exe_line_statement:
  on

exe_run:
  on

exe_finished:
  on

exe_set_variable:
  on

exe_get_variable:
  on

tok_token:
tok_next:
tok_num:
tok_variable_num:
tok_string:
