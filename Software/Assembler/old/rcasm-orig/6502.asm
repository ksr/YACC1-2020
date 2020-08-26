           org   100h
start:     brk
           ora   (0fah,x)
           tsb   0adh
           ora   0c0h
           asl   054h
           rmb0  012h
           php
           ora   #23h
           asl
           tsb   01234h
           ora   01234h
           asl   01234h
           bbr0  01234h
           bpl   01234h
           ora   (012h),y
           ora   (012h)
           trb   012h
           ora   012h,x
           asl   012h,x
           rmb1  012h
           clc
           ora   01234h,y
           inc
           trb   01234h
           ora   01234h,x
           asl   01234h,x
           bbr1  01234h
           jsr   01234h
           and   (012h,x)
           bit   012h
           and   012h
           rol   012h
           rmb2  012h
           plp
           and   #23h
           rol
           bit   1234h
           and   1234h
           rol   1234h
           bbr2  1234h
           bmi   1234h
           and   (12h),y
           and   (12h)
           bit   12h,x
           and   12h,x
           rol   12h,x
           rmb3  12h
           sec
           and   1234h,y
           dec
           bit   1234h,x
           and   1234h,x
           rol   1234h,x
           bbr3  1234h
           rti
           eor   (12h,x)
           eor   12h
           lsr   12h
           rmb4  12h
           pha
           eor   #12h
           lsr
           jmp   1234h
           eor   1234h
           lsr   1234h
           bbr4  1234h
           bvc   1234h
           eor   (12h),y
           eor   (12h)
           eor   12h,x
           lsr   12h,x
           rmb5  12h
           cli
           eor   1234h,y
           phy
           eor   1234h,x
           lsr   1234h,x
           bbr5  1234h
           rts
           adc   (12h,x)
           stz   12h
           adc   12h
           ror   12h
           rmb6  12h
           pla
           adc   #12h
           ror
           jmp   (1234h)
           adc   1234h
           ror   1234h
           bbr6  1234h
           bvs   1234h
           adc   (12h),y
           adc   (12h)
           stz   12h,x
           adc   12h,x
           ror   12h,x
           rmb7  12h
           sei
           adc   1234h,y
           ply
           jmp   (1234h,x)
           adc   1234h,x
           ror   1234h,x
           bbr7  1234h
           bra   1234h
           sta   (12h,x)
           sty   12h
           sta   12h
           stx   12h
           smb0  12h
           dey
           bit   #12h
           txa
           sty   1234h
           sta   1234h
           stx   1234h
           bbs0  1234h
           bcc   1234h
           sta   (12h),y
           sta   (12h)
           sty   12h,x
           sta   12h,x
           stx   12h,x
           smb1  12h
           tya
           sta   1234h,y
           txs
           stz   1234h
           sta   1234h,x
           stz   1234h,x
           bbs1  1234h
           ldy   #12h
           lda   (12h,x)
           ldx   #12h
           ldy   12h
           lda   12h
           ldx   12h
           smb2  12h
           tay
           lda   #12h
           tax
           ldy   1234h
           lda   1234h
           ldx   1234h
           bbs2  1234h
           bcs   1234h
           lda   (12h),y
           lda   (12h)
           ldy   12h,x
           lda   12h,x
           ldx   12h,y
           smb3  12h
           clv
           lda   1234h,y
           tsx
           ldy   1234h,x
           lda   1234h,x
           ldx   1234h,y
           bbs3  1234h
           cpy   #12h
           cmp   (12h,x)
           cpy   12h
           cmp   12h
           dec   12h
           smb4  12h
           iny
           cmp   #12h
           dex
           cpy   1234h
           cmp   1234h
           dec   1234h
           bbs4  1234h
           bne   1234h
           cmp   (12h),y
           cmp   (12h)
           cmp   12h,x
           dec   12h,x
           smb5  12h
           cld
           cmp   1234h,y
           phx
           cmp   1234h,x
           dec   1234h,x
           bbs5  1234h
           cpx   #12h
           sbc   (12h,x)
           cpx   12h
           sbc   12h
           inc   12h
           smb6  12h
           inx
           sbc   #12h
           nop
           cpx   1234h
           sbc   1234h
           inc   1234h
           bbs6  1234h
           beq   1234h
           sbc   (12h),y
           sbc   (12h)
           sbc   12h,x
           inc   12h,x
           smb7  12h
           sed
           sbc   1234h,y
           plx
           sbc   1234h,x
           inc   1234h,x
           bbs7  1234h
