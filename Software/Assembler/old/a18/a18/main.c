/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: ksr77
 *
 * Created on January 12, 2017, 11:49 PM
 */

/*
	HEADER:		CUG149;
	TITLE:		1805A Cross-Assembler (Portable);
	FILENAME:	A18.C;
	VERSION:	2.5;
	DATE:		08/27/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the RCA 1802, 1804, 1805, 1805A, 1806, and
			1806A microprocessors.  The program is written in
			portable C rather than BDS C.  All assembler features
			are supported except relocation, linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			RCA, CDP1802, CDP1805A;

	SEE-ALSO:	CUG113, 1802 Cross-Assembler;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program has compiled successfully on 2 UNIX
			compilers, 5 MSDOS compilers, and 2 CP/M compilers.
			A port to BDS C would be extremely difficult, but see
			volume CUG113.  A port to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		      1805A Cross-Assembler in Portable C

		   Copyright (c) 1985 William C. Colley, III

Revision History:

Ver	Date		Description

2.0	APR 1985	Recoded from BDS C version 1.1.  WCC3.

2.1	AUG 1985	Greatly shortened the routines find_symbol() and
			new_symbol().  Fixed bugs in expression evaluator.
			Added compilation instructions for Aztec C86,
			Microsoft C, and QNIX C.  Added optional optimizations
			for 16-bit machines.  Adjusted structure members for
			fussy compilers.  WCC3.

2.2	SEP 1985	Added the INCL pseudo-op and associated stuff.  WCC3.

2.3	JUL 1986	Added compilation instructions and tweaks for CI-C86,
			Eco-C88, and Lattice C.  WCC3.

2.4	JAN 1987	Fixed bug that made "BYTE 0," legal syntax.  WCC3.

2.5	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A18 FILE.ASM -L".  WCC3 per Alex Cameron.

2.5+ JAN 2010	Program fixed to compile with LCC in Windows.
				Mostly by updating function declarations,
				replacing () with (VOID), etc. SOme fixes need
				when char and int were used interchangably,
				unsigned was given signs, etc.
				labels ending with colon now have colon stripped
				All fixes have HRJ in comment lines    HRJ.

	 Apr 9 2010 HRJ - bad checksum, fixed

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines to
the line assembly routine, and sends the results to the listing and object file
output routines.  It also coordinates the activities of everything.  The line
assembly routine uses the expression analyzer and the lexical analyzer to
parse the source line convert it into the object bytes that it represents.
*/

/*  Get global goodies:  */

#include "newfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* eternal routines HRJ*/

    void asm_line(void);
    void lclose(void), lopen(char *), lputs(void);
    void hclose(void), hopen(char *), hputc(unsigned);
    void error(char), fatal_error(char *), warning(char *);
    void pops(char *), pushc(int), trash(void);
	void hseek(unsigned);
	void unlex(void);
	int isalph(char); /* was int isalph(int) HRJ */

	/* these are local but used before defined HRJ */
	static void do_label(void),normal_op(void), pseudo_op(void);
	static void flush(void);

/*  Define global mailboxes for all modules:				*/

char errcode, line[MAXLINE + 1], title[MAXLINE];
int pass = 0;
int eject, filesp, forwd, listhex;
unsigned address, bytes, errors, listleft, obj[MAXLINE], pagelen, pc;
FILE *filestk[FILES], *source;
TOKEN token;

/*  Mainline routine.  This routine parses the command line, sets up	*/
/*  the assembler at the beginning of each pass, feeds the source text	*/
/*  to the line assembler, feeds the result to the listing and hex file	*/
/*  drivers, and cleans everything up at the end of the run.		*/

static int done, extend, ifsp, off;

void main(argc,argv)
int argc;
char **argv;
{
    SCRATCH unsigned *o;
    int newline(void);

    printf("1802/1805A Cross-Assembler (Portable) Ver 2.5+\n");
    printf("Copyright (c) 1985 William C. Colley, III\n");
	printf("fixes for LCC/Windows by HRJ Jan 2010\n\n");

    while (--argc > 0) {
	if (**++argv == '-') {
	    switch (toupper(*++*argv)) {
		case 'L':   if (!*++*argv) {
				if (!--argc) { warning(NOLST);  break; }
				else ++argv;
			    }
			    lopen(*argv);
			    break;

		case 'O':   if (!*++*argv) {
				if (!--argc) { warning(NOHEX);  break; }
				else ++argv;
			    }
			    hopen(*argv);
			    break;

		default:    warning(BADOPT);
	    }
	}
	else if (filestk[0]) warning(TWOASM);
	else if (!(filestk[0] = fopen(*argv,"r"))) fatal_error(ASMOPEN);
    }
    if (!filestk[0]) fatal_error(NOASM);

    while (++pass < 3) {
	fseek(source = filestk[0],0L,0);  done = extend = off = FALSE;
	errors = filesp = ifsp = pagelen = pc = 0;  title[0] = '\0';
	while (!done) {
	    errcode = ' ';
	    if (newline()) {
		error('*');
		strcpy(line,"\tEND\n");
		done = eject = TRUE;  listhex = FALSE;
		bytes = 0;
	    }
	    else asm_line();
	    pc = word(pc + bytes);
	    if (pass == 2) {
		lputs();
		for (o = obj; bytes--; hputc(*o++));
	    }
	}
    }

    fclose(filestk[0]);  lclose();  hclose();

    if (errors) printf("%d Error(s)\n",errors);
    else printf("No Errors\n");

    exit(errors);
}

/*  Line assembly routine.  This routine gets expressions and tokens	*/
/*  from the source file using the expression evaluator and lexical	*/
/*  analyzer, respectively.  It fills a buffer with the machine code	*/
/*  bytes and returns nothing.						*/

static char label[MAXLINE];
static int ifstack[IFDEPTH] = { ON };

static OPCODE *opcod;

void asm_line(void)
{
    SCRATCH int i, j;
    int popc(void);
    OPCODE *find_code(char *), *find_operator(char *);


    address = pc;  bytes = 0;  eject = forwd = listhex = FALSE;
    for (i = 0; i < BIGINST; obj[i++] = NOP);

    label[0] = '\0';
    if ((i = popc()) != ' ' && i != '\n') {
	if (isalph((char) i)) {
	    pushc(i);  pops(label);
		/* HRJ remove colon from label */
		j = strlen(label); if (label[j-1]==':') label[j-1] ='\0';
		/* printf("asm_Line>>%s<<\n",label);  HRJ diagnostic */
	    if (find_operator(label)) { label[0] = '\0';  error('L'); }
	}
	else {
	    error('L');
	    while ((i = popc()) != ' ' && i != '\n');
	}
    }

    trash(); opcod = NULL;
    if ((i = popc()) != '\n') {
	if (!isalph((char) i)) error('S');
	else {
	    pushc(i);  pops(token.sval);
	    if (!(opcod = find_code(token.sval))) error('O');
	}
	if (!opcod) { listhex = TRUE;  bytes = BIGINST; }
    }

    if (opcod && opcod -> attr & ISIF) { if (label[0]) error('L'); }
    else if (off) { listhex = FALSE;  flush();  return; }

    if (!opcod) { do_label();  flush(); }
    else {
	listhex = TRUE;
	if (opcod -> attr & PSEUDO) pseudo_op();
	else normal_op();
	while ((i = popc()) != '\n') if (i != ' ') error('T');
    }
    source = filestk[filesp];
    return;
}

static void flush(void)
{
	int popc(void);

    while (popc() != '\n');
}

static void do_label(void)
{
    SCRATCH SYMBOL *l;
    SYMBOL *find_symbol(char *), *new_symbol(char *);

    if (label[0]) {
	/* printf("do_label>>%s<<\n",label); /* HRJ diagnostic */
	listhex = TRUE;
	if (pass == 1) {
	    if (!((l = new_symbol(label)) -> attr)) {
		l -> attr = FORWD + VAL;
		l -> valu = pc;
	    }
	}
	else {
	    if (l = find_symbol(label)) {
		l -> attr = VAL;
		if (l -> valu != pc) error('M');
	    }
	    else error('P');
	}
    }
}

static void normal_op(void)
{
    SCRATCH unsigned attrib, *objp, operand;
    unsigned expr(void);
    TOKEN *lex(void);
    void do_label(void), unlex(void);

    do_label();
    bytes = (attrib = opcod -> attr) & BYTES;
    if (pass == 1) return;

    objp = obj;
    if (attrib & IS1805) {
	*objp++ = PREBYTE;
	if (!extend) error('O');
    }
    *objp++ = opcod -> valu;  objp[0] = objp[1] = 0;

    while (attrib & (REGTYP + NUMTYP)) {
	operand = expr();
	switch (attrib & REGTYP) {
	    case IOPORT:    if (operand > 7) { error('R');  return; }

	    case NOT_R0:    if (!operand) { error('R');  return; }

	    case ANY:	    if (operand > 15) { error('R');  return; }
			    *(objp - 1) += operand;  attrib &= ~(REGTYP);
			    break;

	    case NONE:	    switch (attrib & NUMTYP) { /*none was null HRJ */
				case SIXTN:	*objp++ = high(operand);
						*objp = low(operand);
						break;

				case BRANCH:    if (high(operand) !=
						    high(pc + bytes - 1)) {
						    error('B');  return;
						}
						*objp = low(operand);
						break;

				case IMMED:	if (operand > 0xff &&
						    operand < 0xff80) {
						    error('V');  return;
						}
						*objp = low(operand);
			    }
			    attrib &= ~NUMTYP;
			    break;
	}
    }
}

static void pseudo_op(void)
{
    SCRATCH char *s;
    SCRATCH unsigned *o, u;
    SCRATCH SYMBOL *l;
    unsigned expr(void);
    SYMBOL *find_symbol(char *), *new_symbol(char *);
    TOKEN *lex(void);


    o = obj;
    switch (opcod -> valu) {
	case BLK:   do_label();
		    u = word(pc + expr());
		    if (forwd) error('P');
		    else {
			pc = u;
			if (pass == 2) hseek(pc);
		    }
		    break;

	case BYTE:  do_label(); /* printf("\npsop BYTE "); HRJ debug*/
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0; /* return 0 on seperator */
			else if ((token.attr & TYPE) == STR) { /* string added HRJ*/
	    		    for (s = token.sval; *s; *o++ = *s++)
			    ++bytes;
			    if ((lex() -> attr & TYPE) != SEP) unlex();
			} /* end string HRJ */
			else { /* its some kind of value or expression HRJ */
			    unlex();
			    if ((u = expr()) > 0xff && u < 0xff80) {
				u = 0;  error('V'); /* byte values only */
			    }
				*o++ = low(u);  ++bytes;
			}
			/* printf("u=%i s=%u attr=%i ",u, obj[0], token.attr); hrj debug*/
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case CPU:   listhex = FALSE;  do_label();
		    u = expr();
		    if (forwd) error('P');
		    else if (u != 1802 && u != 1805) error('V');
		    else extend = u == 1805;
		    break;

	case EJCT:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) != EOL) {
			unlex();  pagelen = expr();
			if (pagelen > 0 && pagelen < 3) {
			    pagelen = 0;  error('V');
			}
		    }
		    eject = TRUE;
		    break;

	case ELSE:  listhex = FALSE;
		    if (ifsp) off = (ifstack[ifsp] = -ifstack[ifsp]) != ON;
		    else error('I');
		    break;

	case END:   do_label();
		    if (filesp) { listhex = FALSE;  error('*'); }
		    else {
			done = eject = TRUE;
			if (pass == 2 && (lex() -> attr & TYPE) != EOL) {
			    unlex();  hseek(address = expr());
			}
			if (ifsp) error('I');
		    }
		    break;

	case ENDI:  listhex = FALSE;
		    if (ifsp) off = ifstack[--ifsp] != ON;
		    else error('I');
		    break;

	case EQU:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				l -> attr = VAL;
				address = expr();
				if (forwd) error('P');
				if (l -> valu != address) error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

	case IF:    if (++ifsp == IFDEPTH) fatal_error(IFOFLOW);
		    address = expr();
		    if (forwd) { error('P');  address = TRUE; }
		    if (off) { listhex = FALSE;  ifstack[ifsp] = ZERO; } /* was NULL but error */
		    else {
			ifstack[ifsp] = address ? ON : OFF;
			if (!address) off = TRUE;
		    }
		    break;

	case LOAD:  do_label();  bytes = 6;
		    obj[0] = obj[3] = LDI;
		    if ((u = expr()) > 15) { u = 0;  error('R'); }
		    obj[2] = PHI + u;  obj[5] = PLO + u;
		    u = expr();
		    obj[1] = high(u);  obj[4] = low(u);
		    break;

	case INCL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == STR) {
			if (++filesp == FILES) fatal_error(FLOFLOW);
			if (!(filestk[filesp] = fopen(token.sval,"r"))) {
			    --filesp;  error('V');
			}
		    }
		    else error('S');
		    break;

	case ORG:   u = expr();
		    if (forwd) error('P');
		    else {
			pc = address = u;
			if (pass == 2) hseek(pc);
		    }
		    do_label();
		    break;

	case PAGE:  address = pc = (pc + 255) & 0xff00;
		    if (pass == 2) hseek(pc);
		    do_label();
		    break;

	case SET:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)
				|| (l -> attr & SOFT)) {
				l -> attr = FORWD + SOFT + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				address = expr();
				if (forwd) error('P');
				else if (l -> attr & SOFT) {
				    l -> attr = SOFT + VAL;
				    l -> valu = address;
				}
				else error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

	case TEXT:  do_label(); /* printf("\npsop TEXT "); HRJ debug*/
		    while ((lex() -> attr & TYPE) != EOL) {
			if ((token.attr & TYPE) == STR) {
			    for (s = token.sval; *s; *o++ = *s++)
				++bytes;
			    if ((lex() -> attr & TYPE) != SEP) unlex();
			}
			else error('S');
			/* printf("s=%u ",obj[0]);  hrj */
		    }
		    break;

	case TITL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) title[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error('S');
		    else strcpy(title,token.sval);
		    break;

	case WORD:  do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else { unlex();  u = expr(); }
			*o++ = high(u);  *o++ = low(u);
			bytes += 2;
		    } while ((token.attr & TYPE) == SEP);
		    break;
    }
    return;
}
