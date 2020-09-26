#define SUPPORT

#include <stdio.h>
#include <string.h>
#include "header.h"
#include "mstrings.h"

int atoi();

void write_line() {
  int i;
  byte cksum;
  if (pass != 2) return;
  if (outCount > 0) {
    if (emucode == 'I') {
      fprintf(outfile,":%04x",outAddress);
      for (i=0; i<outCount; i++)
        fprintf(outfile," %02x",outRow[i]);
      fprintf(outfile,"\n");
      }
    if (emucode == 'H') {
      fprintf(outfile,":%02x%04x00",outCount,outAddress);
      cksum = outCount + (outAddress & 255) + (outAddress >> 8);
      for (i=0; i<outCount; i++) {
        fprintf(outfile,"%02x",outRow[i]);
        cksum += outRow[i];
        }
      cksum = 0-cksum;
      fprintf(outfile,"%02x\n",cksum);
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
        printf("\n            ");
        bcount=0;
        }
      printf("%02x ",i);
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

word find_label_number(char* buf) {
  int i;
  int f;
  char mode;
  if (pass == 1) return 0;
  for (i=1;i<=nlabel;i++) {
    if (strcmp(labels[i],buf)==0)  f=i;
    }
  if (f==0) {
    printf("%d:Label not found - %s\n",linecount,buf);
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
  char mode;
  if (pass == 1) return 1;
  mode='N';
  strcpy(buffer,buf);
  i=pos('.',buffer);
  if (i!=0) {
    if (buffer[i]=='1') mode='H';
    if (buffer[i]=='0') mode='L';
    buffer[i-1]='\0';
    }
  f=0;
  for (i=1;i<=nlabel;i++) {
    if (strcmp(labels[i],buffer)==0)  f=i;
    }
  if (f==0) {
    printf("%d:Label not found - %s\n",linecount,buf);
    errs++;
    }
  if (f!=0 && doxref=='Y') fprintf(xref,"%s %d\n",buffer,linecount);
  if (f!=0) {
    if (labelf[f]=='X') argflag[ArgCount]=f;
    switch(mode) {
      case 'L':return labela[f] % 256; break;
      case 'H':return labela[f] / 256; break;
      case 'N':return labela[f]; break;
      }
    }
   else return 0;
    return 0;
}

byte char_to_hex(char buffer) {
  byte t;
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
  int i;
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

void buildTokens(char* buffer) {
  int  i;
  int  inQuote;
  char *bPtr;
  int  len;
  len = 0;
  tokenCount = 1;
  bPtr = tokens[0].data.chr;
  inQuote = 0;
  while (*buffer != 0) {
    if (inQuote != 0) {
      if (*buffer == inQuote) {
        *bPtr++ = inQuote;
        *bPtr = 0;
        len=0;
        inQuote = 0;
        *bPtr = 0;
        bPtr = tokens[tokenCount++].data.chr;
        buffer++;
        }
        else {
          *bPtr++ = *buffer++;
          *bPtr = 0;
          len++;
        }
      }
    else if (*buffer == '"' || *buffer == '\'') {
      inQuote = *buffer++;
      *bPtr++ = inQuote;
      *bPtr = 0;
      len++;

      }
    else if (*buffer == '+' || *buffer == '-' || *buffer == '*' ||
        *buffer == '/' || *buffer == '$' || *buffer == '(' ||
        *buffer == ')' || *buffer == '.') {
      if (len == 0) {
        *bPtr++ = *buffer++;
        *bPtr = 0;
        bPtr = tokens[tokenCount++].data.chr;
        } else {
        bPtr = tokens[tokenCount++].data.chr;
        *bPtr++ = *buffer++;
        *bPtr = 0;
        bPtr = tokens[tokenCount++].data.chr;
        }
      len = 0;
      } else if (*buffer > ' ') {
      *bPtr++ = *buffer++;
      *bPtr = 0;
      len++;
      }
    }
  if (tokenCount > 0 && len == 0) --tokenCount;
  for (i=0; i<tokenCount; i++) tokens[i].typ = 'C';
  }

void show_tokens() {
  int v;
  for (v=0; v<tokenCount; v++) {
    if (tokens[v].typ != 'N')
      printf("C %d: %s\n",v,tokens[v].data.chr);
      else printf("N %d: %d\n",v,tokens[v].data.number);
      }
}

int process_tokens(int start,int end) {
  int j;
/* ***** Process . ***** */
  i = start;
  while (i <= end) {
    if (tokens[i].typ != 'N' && strcmp(tokens[i].data.chr,".") == 0) {
      if (i == 0 || i == tokenCount-1) {
        return -1;
        }
      if (tokens[i-1].typ != 'N' || tokens[i+1].typ != 'N') {
        return -1;
        }
      if (tokens[i+1].data.number == 0)
        tokens[i-1].data.number &= 0xff;
      else if (tokens[i+1].data.number == 1)
        tokens[i-1].data.number /= 256;
      else {
        return -1;
        }
      for (j=i; j<tokenCount; j++) tokens[j] = tokens[j+2];
      tokenCount -=2;
      } else i++;
    }

/* ***** Process * and / ***** */
  i = start;
  while (i <= end) {
    if (tokens[i].typ != 'N' && 
        (strcmp(tokens[i].data.chr,"*") == 0 ||
         strcmp(tokens[i].data.chr,"/") == 0)) {
      if (i == 0 || i == tokenCount-1) {
        return -1;
        }
      if (tokens[i-1].typ != 'N' || tokens[i+1].typ != 'N') {
        return -1;
        }
      if (strcmp(tokens[i].data.chr,"*") == 0)
        tokens[i-1].data.number *= tokens[i+1].data.number;
        else tokens[i-1].data.number /= tokens[i+1].data.number;
      for (j=i; j<tokenCount; j++) tokens[j] = tokens[j+2];
      tokenCount -=2;
      } else i++;
    }

/* ***** Process leading + or - ***** */
  if (tokens[start].typ != 'N' && strcmp(tokens[start].data.chr,"+") == 0) {
    if (tokens[start+1].typ != 'N') {
      return -1;
      }
    for (j=start; j<=end; j++) tokens[j] = tokens[j+1];
    tokenCount--;
    }
  if (tokens[start].typ != 'N' && strcmp(tokens[start].data.chr,"-") == 0) {
    if (tokens[start+1].typ != 'N') {
      return -1;
      }
    for (j=start; j<=end; j++) tokens[j] = tokens[j+1];
    tokenCount--;
    tokens[j].data.number = -tokens[j].data.number;
    }

/* ***** Process + and - ***** */
  i = start;
  while (i <= end) {
    if (tokens[i].typ != 'N' && 
        (strcmp(tokens[i].data.chr,"+") == 0 ||
         strcmp(tokens[i].data.chr,"-") == 0)) {
      if (i == 0 || i == tokenCount-1) {
        return -1;
        }
      if (tokens[i-1].typ != 'N' || tokens[i+1].typ != 'N') {
        return -1;
        }
      if (strcmp(tokens[i].data.chr,"+") == 0)
        tokens[i-1].data.number += tokens[i+1].data.number;
        else tokens[i-1].data.number -= tokens[i+1].data.number;
      for (j=i; j<tokenCount; j++) tokens[j] = tokens[j+2];
      tokenCount -=2;
      } else i++;
    }
  return 0;
  }

word get_num(char* buf)
{
  int  i,j,k;
  int  flag;
  word v;
  word e;
  char sign;
  buildTokens(buf);
/*
printf("token count = %d\n",tokenCount);
for (v=0; v<tokenCount; v++) printf("%d: %s\n",v,tokens[v].data.chr);
*/

  for (v=0; v<tokenCount; v++) {
    if (tokens[v].data.chr[0] == '\'') {
      tokens[v].data.number = tokens[v].data.chr[1];
      tokens[v].typ = 'N';
      }
    else if (tokens[v].data.chr[0] == '\"') {
      tokens[v].data.number = tokens[v].data.chr[1];
      tokens[v].typ = 'N';
      }
    else if ((tokens[v].data.chr[0] >='a' && tokens[v].data.chr[0] <= 'z') ||
        (tokens[v].data.chr[0] >='A' && tokens[v].data.chr[0] <= 'Z')) {
      tokens[v].data.number = find_label(tokens[v].data.chr);
      tokens[v].typ = 'N';
      }
    else if (tokens[v].data.chr[0] >='0' && tokens[v].data.chr[9] <= 'z') {
      if (pos('H',tokens[v].data.chr) != 0 ||
          pos('h',tokens[v].data.chr) != 0)
        tokens[v].data.number = conv_hex(tokens[v].data.chr);
        else
          tokens[v].data.number = atoi(tokens[v].data.chr);
      tokens[v].typ = 'N';
      }
    else if (tokens[v].data.chr[0] == '$') {
      tokens[v].data.number = address;
      tokens[v].typ = 'N';
      }
    }
  flag = 1;
  while (flag == 1) {
    flag = 0;
    i=0;
    while (i<tokenCount &&
           (tokens[i].typ != 'C' || tokens[i].data.chr[0] != ')')) i++;
    if (i <tokenCount) {
      j = i;
      while (j>=0 &&
           (tokens[j].typ != 'C' || tokens[j].data.chr[0] != '(')) j--;
      if (j >= 0) {
        for (k=i; k<tokenCount; k++) tokens[k] = tokens[k+1];
        tokenCount--;
        for (k=j; k<tokenCount; k++) tokens[k] = tokens[k+1];
        tokenCount--;
        i-=2;
        if (process_tokens(j,i) < 0) {
          printf("%d: ***ERROR*** Syntax Error - %s\n",linecount,buf);
          errs++;
          return 0;
          }
        flag = 1;
        }
      }
    }

  if (process_tokens(0,tokenCount-1) < 0) {
    printf("%d: ***ERROR*** Syntax Error - %s\n",linecount,buf);
    errs++;
    return 0;
    }
  if (tokenCount != 1) {
    printf("%d: ***ERROR*** Syntax Error - %s\n",linecount,buf);
    errs++;
    return 0;
    }

  return tokens[0].data.number;  
}

