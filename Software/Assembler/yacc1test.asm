top:    ON
        OFF
        HALT

        LDT 01
        LDA 02

        MVI R0,03
        MVI R1,1234
        MVRLA R2
        MVRHA R3
        MVARL R4
mid:    MVARH R5
        LDAVR R6
        STAVR R7
        INCR R0
        DECR R1
        OUTA P9
        OUTI PA,33
        OUTVR PB,R1
        INP PF
        BR 1234h
        BR mid
        BRZ 5678h
        BRNZ 0246h
        BRINH 6420h
        BRINL 1122h

        ADDI 01
        SUBI 02h
        ORI 03
        ANDI 04h
        XORI 05
        INVA
        SHL
        SHR
        ADDT
        SUBT
        ORT
        ANDT
        XORT

        LDTVR R1
        STTVR R2
