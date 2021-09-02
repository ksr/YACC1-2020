;
; BIOS Entry Points
;
stringout:  EQU 0ffc0h
charout:    EQU 0ffc4h
uartout:    EQU 0ffc8h
showaddr:   EQU 0ffcch
toupper:    EQU 0ffd0h
showr7:     EQU 0ffd4h
showbyte:   equ 0ffd8h
showregs:   equ 0ffdch
showbytea:  equ 0ffe0h
showcarry:  equ 0ffe4h
uartin:     equ 0ffe8h
charavail:  equ 0ffech

;
; Basic interpreter tokens
;
TOKENIZER_ERROR: EQU 00
TOKENIZER_EOF: EQU 01
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
TOKENIZER_LEFTP: EQU 31
TOKENIZER_RIGHTP: EQU 32
TOKENIZER_LT: EQU 33
TOKENIZER_GT: EQU 34
TOKENIZER_EQ: EQU 35
TOKENIZER_CR: EQU 36
TOKENIZER_LINENUM: EQU 37
TOKENIZER_LIST: EQU 38
TOKENIZER_RUN: EQU 39
TOKENIZER_NEW: EQU 40
TOKENIZER_EXIT: EQU 41
TOKENIZER_INPUT: EQU 42
TOKENIZER_INP: EQU 43
TOKENIZER_OUTP: EQU 44
TOKENIZER_ON: EQU 45
TOKENIZER_OFF: EQU 46


;
; BASIC interpreter user variables area
; currently 26 1-byte variables
; for now must be 0xAA00 (256 byte) aligned
;
BASIC_VARS: EQU 0100h

;
; Basic interpreter internal variables
;
bas_run_ended:    EQU 0200h ; set to 1 when current "run" is done
bas_txtptr:       EQU 0202h ; pointer into text line being parse
bas_nxtptr:       EQU 0204h ;
bas_tokptr:       EQU 0206h ; pointer into tmp token buffer
bas_tokcounter:   EQU 0208h ; counter for tmp token buffer
bas_currenttoken: EQU 020Ah ; used in parsing
bas_tokenbuffer:  EQU 020Ch ; Pointer to start of token buffer
bas_bufferendhi:  EQU 020Eh ; HACK, above buffer should end on 0xXXFF boundry
                            ; this value should be + 1 end of buffer
                            ; if buffer is 0x1000-0x1fff this is
                            ; set to 0x2000. Used in moving buffer around
                            ; in add and remove line
bas_newlinenum:   EQU 0210h ; line number of new line to be added
bas_newlinelen:   EQU 0212h ; line length of new line to be added
bas_insertptr:    EQU 0214H
bas_negflag:      EQU 0216H ; PROCESS NEGATIVE NUMBER

;
; FOR-NEXT statement stack ptr and stack data
;
; for next stack data
; format
;   2 bytes for variable id (only first byte used for now)
;   2 bytes to value (upper value) (format HL) (only first byte used for now)
;   2 bytes ptr to line after for instruction (format HL)
; later add step amount
;
bas_forstackptr: EQU 0280h
bas_forstack: EQU 0282h

;
; GOSUB stack ptr and data
;
; gosub STACK
; format
;   2 bytes return location ptr
;
bas_gosubptr: EQU 02c0h
bas_gosubstack: EQU 02c2h

;
; Basic interpreter text input line buffer
;
parse_input_line: EQU 300h
;
; Basic interpreter tmp buffer to build up current token line
;
parse_token_buffer: EQU 400h
;
; Basic interpreter token buffer
;
bas_tok_buf_start: EQU 1000h
;
; Basic interpreter token buffer end + 1
;
bas_tok_buf_end: EQU 2000h
; OR
;bas_tok_buf_end: EQU 1100h ;smaller for easy debugging - 256 byte buffer

;
; basic interpreter, add IO and peek/poke
;

;
; Basic Interpreter
;

;
; BASIC Entry Points
;
  ORG 0e000h
;
; list
;
      ldr r7,bas_tokenbuffer
      JSR  basic_list
      Ret

;
      ORG 0e010h
;
; run
;
      jsr basic_run
      Ret

;
      ORG 0e020h
;
; cold start
;
; Hack later pass in R7
;
      MVIW R7,bas_tok_buf_start
      ldai TOKENIZER_EOF          ;write EOF to start of buffer
      stavr r7
      STR R7,bas_tokenbuffer
      mviw r7,bas_tok_buf_end
      str r7,bas_bufferendhi
      Ret

;
      ORG 0e030h
;
; tests - called from monitor - used to test snippets of code
;
      jsr bigtest
      ret
;
      org 0e040h
;
; very basic cmd line driver
;
      jsr basic_interprter
      ret
;
      org 0e050h
;
; parse line
;
      jsr parse_line
      ret

;
      ORG 0e060h
;
; Copy test program into token Buffer, copies 0x0400 bytes
;
      mviw R7,bas_tok_buf_start
      mviw R6,BASIC_TEST
      mviw R5,0400h
;
bas_copyloop:
      ldavr R6
      stavr r7
      incr r6
      incr r7
      decr r5
      mvrha r5
      brnz bas_copyloop
      mvrla r5
      brnz bas_copyloop
      ret

bigtest:
    mviw r5,0105
    mviw r4,0010
    jsr parse_div16
    jsr showregs
    mviw r7,CRLF
    jsr stringout

    mviw r5,0105
    mviw r4,0010
    jsr parse_mod16
    jsr showregs
    mviw r7,CRLF
    jsr stringout

    mviw r7,01234h
    jsr exe_itoa
    mviw r7,CRLF
    jsr stringout


    ret

;
; basic interpreter messages
;
bas_msg1: db "accept - unexpected token",0,0ah,0dh
bas_msg2: db "line not found",0,0ah,0dh
bas_msg3: db "token not supported ",0ah,0dh
exe_stmt_msg: db "EXE STMT ",0
parse_cmd_error: db "UNKNOWN COMMAND",0,0ah,0DH

;
; Basic interpreter - execution engine
; Register Usage
; R0 - Program Counter
; R1 - Stack Pointer
; R2 - Direct addressing mode
; R3 - Basic Interpreter Token Buffer ptr
; R4 - working register  ; or is this for-next stack ptr
; R5 - working register  ; or is this gosub stack ptr
; R6 - Working register
; R7 - parameter passing & return value
;
;exe: Is this label needed
;
; void ubasic_init()
;
; Setup basic interpreter execution engine
;
exe_init:
    push
    MVIW R5,bas_gosubstack
    MVIW R4,bas_forstack
    MVIW R6,bas_run_ended
    LDAI 0
    STAVR R6
    LDR R3,bas_tokenbuffer
    pop
    RET

;
; void accept(int)
;
; compare accumulator with current token then advance to next token in buffer
; assume accumulator destroyed
;
exe_accept:
    push
;
; get current token into accumulator
;
; FIX: Switch to BRNEQ to error state and save a branch
;
    LDAVR R3
    MVAT
    POP
    BREQ exe_accept_done
;
; debug information
;
    JSR showbytea
    ldai '#'
    JSR uartout
    mviw R7,bas_msg1
    JSR bas_error
;
exe_accept_done:
    jsr exe_next_token
    RET

;
; int varfactor()
;
; get variable id pointed to by exe_variable_num in token stream
; (this is not a value but a variable id)
; value returned by exe_get_variable in r7
;
; return value in R7
;
exe_varfactor:
    jsr exe_variable_num    ; get variable number id (one byte for now a-z)
                            ;into accumulator
;
; variable id in accumulator, return value in r7
;
    jsr exe_get_variable
    LDAI TOKENIZER_VARIABLE
    JSR exe_accept
    RET

;
; int factor()
; return value in R7
;
; IS MOVE T TO A NEEDED IN ALL THESE :
;
exe_factor:
;
    LDAVR R3               ;get current token into accumulator
    LDTI TOKENIZER_PLUS
    BRNEQ exe_factor1A
    JSR exe_accept
    JSR EXE_FACTOR
    MVTA
    RET

exe_factor1A:
    LDAVR R3               ;get current token into accumulator
    LDTI TOKENIZER_MINUS
    BRNEQ exe_factor1B
    JSR exe_accept
    JSR EXE_FACTOR
    JSR PARSE_NEG
    MVTA
    RET

exe_factor1B:
    LDAVR R3               ;get current token into accumulator
    LDTI TOKENIZER_NUMBER
    BRNEQ exe_factor1
    JSR exe_num
    MVTA
    JSR exe_accept
    RET

exe_factor1:
    LDTI TOKENIZER_LEFTP
    BRNEQ exe_factor2
    JSR exe_accept
    JSR exe_expr
    LDAI TOKENIZER_RIGHTP
    JSR exe_accept
    RET

exe_factor2:
    jsr exe_varfactor
    ret

;
; int term()
; R5 = F1
; R6 = F2
; R7 = return value
;
exe_term:
    pushr r5
    pushr r6

    jsr exe_factor
    MOVRR r7,r5

exe_term_loop:
    LDAVR R3
    ldti TOKENIZER_ASTR
    BREQ exe_term_astr
    LDTI TOKENIZER_SLASH
    breq exe_term_slash
    ldti TOKENIZER_MOD
    breq exe_term_mod
    br exe_term_done

exe_term_astr:
    jsr exe_next_token
    jsr exe_factor
    movrr r7,r6
;
; do mulitply f1 = f1 * f2
;
    pushr r4
    movrr r6,r4
    jsr PARSE_MUL16
    movrr r7,r5
    popr r4
    BR exe_term_loop

exe_term_slash:
    jsr exe_next_token
    jsr exe_factor
    movrr r7,r6
;
; do divide f1 = f1 / f2
;
    pushr r4
    movrr r6,r4
    jsr PARSE_DIV16
    movrr r7,r5
    popr r4
    BR exe_term_loop

exe_term_mod:
    jsr exe_next_token
    jsr exe_factor
    movrr r7,r6
;
; f1 = f1 % f2
;
    pushr r4
    movrr r6,r4
    jsr PARSE_MOD16
    movrr r7,r5
    popr r4
    BR exe_term_loop

exe_term_done:
    MOVRR r5,r7
    popr r6
    popr r5
    ret

;
; static VARIABLE_TYPE expr()
;
; R5 = t1
; R6 = t2
; R7 = return value
;
exe_expr:
    pushr r5
    pushr r6

    jsr exe_term
    MOVRR r7,r5

exe_expr_loop:
    LDAVR R3
    ldtI TOKENIZER_PLUS
    BREQ exe_expr_plus
    ldtI TOKENIZER_MINUS
    breq exe_expr_minus
    ldtI TOKENIZER_AND
    breq exe_expr_and
    ldti TOKENIZER_OR
    breq exe_expr_or

    br exe_expr_done

exe_expr_plus:
    jsr exe_next_token
    jsr exe_term
    movrr r7,r6
;
; t1 = t1 + t2
;
    movrr r5,r7
    jsr parse_add16
    movrr r7,r5
    BR exe_expr_loop

exe_expr_minus:
    jsr exe_next_token
    jsr exe_term
    movrr r7,r6
;
; t1 = t1 - t2
;
    movrr r5,r7
    jsr parse_sub16
    movrr r7,r5
    BR exe_expr_loop

exe_expr_and:
    jsr exe_next_token
    jsr exe_term
    movrr r7,r6
;
; t1 = t1 & t2
;
    movrr r5,r7
    jsr parse_and16
    movrr r7,r5
    BR exe_expr_loop

exe_expr_or:
    jsr exe_next_token
    jsr exe_term
    movrr r7,r6
;
; t1 = t1 | t2
;
    movrr r5,r7
    jsr parse_or16
    movrr r7,r5
    BR exe_expr_loop
    BR exe_expr_loop

exe_expr_done:
    MOVRR r5,r7
    popr r6
    popr r5
    ret

;
; int relation ()
;
; R5 = r1
; R6 = r2
; R7 = return value
;
exe_relation:
    pushr r5
    pushr r6

    jsr exe_expr
    MOVRR r7,r5

exe_relation_loop:
    LDAVR R3
    LDTI TOKENIZER_LT
    BREQ exe_relation_lt
    ldti TOKENIZER_GT
    breq exe_relation_gt
    ldti TOKENIZER_EQ
    breq exe_relation_eq

    br exe_relation_done

exe_relation_lt:
    jsr exe_next_token
    jsr exe_expr
    movrr r7,r6
;
; r1 = r1 < r2
;
    movrr  r5,r7
    jsr parse_compare
    ldti 0ffh
    BRNEQ exe_rel_nlt
    mviw r5,01h
    BR exe_relation_loop
exe_rel_nlt:
    mviw r5,00h
    BR exe_relation_loop

exe_relation_gt:
    jsr exe_next_token
    jsr exe_expr
    movrr r7,r6
;
; r1 = r1 > r2
;
    movrr  r5,r7
    jsr parse_compare
    ldti 01h
    BRNEQ exe_rel_ngt
    mviw r5,01h
    BR exe_relation_loop
exe_rel_ngt:
    mviw r5,00h
    BR exe_relation_loop

exe_relation_eq:
    jsr exe_next_token
    jsr exe_expr
    movrr r7,r6
;
; r1 = r1 == r2
;
    movrr  r5,r7
    jsr parse_compare
    brnz exe_rel_ne
    mviw r5,01h
    BR exe_relation_loop
exe_rel_ne:
    mviw r5,00h
    BR exe_relation_loop

exe_relation_done:
    MOVRR r5,r7
    popr r6
    popr r5
    ret

;
; char * index_find (int linenum)
;
; call with line in R7
; return with val in R7 - tokenbuffer ptr
; THIS CAN BE REMOVED SINCE IT IS ONLY A SINGLE CALL
;
exe_index_find:
    JSR basu_find
    RET

;
; void jump_linenum(linenum)
;
; Linenum in R7
;
exe_jump_line:
    JSR exe_index_find
    MVRHA R7
    BRNZ exe_jump_line1
    MVRLA R7
    BRNZ exe_jump_line1
;
; returned 0 line not found
;
    LDAI TOKENIZER_CR
    JSR exe_accept
    MVIW r6,bas_run_ended
    LDAI 1
    STAVR R6
    ret
exe_jump_line1:
;   jsr tok_goto ; WHAT IS THIS
    movrr r7,r3
    ret

;
; void goto_statement()
;
exe_goto_stmt:
    LDAI TOKENIZER_GOTO
    JSR exe_accept
    JSR exe_expr
    JSR exe_jump_line
    RET

;
; void print_statment()
;
exe_print_stmt:
    LDAI TOKENIZER_PRINT
    jsr exe_accept

exe_print_stmt_loop:
    LDAVR R3
    LDTI TOKENIZER_STRING
    BRNEQ exe_print_stmt1
;
; this should return string to print in r2
; exe_string may not be needed tokenbufferptr is at string ?
;
    movrr r3,r7
    incr r7
    jsr stringout
    jsr exe_next_token
    br exe_print_stmt_test

exe_print_stmt1:
    ldti TOKENIZER_COMMA
    BRNEQ exe_print_stmt2
    LDAI ' '
    JSR charout
    jsr exe_next_token
    br exe_print_stmt_test

exe_print_stmt2:
    ldti TOKENIZER_SEMICOLON
    BRNEQ exe_print_stmt3
    jsr exe_next_token
    br exe_print_stmt_test

exe_print_stmt3:
    ldti TOKENIZER_VARIABLE
    BREQ exe_print_stmt4
    ldti TOKENIZER_NUMBER
    breq exe_print_stmt4
    ldti  TOKENIZER_LEFTP
    breq exe_print_stmt4

    br exe_print_stmt_done

exe_print_stmt4:
    jsr exe_expr
    ;JSR showr7
    jsr exe_itoa

exe_print_stmt_test:
    LDAVR R3
    LDTI TOKENIZER_CR
    breq exe_print_stmt_done
    LDTI TOKENIZER_EOF
    breq exe_print_stmt_done
    br exe_print_stmt_loop

exe_print_stmt_done:
    mviw R7,CRLF
    jsr stringout
    jsr exe_next_token
    ret

;
; void if_statement()
;
exe_if_stmt:
    LDAI TOKENIZER_IF
    JSR exe_accept

    JSR exe_relation
    MVRLA R7
    push
    LDAI TOKENIZER_THEN
    JSR exe_accept
    pop

    LDTI 1
    BRNEQ exe_if_stmt1
    JSR exe_stmt
    ret

exe_if_stmt1:
    jsr exe_next_token
    LDAVR R3
    LDTI TOKENIZER_CR
    BREQ EXE_IF_STMT2
    LDTI TOKENIZER_EOF
    BRNEQ EXE_IF_STMT1
    RET

exe_if_stmt2:
    JSR EXE_NEXT_TOKEN
    RET

;
; void let_statement()
;
exe_let_stmt:
    jsr exe_variable_num
    mvarl r7
    movrr r7,r6
    ldai TOKENIZER_VARIABLE
    jsr exe_accept
    ldai TOKENIZER_EQ
    jsr exe_accept
    jsr exe_expr
    mvrla r6
    JSR exe_set_variable
    ldai TOKENIZER_CR
    jsr exe_accept
    ret

;
; void gosub_statement()
;
exe_gosub_stmt:
    ldai TOKENIZER_GOSUB
    jsr exe_accept
    jsr exe_num
    ldai TOKENIZER_NUMBER
    jsr exe_accept
    ldai TOKENIZER_CR
    jsr exe_accept
;
; save current token buffer ptr to gosub stack
;
   mvrla r3
   stavr r5
   incr r5
   mvrha r3
   stavr r5
   incr  r5
;
; write new gosub stack ptr location back
;
   JSR exe_jump_line
   RET

;
; void return_statment()
;
exe_return_stmt:
    ldai TOKENIZER_RETURN
    jsr exe_accept
;
; setup to use for storage area via R4
;
   decr r5
   decr r5

   ldavr r5
   mvarl r3
   incr r5
   ldavr r5
   mvarh r3

   decr r5
   ret

;
; void next_statement()
;
exe_next_stmt:
;
; backup to the TO Value
    decr r4
    decr r4
    decr r4
    decr r4
;
; eat next
;
    ldai TOKENIZER_NEXT
    jsr exe_accept
;
; get variable id (in accumulator) and hold in r6 lo
;
    jsr exe_variable_num
    mvarl r6
;
; eat TOKENIZER_VARIABLE id
;
    ldai TOKENIZER_VARIABLE
    jsr exe_accept
;
; get variable value, inc by 1 and store - ADD STEP VALUE
;
    mvrla r6
    jsr exe_get_variable
    incr r7
    mvrla r6
    jsr exe_set_variable
;
; get TO value into R6
    ldavr r4
    mvarl r6
    incr r4
    ldavr r4
    mvarh r6
    incr r4
;
; for now only compare low byte HACK use compare code
; r7=r6 00
; r7>r6 01
; r7<r6 ff
;
     jsr parse_compare
     ldti 01
     breq exe_next_done
;    mvrla r6
;    mvat
;    mvrla r7
;    brgt exe_next_done
;
    ldavr r4
    mvarl r3
    incr r4
    ldavr r4
    mvarh r3
    incr r4
    ret

exe_next_done:
  decr r4
  decr r4
  decr r4
  decr r4
  ldai TOKENIZER_CR
  jsr exe_accept
  ret

;
; void for_statement()
;
exe_for_stmt:
;
; setup to use for storage area via R4
;
    ldai TOKENIZER_FOR    ; eat FOR
    jsr exe_accept
;
; get variable id (in accumulator) and hold in r6 lo
;
    jsr exe_variable_num
    mvarl r6
;
; store variable id in FOR storage - hack only lo byte, hi byte 0
;
    stavr r4
    incr r4
    ldai 0
    stavr r4
    incr r4
;
; eat variable id and = then get starting value
;
    ldai TOKENIZER_VARIABLE
    jsr exe_accept
    ldai TOKENIZER_EQ
    jsr exe_accept
    jsr exe_expr
;
; set variable to starting value
;
    mvrla r6
    JSR exe_set_variable
;
; eat TO and get to value
;
    ldai TOKENIZER_TO
    jsr exe_accept
    jsr exe_expr
;
; store to value in FOR storage area
;
    mvrla r7
    stavr r4
    incr r4
    mvrha r7
    stavr r4
    incr r4
;
; EAT EOL
;
    ldai TOKENIZER_CR
    jsr exe_accept
;
; store ptr to instruction after FOR in for storage area
;
    mvrla r3
    stavr r4
    incr r4
    mvrha r3
    stavr r4
    incr r4
;
;   save for stackptr
;
    ret

;
; INP
;
exe_inp_stmt:
    ldai TOKENIZER_INP   ; eat INP
    jsr exe_accept
    ret

;
; outp
;
exe_outp_stmt:
    ldai TOKENIZER_OUTP   ; eat OUTP
    jsr exe_accept
    ret

;
; INPUT
;
exe_input_stmt:
    ldai TOKENIZER_INPUT   ; eat INPUT
    jsr exe_accept
    jsr exe_variable_num
    Push

    mviw r7,parse_input_line

    jsr get_inputline

    mviw r7,parse_input_line
    str r7,bas_txtptr


    jsr parse_num
    pop
    jsr  EXE_SET_VARIABLE

    ldai TOKENIZER_VARIABLE
    jsr EXE_ACCEPT
    ldai TOKENIZER_CR
    jsr exe_accept
    RET
;
; void peek_statment()
;
;peek_statement(void) {
;
exe_peek_stmt:
    ldai TOKENIZER_PEEK   ; eat PEEK
    jsr exe_accept
    jsr exe_expr
    LDAVR R7
    MVARL R7
    LDAI 0
    MVARH R7
    PUSHR R7

    ldai TOKENIZER_COMMA   ; eat COMMA
    jsr exe_accept
    jsr exe_variable_num
    popr r7

    jsr  EXE_SET_VARIABLE
    ldai TOKENIZER_VARIABLE
    jsr EXE_ACCEPT
    ldai TOKENIZER_CR
    jsr exe_accept
    RET

;
; void poke_statement()
;
;poke_statement(void) {
;    VARIABLE_TYPE poke_addr;
;    VARIABLE_TYPE value;
;
;    accept(TOKENIZER_POKE);
;    poke_addr = expr();
;    accept(TOKENIZER_COMMA);
;    value = expr();
;    accept(TOKENIZER_CR);

;    poke_function(poke_addr, value);

exe_poke_stmt:
    ldai TOKENIZER_POKE   ; eat PEEK
    jsr exe_accept

    jsr exe_expr
    PUSHR R7

    ldai TOKENIZER_COMMA   ; eat COMMA
    jsr exe_accept

    jsr exe_expr
    MVRLA R7

    POPR R7

    STAVR R7

    ldai TOKENIZER_CR
    jsr exe_accept
    RET

;
; void end_statement()
;
exe_end_stmt:
    mviw r6,bas_run_ended
    LDAI 1
    STAVR r6
    ret

;
; void statment()
;
exe_stmt:
    LDAVR R3

    LDTI TOKENIZER_VARIABLE
    BRNEQ exe_stmt1
    JSR exe_let_stmt
    ret

exe_stmt1:
    LDTI TOKENIZER_IF
    BRNEQ exe_stmt2
    JSR exe_if_stmt
    ret

exe_stmt2:
    LDTI TOKENIZER_NEXT
    BRNEQ exe_stmt3
    JSR exe_next_stmt
    ret

exe_stmt3:
    LDTI TOKENIZER_PRINT
    BRNEQ exe_stmt4
    JSR exe_print_stmt
    ret

exe_stmt4:
    LDTI TOKENIZER_FOR
    BRNEQ exe_stmt5
    JSR exe_for_stmt
    ret

exe_stmt5:
    LDTI TOKENIZER_GOTO
    BRNEQ exe_stmt5a
    JSR exe_goto_stmt
    ret

exe_stmt5a:
    LDTI TOKENIZER_ON
    BRNEQ exe_stmt5b
    ldai TOKENIZER_ON
    jsr exe_accept
    ldai TOKENIZER_CR
    jsr exe_accept
    ON
    ret

exe_stmt5b:
    LDTI TOKENIZER_OFF
    BRNEQ exe_stmt6
    ldai TOKENIZER_OFF
    jsr exe_accept
    ldai TOKENIZER_CR
    jsr exe_accept
    OFF
    ret

exe_stmt6:
    LDTI TOKENIZER_INPUT
    BRNEQ exe_stmt7
    JSR exe_input_stmt
    ret

exe_stmt7:
    LDTI TOKENIZER_GOSUB
    BRNEQ exe_stmt8
    JSR exe_gosub_stmt
    ret

exe_stmt8:
    LDTI TOKENIZER_RETURN
    BRNEQ exe_stmt9
    JSR exe_return_stmt
    ret

exe_stmt9:
    LDTI TOKENIZER_PEEK
    BRNEQ exe_stmt10
    JSR exe_peek_stmt
    ret

exe_stmt10:
    LDTI TOKENIZER_POKE
    BRNEQ exe_stmt11
    JSR exe_poke_stmt
    ret

exe_stmt11:
    LDTI TOKENIZER_END
    BRNEQ exe_stmt12
    JSR exe_end_stmt
    ret

exe_stmt12:
    LDTI TOKENIZER_LET
    BRNEQ exe_stmt13
    LDAI TOKENIZER_LET
    JSR exe_accept
    JSR exe_let_stmt
    ret

exe_stmt13:
    LDTI TOKENIZER_INP
    BRNEQ exe_stmt14
    JSR exe_inp_stmt
    ret

exe_stmt14:
    LDTI TOKENIZER_OUTP
    BRNEQ exe_stmt15
    JSR exe_outp_stmt
    ret

exe_stmt15:
    jsr showbytea
    ldai '='
    jsr uartout
    MVIW R7,bas_msg3
    jsr stringout
    JSR SHOWREGS
    jsr bas_error

;
; void line_statement (void)
;
; IS LINENUM NEEDED? CAN we skip over here?
;
exe_line_stmt:
    LDAI TOKENIZER_LINENUM
    jsr exe_accept
    jsr exe_stmt
    ret

;
; void ubasic_run()
;
basic_run:
    JSR exe_init

exe_run:
    jsr exe_finished
    LDTI 1
    BRNEQ exe_run_cont
    ret

exe_run_cont:
    jsr exe_line_stmt
    BR exe_run

;
; int ubasic_finished()
;
exe_finished:
;    MVIW R2,bas_run_ended
;    LDAVR R2
    JSR test_input
    LDA bas_run_ended
    LDTI 1
    BREQ exe_finished_yes

    ldavr r3
    ldti TOKENIZER_EOF
    breq exe_finished_yes
    ldai 0
    ret

exe_finished_yes:
    LDAI 1
    ret

;
; void ubasic_set_variable(int varnum, VARIABLE_TYPE value)
;
; R7 value (only using low byte)
; ACCUMULATOR Variable ref number
;
; Destroys R2 and accumulator, could be fixed with a push/pop
; DANGER USING R2 - NOW FOR MEM-DIRECT ADDRESSING SWITCH TO ANOTHER R ?
;
exe_set_variable:
    MVIW R2,BASIC_VARS  ; SEE COMMENT below about memory alignment
    SHL                 ; memory location x 2 (2 byte vars)
    MVARL R2            ; change to add16 to remove align issue
;
    MVRLA R7
    STAVR R2
    INCR r2
    mvrha r7
    stavr r2

    RET
;
; VARIABLE_TYPE ubasic_get_variable(int varnum)
;
; ACCUMULATOR HOLDS VARIABLE REF NUMBER, VALUE RETURN IN R7
;
; HACK for now BASIC_VARS needs to be 256 byte 0xAA00 aligned
; and only 0-25 (var names a-z) supported
; for now values are 1 byte, upper byte of R7 forced to 0
;
; Destroys R2 and accumulator, could be fixed with a push/pop
;
exe_get_variable:
    MVIW R2,BASIC_VARS
    shl
    MVARL R2
;
    LDAVR R2
    MVARL R7
    INCR R2
    LDAVR R2
    MVARH R7
    RET

;
; hack this needs work
;
bas_error:
  HALT
  RET

;
; Tokenizer execute support code
;
; tokenizer_next - exe version
;
exe_next_token:
  ldavr r3
  ldti TOKENIZER_NUMBER
  BRNEQ exe_next_token1
  incr r3
  incr r3
  incr r3
  RET

exe_next_token1:
  ldti TOKENIZER_VARIABLE
  BRNEQ exe_next_token2
  incr r3
  incr r3
  incr r3
  RET

exe_next_token2:
  ldti TOKENIZER_LINENUM
  BRNEQ exe_next_token3
  incr r3
  incr r3
  incr r3
  incr r3
  incr r3
  RET

exe_next_token3:
  ldti TOKENIZER_STRING
  BRNEQ exe_next_token4
  incr r3

exe_next_tok3_loop:
  LDAVR R3
  BRZ exe_next_tok3_done
  incr r3
  BR exe_next_tok3_loop

exe_next_tok3_done:
  incr r3
  RET

exe_next_token4:
  incr r3
  ret

;
; numbers stored low byte followed by high byte in memory
;
exe_num:
    incr r3
    ldavr r3
    mvarl r7
    incr r3
    ldavr r3
    mvarh r7
    decr r3
    decr r3
    ret

;
; hack only using low byte of id for now
; return in accumulator
;
exe_variable_num:
    incr r3
    ldavr r3
    decr r3
    ret


;
; Convert number in r7 to ASCII
;
; for for negative numbers
;
dividers: DW 02710h,03e8h,0064h,000Ah,0000h

exe_itoa:
      push
      mvrha r7
      andi 080h
      brz exe_itoa_pos

      mvrha r7
      inva
      mvarh r7
      mvrla r7
      inva
      mvarl r7
      incr r7
      ldai '-'
      jsr charout

exe_itoa_pos:
      pushr r4    ; dividers ptr
      pushr r5    ; value holder
      mviw r5,0
      ldai 1
      mvarh r5
      pushr r6    ;
      mviw  r4,dividers

exe_itoa_loop:
      ldavr r4
      mvarh r6
      incr r4
      ldavr r4
      mvarl r6
      decr r4
      jsr parse_compare
      ldti 0ffh
      breq exe_itoa_next
      ldai 0
      mvarh r5
      jsr PARSE_SUB16
      incr r5
      br exe_itoa_loop

exe_itoa_next:
      mvrha r5
      brnz exe_itoa_notyet
      mvrla r5
      addi '0'
      JSR charout
      ldai 0
      mvarh r5

exe_itoa_notyet:
      ldai 0
      mvarl r5
      incr r4
      incr r4
      ldavr r4
      brnz exe_itoa_loop
      incr r4
      ldavr r4
      decr r4
      brnz exe_itoa_loop

;      mvrha r5
;      brz exe_itoa_noones
      mvrla r7
      addi '0'
      JSR charout
exe_itoa_noones:
      popr r6
      popr r5
      popr r4
      pop
      RET



;
; List out token buffer in human readable form
;
; list out token buffer
; buffer address in r7
;
tokl_let: DB "LET ",0
tokl_print: DB "PRINT ",0
tokl_if: DB "IF ",0
tokl_then: DB "THEN ",0
tokl_else: DB "ELSE ",0
tokl_for: DB "FOR ",0
tokl_to: DB "TO ",0
tokl_next: DB "NEXT ",0
tokl_goto: DB "GOTO ",0
tokl_gosub: DB "GOSUB ",0
tokl_return: DB "RETURN ",0
tokl_rem: DB "REM ",0
tokl_peek: DB "PEEK ",0
tokl_poke: DB "POKE ",0
tokl_end: DB "END ",0
tokl_call: DB "CALL ",0
tokl_input: DB "INPUT ",0
tokl_inp: DB "INP ",0
tokl_outp: DB "OUTP ",0
tokl_on: DB "ON ",0
tokl_off: DB "OFF ",0
tokl_error: DB "LIST ERROR",0


basic_list:
    pushr r3
    movrr r7,r3


baslist_loop:
    ldavr r3

    LDTI TOKENIZER_ERROR
    BRNEQ baslist1
    MVIW R7,tokl_error
    JSR stringout
    JSR showaddr
    BR baslist_done
    BR baslist_loop

baslist1:
    LDTI TOKENIZER_EOF
    BRNEQ baslist2
    BR baslist_done
    BR baslist_loop

baslist2:
    LDTI TOKENIZER_NUMBER
    BRNEQ baslist3
    incr r3
    ldavr r3
    mvarl r7
    incr r3
    ldavr r3
    mvarh r7
    INCR R3
;    jsr showr7
    jsr exe_itoa
    ldai ' '
    jsr uartout
    BR baslist_loop

baslist3:
;
; output quotes around string
;
    LDTI TOKENIZER_STRING
    BRNEQ baslist4
    incr r3
    movrr r3,r7
    jsr stringout
    ldai ' '
    jsr uartout

;
; does r7 point to end of string here?
;
baslist_string:
    ldavr r3
    brz baslist_stringend
    incr r3
    br baslist_string

baslist_stringend:
    incr r3
    BR baslist_loop

baslist4:
    LDTI TOKENIZER_VARIABLE
    BRNEQ baslist5
    incr r3
    ldavr r3
    ldti 'A'
    ADDT
    incr r3
    incr r3
    jsr uartout

;    ldavr r3
;    mvarl r7
;    incr r3
;    ldavr r3
;    mvarh r7
;    INCR R3
;    jsr showr7

    ldai ' '
    jsr uartout
    BR baslist_loop

baslist5:
    LDTI TOKENIZER_LET
    BRNEQ baslist6
    MVIW R7,tokl_let
    JSR stringout
    INCR R3
    BR baslist_loop

baslist6:
    LDTI TOKENIZER_PRINT
    BRNEQ baslist7
    MVIW R7,tokl_print
    jsr STRINGOUT
    INCR R3
    BR baslist_loop

baslist7:
    LDTI TOKENIZER_IF
    BRNEQ baslist8
    MVIW R7,tokl_if
    JSR stringout
    INCR R3
    BR baslist_loop

baslist8:
    LDTI TOKENIZER_THEN
    BRNEQ baslist9
    MVIW R7,tokl_then
    JSR stringout
    INCR R3
    BR baslist_loop

baslist9:
    LDTI TOKENIZER_ELSE
    BRNEQ baslist10
    MVIW R7,tokl_else
    JSR stringout
    INCR R3
    BR baslist_loop

baslist10:
    LDTI TOKENIZER_FOR
    BRNEQ baslist11
    MVIW R7,tokl_for
    JSR stringout
    INCR R3
    BR baslist_loop

baslist11:
    LDTI TOKENIZER_TO
    BRNEQ baslist12
    MVIW R7,tokl_to
    JSR stringout
    INCR R3
    BR baslist_loop

baslist12:
    LDTI TOKENIZER_NEXT
    BRNEQ baslist13
    MVIW R7,tokl_next
    JSR stringout
    INCR R3
    BR baslist_loop

baslist13:
    LDTI TOKENIZER_GOTO
    BRNEQ baslist14
    MVIW R7,tokl_goto
    JSR stringout
    INCR R3
    BR baslist_loop

baslist14:
    LDTI TOKENIZER_GOSUB
    BRNEQ baslist15
    MVIW R7,tokl_gosub
    JSR stringout
    INCR R3
    BR baslist_loop

baslist15:
    LDTI TOKENIZER_RETURN
    BRNEQ baslist16
    MVIW R7,tokl_return
    JSR stringout
    INCR R3
    BR baslist_loop

baslist16:
    LDTI TOKENIZER_CALL
    BRNEQ baslist17
    MVIW R7,tokl_call
    JSR stringout
    INCR R3
    BR baslist_loop

baslist17:
    LDTI TOKENIZER_REM
    BRNEQ baslist18
    MVIW R7,tokl_rem
    JSR stringout
    INCR R3
    BR baslist_loop

baslist18:
    LDTI TOKENIZER_PEEK
    BRNEQ baslist19
    MVIW R7,tokl_peek
    JSR stringout
    INCR R3
    BR baslist_loop

baslist19:
    LDTI TOKENIZER_POKE
    BRNEQ baslist20
    MVIW R7,tokl_poke
    JSR stringout
    INCR R3
    BR baslist_loop

baslist20:
    LDTI TOKENIZER_END
    BRNEQ baslist20a
    MVIW R7,tokl_end
    JSR stringout
    INCR R3
    BR baslist_loop

baslist20a:
    LDTI TOKENIZER_ON
    BRNEQ baslist20b
    MVIW R7,tokl_on
    JSR stringout
    INCR R3
    BR baslist_loop

baslist20b:
    LDTI TOKENIZER_OFF
    BRNEQ baslist21
    MVIW R7,tokl_off
    JSR stringout
    INCR R3
    BR baslist_loop

baslist21:
    LDTI TOKENIZER_COMMA
    BRNEQ baslist22
    ldai ','
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist22:
    LDTI TOKENIZER_SEMICOLON
    BRNEQ baslist23
    ldai ';'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist23:
    LDTI TOKENIZER_PLUS
    BRNEQ baslist24
    ldai '+'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist24:
    LDTI TOKENIZER_MINUS
    BRNEQ baslist25
    ldai '-'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist25:
    LDTI TOKENIZER_AND
    BRNEQ baslist26
    ldai '&'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist26:
    LDTI TOKENIZER_OR
    BRNEQ baslist27
    ldai '|'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist27:
    LDTI TOKENIZER_ASTR
    BRNEQ baslist28
    ldai '*'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist28:
    LDTI TOKENIZER_SLASH
    BRNEQ baslist29
    ldai '/'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist29:
    LDTI TOKENIZER_MOD
    BRNEQ baslist30
    ldai '%'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist30:
    LDTI TOKENIZER_HASH
    BRNEQ baslist31
    ldai '#'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist31:
    LDTI TOKENIZER_LEFTP
    BRNEQ baslist32
    ldai '()'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist32:
    LDTI TOKENIZER_RIGHTP
    BRNEQ baslist33
    ldai ')'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist33:
    LDTI TOKENIZER_LT
    BRNEQ baslist34
    ldai '<'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist34:
    LDTI TOKENIZER_GT
    BRNEQ baslist35
    ldai '>'
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist35:
    LDTI TOKENIZER_EQ
    BRNEQ baslist36
    ldai '='
    jsr uartout
    ldai ' '
    jsr uartout
    INCR R3
    BR baslist_loop

baslist36:
    LDTI TOKENIZER_CR
    BRNEQ baslist37
    mviw R7,CRLF
    jsr stringout
    INCR R3
    BR baslist_loop

baslist37:
    LDTI TOKENIZER_LINENUM
    BRNEQ baslist38
    incr r3
    ldavr r3
    mvarl r7
    incr r3
    ldavr r3
    mvarh r7
    INCR R3
    incr r3         ;skip over line length
    incr r3
    jsr exe_itoa
    ;jsr showr7
    ldai ' '
    jsr uartout
    BR baslist_loop

baslist38:
    LDTI TOKENIZER_INPUT
    BRNEQ baslist39
    MVIW R7,tokl_input
    JSR stringout
    INCR R3
    BR baslist_loop

baslist39:
    LDTI TOKENIZER_INP
    BRNEQ baslist40
    MVIW R7,tokl_inp
    JSR stringout
    INCR R3
    BR baslist_loop

baslist40:
    LDTI TOKENIZER_OUTP
    BRNEQ baslist41
    MVIW R7,tokl_outp
    JSR stringout
    INCR R3
    BR baslist_loop

baslist41:
    MVIW R7,tokl_error
    JSR stringout
    JSR showaddr
    BR baslist_done
;
; any cleanup
;
baslist_done:
    popr r3
    ret

;
; BASIC Interpreter Parser
;
parse_keywords:
      DB "let",0,TOKENIZER_LET
      DB "print",0,TOKENIZER_PRINT
      DB "if",0,TOKENIZER_IF
      DB "then",0,TOKENIZER_THEN
      DB "else",0,TOKENIZER_ELSE
      DB "for",0,TOKENIZER_FOR
      DB "to",0,TOKENIZER_TO
      DB "next",0,TOKENIZER_NEXT
      DB "goto",0,TOKENIZER_GOTO
      DB "gosub",0,TOKENIZER_GOSUB
      DB "return",0,TOKENIZER_RETURN
      DB "call",0,TOKENIZER_CALL
      DB "rem",0,TOKENIZER_REM
      DB "peek",0,TOKENIZER_PEEK
      DB "poke",0,TOKENIZER_POKE
      DB "end",0,TOKENIZER_END
      DB "list",0,TOKENIZER_LIST
      DB "run",0,TOKENIZER_RUN
      DB "new",0,TOKENIZER_NEW
      DB "exit",0,TOKENIZER_EXIT
      DB "input",0,TOKENIZER_INPUT
      DB "inp",0,TOKENIZER_INP
      DB "outp",0,TOKENIZER_OUTP
      DB "on",0,TOKENIZER_ON
      DB "off",0,TOKENIZER_OFF
      DB 0,0,TOKENIZER_ERROR

;
; int singlechar (void)
;
; return TOKEN result in accumulator
;
; ready for testing
;
parse_singlechar:
;
    pushr r3
    LDR r3,bas_txtptr
    ldavr r3
    popr r3
;
    ldti 0ah
    brneq parsechar1
    ldai TOKENIZER_CR
    ret
parsechar1:
    ldti ','
    brneq parsechar2
    ldai TOKENIZER_COMMA
    ret
parsechar2:
    ldti ';'
    brneq parsechar3
    ldai TOKENIZER_SEMICOLON
    ret
parsechar3:
    ldti '+'
    brneq parsechar4
    ldai TOKENIZER_PLUS
    ret
parsechar4:
    ldti '-'
    brneq parsechar5
    ldai TOKENIZER_MINUS
    ret
parsechar5:
    ldti '&'
    brneq parsechar6
    ldai TOKENIZER_AND
    ret
parsechar6:
    ldti '|'
    brneq parsechar7
    ldai TOKENIZER_OR
    ret
parsechar7:
    ldti '*'
    brneq parsechar7A
    ldai TOKENIZER_ASTR
    ret
parsechar7A:
    ldti '/'
    brneq parsechar8
    ldai TOKENIZER_SLASH
    ret
parsechar8:
    ldti '%'
    brneq parsechar9
    ldai TOKENIZER_MOD
    ret
parsechar9:
    ldti '('
    brneq parsechar10
    ldai TOKENIZER_LEFTP
    ret
parsechar10:
    ldti '#'
    brneq parsechar11
    ldai TOKENIZER_HASH
    ret
parsechar11:
    ldti ')'
    brneq parsechar12
    ldai TOKENIZER_RIGHTP
    ret
parsechar12:
    ldti '<'
    brneq parsechar13
    ldai TOKENIZER_LT
    ret
parsechar13:
    ldti '>'
    brneq parsechar14
    ldai TOKENIZER_GT
    ret
parsechar14:
    ldti '='
    brneq parsechar15
    ldai TOKENIZER_EQ
    ret
parsechar15:
    ldai 0
    ret

;
; int get_next_token(void)
;
; under development fix ptr nextptr

parse_get_next_tok:
    pushr r3
    LDR R3,bas_txtptr
;
; is digit
;
; chcek for number
;
parse_gnt1:
    ldavr r3
;    ldti '-'
;    brneq parse_gnt1_chkpos
;    incr r3
;    ldavr r3
;    jsr parse_isdigit
;    brz parse_gnt1_notneg
;    br parse_gnt1

parse_gnt1_chkpos:
    jsr parse_isdigit
    brz parse_gnt2

parse_gnt1loop:
    incr R3
    ldavr r3
    jsr parse_isdigit
    brnz parse_gnt1loop
    STR r3,bas_nxtptr
    ldai TOKENIZER_NUMBER
    popr r3
    ret

;parse_gnt1_notneg:
;    decr r3

;
; singlechar
;
parse_gnt2:
   ldavr r3
   jsr parse_singlechar
   BRZ parse_gnt3
   incr r3
   STR r3,bas_nxtptr
   popr r3
   ret
;
; quote
;
parse_gnt3:
    ldavr R3
    ldti '"'
    brneq parse_gnt4
parse_gnt3loop:
    incr r3
    ldavr R3
    ldti '"'
    brneq parse_gnt3loop
    incr r3
    STR r3,bas_nxtptr
    ldai TOKENIZER_STRING
    popr r3
    ret
;
parse_gnt4:
;
; keyword search
;
    pushr r4
    mviw r4,parse_keywords
top:
    ldavr r4
    LDR r3,bas_txtptr
    brz nokeyowrdsfound
pcmploop:
    ldavr r4
    brz found
    MVAT
    ldavr r3
    brneq skiptoend
    incr r4
    incr r3
    br pcmploop

skiptoend:
;  skip to end of string and skip over token
skiploop:
    ldavr r4
    incr r4
    brnz skiploop
    incr r4
    br top

found:
    str r3,bas_nxtptr
    incr r4
    ldavr r4
    popr r4
    popr r3
    ret

nokeyowrdsfound:
    popr r4
;
;  check for variables
;
    ldavr r3
    ldti 'A'
    BRLT parse_gnt_error
    ldti 'Z'
    BRGT parse_gnt_error
    INCR r3
    str r3,bas_nxtptr
    LDAI tokenizer_variable
    popr r3
    RET

parse_gnt_error:
;    halt
    ldai tokenizer_error
    popr r3
    ret

;
; void TOKENIZER_GOTO (char * program)
;
; under development, may not be needed
;
;parse_goto:
;    str r3,bas_txtptr
;    jsr parse_get_next_tok
;    sta bas_currenttoken
;    ret

;
; void tokenizer_init(int ptr)
;
; paramater ptr : R7 Points to text input line
;
; under development
;
parse_init:

    pushr r3
    STR R7,bas_txtptr
    MVIW r3,parse_token_buffer
    STR  R3,bas_tokptr
    mviw r3,0
    str r3,bas_tokcounter
    jsr parse_get_next_tok
    sta bas_currenttoken
    popr r3
    ret

;
; int tokenizer_token(void)
:
parse_token:
    lda bas_currenttoken
    ret

;
; void tokenizer_next(void) parser version
;
parse_next:
    jsr parse_finished
    brz parse_next1
    ret

parse_next1:
    pushr r3
    ldr r3,bas_nxtptr
    str r3,bas_txtptr

parse_next1loop:
    ldavr r3
    ldti ' '
    brneq parse_next2
    incr r3
    br parse_next1loop


parse_next2:
    str r3,bas_txtptr
    jsr parse_get_next_tok
    sta bas_currenttoken

    ldti TOKENIZER_REM
    breq parse_next3
    popr r3
    ret

;
; this needs to be fixed
; if rem return to parsel line then copy into token buffer
;
parse_next3:
    JSR parse_finished
    BRNZ parse_nextdone
    ldavr r3
    ldti  0ah
    breq parse_nextdone
    incr r3
    br parse_next3

parse_nextdone:
    ldavr r3
    ldti  0ah
    brneq parse_nextdone2
    incr r3

parse_nextdone2:
    str r3,bas_nxtptr
    jsr parse_next
    popr r3
    ret

;
; VARIABLE_TYPE tokenizer_num(void)
;
; Hack for now input characters are ascii hex format HHHH
;
; value returned in R7
;
; Should this advance the line ptr?
;

parse_num:
    pushr r3
    pushr r4
    pushr r5
    pushr r6
    mviw r6,0           ;accumlate val in r6
    LDR r3,bas_txtptr
    MVIW R4,0000h
;    STR R4,bas_negflag
    LDAVR R3
;  ldti '-'
;    brneq parse_num_pos
;    MVIW R4,0ffffh
;    STR R4,bas_negflag
;    incr r3

parse_num_pos:
    jsr parse_num_char
;   jsr SHOWBYTEA
    mvarl r6
parse_num_loop:
    jsr parse_num_char
;   jsr showbytea
    push
    ldti 0FFH
    breq parse_num_done

    movrr r6,r5
    mviw r4,0ah
    jsr parse_mul16
;    jsr showr7
    pop
    mviw r6,0
    mvarl r6
    jsr parse_add16
    movrr r7,r6
;    jsr showr7
    br parse_num_loop

parse_num_done:
    pop
    movrr r6,r7
;    ldr r4,bas_negflag
;    mvrla r4
;    brz parse_num_done1
;    jsr parse_neg
parse_num_done1:
    popr r6
    popr r5
    popr r4
    popr r3

;    jsr showr7
    ret

parse_num_char:
    LDAVR R3
    JSR parse_isdigit
    BRNZ parse_num_char1
    ldai 0ffh
    ret
parse_num_char1:
    ldavr r3
    SUBI '0'
    incr r3
    RET

;
; add error checking
;
; return hex nibble in accumulator
;
parse_getnibble:
      LDAVR R3
      incr r3
      LDTI '9'
      BRGT parse_af
      SUBI '0'
      RET
parse_af:
      JSR toupper
      SUBI 'A'
      ADDI 10
      RET
;
; void tokenizer_string(char *dest, int len)
;
; not used, in line parse_line
;
parse_string:
    JSR parse_token
    LDTI TOKENIZER_STRING
    BREQ parse_string1
    ret

parse_string1:
    pushr r3
    LDR r3,bas_txtptr
    LDAVR R3


;
; void tokenizer_error_print(void)
;
; fix
;
;parse_error_print:
;    halt

;
; int tokenizer_finished(void)
;
parse_finished:
    pushr r3
    LDR r3,bas_txtptr
    ldavr r3
    brz parse_finished_yes
;    ldti 0ah
;    breq parse_finished_yes
;    ldti 0dh
;    breq parse_finished_yes
    ldti TOKENIZER_EOF
    breq parse_finished_yes
    lda bas_currenttoken
    ldti TOKENIZER_EOF
    breq parse_finished_yes
    popr r3
    ldai 0
    ret
parse_finished_yes:
    ldai 1
    popr r3
    ret

;
; int tokenizer_variable_num(void)
;
parse_variable_num:
      pushr r3
      LDR r3,bas_txtptr
      ldavr r3
      subi 'A'
      mvarl r7
      ldai 0
      mvarh r7
      popr r3
      ret

;
; char *const tokenizer_pos(void)
;
; probably not used
parse_pos:

;not used old
; char *tokenize(char *program)
;
; parse:

;
; char *tokenizeLine(char *line)
;
; assume input text string in parse_input_line
;

parse_line:
    JSR PARSE_INIT

    MVIW R3,6              ;all lines have a 6 bytes including EOL token
    str r3,bas_tokcounter
    MVIW R3,parse_token_buffer

    pushr r3
    LDR r3,bas_txtptr
    ldavr r3
    jsr parse_isdigit
    popr r3
    ldti 1
    breq parse_code
    jsr parse_get_next_tok

parse_line_list:
    ldti tokenizer_list
    brneq parse_line_exit
    ldr r7,bas_tokenbuffer
    JSR  basic_list
    ldai 0
    RET

parse_line_exit:
    ldti TOKENIZER_EXIT
    brneq parse_line_run
    jsr basic_run
    ldai 1
    RET

parse_line_run:
    ldti TOKENIZER_RUN
    brneq parse_line_new
    jsr basic_run
    ldai 0
    RET

parse_line_new:
    ldti TOKENIZER_NEW
    brneq parse_line_error
    MVIW R7,bas_tok_buf_start
    ldai TOKENIZER_EOF          ;write EOF to start of buffer
    stavr r7
    ldai 0
    RET

parse_line_error:
    mviw r7,parse_cmd_error
    jsr stringout
    mviw r7,CRLF
    jsr stringout
    ldai 0
    ret

parse_code:
    LDAI TOKENIZER_LINENUM ;start with linenum token
    STAVR R3
    INCR R3

    JSR parse_num           ;store line number
    str r7,bas_newlinenum
    mvrla r7
    stavr r3
    incr r3
    mvrha r7
    stavr r3
    incr r3

    incr r3                ; skip over line length - fill in later
    incr r3

parse_line_loop:
    JSR parse_next
    JSR parse_token
    ldti TOKENIZER_CR
    BREQ parse_line_done

    STAVR R3
    INCR R3
    ldr r5,bas_tokcounter
    incr r5
    str r5,bas_tokcounter

    ldti TOKENIZER_NUMBER
    BRNEQ parse_line1
    JSR parse_num
    mvrla r7
    stavr r3
    incr r3
    mvrha r7
    stavr r3
    incr r3
    ldr r5,bas_tokcounter
    incr r5
    incr r5
    str r5,bas_tokcounter
    BR parse_line_loop

parse_line1:
    ldti TOKENIZER_STRING
    BRNEQ parse_line2
    ldr r4,bas_txtptr
    incr r4                   ;skip over opening "

parse_line1loop:
    ldavr r4
    ldti '"'
    breq parse_line1done
    stavr r3
    incr r3
    incr r4
    ldr r5,bas_tokcounter
    incr r5
    str r5,bas_tokcounter
    BR parse_line1loop
parse_line1done:
    incr r4                   ;skip over closing "
    ldai 0                    ;add null into token buffer
    stavr R3
    incr r3
    ldr r5,bas_tokcounter
    incr r5
    str r5,bas_tokcounter
    str r3,BAS_TOKPTR
    str r4,bas_txtptr
    BR parse_line_loop

parse_line2:
    ldti TOKENIZER_VARIABLE
    BRNEQ parse_line_loop
    JSR parse_variable_num
    mvrla r7
    stavr r3
    incr r3
    mvrha r7
    stavr r3
    incr r3
    ldr r5,bas_tokcounter
    incr r5
    incr r5
    str r5,bas_tokcounter
    BR parse_line_loop

parse_line_done:
    ldai TOKENIZER_CR           ;tack CR to end of buffer
    STAVR R3
    INCR R3
;    ldr r5,bas_tokcounter
;    incr r5
;    str r5,bas_tokcounter

    mviw r3,parse_token_buffer ; write length into token buffer
    incr r3
    incr r3
    incr r3
    ldr r7,bas_tokcounter
    str r7,bas_newlinelen
    mvrla r7
    stavr r3
    incr r3
    mvrha r7
    stavr r3

;
; void addLine(char *buff)
; since addline is only called from parse_tokenizeline where are these
; values are know, pass them in or
; maybe merge addLine to bottom of parse_tokenizeline
;
; r7 points to line to insert
; r6 linenumber
; r5 linelength
;
parse_addline:
      ldr r7,BAS_NEWLINENUM
      JSR parse_removeline      ; if line number already exists delete it
      ldr r7,bas_newlinelen     ; if new line is 6 bytes line just delete nothing to add
      mvrla r7
      ldti 6
      brneq parse_addline1
      ldai 0
      ret

parse_addline1:                 ;find insert location
      ldr r7,BAS_NEWLINENUM
      ldr r3,bas_tokenBuffer

parse_addlineloop:
      str r3,bas_insertptr
      ldavr r3
      ldti TOKENIZER_EOF
      BREQ parse_addroom
      incr r3                   ;get line number of line in token buffer into r6
      ldavr r3
      mvarl r6
      incr r3
      ldavr r3
      mvarh r6
      jsr parse_compare
      ldti 0ffh
      breq parse_addroom

      incr r3                   ;get line length
      ldavr r3
      mviw r6,0                 ;set r6 to length - assumes < 256 bytes
      mvarl r6
      decr r6                   ;account for 3 bytes of linenum token and linenum
      decr r6
      decr r6

addline_moveptr:              ; advance pointer to next line
      incr r3
      decr r6
      mvrla r6
      brnz addline_moveptr
      br parse_addlineloop


parse_addroom:               ; make room
      mviw r7,BAS_TOK_BUF_END
;      decr r7
      ldr  r6,BAS_NEWLINELEN
      jsr parse_sub16
      mviw r5,BAS_TOK_BUF_END
;      decr r5

      ldr r6,bas_insertptr
parse_roomloop:
      decr r7
      decr r5
      ldavr r7
      stavr r5

      mvrla r6
      mvat
      mvrla r7
      brneq parse_roomloop

      mvrha r6
      mvat
      mvrha r7
      brneq parse_roomloop

      ldr r7,bas_insertptr
      MVIW R6,parse_token_buffer

parse_insertloop:
      ldavr r6
      stavr r7
      incr r6
      incr r7
      ldti 024h ; sub in actual eol token here
      brneq parse_insertloop
      ldai 0
      ret

;
; void removeLine(int lineNum)
;
parse_removeline:
    JSR basu_find
    mvrha R7
    brnz parse_removeline1
    mvrla R7
    brnz parse_removeline1
    ret
parse_removeline1:
    movrr R7,R5     ; R5 = destination ptr
    incr r7
    incr r7
    incr r7
    ldavr r7
    mvarl r6        ; R6 = remove length
    incr r7
    ldavr r7
    mvarh r6

    decr r7         ;backup to start of line
    decr r7
    decr r7
    decr r7
    JSR parse_add16
parse_removeloop:
    ldavr r7
    stavr r5
    incr r5
    incr r7
    MVRHA r7

    ldt  bas_bufferendhi

    brneq parse_removeloop
;
; no need to explicitly set TOKENIZER_ENDOFINPUT should have been copied
; since we are moving complete contents of buffer
; Make sure it is set during addline
;


; run & parse utilites
;

;
; find in the token buffer linenum in r7
;
basu_find:
    pushr r5                  ; save r5
    LDR  r5,bas_tokenBuffer   ; r5 ptr to token buffer

basu_find_loop:
    ldavr r5                  ;get token
    ldti TOKENIZER_EOF        ;EOF
    brneq basu_find_loop1     ;no continue
    mviw r7,0                 ;return 0 in r7 and 0 in accumulator (why?)
    mvrla r7
    popr r5
    ret

basu_find_loop1:
    incr r5                   ;skip past linenum token
    mvrla r7                  ;test low byte of linenum
    MVAT
    ldavr r5
    brneq basu_find1          ;not eqaul
    incr r5
    mvrha r7                  ;test high byte of linenum
    MVAT
    ldavr r5
    brneq basu_find2          ;not equal
    decr r5                   ;if equal backup to linenum token
    decr r5
    movrr r5,r7               ;set r7 to point to start of line
    ldai 1                    ;return 1 in accumulator
    popr r5
    ret

basu_find1:
    incr r5                   ;skip over linenum high byte
basu_find2:
    incr r5                   ;get line length
    ldavr r5
    mviw r6,0                 ;set r6 to length - assumes < 256 bytes
    mvarl r6
    decr r6                   ;account for 3 bytes of linenum token and linenum
    decr r6
    decr r6
basu_find_moveptr:            ; advance pointer to next line
    incr r5
    decr r6
    mvrla r6
    brnz basu_find_moveptr
    br basu_find_loop

;
; Divide r5 by r4 return answer in r7
;
; return result in r7
;
parse_div16:
      pushr r6
      movrr r5,r7
      movrr r4,r6
      mviw r4,0
;
; 16 bit SUB of r6 from r7, return result in r7
;
; r7=r6 00
; r7>r6 01
; r7<r6 ff
;
divloop:
    jsr parse_compare
    ldti 0FFH
    breq div_done
    pushr r6
    jsr parse_sub16
    popr r6
    incr r4
    br divloop
div_done:
    popr r6
    MOVRR r4,r7
    ret

;
; mod r5 by r4 return answer in r7
;
; return result in r7
;
parse_mod16:
      pushr r6
      movrr r5,r7
      movrr r4,r6
      mviw r4,0
;
; 16 bit SUB of r6 from r7, return result in r7
;
modloop:
    jsr parse_compare
    ldti 0FFH
    breq mod_done
    pushr r6
    jsr parse_sub16
    popr r6
    incr r4
    br modloop
mod_done:
    popr r6
    ret

;
; Multiple numbers in R4 and R5
;
; Return in R7
;
; R6 = Bit counter
;
parse_mul16:
        pushr r6

        MVIW R7,0
        MVIW R6,10h
;        jsr showregs

parse_mulloop:
;        jsr showregs

        mvrla r5
        andi  01h
        brz parse_mulskip
        jsr parse_muladd16

parse_mulskip:
;
; clear carry flag HACK
;
        ldai 0      ;clear carry -  maybe add a clear carry ins
        cshl
        addi 0      ; may not be needed
;
        mvrla r4
;        jsr showcarry
        cshl
;        jsr showcarry
        mvarl r4
        mvrha r4
;        jsr showcarry
        cshl
;        jsr showcarry
        mvarh r4

        ldai 0      ;clear carry
        cshl
;        addi 0

        mvrha r5
        cshr
        mvarh r5
        mvrla r5
        cshr
        mvarl r5

        decr r6
        mvrla r6
        brnz parse_mulloop
        popr r6
        ret

parse_muladd16:
        ldai 0      ;clear carry
        cshl

        MVRLA R7
        MVAT
        mvrla r4
        ADDT
        mvarl r7

        mvrha r7
        MVAT
        mvrha r4
        addtc
        mvarh r7
        ret

;
; 16 bit ADD r6 and r7, return result in r7
;
parse_add16:

      ldai 0      ;clear carry
      cshl
      MVRLA R6
      MVAT
      mvrla r7
      ADDT
      mvarl r7
      mvrha r6
      MVAT
      mvrha r7
      addtc
      mvarh r7
      ret

;
; 16 bit SUB of r6 from r7, return result in r7
;
parse_sub16:
      mvrha r6
      inva
      mvarh r6
      mvrla r6
      inva
      mvarl r6
      incr r6
      br parse_add16

;
; convert positive in r7 to negative
;
parse_neg:
      mvrha r7
      inva
      mvarh r7
      mvrla r7
      inva
      mvarl r7
      incr r7
      ret

;
; 16 bit AND r6 and r7, return result in r7
;
parse_and16:
      MVRLA R6
      MVAT
      mvrla r7
      ANDT
      mvarl r7
      mvrha r6
      MVAT
      mvrha r7
      ANDT
      mvarh r7
      ret

;
; 16 bit OR r6 and r7, return result in r7
;
parse_or16:
      MVRLA R6
      MVAT
      mvrla r7
      ORT
      mvarl r7
      mvrha r6
      MVAT
      mvrha r7
      ORT
      mvarh r7
      ret


;
; is digit in accumulator hex
; returns 1 if yes and 0 if no
;
; what about upper/lower case
;
; destroys tmp register
parse_ishex:
   ldti '0'
   brlt parse_ishex_no
   ldti '9'
   brgt parse_ishex_af
   br parse_ishex_yes

parse_ishex_af:
    ldti 'A'
    brlt parse_ishex_no
    ldti 'F'
    brgt parse_ishex_no

parse_ishex_yes:
    ldai 1
    ret

parse_ishex_no:
    ldai 0
    ret

;
; is digit in accumulator decimal
; returns 1 if yes and 0 if no
;
; destroys tmp register
;
parse_isdigit:
   ldti '0'
   brlt parse_isdigit_no
   ldti '9'
   brgt parse_isdigit_no

    ldai 1
    ret

parse_isdigit_no:
    ldai 0
    ret

;
; 16 bit compare r6 and r7
;
; returns in accumulator
; r7=r6 00
; r7>r6 01
; r7<r6 ff
;
; destory Tmp
;
parse_compare:
  mvrha r6
  mvat
  mvrha r7
  brneq parse_compareh
  mvrla r6
  mvat
  mvrla r7
  brneq parse_comparel
  ldai 0
  ret

parse_compareh:
  brlt parse_compareh1
  ldai 1
  ret
parse_compareh1:
  ldai 0ffh
  ret

parse_comparel:
  brlt parse_comparel1
  ldai 1
  ret
parse_comparel1:
  ldai 0ffh
  ret

basic_prompt: db ">>",0
basic_interprter:
        ;build input string
        ;point register to BUFFER
        ;loop fetch chars
        ;until CR
        ;be sure line ends with a NULL or CR
        ;what does parse require???
        MVIW R7,basic_prompt
        JSR stringout

        mviw r7,parse_input_line

parse_inputloop:
        jsr get_inputline

        mviw r7,parse_input_line
        JSR parse_line

        BRZ basic_interprter
        RET

        mviw r3,parse_input_line

get_inputline:
        jsr uartin
        stavr r7
        incr r7
        ldti 0ah  ;1 changed from 0a to 0D for new emulator code, changed back
        brneq get_inputline
        MVIW R7,CRLF
        JSR STRINGOUT
        RET

test_input:
        jsr charavail
        brnz setbreak
        ret
setbreak:
        pushr r6
        mviw r6,bas_run_ended
        LDAI 1
        STAVR r6
        popr r6
        ret

;
; STRINGS
;
CRLF: DB 0ah,0dh,0
;

  ORG 0EF00h
BASIC_TEST:
;
; The End
;
  ORG 0EFFFh
ZZZZ:
  DB   0
