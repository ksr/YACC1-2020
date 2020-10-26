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

#define TOKEN_BUFFER_SIZE 2000
unsigned char tokenBuffer[TOKEN_BUFFER_SIZE];
int tokenBufferPtr;
int endOfBuffer=0;

#define MODE_TOKENIZE 1
#define MODE_RUN 2

int mode = MODE_TOKENIZE;

#define MAX_NUMLEN 6

struct keyword_token {
    char *keyword;
    int token;
};

static int current_token = TOKENIZER_ERROR;

void removeLine(int);
int findLine(int);
void addLine(char *);

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
    DEBUG_PRINTF("new_tokenizer_goto %d\n", ptr);
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
    DEBUG_PRINTF("new_tokenizer_init %d\n", ptr);
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
                // oct 21 tokenBufferPtr += (strlen(&tokenBuffer[tokenBufferPtr]) + 1);
                tokenBufferPtr += (strlen(&tokenBuffer[tokenBufferPtr]) + 1); // + 2 null and token_string
                DEBUG_PRINTF("--ptr = %d\n", tokenBufferPtr);
                break;
            case TOKENIZER_VARIABLE:
                tokenBufferPtr += 3;
                break;
            case TOKENIZER_LINENUM:
                tokenBufferPtr += 5;
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
        memcpy(&tmp, &tokenBuffer[tokenBufferPtr + 1 ], 2);
        //DEBUG_PRINTF("tn=[%d] %d %d \n",tmp,tokenBuffer[tokenBufferPtr + 1],tokenBuffer[tokenBufferPtr + 2]);

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
        // orig oct 21 memcpy(dest, &tokenBuffer[tokenBufferPtr], strlen(&tokenBuffer[tokenBufferPtr]));
        memcpy(dest, &tokenBuffer[tokenBufferPtr+1], strlen(&tokenBuffer[tokenBufferPtr+1])); //added +1 to skip over token_string
        dest[strlen(&tokenBuffer[tokenBufferPtr])] = 0;
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
    else {
        memcpy(&tmp, &tokenBuffer[tokenBufferPtr + 1], 2);
        //DEBUG_PRINTF("tvn=[%d] %d %d \n",tmp,tokenBuffer[tokenBufferPtr +1 ],tokenBuffer[tokenBufferPtr + 2]);
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
    char newLine[20];

    printf(" tokenize \n");
    tokenizer_init(program);
    while (tokenizer_token() != TOKENIZER_ENDOFINPUT) {

        workLine.lineNumber = tokenizer_num();
        //workLine.lineTokens=string
        tokencounter = 0;
        tokenizer_next();
        while (tokenizer_token() != TOKENIZER_CR) {
            workLine.lineTokens[tokencounter++] = tokenizer_token();
            switch (tokenizer_token()) {
                case TOKENIZER_NUMBER:
                    //workLine.lineTokens[tokencounter].val = tokenizer_num();
                    //DEBUG_PRINTF("in tnum [%d]",tokenizer_num());
                    tmp = tokenizer_num();
                    //DEBUG_PRINTF("tmp=%d %d\n", tmp, sizeof (tmp));
                    memcpy(&workLine.lineTokens[tokencounter], &tmp, sizeof (tmp));
                    //workLine.lineTokens[tokencounter] = tokenizer_num();
                    tokencounter += 2;
                    break;
                case TOKENIZER_STRING:
                    tokenizer_string(string, sizeof (string));
                    strcpy(&workLine.lineTokens[tokencounter], string);
                    tokencounter += (strlen(string));
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

        DEBUG_PRINTF("tokencounter=[%d]\n", tokencounter);

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
                    i += (strlen(&workLine.lineTokens[i]));
                    break;
                case TOKENIZER_VARIABLE:
                    memcpy(&tmp, &workLine.lineTokens[i], 2);
                    DEBUG_PRINTF("var_id=[%04xH] [%d] ", tmp, tmp);
                    i += 2;
                    //DEBUG_PRINTF("var_id=[%d] ", workLine.lineTokens[i++]);
                    break;
                case TOKENIZER_LINENUM:
                    memcpy(&tmp, &workLine.lineTokens[i], 2);
                    DEBUG_PRINTF("number=[%04xH] [%d] ", tmp, tmp);
                    i += 4;
                    break;
                default:
                    break;
            }
        }

        DEBUG_PRINTF("\n");

        tokenBuffer[bufferptr++] = TOKENIZER_LINENUM;
        memcpy(&tokenBuffer[bufferptr], &workLine.lineNumber, 2);
        int tmp1 = tokencounter + 6;
        memcpy(&tokenBuffer[bufferptr + 2], &tmp1, 2);
        bufferptr += 4;

        memcpy(&tokenBuffer[bufferptr], &workLine.lineTokens[0], tokencounter);

        bufferptr += tokencounter;

        tokenBuffer[bufferptr++ ] = TOKENIZER_CR; //   kkk

    }
    tokenBuffer[bufferptr] = TOKENIZER_ENDOFINPUT;
    dumpBuffer(0, bufferptr);
    DEBUG_PRINTF("find 2 %d\n", findLine(2));
    DEBUG_PRINTF("find 4 %d\n", findLine(4));
    DEBUG_PRINTF("find 5 %d\n", findLine(5));
    DEBUG_PRINTF("find 9 %d\n", findLine(9));
    removeLine(26);
    dumpBuffer(0, bufferptr);
    removeLine(17);
    newLine[0] = 37;
    newLine[1] = 1;
    newLine[2] = 0;
    newLine[3] = 14;
    newLine[4] = 0;
    newLine[5] = 6;
    newLine[6] = 3;
    newLine[7] = 115;
    newLine[8] = 116;
    newLine[9] = 117;
    newLine[10] = 118;
    newLine[11] = 119;
    newLine[12] = 0;
    newLine[13] = 36;

    addLine(newLine);
#if DEBUG
    dumpBuffer(0, bufferptr);
#endif

    //dumpBuffer(0, bufferptr + 5);
    return (tokenBuffer);
}

const char *tokenizeLine(char *line) {
    int tokencounter;
    char string[50];
    int bufferptr = 0;
    int firstLine = 1;
    VARIABLE_TYPE tmp;
    char newLine[20];
    char buff[100];

    DEBUG_PRINTF("tl start[%s]\n", line);
    tokenizer_init(line);

    workLine.lineNumber = tokenizer_num();
    DEBUG_PRINTF("lineNumber=[%d]\n", workLine.lineNumber);
    tokencounter = 0;
    tokenizer_next();
    while (tokenizer_token() != TOKENIZER_CR) {
        workLine.lineTokens[tokencounter++] = tokenizer_token();
        switch (tokenizer_token()) {
            case TOKENIZER_NUMBER:
                tmp = tokenizer_num();
                memcpy(&workLine.lineTokens[tokencounter], &tmp, sizeof (tmp));
                tokencounter += 2;
                break;
            case TOKENIZER_STRING:
                tokenizer_string(string, sizeof (string));
                strcpy(&workLine.lineTokens[tokencounter], string);
                tokencounter += (strlen(string));
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
    //tokenizer_next();

    DEBUG_PRINTF("tokencounter=[%d]\n", tokencounter);

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
                i += (strlen(&workLine.lineTokens[i]));
                break;
            case TOKENIZER_VARIABLE:
                memcpy(&tmp, &workLine.lineTokens[i], 2);
                DEBUG_PRINTF("var_id=[%04xH] [%d] ", tmp, tmp);
                i += 2;
                //DEBUG_PRINTF("var_id=[%d] ", workLine.lineTokens[i++]);
                break;
            case TOKENIZER_LINENUM:
                memcpy(&tmp, &workLine.lineTokens[i], 2);
                DEBUG_PRINTF("number=[%04xH] [%d] ", tmp, tmp);
                i += 4;
                break;
            default:
                break;
        }
    }

    DEBUG_PRINTF("\n--\n");

    buff[bufferptr++] = TOKENIZER_LINENUM;
    memcpy(&buff[bufferptr], &workLine.lineNumber, 2);
    DEBUG_PRINTF("\n--1\n");

    int tmp1 = tokencounter + 6;
    DEBUG_PRINTF("\n--1.5 [%d]\n", tmp1);
    memcpy(&buff[bufferptr + 2], &tmp1, 2);
    DEBUG_PRINTF("\n--2\n");

    bufferptr += 4;

    memcpy(&buff[bufferptr], &workLine.lineTokens[0], tokencounter);
    DEBUG_PRINTF("\n--3\n");
    bufferptr += tokencounter;

    buff[bufferptr++ ] = TOKENIZER_CR; //   kkk

    //tokenBuffer[bufferptr] = TOKENIZER_ENDOFINPUT;

    addLine(buff);

#if DEBUG
    dumpBuffer(0, 350);
#endif
    return (tokenBuffer);
}
int line = 0;

dumpBuffer(int start, int end) {
    int i;


    printf("dump buffer start=%d end=%d\n", start, end);

    for (i = start; i < end; i++) {
        if ((i & 0x0000f) == 0) {
            printf("\n%02d ", i);
        }
        printf("%02d ", tokenBuffer[i]);
    }
    printf("\n");

}

dumpBufferHex(int start, int end) {
    int i;


    printf("dump buffer hex start=%d end=%d\n", start, end);

    for (i = start; i < end; i++) {
        if ((i & 0x0000f) == 0) {
            printf("\n\tDB  %02xh", tokenBuffer[i]);
            i++;
        }
        printf(",%02xh", tokenBuffer[i]);
    }
    printf("\n");

}

int firstLineOut = 1;

void addLine(char *buff) {
    VARIABLE_TYPE newLineNum, newLineLen, ptr;
    VARIABLE_TYPE existingLineNum, existingLineLen;
    int insertPtr;
    int src, dest;

    for (int i = 0; i < 20; i++)
        DEBUG_PRINTF("%d=[%d]\n", i, buff[i]);
    DEBUG_PRINTF("\n");

    memcpy(&newLineNum, &buff[1], 2);

    memcpy(&newLineLen, &buff[3], 2);

    DEBUG_PRINTF("add line newLineNum=%d  newLineLen=%d\n", newLineNum, newLineLen);

    if (firstLineOut == 0) {
        if (findLine(newLineNum) != -1) {
            DEBUG_PRINTF("addLine remove %d\n", newLineNum);
            removeLine(newLineNum);
            if(newLineLen==6) // blank line
                return;
                
        }

        // Find insertion location

        insertPtr = ptr = 0;
        memcpy(&existingLineNum, &tokenBuffer[ptr + 1], 2);
        memcpy(&existingLineLen, &tokenBuffer[ptr + 3], 2);
        DEBUG_PRINTF("existingLineNum=%d existingLineLen=%d\n", existingLineNum, existingLineLen);

        while ((newLineNum > existingLineNum) && (existingLineNum != 0)) {
            memcpy(&existingLineNum, &tokenBuffer[ptr + 1], 2);
            memcpy(&existingLineLen, &tokenBuffer[ptr + 3], 2);
            DEBUG_PRINTF("existingLineNum=%d existingLineLen=%d\n", existingLineNum, existingLineLen);
            insertPtr = ptr;
            ptr += existingLineLen;
        }

        // make room
        src = insertPtr;
        dest = insertPtr + newLineLen;
        DEBUG_PRINTF("src=%d dest=%d len=%d\n", src, dest, newLineLen);

        for (int i = TOKEN_BUFFER_SIZE; i >= src; i--)
            tokenBuffer[i] = tokenBuffer[i - newLineLen];
#if DEBUG
        dumpBuffer(0, 350);
#endif

    } else {
        insertPtr = ptr = 0;
        firstLineOut = 0;
    }
    for (int i = 0; i < newLineLen; i++)
        tokenBuffer[i + insertPtr] = buff[i];
    endOfBuffer+=newLineLen;
    tokenBuffer[endOfBuffer]=TOKENIZER_ENDOFINPUT;

#if DEBUG
    dumpBuffer(0, 350);
#endif

}

void removeLine(int lineNumber) {
    int ptr;
    int dest, src;
    int removeLength;
    int i;

    ptr = findLine(lineNumber);
    if (ptr == -1) {
        DEBUG_PRINTF("removeLine: Line[%d] not found\n", lineNumber);
        return;
    }

    dest = ptr;
    memcpy(&removeLength, &tokenBuffer[ptr + 3], 2);
    src = dest + removeLength;
    DEBUG_PRINTF("dest=%d src=%d length=%d\n", dest, src, removeLength);
    for (i = dest; i < TOKEN_BUFFER_SIZE; i++)
        tokenBuffer[i] = tokenBuffer[i + removeLength];
    endOfBuffer-=removeLength;
    tokenBuffer[endOfBuffer]=TOKENIZER_ENDOFINPUT;
}

int tokenizer_find(int lineNum){
    
    return(findLine(lineNum));
}

int findLine(int lineNumber) {
    int i;
    VARIABLE_TYPE line, len;

    i = 0;
    while (tokenBuffer[i] == TOKENIZER_LINENUM) {
        memcpy(&line, &tokenBuffer[i + 1], 2);
        //DEBUG_PRINTF("line %d %d\n",i, line);
        if (line == lineNumber)
            return (i);
        memcpy(&len, &tokenBuffer[i + 3], 2);
        //DEBUG_PRINTF("len %d %d\n",i,len);
        i = i + len;
    }
    return (-1);
}

void detokenize() {
    int i;
    VARIABLE_TYPE var;

    i = 0;
    while (tokenBuffer[i] != 0) {
        DEBUG_PRINTF("token=[%d]\n", tokenBuffer[i]);
        switch (tokenBuffer[i]) {
            case TOKENIZER_ERROR:
                printf("detokinze TOKENIZER_ERROR - should not happen\n");
                exit(0);
                break;
            case TOKENIZER_ENDOFINPUT:
                return;
                break;
            case TOKENIZER_NUMBER:
                memcpy(&var, &tokenBuffer[i + 1], 2);
                printf("%d ", var);
                i += 3;
                break;
            case TOKENIZER_STRING:
                printf("\"");
                i++;
                while (tokenBuffer[i] != 0) {
                    printf("%c", tokenBuffer[i++]);
                }
                printf("\" ");
                i++;
                break;
            case TOKENIZER_VARIABLE: memcpy(&var, &tokenBuffer[i + 1], 2);
                char varname = 'a' + var;
                printf("%c ", varname);
                i += 3;
                break;
                break;
            case TOKENIZER_LET:
                printf("let ");
                i++;
                break;
            case TOKENIZER_PRINT:
                printf("print ");
                i++;
                break;
            case TOKENIZER_IF:
                printf("if ");
                i++;
                break;
            case TOKENIZER_THEN:
                printf("then ");
                i++;
                break;
            case TOKENIZER_ELSE:
                printf("else ");
                i++;
                break;
            case TOKENIZER_FOR:
                printf("for ");
                i++;
                break;
            case TOKENIZER_TO:
                printf("to ");
                i++;
                break;
            case TOKENIZER_NEXT:
                printf("next ");
                i++;
                break;
            case TOKENIZER_GOTO:
                printf("goto ");
                i++;
                break;
            case TOKENIZER_GOSUB:
                printf("gosub ");
                i++;
                break;
            case TOKENIZER_RETURN:
                printf("return ");
                i++;
                break;
            case TOKENIZER_CALL:
                break;
            case TOKENIZER_REM:
                printf("rem ");
                i++;
                break;
            case TOKENIZER_PEEK:
                printf("peek ");
                i++;
                break;
            case TOKENIZER_POKE:
                printf("poke ");
                i++;
                break;
            case TOKENIZER_END:
                printf("end\n");
                return;
                i++; // not needed
                break;
            case TOKENIZER_COMMA:
                printf(", ");
                i++;
                break;
            case TOKENIZER_SEMICOLON:
                printf("; ");
                i++;
                break;
            case TOKENIZER_PLUS:
                printf("+ ");
                i++;
                break;
            case TOKENIZER_MINUS:
                printf("- ");
                i++;
                break;
            case TOKENIZER_AND:
                printf("& ");
                i++;
                break;
            case TOKENIZER_OR:
                printf("| ");
                i++;
                break;
            case TOKENIZER_ASTR:
                printf("* ");
                i++;
                break;
            case TOKENIZER_SLASH:
                printf("/ ");
                i++;
                break;
            case TOKENIZER_MOD:
                printf("%% ");
                i++;
                break;
            case TOKENIZER_HASH:
                printf("#");
                i++;
                break;
            case TOKENIZER_LEFTPAREN:
                printf("( ");
                i++;
                break;
            case TOKENIZER_RIGHTPAREN:
                printf(") ");
                i++;
                break;
            case TOKENIZER_LT:
                printf("< ");
                i++;
                break;
            case TOKENIZER_GT:
                printf("< ");
                i++;
                break;
            case TOKENIZER_EQ:
                printf("= ");
                i++;
                break;
            case TOKENIZER_CR:
                printf("\n");
                i++;
                break;
            case TOKENIZER_LINENUM:
                memcpy(&var, &tokenBuffer[i + 1], 2);
                printf("%d ", var);
                i += 5;
                break;
        }
    }
}