/*
 * Copyright (c) 2006, Adam Dunkels
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "ubasic.h"
#include <stdio.h>

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINTF(...)  printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif


static const char program[] =
        "1 print \"start\"\n\
10 gosub 100\n\
20 for i = 1 to 10\n\
30 print i\n\
25 print \"before\"\n\
40 next i\n\
50 print \"end\"\n\
60 end\n\
100 print \"subroutine\"\n\
105 print 2+3*4+5\n\
106 print 2\n\
107 print ( 2 + 3 ) * ( 4 + 5 ) \n\
108 print 3\n\
110 return\n";

#ifdef OLD

/*---------------------------------------------------------------------------*/
int
main(void) {
    ubasic_init(program);

    do {
        ubasic_run();
    } while (!ubasic_finished());

    return 0;
}
/*---------------------------------------------------------------------------*/
#endif

#define MAX_FILE_SIZE 10000

int main(int argc, char *argv[]) {

    char *input, *filename;
    FILE *fptr;
    char linebuff[1000];
    int i;
    char c;

    char source[MAX_FILE_SIZE + 1];

    if (argc == 2) {
        filename = argv[1];

        if ((fptr = fopen(filename, "r")) == NULL) {
            printf("Error! opening file");
            exit(1);
        } else {
            tokenizer_tokenize(); // set mode
            do {
                i = 0;
                do {
                    c = fgetc(fptr);
                    linebuff[i++] = c;
                } while ((c != '\n') && (c != -1));
                if (c != -1) {
                    linebuff[i++] = 0;
                    DEBUG_PRINTF("line=[%s] [%d]\n", linebuff, c);
                    char *buff = tokenizeLine(linebuff);
                }
            } while (c != EOF);

            fclose(fptr);

            printf("Load file done\n");
            tokenizer_tokenize(); // set mode

            //char *buff = tokenizeLine(linebuff);

            printf("tokening done\n");

            //new_ubasic_init(buff); //probably should be 0
            tokenizer_run(); // set mode
            new_ubasic_init(0);

            //ubasic_init(program); orignal code

            do {
                ubasic_run();
            } while (!ubasic_finished());

            return 0;
            exit(0);
        }
    } else {
        printf("Interactive mode\n");
        while (1) {
            tokenizer_tokenize(); // set mode

            i = 0;
            do {
                c = fgetc(stdin);
                linebuff[i++] = c;
            } while ((c != '\n') && (c != -1));

            DEBUG_PRINTF("input line=[%s]\n", linebuff);

            if (strncmp(linebuff, "run", 3) == 0) {
                DEBUG_PRINTF(" found run \n");
                tokenizer_run(); // set mode
                new_ubasic_init(0);
                do {
                    ubasic_run();
                } while (!ubasic_finished());
                DEBUG_PRINTF("run done\n");
            } else if (strncmp(linebuff, "list", 4) == 0) {
                DEBUG_PRINTF(" do list\n");
                detokenize();
            } else if (strncmp(linebuff, "dump", 4) == 0) {
                DEBUG_PRINTF(" do list\n");
                dumpBuffer(0, 350);
            } else if (strncmp(linebuff, "dhex", 4) == 0) {
                DEBUG_PRINTF(" do list\n");
                dumpBufferHex(0, 350);
            } else {
                tokenizeLine(linebuff);
            }

        }
    }


#ifdef OLD
    tokenizer_tokenize(); // set mode

    char *buff = tokenize(source);

    tokenizer_run();

    printf("tokening done\n");

    //new_ubasic_init(buff); //probably should be 0

    new_ubasic_init(0);
    //ubasic_init(program); orignal code

    do {
        ubasic_run();
    } while (!ubasic_finished());

    return 0;
#endif
}

