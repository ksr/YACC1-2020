;
; Basic Interpreter Entry points
;
basic_list:   EQU 0e000h
basic_run:    EQU 0e010h
basic_cold:   EQU 0e020h
basic_test:   EQU 0e030h
basic_interprter: EQU 0e040h
basic_parse:  EQU 0e050h
basic_copy:   EQU 0e060h
;
; Hardware info
;
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

;
; MONITOR MODES
;
NOMODE:       EQU 0
EXAMINEMODE:  EQU 1
DUMPMODE:     EQU 2
BLOCKMODE:    EQU 3
FILLMODE:     EQU 4

;
; Monitor variables 0x0f00 - 0x0fff
;
monmode:        EQU 0f00h
continue_addr:  EQU 0f02h
line_buffer:    EQU 0f80h    ; 128 bytes long max


;
; Setup Stack, use R1 0eff -> down to 0c00 (but no checking)
;
STACK: EQU 0EFFh

;
; remap eprom from 0x0000 to 0xf000 by initial access to 0xf003 via BRanch
;
         ORG 0f000h
         BR eprom
         ORG 0f003h
eprom:
;
; Setup Stack
;
         MVIW R1,STACK

; test
;         brdev devon
;
;         off
;         halt
;devon:
;        on
;        halt

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

;         MVIB R6,NOMODE

          LDAI NOMODE
          STA MONMODE
;
; Main
;
          JSR lblink
          MVIW R7,hello
          JSR stringout
          JSR basic_cold   ; initialize basic interpreter
                           ; hack should this pass in token buffer ptr
;
; additional proof of life
;
; show first 16 bytes of ROM & REGISTERS
;
         MVIW R7,0f000h
         JSR showaddr
         JSR show16
         JSR showregs
         MVIW R7,CRLF
         JSR stringout
;
; show test code addr to use with go command
;
         MVIW R7,tttt
         JSR showaddr
         MVIW R7,CRLF
         JSR stringout
;
; if INPUT high start the monitor
;
         BRINH cmdloop
;
; else run test/code below at completetion blink OUT LED jump to cmdloop
;
tttt:
        MVIW R7,TESTMSG
        JSR stringout
;
; Tests to be run at startup
;

;
; startup tests complete
;
        BR cmdloop
;
;
;
alltests:
;         jsr shltest
;         JSR shrtest
;         jsr rshltest
;         jsr rshrtest
;         jsr cshltest
;         JSR cshrtest
;         JSR pshrtest
;         JSR additest
;         JSR addictest
;         JSR subtest
;         JSR cmptest
;         JSR shrtest
;         JSR shltest
;         JSR rshrtest
;         JSR rshltest
;         JSR cshltest
;         JSR cshrtest
;         JSR accumtest
;         JSR pushpoptest
;         JSR ortest
;         JSR orttest
;         JSR additest
;         JSR movrrtest
;         JSR add16tests
;         jsr mem_indirect_tests
;         jsr mul16
;         jsr pushr_popr_tests
;         jsr sub16tests

alltestsdone:
          JSR lblink
          BR alltestsdone

;
; Individual Tests
;

;
; 16 x 16 bit multiply test
;
mul16:
      mviw r4,0FFFAh
      mviw r5,0002h

;
; test new memory based load/store instructions
;
mem_indirect_tests:

        MVIW R3,02000h
clearmem:
        LDAI 0
        STAVR R3
        INCR R3
        MVRLA R3
        ANDI  0FFH
        BRNZ clearmem

; initial dump

;
; step 10
;
        ldai 010h       ;counter
        jsr mem_indirect_util

; str 0x5544 into 0x2000-1 with tested opcodes

        MVIW R4,02000h
        LDAI 055h
        STAVR R4
        INCR R4
        LDAI 044h
        STAVR R4

; step 11

        ldai 011h
        jsr mem_indirect_util

; load register R5 from memory 0x2000

        LDR R5,02000h
        movrr r2,r6       ;move r2 to r6 for output purposes
                          ;R2 is used by LDR/STR instuctions

; step 12

        ldai 012h
        jsr mem_indirect_util

; store info in R5 into 0x2004-5
; step 13

        STR R5,2004h
        movrr r2,r6

        ldai 013h
        jsr mem_indirect_util

; load 0x66 into accumulator and store in 0x2006
; Save R2 into R6 for display
; step 14

        LDAI 066h
        STA  02006h
        movrr r2,r6
        ldai 014h
        jsr mem_indirect_util

; load accumulator from 0xf000
; save R2 in R6 for display
; store value in accumulator into 0x2008
; step 15

        lda 0f000h

; store value from 0f000h to 2008h

        sta 02008h
        movrr r2,r6
        ldai 015h
        jsr mem_indirect_util
;
;  load 88 into tmp register and store t 200a
;  step 16
;
        ldti 088h
        stt 0200ah
        movrr r2,r6
        ldai 016h
        jsr mem_indirect_util
;
; load tmp from memory f001 and store to 200c
; step 17
;
        ldt 0f001h
        stt 0200ch
        movrr r2,r6
        ldai 017h
        jsr mem_indirect_util
        ret

mem_indirect_util:
        push
        MVIW R7,02000h
        JSR showaddr
        JSR show16
        JSR showregs
        pop
        JSR showbytea
        MVIW R7,CRLF
        JSR stringout
        ret
;
; 16 bit add carry tests
;
add16tests:
;
; add r4 and r5
;

      mviw r4,01234h
      mviw r5,05678h
      jsr do_add16_util

      mviw r4,01288h
      mviw r5,05699h
      jsr do_add16_util

      mviw r4,0FFFAh
      mviw r5,0FFFCh
      jsr do_add16_util

      mviw r4,05689h
      mviw r5,0abcdh
      jsr do_add16_util

      mviw r4,0FFFAh
      mviw r5,0FFFEh
      jsr do_add16_util
      ret

do_add16_util:
      jsr showregs
      jsr do_add16
      jsr showregs
      ret

do_add16:
      ldai 0      ;clear carry
      cshl
;      addi 0
      MVRLA R4
      MVAT
      mvrla r5
      ADDT
      mvarl r4
      mvrha r4
      MVAT
      mvrha r5
      addtc
      mvarh r4
      ret

;
; 16 bit sub  tests
;
sub16tests:
;
; sub r4 from r5
;

      mviw r4,0001h
      mviw r5,0009h
      jsr do_sub16_util

      mviw r4,0001h
      mviw r5,0100h
      jsr do_sub16_util

      mviw r4,0009h
      mviw r5,0001h
      jsr do_sub16_util


      mviw r4,0220h
      mviw r5,0110h
      jsr do_sub16_util

      mviw r4,0001h
      mviw r5,0FFFCh
      jsr do_sub16_util

      ret

do_sub16_util:
      jsr showregs
      jsr do_sub16
      jsr showregs
      ret
;
; 16 bit subtract of r4 from r5, return result in r5
;
do_sub16:
      mvrha r4
      inva
      mvarh r4
      mvrla r4
      inva
      mvarl r4
      incr r4
      jsr showregs
      br do_add16
;
; pushr popr test
;
; requires testing changing r6 to r4
;
pushr_popr_tests:
         MVIW R3,0ff0h  ;setup to show stack
         movrr r1,r5    ;put a copy of stack ptr into r5
         JSR showaddr
         JSR show16
         JSR showregs
         MVIW R7,CRLF
         JSR stringout

         mviw R4,01234h ;put a value into R4
         movrr r1,r5
         MVIW R3,0ff0h
         JSR showaddr
         JSR show16
         JSR showregs
         MVIW R7,CRLF
         JSR stringout

         pushr r4
         movrr r1,r5
         MVIW R3,0ff0h
         JSR showaddr
         JSR show16
         JSR showregs
         MVIW R7,CRLF
         JSR stringout

         mviw r4,0h
         movrr r1,r5
         MVIW R3,0ff0h
         JSR showaddr
         JSR show16
         JSR showregs
         MVIW R7,CRLF
         JSR stringout

         popr r4
         movrr r1,r5
         MVIW R3,0ff0h
         JSR showaddr
         JSR show16
         JSR showregs
         MVIW R7,CRLF
         JSR stringout
         ret

;
; Register to Register move test
;
movrrtest:
        MVIW   R7,MOVRRHELP
        JSR    stringout

        MVIW R3,1234h
        MVIW R4,5678h

        jsr showreg34

        MOVRR R3,R4

        jsr showreg34

        MVIW R3,4321h

        jsr SHOWREG34

        MVIW R4,1234h
        MVIW R5,5678h

        jsr showreg45

        MOVRR R4,R5

        jsr showreg45

        MVIW R4,4321h

        jsr showreg45

        ret

;
; sho registers (hard coded)
;
showreg34:              ; registers across two boards
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

showreg45:             ;register on same board
        LDAI 0h
        JSR ledout
        MVRHA r4
        JSR TIL311out
        JSR switchtoggle
        LDAI 1h
        JSR ledout
        MVRLA r4
        JSR TIL311out
        JSR switchtoggle
        LDAI 2h
        JSR ledout
        MVRHA r5
        JSR TIL311out
        JSR switchtoggle
        LDAI 3h
        JSR ledout
        MVRLA r5
        JSR TIL311out
        JSR switchtoggle
        ret

;
; OR - OR accumulator immediate
;
ORHELP: DB "OR tests - or input switches with 0x55 (5x)",0ah,0dh,0
ortest:
         MVIW   R7,ORHELP
         JSR    stringout
         MVIB   R3,5
orloop:
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         ORI  055H
         OUTA  P1

         DECR R3
         MVRLA R3
         BRNZ orloop
         RET

;
; ADDI - ADD accumulator with immediate
;
additest:
       MVIW   R7,ADDIHELP
       JSR    stringout
       MVIB   R3,5
addiloop:
       JSR switchtoggle
       OUTI P0,(SWITCHLED)
       INP P1
       ADDI 02h
       OUTA  P1

       DECR R3
       MVRLA R3
       BRNZ addiloop
       RET

;
; ADDIC - ADD accumulator with immediate with carry
;
addictest:
      MVIW   R7,ADDICHELP
      JSR    stringout
      MVIB   R3,5
addicloop:
      JSR switchtoggle
      OUTI P0,(SWITCHLED)
      INP P1
      ADDIC 02h
      OUTA  P1

      DECR R3
      MVRLA R3
      BRNZ addicloop
      RET

;
; ORT - OR accumulator with tmp register
;
orttest:
        MVIW   R7,ORTHELP
        JSR    stringout
        MVIB   R3,5
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

        DECR R3
        MVRLA R3
        BRNZ ortloop
        RET
;
; push pop tests - push 3 values onto stack, pop 3values from stack
;
pushpoptest:
         MVIW   R7,PUSHPOPHELP
         JSR    stringout

         MVIB   R3,3
ppenterloop:
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         push

         DECR R3
         MVRLA R3
         BRNZ ppenterloop

         MVIB   R3,3
ppdisloop:
        JSR switchtoggle
        OUTI P0,(SWITCHLED)
        POP
        OUTA  P1
        DECR R3
        MVRLA R3
        BRNZ ppdisloop

        RET
;
; accumulator register Tests - move values between accumulator and register HI 8 bits
;                              move values between accumulator and register LO 8 bits
;
accumtest:
         MVIW   R7,accumhelp
         JSR    stringout
         MVIB   R3,10
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

         DECR R3
         MVRLA R3
         BRNZ accloop
         RET
;
; shift tests
;
; Shift left
;
SHIFT_LEFTHELP: DB "Shift Left - shift input switches (5x)",0ah,0dh,0
shltest:
         MVIW   R7,SHIFT_LEFTHELP
         JSR    stringout
         MVIB   R3,5
shlloop:
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         SHL
         OUTA  P1

         DECR R3
         MVRLA R3
         BRNZ shlloop
         RET
;
; shift Right
;
SHIFT_RIGHTHELP: DB "Shift Right - shift input switches (5x)",0ah,0dh,0
shrtest:
          MVIW   R7,SHIFT_RIGHTHELP
          JSR    stringout
          MVIB   R3,5
shrloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          SHR
          OUTA  P1

          DECR R3
          MVRLA R3
          BRNZ shrloop
          RET
;
; ring shift left
;
rshltest:
         MVIW   R7,RSHIFT_LEFTHELP
         JSR    stringout
         MVIB   R3,10
rshlloop:
         JSR switchtoggle
         OUTI P0,(SWITCHLED)
         INP P1
         RSHL
         OUTA  P1

         DECR R3
         MVRLA R3
         BRNZ rshlloop
         RET
;
; ring shift right
;
rshrtest:
          MVIW   R7,RSHIFT_RIGHTHELP
          JSR    stringout
          MVIB   R3,10
rshrloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          RSHR
          OUTA  P1

          DECR R3
          MVRLA R3
          BRNZ rshrloop
          RET
;
; shift right and propagate sign bit
;
pshrtest:
          MVIW   R7,PSHIFT_RIGHTHELP
          JSR    stringout
          MVIB   R3,10
pshrloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          PSHR
          OUTA  P1

          DECR R3
          MVRLA R3
          BRNZ pshrloop
          RET
;
; ring shift left through carry bit
;
cshltest:
          MVIW   R7,CSHIFT_LEFTHELP
          JSR    stringout
          MVIB   R3,10
cshlloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          CSHL
          OUTA  P1

          DECR R3
          MVRLA R3
          BRNZ cshlloop
          RET
;
; ring shift right through carry bit
;
cshrtest:
          MVIW   R7,CSHIFT_RIGHTHELP
          JSR    stringout
          MVIB   R3,10
cshrloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          CSHR
          OUTA  P1

          DECR R3
          MVRLA R3
          BRNZ cshrloop
          RET
;
; subtraction tests
;
subtest:
          MVIW   R7,SUBHELP
          JSR    stringout
          MVIB   R3,10
subloop:
          JSR switchtoggle
          OUTI P0,(SWITCHLED)
          INP P1
          SUBI  1
          OUTA  P1

          DECR R3
          MVRLA R3
          BRNZ subloop
          RET
;
; Compare Tests/compare input switches to 0x55 10 times
;
cmptest:
          MVIW   R7,COMPAREHELP
          JSR    stringout
          MVIB   R3,10
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
          DECR R3
          MVRLA R3
          BRNZ cmploop
          RET

;
; Monitor
;
; See help code below

;
; eumaltor eat 0x0a
;
;
; added for emulator eat cr
;
eat_nl:
;1      BRDEV eat_nl_done
;1      PUSH
;1      JSR uartin
;1      pop
eat_nl_done:
      ret
;
; Output Prompt
:

cmdloop:
      MVIW R7,PROMPT
      JSR stringout
;
;
; Input test
;
;intest:
;      JSR uartin
;      jsr showbytea
;      BR intest
;
; end test
;
      JSR uartin
      JSR toupper

;
; added for emulator eat cr
;
      jsr eat_nl
      LDTI 'H'
      BRNEQ testexamine
      MVIW R7,CRLF
      JSR stringout
      MVIW R7,helpmenu
      JSR stringout
      BR cmdloop

testexamine:
      LDTI '0'
      BREQ cmd_exit
      LDTI 'B'
      BREQ dumpblock
      LDTI 'C'
      BREQ cmd_basic_copy
      LDTI 'D'
      BREQ dump
      LDTI 'E'
      BREQ examine
      LDTI 'F'
      BREQ fillblock
      LDTI 'G'
      BREQ go
      ldti 'I'
      BREQ interpreter
      LDTI 'L'
      BREQ cmd_basiclist
      LDTI 'P'
      BREQ cmd_basicparse
      LDTI 'R'
      BREQ dumpreg
      LDTI 'T'
      BREQ tests
      LDTI 'Y'
      BREQ cmd_basic_test
      LDTI 'Z'
      BREQ cmd_basic
      LDTI 0Dh        ; hardware continue
      BREQ continue
;
; add for emulator
; hardware sends 0dh on CR but emulator sends 0dh 0ah
; code at top of loop eats the 0dh if running in emulator
; so continue on 0ah as well
;
      LDTI 0ah      ; emulator continue
      BREQ continue

      MVIW R7,CRLF

      JSR stringout

      MVIW R7,ERROR
      JSR stringout

      MVIW R7,helpmenu
      JSR stringout
      BR cmdloop
;
continue:

;       MVRLA R6
       LDA MONMODE

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
       MVIW R7,CONTINUEERROR
       JSR stringout
       BR cmdloop

stop:   BR stop

cmd_exit:
      BRDEV stop
      DB 0

cmd_basic:
        MVIW R7,CRLF
        JSR stringout
       jsr basic_run
       BR cmdloop

cmd_basicparse:
        ;build input string
        ;point register to BUFFER
        ;loop fetch chars
        ;until CR
        ;be sure line ends with a NULL or CR
        ;what does parse require???
        MVIW R7,BASIC_PARSEMSG
        JSR stringout

        mviw r3,line_buffer
parse_inputloop:
        jsr uartin
        stavr r3
        incr r3
        ldti 0ah  ;1 changed from 0a to 0D for new emulator code, changed back
;       halt
        brneq parse_inputloop
        mviw r7,line_buffer
        jsr show16
        mviw r7,line_buffer
        JSR BASIC_PARSE
        mviw r7,0400H
;        jsr show256
        BR cmdloop
do_parse:
        JSR basic_parse
        BR cmdloop

interpreter:
        JSR BASIC_INTERPRTER
        BR CMDLOOP

cmd_basiclist:
        MVIW R7,CRLF
        JSR stringout
        JSR basic_list
        BR cmdloop

cmd_basic_copy:
        MVIW R7,CRLF
        JSR stringout
        JSR basic_copy
        BR cmdloop

cmd_basic_test:
        MVIW R7,CRLF
        JSR stringout
        JSR basic_test
        BR cmdloop

dumpblock:
;      MVIB R6,BLOCKMODE
       LDTI BLOCKMODE
       STT monmode

       MVIW R7,DUMPBLOCKMSG
       JSR stringout
       jsr getaddress
       str r7,continue_addr
       MVIW R7,CRLF
       JSR stringout

dumpblockcont:
       ldr r7,continue_addr
       jsr show256
       str r7,continue_addr
       BR cmdloop
;
; dump 16 bytes on 16 byte boundry
;
dump:
;       MVIB R6,DUMPMODE
       LDTI DUMPMODE
       STT monmode
       MVIW R7,DUMPMSG
       JSR stringout
       jsr getaddress
       str r7,continue_addr
       MVIW R7,CRLF
       JSR stringout

dumpcont:
       ldr r7,continue_addr
       jsr showaddr
       jsr show16
       str r7,continue_addr
       BR cmdloop

examine:
;       MVIB R6,EXAMINEMODE
      LDTI EXAMINEMODE
      STT monmode
      MVIW R7,EXAMINEMSG
      JSR stringout
      jsr getaddress
      str r7,continue_addr
      MVIW R7,CRLF
      JSR stringout

examinecont:
      ldr r7,continue_addr
      JSR showaddr
      LDAI ' '
      JSR uartout

      JSR SHOWBYTE

      JSR uartin
      LDTI 01bh
      BREQ examdone
      LDTI '-'
      BREQ examdone
      LDTI 0dh
      BREQ examnext
      LDTI 0ah
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
      STAVR R7

examnext:
      INCR R7
      str r7,continue_addr
      LDAI 0ah
      JSR uartout
      LDAI 0dh
      JSR uartout
      BR examinecont

examdone:
      MVIW R7,CRLF
      JSR stringout
      BR cmdloop

fillblock:
;       MVIB R6,FILLMODE
       LDTI FILLMODE
       STT monmode

       MVIW R7,FILLMSG
       JSR stringout
       jsr getaddress
       STR r7,continue_addr
       MVIW R7,CRLF
       JSR stringout

fillcont:
      ldr r7,continue_addr
      jsr showaddr
      MVIW R7,CRLF
      JSR stringout
      ldr r7,continue_addr
morefill:
      LDAI 0
      STAVR R7
      INCR R7
      MVRLA R7
      ANDI  0FFH
      BRNZ morefill
      str r7,continue_addr
      LDAI 0ah
      JSR uartout
      LDAI 0dh
      JSR uartout
      BR cmdloop


go:
      MVIW R7,GOMSG
      JSR stringout
      jsr getaddress
      BRVR R7

dumpreg:
      JSR showregs
;      MVIB R6,NOMODE
      LDTI NOMODE
      STT monmode

      BR cmdloop

tests:
;      MVIB R6,NOMODE
      LDTI NOMODE
      STT monmode
      MVIW R7,CRLF
      JSR stringout
      MVIW R3,testmenu
      MVIW R4,0000h  ; counter

testsloop:

      INCR R3
      INCR R3
      LDAVR R3
      MVARH R7
      INCR  R3
      LDAVR R3
      MVARL R7
      INCR R3
      LDAVR R7
      LDTI '-'
      BREQ testsloopdone
      MVRLA R4
      JSR showbytea
      LDAI  '-'
      JSR uartout
      JSR stringout
      MVIW R7,CRLF
      JSR stringout
      INCR R4
      BR testsloop

testsloopdone:
; get test number
; multiple by 4 and add to test list base
; JSR via register holding info
;
      MVIW R7,gettestpromopt
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
      MVARH R7
      INCR  R3
      LDAVR R3
      MVARL R7
      JSRUR R7
      BR cmdloop

getaddress:
;
; Read 4 char address and return in R7
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
            MVARH R7

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
            MVARL R7
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
; value in accumulator convert to uppercase
;
toupper:  LDTI 'Z'
          BRGT lower
          RET
lower:
          SUBI 020h
          RET
;
; display R7 (old r3) followed by
; ":" and " " for showaddr and nothing for shownum
;
showaddr:   Push
            MVRHA R7
            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            MVRHA R7
            ANDI 0FH
            JSR shownibble
            MVRLA R7
            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            MVRLA R7
            ANDI 0FH
            JSR shownibble
            LDAI ':'
            JSR uartout
            LDAI ' '
            JSR uartout
            POP
            RET

shownum:
showr7:     Push
            MVRHA R7
            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            MVRHA R7
            ANDI 0FH
            JSR shownibble
            MVRLA R7
            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            MVRLA R7
            ANDI 0FH
            JSR shownibble
            POP
            RET
;
;
;
showregs:
            pushr r7
            pushr r7
            MVIW R7,CRLF
            JSR stringout
            MOVRR r0,r7
            jsr showaddr
            MOVRR r1,r7
            jsr showaddr
            MOVRR r2,r7
            jsr showaddr
            MOVRR r3,r7
            jsr showaddr
            MOVRR r4,r7
            jsr showaddr
            MOVRR r5,r7
            jsr showaddr
            MOVRR r6,r7
            jsr showaddr
            popr r7
            jsr showaddr
            push
            ldai ' '
            jsr uartout
            pop
            jsr showcarry

            MVIW R7,CRLF
            JSR stringout
            popr r7
            RET
;
; display upto 16 bytes point to by R7 (old r3), stops on a 16 byte boundry
; increments R7
;
show16:     JSR showbyte
            INCR R7
            LDAI ' '
            JSR uartout
            MVRLA R7
            ANDI 0FH
            BRNZ show16
            LDAI 0ah
            JSR uartout
            LDAI 0dh
            JSR uartout
            RET
;
; display upto 256 bytes point to by R7 (old r3),
; stops on a 256 byte boundry, increments R7
;
show256:
          push
show256loop:
          jsr showaddr
          jsr show16

;         MVIW R7,CRLF
;         JSR stringout

          MVRLA R7
          ANDI  0FFH
          BRNZ show256loop
          JSR uartout
          LDAI 0dh
          JSR uartout
          pop
          RET
;
; Output ASCII representation of a BYTE pointed to by R7 (OLD r7)
; or use showbytea in accumulator
; both destructive for accumulator - no longer true with push/pop
:
showbyte:   PUSH
            LDAVR R7
            SHR
            SHR
            SHR
            SHR
            JSR shownibble
            LDAVR R7
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
; Show carry flag
;
showcarry:
            Push
            brc show_yescarry
            ldai 'X'
            JSR uartout
            pop
            ret
show_yescarry:
            ldai 'C'
            JSR uartout
            pop
            ret

;
; Display nibble in accumulator ((this looks wrong) destructive)
;  destroys tmp register  - maybe add pusht - popt
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
;
TIL311out:
        OUTI  P0,(TIL311)
        OUTA  P1
        RET
;
; OLD: Output null terminated string pointed to by R2 to UART then send CR and LF
; Advances R2 to end of string
;
; Output null terminated string pointed to by R7 to UART
; Advances R7 to end of string
;
stringout:
        Push
sloop:
        LDAVR R7
        BRZ sloopdone
        JSR uartout
        INCR R7
        BR sloop
;
; (not done send CR and LF)
;
sloopdone:
        POP
        RET
;
; output accumulator to UART, wait for UART out available
;
charout:
uartout:
;
; add for emulator, outputs via putch
;
        BRDEV emulator2
        outa p2
        ret
;
emulator2:
        PUSH
        push
;
; doubt 2nd push pop is needed, to be tested
;
uartoutw:
;
; test uart out is available
;
        OUTI  P0,(UARTCS!UARTA5)
        INP   p1
        ANDI  040h
        BRZ   uartoutw
        POP
        OUTI  P0,UARTCS
        OUTA  P1
;
; may not be needed
       Pop
       RET

;
; wait for UART character available then input to accumulator
;
; Looks like this echos out character
; should this be settable via a flag
;
uartin:
;
; added for emulator, emulator P2 reads a char via getch
;
        BRDEV emulator3
        inp p2
        ret
;
emulator3:
;
; wait for a charater available at input
;
        OUTI  P0,(UARTCS!UARTA5)
        INP   p1
        ANDI  01h
        BRZ   uartin
        OUTI  P0,(UARTCS)
        INP   P1
        ldti 0dh          ; cobvert 0x0d to 0x0a
        brneq uartinc
        ldai 0ah
uartinc:
        JSR   LEDOUT
;
; emulator
;
;        ldti  0ah
;        breq uartin
        JSR   uartout
        RET
;
; long delay (approx 5 seconds)
; destroys r7
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
; destroys R7
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

;
; toggle input switch (with debounce)
; destroys r7
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
;
; added for emulator, return immediately to skip counting
; destroys r7
;
;       ret
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
;
; emulator change, return immediately to skip counting
; destroys r7
;
;       ret
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
; emulator change, return immediately to skip counting
;
;    ret
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
DUMPMSG: DB 0ah,0dh,"DUMP ADDR:",0
DUMPBLOCKMSG: DB 0ah,0dh,"DUMP BLOCK ADDR:",0
FILLMSG: DB 0ah,0dh,"FILL BLOCK ADDR:",0
GOMSG: DB 0ah,0dh,"GO ADDRESS:",0
EXAMINEMSG: DB 0ah,0Dh,"EXAMINE ADDRESS:",0
CONTMSG: DB "CONTINUE MODE",0
gettestpromopt: DB "Enter Test number:",0
BASIC_PARSEMSG: DB 0ah,0dh,"Enter Line:",0
;
helpmenu:
DB "0      - Exit (emulator only)",0ah,0dh
DB "H      - This help menu",0ah,0dh,0ah,0dh
DB "B AAAA - Show 256 bytes of memory (16 byte aligned)",0ah,0dh
DB "         CR display next 256 bytes",0ah,0dh
DB "C      - Copy BASIC test program into interpreter buffer",0ah,0dh
DB "D AAAA - Show 16 bytes of memory at (16 byte aligned)",0ah,0dh
DB "         CR display next 16 bytes",0ah,0dh
DB "E AAAA - show contents of location AAAA (Output AAAA:XX)",0ah,0dh
DB "         if followed by ASCII-HEX modify location with new value (and redisplay)",0ah,0DH
DB "         if followed by CR display next location",0ah,0dh
DB "F AAAA   Fill contents 256 bytes of memory at address AAAA with 0(16 byte aligned) with 0",0ah,0dh
DB "         if followed by CR fill next 256 bytes",0ah,0dh
DB "G AAAA - Jump to (and execute) starting at AAAA",0ah,0dh
DB "         code could end in BR to 0xf000h to restart monitor or RET if called via JSR",0ah,0dh
DB "I      - BASIC Interpreter",0ah,0dh 
DB "L      - List BASIC program",0ah,0dh
DB "P      - Enter program line to BASIC",0ah,0dh
DB "R      - Show registers",0ah,0dh
DB "T      - Test menu",0ah,0DH
DB "Y      - run BASIC test code",0ah,0DH
DB "Z      - Run program with Basic interpreter",0ah,0DH
DB 0
;
; TEST HELP MESSAGES
;
COMPAREHELP: DB "Compare Tests",0ah,0dh,0
RSHIFT_LEFTHELP: DB "Ring Shift Left",0ah,0dh,0
RSHIFT_RIGHTHELP: DB "Ring Shift Right",0ah,0dh,0
PSHIFT_RIGHTHELP: DB "PROP Shift Right",0ah,0dh,0
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
      DW pshrtest,pshrmenu
      DW subtest,submenu
      DW cmptest,cmpmenu
      DW additest,addimenu
      DW addictest,addicmenu
      DW movrrtest,movrrmenu
      DW add16tests,add16menu
      DW mem_indirect_tests,mem_indirect_menu
      DW mul16,mul16menu
      DW pushr_popr_tests,pushr_popr_menu
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
pshrmenu: DB "PSHR",0
submenu: DB "SUB",0
cmpmenu: DB "Compare Branch",0
addimenu: DB "ADDI",0
addicmenu: DB "ADDI",0
movrrmenu: DB "MOVRR",0
add16menu: DB "add16",0
mem_indirect_menu: DB "mem indirect",0
mul16menu: DB "mul16",0
pushr_popr_menu: DB "pushr popr",0
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
;      JSRUR R2

;
; BIOS ENTRY Points
;
    org 0ffc0h

e_stringout:
    jsr stringout
    ret
e_charout:
    jsr charout
    ret
e_uartout:
    jsr uartout
    ret
e_showaddr:
    jsr showaddr
    ret
e_toupper:
    jsr toupper
    ret
e_showr7:
    jsr showr7
    ret
e_showbyte:
    jsr showbyte
    ret
e_ showregs:
    jsr SHOWREGS
    ret
e_showbytea:
    jsr showbytea
    ret
e_showcarry:
    jsr showcarry
    ret
e_uartin:
    jsr uartin
    ret
;
; The End
;

ZZZZ:
  DB   0
