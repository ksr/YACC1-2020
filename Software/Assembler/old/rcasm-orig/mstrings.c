#include <string.h>

char _strbuffer[255];

void replace(char* buffer,char* search,char* rpl)
{
  char *origin;
  char *tmpb;
  char *tmps;
  char *dst;
  origin=buffer;
  dst=_strbuffer;
  while (*buffer!='\0') {
    tmpb=buffer;
    tmps=search;
    while (*tmpb==*tmps && *tmps!='\0' && *tmpb!='\0') {
      tmpb++; tmps++;
      }
    if (*tmpb!='\0' && *tmps=='\0') {
      tmps=rpl;
      while (*tmps!='\0') *dst++=*tmps++;
      buffer=tmpb; buffer--;
      }
    else if (*tmpb=='\0' && *tmps=='\0') {
      tmps=rpl;
      while (*tmps!='\0') *dst++=*tmps++;
      buffer=tmpb; buffer--;
      }
    else *dst++=*buffer;
    buffer++;
    }
  *dst='\0';
  strcpy(origin,_strbuffer);
}

unsigned char ord(char x)
{
  return x;
}

int length(char *instr)
{
  return strlen(instr);
}

int pos(char key,char* strng)
{
  int ret;
  int i;
  ret=0;
  i=1;
  while (*strng!='\0') {
    if (*strng==key && ret==0) ret=i;
    i++;
    strng++;
    }
  return ret;
}

char* copy(char* source,int pos,int length)
{
  char* cptr;
  while (pos!=1 && *source!='\0') { source++; pos--; }
  cptr=_strbuffer;
  while (length!=0 && *source!='\0') {
    *cptr++=*source++;
    length--;
    }
  *cptr='\0';
  return _strbuffer;
}

int val(char* buffer)
{
  int total;
  total=0;
  while (*buffer>='0' && *buffer<='9') {
    total*=10;
    switch (*buffer++) {
      case '0':total+=0; break;    case '1':total+=1; break;
      case '2':total+=2; break;    case '3':total+=3; break;
      case '4':total+=4; break;    case '5':total+=5; break;
      case '6':total+=6; break;    case '7':total+=7; break;
      case '8':total+=8; break;    case '9':total+=9; break;
      }
    }
  return total;
}

char* left(char* buffer,int count)
{
  char* dest;
  dest=_strbuffer;
  while (*buffer!='\0' && count>0)
    *dest++=*buffer++;
  *dest='\0';
  return _strbuffer;
}

char* remspace(char* buffer)
{
  char *dest;
  dest=_strbuffer;
  while (*buffer==' ') buffer++;
  strcpy(dest,buffer);
  return _strbuffer;
}

char* trailspace(char* buffer)
{
  char *dest;
  dest=_strbuffer;
  strcpy(_strbuffer,buffer);
  while (strlen(_strbuffer)>0 && _strbuffer[strlen(_strbuffer)-1]==' ')
    _strbuffer[strlen(_strbuffer)-1]='\0';
  return _strbuffer;
}

char upcase(char in)
{
  if (in>='a' && in<='z') return in-32; else return in;
}

char* upper(char* buffer)
{
  char *dest;
  dest=_strbuffer;
  strcpy(_strbuffer,buffer);
  while (*dest|='\0') {
    if (*dest>='a' && *dest<='z') *dest=(*dest)-32;
    dest++;
    }
  return _strbuffer;
}
