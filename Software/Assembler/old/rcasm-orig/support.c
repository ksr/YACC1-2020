#define SUPPORT

#include <stdio.h>
#include <string.h>
#include "header.h"
#include "mstrings.h"

void write_line() {
  int i;
  byte cksum;
  if (pass != 2) return;
  if (outCount > 0) {
    if (emucode == 'I') {
      if (outUC == 'Y') fprintf(outfile,":%04X",outAddress);
        else fprintf(outfile,":%04x",outAddress);
      for (i=0; i<outCount; i++) {
        if (outUC == 'Y') fprintf(outfile," %02X",outRow[i]);
          else fprintf(outfile," %02x",outRow[i]);
        }
#ifdef WIN32
      fprintf(outfile,"\r\n");
#else
      fprintf(outfile,"\n");
#endif
      }
    if (emucode == 'H') {
      if (outUC == 'Y') fprintf(outfile,":%02X%04X00",outCount,outAddress);
        else fprintf(outfile,":%02x%04x00",outCount,outAddress);
      cksum = outCount + (outAddress & 255) + (outAddress >> 8);
      for (i=0; i<outCount; i++) {
        if (outUC == 'Y') fprintf(outfile,"%02X",outRow[i]);
          else fprintf(outfile,"%02x",outRow[i]);
        cksum += outRow[i];
        }
      cksum = 0-cksum;
#ifdef WIN32
      if (outUC == 'Y') fprintf(outfile,"%02X\r\n",cksum);
        else fprintf(outfile,"%02x\r\n",cksum);
#else
      if (outUC == 'Y') fprintf(outfile,"%02X\n",cksum);
        else fprintf(outfile,"%02x\n",cksum);
#endif
      }
    }
  outCount = 0;
  outAddress = address;
  }

void write_byte(byte i)
{
  if (pass==2) {
    total_bytes++;
    outRow[outCount++] = i;

    if (showlist=='*') {
      if (bcount==4) {
#ifdef WIN32
        printf("\r\n            ");
#else
        printf("\n            ");
#endif
        bcount=0;
        }
      if (outUC == 'Y') printf("%02X ",i);
        else printf("%02x ",i);
      bcount++;
      }
    }
  if (--outputCount == 0) {
    address++;
    if (address>highest) highest=address;
    outputCount=addressSize;
    }
  if (pass == 2 && outCount == 16) write_line();
  valid=1;
}

void write_word(word i) {
  if (endianess == 'B') {
    write_byte(i >> 8);
    write_byte(i & 0xff);
    } else {
    write_byte(i & 0xff);
    write_byte(i >> 8);
    }
  }

char* ltrim(char* buffer) {
  while (*buffer != 0 && *buffer <= ' ') buffer++;
  return buffer;
  }

word find_label_number(char* buf) {
  int i;
  int f;
  char ebuffer[64];
  f = 0;
  for (i=1;i<=nlabel;i++) {
    if (strcmp(labels[i],buf)==0)  f=i;
    }
  if (pass == 1) return f;
  if (f==0) {
    sprintf(ebuffer,"Label not found >> %s",buf);
    errorOut(ebuffer);
    errs++;
    return 0;
    }
  return f;
  }

word find_label(char* buf)
{
  int i;
  int f;
  char buffer[30];
  char ebuffer[64];
  char mode;
  mode='N';
  strcpy(buffer,buf);
/*
  i=pos('.',buffer);
  if (i!=0) {
    if (buffer[i]=='1') mode='H';
    if (buffer[i]=='0') mode='L';
    buffer[i-1]='\0';
    }
*/
  f=0;
  for (i=1;i<=nlabel;i++) {
    if (strcmp(labels[i],buffer)==0)  f=i;
    }
  if (f==0 && pass == 1) return 1;
  if (f==0) {
    sprintf(ebuffer,"Label not found >> %s",buf);
    errorOut(ebuffer);
    errs++;
    }
  if (pass != 1) {
#ifdef WIN32
    if (f!=0 && doxref=='Y') fprintf(xref,"%s %d\r\n",buffer,linecount);
#else
    if (f!=0 && doxref=='Y') fprintf(xref,"%s %d\n",buffer,linecount);
#endif
    }
  if (f!=0) {
    if (labelf[f]=='X') argflag[ArgCount]=f;
    switch(mode) {
      case 'L':return labela[f] % 256; break;
      case 'H':return labela[f] / 256; break;
      case 'N':return labela[f]; break;
      }
    return 0;
    }
   else return 0;
}

byte char_to_hex(char buffer) {
  byte t;
  t = 255;
  switch (buffer) {
    case '0':t=0; break;   case '1':t=1; break;
    case '2':t=2; break;   case '3':t=3; break;
    case '4':t=4; break;   case '5':t=5; break;
    case '6':t=6; break;   case '7':t=7; break;
    case '8':t=8; break;   case '9':t=9; break;
    case 'A':t=10; break;  case 'B':t=11; break;
    case 'C':t=12; break;  case 'D':t=13; break;
    case 'E':t=14; break;  case 'F':t=15; break;
    case 'a':t=10; break;  case 'b':t=11; break;
    case 'c':t=12; break;  case 'd':t=13; break;
    case 'e':t=14; break;  case 'f':t=15; break;
    }
  return t;
  }

word conv_hex(char* buffer)
{
  word t;
  t=0;
  while (*buffer!='\0')
    {
      switch (upcase(*buffer++)) {
        case '0':t=t*16+0; break;   case '1':t=t*16+1; break;
        case '2':t=t*16+2; break;   case '3':t=t*16+3; break;
        case '4':t=t*16+4; break;   case '5':t=t*16+5; break;
        case '6':t=t*16+6; break;   case '7':t=t*16+7; break;
        case '8':t=t*16+8; break;   case '9':t=t*16+9; break;
        case 'A':t=t*16+10; break;  case 'B':t=t*16+11; break;
        case 'C':t=t*16+12; break;  case 'D':t=t*16+13; break;
        case 'E':t=t*16+14; break;  case 'F':t=t*16+15; break;
        case 'a':t=t*16+10; break;  case 'b':t=t*16+11; break;
        case 'c':t=t*16+12; break;  case 'd':t=t*16+13; break;
        case 'e':t=t*16+14; break;  case 'f':t=t*16+15; break;
        }
   }
  return t;
}


char id_number(char* buffer) {
  char ret;
  char flag;
  ret = 'D';
  flag = '*';
  while (flag == '*') {
    if (*buffer == 'H') return 'H';
    if (*buffer == 'h') return 'H';
    if (*buffer < '0') return ret;
    if (*buffer > '9' && *buffer < 'A') return ret;
    if (*buffer > 'F') return ret;
    if (*buffer >= 'A' && *buffer <='F') ret='H';
    if (*buffer >= 'a' && *buffer <='f') ret='H';
    buffer++;
    }
  return 0;
  }

word number(char* buffer) {
  word ret;
  byte v;
  char numType;
  ret = 0;
printf("number(%s)\n",buffer);
  if (*buffer >= '0' && *buffer <= '9') {
    numType = id_number(buffer);
    if (numType == 'D') {
      ret = 0;
      while (*buffer >= '0' && *buffer <= '9') {
        ret = ret * 10 + (*buffer - '0');
        buffer++;
        }
      return ret;
      }
    if (numType == 'H') {
      while ((v = char_to_hex(*buffer)) != 255) {
        ret = ret * 16 + v;
        buffer++;
        }
        buffer++;
      return ret;
      }
    }
  return 0;
  }

char* level_3(char* buffer,word* ret) {
  char ebuffer[64];
  char label[256];
  char *pLabel;
  char numType;
  byte v;
  buffer = ltrim(buffer);
  *ret = 0;
  if (*buffer == '$') {
    *ret = address;
    buffer++;
    return buffer;
    }
  if (*buffer == '\'') {
    buffer++;
    if (*buffer == 0) {
      *ret = 0;
      return buffer;
      }
    if (*buffer == '\'') {
      *ret = 0;
      buffer++;
      return buffer;
      }
    *ret = *buffer;
    while (*buffer != 0 && *buffer != '\'') buffer++;
    if (*buffer == '\'') buffer++;
    return buffer;
    }
  if (*buffer == '(') {
    buffer++;
    buffer = get_num(buffer,ret);
    if (*buffer != ')') {
      } else {
      buffer++;
      return buffer;
      }
    }
  if (*buffer >= 'A' && *buffer <= 'Z') {
    pLabel = label;
    while ((*buffer >= 'A' && *buffer <= 'Z') ||
           (*buffer >= '0' && *buffer <= '9') ||
           *buffer == '_') *pLabel++ = *buffer++;
    *pLabel = 0;
    *ret = find_label(label);
    return buffer;
    }
  if (*buffer >= '0' && *buffer <= '9') {
    numType = id_number(buffer);
    if (numType == 'D') {
      *ret = 0;
      while (*buffer >= '0' && *buffer <= '9') {
        *ret = *ret * 10 + (*buffer - '0');
        buffer++;
        }
      return buffer;
      }
    if (numType == 'H') {
      while ((v = char_to_hex(*buffer)) != 255) {
        *ret = *ret * 16 + v;
        buffer++;
        }
        buffer++;
      return buffer;
      }
    }
  *ret = 0;
  if (pass == 2) {
    errs++;
    sprintf(ebuffer,"Expression Error >> %s",command);
    errorOut(ebuffer);
    }
  return buffer;
  }

char* level_2(char* buffer,word* ret) {
  word acc;
  word v2;
  buffer = ltrim(level_3(buffer,&acc));
  while (*buffer == '.') {
    buffer++;
    buffer = ltrim(level_3(buffer,&v2));
    acc = (v2 == 0) ? acc % 256 : acc / 256;
    }
  *ret = acc;
  return buffer;
  }

char* ex_mul_div(char* buffer,word* ret) {
  word acc;
  word v2;
  char op;
  buffer = ltrim(level_2(buffer,&acc));
  while (*buffer == '*' || *buffer == '/') {
    op = *buffer++;
    buffer = ltrim(level_2(buffer,&v2));
    acc = (op == '*') ? acc*v2 : acc/v2;
    }
  *ret = acc;
  return buffer;
  }

char* ex_add_sub(char* buffer,word* ret) {
  word acc;
  word v2;
  char op;
  buffer = ltrim(ex_mul_div(buffer,&acc));
  while (*buffer == '+' || *buffer == '-') {
    op = *buffer++;
    buffer = ltrim(ex_mul_div(buffer,&v2));
    acc = (op == '+') ? acc+v2 : acc-v2;
    }
  *ret = acc;
  return buffer;
  }

char* ex_shl_shr(char* buffer,word* ret) {
  word acc;
  word v2;
  char op;
  buffer = ltrim(ex_add_sub(buffer,&acc));
  while (*buffer == '<' || *buffer == '>') {
    op = *buffer++;
    if (op != *buffer) {
      buffer--;
      return buffer;
      }
    buffer++;
    buffer = ltrim(ex_add_sub(buffer,&v2));
    acc = (op == '<') ? acc << v2 : acc >> v2;
    }
  *ret = acc;
  return buffer;
  }

char* ex_and(char* buffer,word* ret) {
  word acc;
  word v2;
  char op;
  buffer = ltrim(ex_shl_shr(buffer,&acc));
  while (*buffer == '&') {
    op = *buffer++;
    buffer = ltrim(ex_shl_shr(buffer,&v2));
    acc = acc & v2;
    }
  *ret = acc;
  return buffer;
  }

char* get_num(char* buffer,word* ret) {
  word acc;
  word v2;
  char op;
  if (*buffer == '-') {
    buffer++;
    buffer = ltrim(ex_and(buffer,&acc));
    acc = (word) -((int)acc);
    }
  else if (*buffer == '+') {
    buffer++;
    buffer = ltrim(ex_and(buffer,&acc));
    }
  else buffer = ltrim(ex_and(buffer,&acc));
  while (*buffer == '|' || *buffer == '^') {
    op = *buffer++;
    buffer = ltrim(ex_and(buffer,&v2));
    acc = (op == '|') ? acc|v2 : acc^v2;
    }
  *ret = acc;
  return buffer;
  }

void errorOut(char *msg) {
#ifdef WIN32
  fprintf(stderr,"*** ERROR %d: %s\r\n",linecount,msg);
#else
  fprintf(stderr,"*** ERROR %d: %s\n",linecount,msg);
#endif
  }

void doIndent() {
  int i;
  for (i=0; i<indent; i++) printf(" ");
  }

char* nextWord(char* buffer) {
  while (*buffer > ' ') buffer++;
  while (*buffer > 0 && *buffer <= ' ') buffer++;
  return buffer;
  }

char* findSpace(char* buffer) {
  while (*buffer > ' ') buffer++;
  return buffer;
  }

int getDefine(char* buffer) {
  int i;
  for (i=0; i<numDefines; i++)
    if (strcmp(buffer,defines[i]) == 0) return i;
  return -1;
  }

void processDefs(char* buffer) {
  int i;
  char* t;
  char* e;
  char tb[255];
  for (i=0; i<numDefines; i++) {
    t = strstr(buffer,defines[i]);
    if (t != buffer) {
      if (t != NULL && *(t-1) >='A' && *(t-1) <='Z') t = NULL;
      if (t != NULL && *(t-1) >='a' && *(t-1) <= 'z') t= NULL;
      }
    if (t != NULL) {
      e = t+strlen(defines[i]);
      if (t != NULL && *e >='A' && *e <='Z') t = NULL;
      if (t != NULL && *e >='a' && *e <= 'z') t= NULL;
      }
    if (t != NULL) {
      strcpy(tb,(t+strlen(defines[i])));
      *t = 0;
      strcat(buffer,defValues[i]);
      strcat(buffer,tb);
      i = -1;
      }
    }
  }

