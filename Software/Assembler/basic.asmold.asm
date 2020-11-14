


;
; Tokenizer execute support code
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

exe_string:
    movrr r3,r7
    incr r7
    ret




;
; STRINGS
;
CRLF: DB 0ah,0dh,0
;

      ORG 0EC00h

basic_test:
    DB  25h,0ah,00h,0eh,00h,06h,03h,68h,65h,6ch,6ch,6fh,00h,24h,25h,14h

;
; The End
;
ZZZZ:
  DB   0
