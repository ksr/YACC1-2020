	movi R3,0fh
test:   movi R2,01234h
        movdb R4,01234h
	movdw R4,05678h
	movrr R6,R2
	movrra R7,R1
	movrar R2,R4
	movrara R7,R1
	ldai 02h
	ldad 04567h
	ldar R4
	ldarl R7
	ldarh R3
	stad 05678h
	star R2
	starl r3
	starh r4
	br test
	bz test
	bnz test
	biz test
	binz test
	bcz test
	bcnz test
	brrz r2,test
	addi 1
	subi 1
	andi 1
	ori 1
	xori 1
	not
	addr r1
	subr r1
	andr r1
	orr r1
	xorr r1
	movmb test,r1
	movmw test,r1
	br test
	outon
	outoff
