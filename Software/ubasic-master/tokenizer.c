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

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINTF(...)  printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#include "tokenizer.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static char const *ptr, *nextptr;

unsigned char tokenBuffer[2000];
int tokenBufferPtr;

#define MODE_TOKENIZE 1
#define MODE_RUN 2

int mode = MODE_TOKENIZE;

#define MAX_NUMLEN 6

struct keyword_token {
    char *keyword;
    int token;
};

static int current_token = TOKENIZER_ERROR;


static const struct keyword_token keywords[] = {
    {"let", TOKENIZER_LET},
    {"print", TOKENIZER_PRINT},
    {"if", TOKENIZER_IF},
    {"then", TOKENIZER_THEN},
    {"else", TOKENIZER_ELSE},
    {"for", TOKENIZER_FOR},
    {"to", TOKENIZER_TO},
    {"next", TOKENIZER_NEXT},
    {"goto", TOKENIZER_GOTO},
    {"gosub", TOKENIZER_GOSUB},
    {"return", TOKENIZER_RETURN},
    {"call", TOKENIZER_CALL},
    {"rem", TOKENIZER_REM},
    {"peek", TOKENIZER_PEEK},
    {"poke", TOKENIZER_POKE},
    {"end", TOKENIZER_END},
    {NULL, TOKENIZER_ERROR}
};

void tokenizer_run() {
    mode = MODE_RUN;
}

void tokenizer_tokenize() {
    mode = MODE_TOKENIZE;
}

/*---------------------------------------------------------------------------*/
static int
singlechar(void) {
    if (mode == MODE_TOKENIZE) {
        if (*ptr == '\n') {
            return TOKENIZER_CR;
        } else if (*ptr == ',') {
            return TOKENIZER_COMMA;
        } else if (*ptr == ';') {
            return TOKENIZER_SEMICOLON;
        } else if (*ptr == '+') {
            return TOKENIZER_PLUS;
        } else if (*ptr == '-') {
            return TOKENIZER_MINUS;
        } else if (*ptr == '&') {
            return TOKENIZER_AND;
        } else if (*ptr == '|') {
            return TOKENIZER_OR;
        } else if (*ptr == '*') {
            return TOKENIZER_ASTR;
        } else if (*ptr == '/') {
            return TOKENIZER_SLASH;
        } else if (*ptr == '%') {
            return TOKENIZER_MOD;
        } else if (*ptr == '(') {
            return TOKENIZER_LEFTPAREN;
        } else if (*ptr == '#') {
            return TOKENIZER_HASH;
        } else if (*ptr == ')') {
            return TOKENIZER_RIGHTPAREN;
        } else if (*ptr == '<') {
            return TOKENIZER_LT;
        } else if (*ptr == '>') {
            return TOKENIZER_GT;
        } else if (*ptr == '=') {
            return TOKENIZER_EQ;
        }
        return 0;
    } else {
        return (tokenBuffer[tokenBufferPtr]);
    }
}

/*---------------------------------------------------------------------------*/
static int
get_next_token(void) {
    struct keyword_token const *kt;
    int i;

    DEBUG_PRINTF("get_next_token(): '%s'\n", ptr);

    if (mode == MODE_TOKENIZE) {
        if (*ptr == 0) {
            return TOKENIZER_ENDOFINPUT;
        }

        if (isdigit(*ptr)) {
            for (i = 0; i < MAX_NUMLEN; ++i) {
                if (!isdigit(ptr[i])) {
                    if (i > 0) {
                        nextptr = ptr + i;
                        return TOKENIZER_NUMBER;
                    } else {
                        DEBUG_PRINTF("get_next_token: error due to too short number\n");
                        return TOKENIZER_ERROR;
                    }
                }
                if (!isdigit(ptr[i])) {
                    DEBUG_PRINTF("get_next_token: error due to malformed number\n");
                    return TOKENIZER_ERROR;
                }
            }
            DEBUG_PRINTF("get_next_token: error due to too long number\n");
            return TOKENIZER_ERROR;
        } else if (singlechar()) {
            nextptr = ptr + 1;
            return singlechar();
        } else if (*ptr == '"') {
            nextptr = ptr;
            do {
                ++nextptr;
            } while (*nextptr != '"');
            ++nextptr;
            return TOKENIZER_STRING;
        } else {
            for (kt = keywords; kt->keyword != NULL; ++kt) {
                if (strncmp(ptr, kt->keyword, strlen(kt->keyword)) == 0) {
                    nextptr = ptr + strlen(kt->keyword);
                    return kt->token;
                }
            }
        }

        if (*ptr >= 'a' && *ptr <= 'z') {
            nextptr = ptr + 1;
            return TOKENIZER_VARIABLE;
        }
        return TOKENIZER_ERROR;
    } else {
        return (tokenBuffer[tokenBufferPtr]);
    }
}

/*---------------------------------------------------------------------------*/
void
tokenizer_goto(const char *program) {
    DEBUG_PRINTF("tokenizer_goto\n");
    ptr = program;
    current_token = get_next_token();
    DEBUG_PRINTF("tokenizer_goto %d\n", current_token);
}

void
new_tokenizer_goto(int ptr) {
    DEBUG_PRINTF("new_tokenizer_goto %d\n",ptr);
    tokenBufferPtr = ptr;
    current_token = get_next_token();
    DEBUG_PRINTF("new_tokenizer_goto %d\n", current_token);
}

/*---------------------------------------------------------------------------*/
void
tokenizer_init(const char *program) {
    DEBUG_PRINTF("tokenizer_init\n");
    tokenizer_goto(program);
    current_token = get_next_token();
    DEBUG_PRINTF("tokenizer_init %d\n", current_token);
}

void
new_tokenizer_init(int ptr) {
    DEBUG_PRINTF("new_tokenizer_init %d\n",ptr);
    new_tokenizer_goto(ptr);
    current_token = get_next_token();
    DEBUG_PRINTF("new_tokenizer_init %d\n", current_token);
}

/*---------------------------------------------------------------------------*/
int
tokenizer_token(void) {
    if (mode == MODE_TOKENIZE)
        return current_token;
    else
        return tokenBuffer[tokenBufferPtr];
}

/*---------------------------------------------------------------------------*/
void
tokenizer_next(void) {

    if (tokenizer_finished()) {
        return;
    }

    

    if (mode == MODE_TOKENIZE) {
        DEBUG_PRINTF("tokenizer_next: %p\n", nextptr);
        ptr = nextptr;

        while (*ptr == ' ') {
            ++ptr;
        }
        current_token = get_next_token();

        if (current_token == TOKENIZER_REM) {
            while (!(*nextptr == '\n' || tokenizer_finished())) {
                ++nextptr;
            }
            if (*nextptr == '\n') {
                ++nextptr;
            }
            tokenizer_next();
        }

        DEBUG_PRINTF("tokenizer_next: '%s' %d\n", ptr, current_token);
        return;
    } else {
        DEBUG_PRINTF("tokenizer_next run: %d\n", tokenBufferPtr);
        switch (tokenBuffer[tokenBufferPtr]) {
            case TOKENIZER_NUMBER:
                tokenBufferPtr += 3;
                break;
            case TOKENIZER_STRING:
                DEBUG_PRINTF("-ptr = %d\n", tokenBufferPtr);
                tokenBufferPtr += (strlen(&tokenBuffer[tokenBufferPtr]) + 1);
                DEBUG_PRINTF("--ptr = %d\n", tokenBufferPtr);
                break;
            case TOKENIZER_VARIABLE:
                tokenBufferPtr += 3;
                break;
            default:
                tokenBufferPtr++;
                break;
        }

    }
}

/*---------------------------------------------------------------------------*/
VARIABLE_TYPE
tokenizer_num(void) {
    VARIABLE_TYPE tmp;

    if (mode == MODE_TOKENIZE)
        return atoi(ptr);
    else {
        memcpy(&tmp, &tokenBuffer[tokenBufferPtr +1 ], 2);
        //printf("tn=[%d] %d %d \n",tmp,tokenBuffer[tokenBufferPtr + 1],tokenBuffer[tokenBufferPtr + 2]);
       
        return tmp;
    }

}

/*---------------------------------------------------------------------------*/
void
tokenizer_string(char *dest, int len) {
    char *string_end;
    int string_len;

    if (mode == MODE_TOKENIZE) {
        if (tokenizer_token() != TOKENIZER_STRING) {
            return;
        }
        string_end = strchr(ptr + 1, '"');
        if (string_end == NULL) {
            return;
        }
        string_len = string_end - ptr - 1;
        if (len < string_len) {
            string_len = len;
        }
        memcpy(dest, ptr + 1, string_len);
        dest[string_len] = 0;
    } else {
        memcpy(dest, &tokenBuffer[tokenBufferPtr], strlen(&tokenBuffer[tokenBufferPtr]));
        dest[strlen(&tokenBuffer[tokenBufferPtr])]=0;
    }
}

/*---------------------------------------------------------------------------*/
void
tokenizer_error_print(void) {
    DEBUG_PRINTF("tokenizer_error_print: '%s'\n", ptr);
}

/*---------------------------------------------------------------------------*/
int
tokenizer_finished(void) {
    if (mode == MODE_TOKENIZE)
        return *ptr == 0 || current_token == TOKENIZER_ENDOFINPUT;
    else
        return (tokenBuffer[tokenBufferPtr] == TOKENIZER_ENDOFINPUT);
}

/*---------------------------------------------------------------------------*/
int
tokenizer_variable_num(void) {
    VARIABLE_TYPE tmp;
    
    if (mode == MODE_TOKENIZE)
        return *ptr - 'a';
    else{
        memcpy(&tmp, &tokenBuffer[tokenBufferPtr + 1], 2);
        //printf("tvn=[%d] %d %d \n",tmp,tokenBuffer[tokenBufferPtr +1 ],tokenBuffer[tokenBufferPtr + 2]);
        return tmp;
        //return (tokenBuffer[tokenBufferPtr]);
    }
        
}

/*---------------------------------------------------------------------------*/
char const *
tokenizer_pos(void) {
    if (mode == MODE_TOKENIZE)
        return (ptr);
    else
        return (tokenBufferPtr);
}


// NEW NEW NEW

#define MAX_TOKENS_PER_LINE 100

struct tokenizedLine {
    int lineNumber;
    unsigned char lineTokens[MAX_TOKENS_PER_LINE];
};

struct tokenizedLine workLine;

const char *tokenize(char *program) {
    int tokencounter;
    char string[50];
    int bufferptr = 0;
    int firstLine = 1;
    VARIABLE_TYPE tmp;

    printf("\n");
    tokenizer_init(program);
    while (tokenizer_token() != TOKENIZER_ENDOFINPUT) {

        workLine.lineNumber = tokenizer_num();
        //workLine.lineTokens=string;

        tokencounter = 0;
        tokenizer_next();
        while (tokenizer_token() != TOKENIZER_CR) {
            workLine.lineTokens[tokencounter++] = tokenizer_token();
            switch (tokenizer_token()) {
                case TOKENIZER_NUMBER:
                    //workLine.lineTokens[tokencounter].val = tokenizer_num();
                    //printf("in tnum [%d]",tokenizer_num());
                    tmp = tokenizer_num();
                    //printf("tmp=%d %d\n", tmp, sizeof (tmp));
                    memcpy(&workLine.lineTokens[tokencounter], &tmp, sizeof (tmp));
                    //workLine.lineTokens[tokencounter] = tokenizer_num();
                    tokencounter += 2;
                    break;
                case TOKENIZER_STRING:
                    tokenizer_string(string, sizeof (string));
                    strcpy(&workLine.lineTokens[tokencounter], string);
                    tokencounter += (strlen(string) );
                    workLine.lineTokens[tokencounter++] = 0;
                    break;
                case TOKENIZER_VARIABLE:
                    tmp = tokenizer_variable_num();
                    memcpy(&workLine.lineTokens[tokencounter], &tmp, sizeof (tmp));
                    tokencounter += 2;
                    break;
            }
            tokenizer_next();
        }
        tokenizer_next();
        
        DEBUG_PRINTF("tokencounter=[%d]\n",tokencounter);
        
        DEBUG_PRINTF("Line[%d] Tokens: ", workLine.lineNumber);
        for (int i = 0; i < tokencounter;) {
            DEBUG_PRINTF(" T-%02d ", workLine.lineTokens[i]);
            switch (workLine.lineTokens[i++]) {
                case TOKENIZER_NUMBER:
                    memcpy(&tmp, &workLine.lineTokens[i], 2);
                    DEBUG_PRINTF("number=[%04xH] [%d] ", tmp, tmp);
                    i += 2;
                    break;
                case TOKENIZER_STRING:
                    DEBUG_PRINTF("string=[%s] ", &workLine.lineTokens[i]);
                    i += (strlen(&workLine.lineTokens[i]) );
                    break;
                case TOKENIZER_VARIABLE:
                    memcpy(&tmp, &workLine.lineTokens[i], 2);
                    DEBUG_PRINTF("var_id=[%04xH] [%d] ", tmp, tmp);
                    i += 2;
                    //printf("var_id=[%d] ", workLine.lineTokens[i++]);
                    break;
                default:
                    break;
            }
        }

        DEBUG_PRINTF("\n");

        tokenBuffer[bufferptr++] = TOKENIZER_NUMBER;
        memcpy(&tokenBuffer[bufferptr], &workLine.lineNumber, 2);

        bufferptr += 2;

        memcpy(&tokenBuffer[bufferptr], &workLine.lineTokens[0], tokencounter);

        bufferptr += tokencounter;

        tokenBuffer[bufferptr++ ] = TOKENIZER_CR;//   kkk

        //dumpBuffer(0, bufferptr);
    }
    tokenBuffer[bufferptr] = TOKENIZER_ENDOFINPUT;
    //dumpBuffer(0, bufferptr + 5);
    return (tokenBuffer);
}
int line = 0;

dumpBuffer(int start, int end) {

    printf("dump buffer\n");

    for (int i = start; i < end; i++)
        printf("%d-[%d]\n", i,tokenBuffer[i]);
}

void addline(){
    
}

int removeLine(int lineNumber){
    
}

void findLine(){
    
}