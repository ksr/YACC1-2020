top:    ON
        MVIB R1,080h
loop1:  DECR R1
        MVRLA R1
        BRNZ loop1
        OFF
        MVIB R1,080h
loop2:  DECR R1
        MVRLA R1
        BRNZ loop2
        BR top
        OUTA P1
        OUTA PF
        OUTI P3,055H
        OUTVR P2,R1
        INP P3