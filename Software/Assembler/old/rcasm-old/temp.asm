;
; Define BIOS to generate code for use with ELFOS BIOS
;



;
; Define 6845VIDEOLOWER when video card (REV B and beyond) supports lower 
; case characters and there is no need to force characters to upper case
;



;
; Define BASIC if the is for use with Tiny basic and not for ELFOS 
;

;
; minimon is entry point in ELFOS  to jump to once bios has booted
; 

minimon:	equ     0ff54h

crtca:		equ	07800h		; CRTC Address register
crtcd:		equ	crtca+1		; CRTC Data register
cur_h:		equ     14		; Cursor postion high
cur_l:		equ     15		; Cursor postion low

vmem:		equ	07000h		; Start of video memory
vmem_end:	equ     07800h		; End of video memory
vmem_ch:	equ	vmem_end-2
vmem_cl:	equ     vmem_end-1
space:		equ	020h		; Ascii Space

JUMPTBL:	equ	0ff00h
basic_start:	equ	0f700h
basic_end:	equ	0feffh
basic_dest:	equ	00100h
;expline23:		equ     vmem+(80*23)
line23:		equ     vmem+(64*15)
basmon:		equ	0f500h

		org 0f000h
boot:		ldi high boot	;Trigger Boot rom remap 
		phi r0		; 
      		ldi high crtca  ; Register E set to CRTC Address register
      		phi re          ;
	        ldi low crtca   ;
	        plo re          ;
      		ldi high crtcd  ; Register F set to CRTC Data register 
		phi rf          ;
	        ldi low crtcd   ;
                plo rf          ;
		ldi high vtable ; Register D set to vtable (CRTC parameters)
		phi rd          ;
		ldi low vtable  ; 
		plo rd          ; 
		ldi 00          ; Register C set to 0000
		phi rc          ; Register C will be a counter used to point to the CRTC data register
		plo rc          ;
;
; Load Video Paramters into CRTC
;
vload:          glo rc          ; Main loop, get counter register 
	        str re          ; Store into CRTC address register
	        lda rd          ; Get video paramter 
	        str rf          ; Store into CRTC data register
      		inc rc          ; Increment counter
	        glo rc          ; Has counter reached 010h
	        xri 010h        ;
	        bnz  vload      ; If not load next CRTC Video paramter
;
; Clear Video memory (with space charachter)
;

		ldi high vmem   ; Register F set to start of Video memory
		phi rf          ;
		ldi low vmem    ;
		plo rf          ; 
clear:          ldi space        ; Load D  space character
		str rf          ; Sstore in Video memory
		inc rf          ; Increment Register F to next video memory location
		ghi rf          ; Check if past end of video memory
		xri high vmem_end ; Logical XOR D with value
		bnz clear       ; Short branch on D!=0
;
; Store cursor position in memory
;
		ldi high vmem_ch
		phi rf
		ldi low vmem_ch
		plo rf
		ldi 073h	;;;
		str rf
		inc rf
		ldi 0c0h	;;;
		str rf
;
; Initialization complete jump to real start
;
		lbr  cold_entry      ; Long branch
;
; Video Paramters
;
;vtable:	 db 06dh,050h,055h,028h,01ch,01h,018h,01ah,00h,08h,060h,01fh,00h,00h,07h,30h

;vtable:		 db 80 ; R0 total number of horiz chars
;		 db 64 ; R1 number of horiz chars displayed 
;		 db 65 ; R2 Horiz sync position (chars)
;		 db 1ah ; R3 VSYNC/HSYNC Widths
;		 db 24 ; R4 Vert Total (chars)
;		 db 02 ; R5 Vert total Adjust (scan lines)
;		 db 16 ; R6 Vert displayed (chars)
;		 db 20 ; R7 Vert Sync position (chars)
;		 db 000 ; R8 mode contro;
;		 db 008 ; R9 scan lines/char
;		 db 040h ; R10 cursor start (scan line/char) + blink rate
;		 db 008h ; R11 cursor end (scan line/char)
;		 db 000h ; R12 display addr start high
;		 db 000h ; R13 display addr start low
;		 db 003h ; R14 Cursor position high
;		 db 0c0h ; R15 Cursor positon low

vtable:		 db 94 ; R0 total number of horiz chars
		 db 64 ; R1 number of horiz chars displayed 
		 db 77 ; R2 Horiz sync position (chars)
		 db 8 ; R3 VSYNC/HSYNC Widths
		 db 28 ; R4 Vert Total (chars)
		 db 2 ; R5 Vert total Adjust (scan lines)
		 db 16 ; R6 Vert displayed (chars)
		 db 22 ; R7 Vert Sync position (chars)
		 db 0 ; R8 mode contro;
		 db 8 ; R9 scan lines/char
		 db 64 ; R10 cursor start (scan line/char) + blink rate
		 db 8 ; R11 cursor end (scan line/char)
		 db 0 ; R12 display addr start high
		 db 0 ; R13 display addr start low
		 db 003h ; R14 Cursor position high
		 db 0c0h ; R15 Cursor positon low
;
; Input/Output routines
;
; output a character
; Character to output in D
;
outchar: 	ani 07fh         ; Strip off high bit
		plo re           ; Store Character to output in Register e.0
		xri 07fh         ; Check if character is is 07Fh (??? Why)
		bz  outnochar    ; If yes done 
		glo re




;save registers
cont1:        	sex 2
		glo rf
		stxd
		ghi rf
		stxd
		glo re
		stxd
		ghi re
		stxd	
		glo rc
		stxd
		ghi rc
		stxd	
;
; Get character position in RC
;
		ldi high vmem_ch ; Register F points to CRTC Address register
		phi rf         ; 
		ldi low vmem_ch ;
		plo rf         ; 
		ldn rf	       ; Get cursor position high	
		phi rc         ; Store in RC.1
		inc rf
		ldn rf	       ; Get cursor position low
		plo rc	       ; Store in RC.0	


	        glo re           ; Check for a control Character below 020h
                ani 0e0h         ; 
		bnz  normal      ; Normal character
		glo re           ; Check for carriage return 0dh
		xri 0dh          ;
		bz  cr           ; Process carriage return 
		glo re           ; Check for line feed 0ah 
		xri 0ah          ;
		bz  lf           ; Process line feed
		glo re           ; Check for backspace
		xri 08h          ;
		bz  bs           ; process backspace
		br  outdn		
;
; Process a carriage return
;
cr:		ldi high line23  ; Set Register C to start of line 23
		phi rc         	 ;
		ldi low line23   ;
		plo rc           ;
		br  update       ; Update cursor position
;
; Process a line feed  >>>>>>redo registers
;
lf:		ldi high vmem  ; Set Register E to start of Video memory
		phi re         ; 
		ldi low vmem   ;
		plo re         ; 
		ldi high vmem+64 ; set Register F to 2nd line of Video memory
		phi rf         ; 
		ldi low vmem+64 ; 
		plo rf         ;
; Scrool loop, copy all lines up one
sloop:          lda rf         ; 
		str re         ;
		inc re         ;
		ghi rf         ;
		xri high vmem_end ; Is Register E past end of video memory
		bnz  sloop     ; No, continue scrool loop
; Clear last line, Register D now point to first character position of last line
		ldi high line23
		phi re
		ldi low line23
		plo re	
erl:		ldi space       ; Space Character
	        str re         ; 
       		inc re         ; 
        	ghi re         ; 
                xri high vmem_end ; Is register D past end of video memory
        	bnz  erl       ; Continue erasing line
; Line feed done, return (Why not update cursor position)
		br update
;
; Process Back Space
;
bs:             dec rc         ; Register C is character pointer
		ldi space       ; Clear postion with a space character 
		str rc         ; 
	        br  update     ; Update Cursor postion
;
; Process Normal Character
; 
normal:         glo re         ; Register F.1 contains character
		str rc         ; Store at register C, Character pointer
                inc rc         ; 
;
; Update Cursor position
;
update: 
		ldi high crtca	
		phi rf
		ldi low crtca
		plo rf
		ldi cur_h      ; CRTC High cursor register 
		str rf         ; set CRTC Address register
		ghi rc         ; Get High portion of cursor postion
		ani 03fh       ; Strip off address, only want zero based character postion
		inc rf
		str rf	       ; Get cursor position high	
		dec rf
		ldi cur_l      ; CRTC low cursor register 
		str rf         ; set CRTC Address register
		inc rf
		glo rc
		str rf	       ; get cursor positon low	
;
; Store cursor position in memory
;
		ldi high vmem_ch
		phi rf
		ldi low vmem_ch
		plo rf
		ghi rc
		str rf
		inc rf
		glo rc
		str rf
;
;restore registers
;
outdn:
		irx
		ldxa
		phi rc
		ldxa
		plo rc
		ldxa
		phi re
		ldxa
		plo re
		ldxa
		phi rf
		ldx
		plo rf
;
outnochar:		sep r5         ; Return
;
;
;
		org 0f100h
inchar:         bn3  inchar    ; Wait for an input character
        	sex 2
		inp 1          ; Read character
		dec r2         ; Decrement (R2) Save inputed character across call
		sep r4         ; Output the character
		dw out         ; 
		inc r2         ; Increment (R2) restore inputed character
		ldn r2         ; Get the input Character
		sep r5         ; Return
;
; Check for a break character
;
brk:		b3  brkchk     ; Character is available
bno:		ldi 00         ; Clear DF 
		shl            ; 
	        br brkdone       ; Return
;
;
;
brkchk:		
        	sex 2
		inp 1          ; Input to (R(X)) and D, N=001
		xri 01bh       ; Check for ESC
                bnz bno        ; Not a break character
		ldi 0ffh       ; Set DF to 1
		shl            ; Shift left D
brkdone:	sep r5         ; Return




		org 0f200h
in:		lbr inchar
out:		lbr outchar
break:		lbr brk 
cold_entry:	lbr minimon



