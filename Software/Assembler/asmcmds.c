#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"
#include "mstrings.h"

int Check_Entry(char *Entry,char *Buffer)
{
  int ret;
  char Test[20];
  char *Ptr;
  Ptr=Test;
  while (*Entry!='=') *Ptr++=*Entry++;
  *Ptr='\0'; Entry++;
  if (strncmp(Test,Buffer,strlen(Test))==0) {
    sscanf(Entry,"%d",&ret);
    }
  else ret=-1;
  return ret;
}

int Class_Match(char *class,char *buffer)
{
  int i;
  int flag;
  char ClassText[256];
  char Entry[20];
  char *CPtr;
  char *DPtr;
  flag=-1;
  for (i=0;i<DefFile.ClassCount;i++)
    if (strcmp(class,DefFile.ClassName[i])==0) flag=i;
  if (flag!=-1) {
    strcpy(ClassText,DefFile.ClassText[flag]);
    CPtr=ClassText;
    while (*CPtr!='\0') {
      DPtr=&Entry[0];
      while (*CPtr!=',' && *CPtr!='\0') *DPtr++=*CPtr++;
      *DPtr='\0';
      flag=Check_Entry(Entry,buffer);
      if (*CPtr==',') CPtr++;
      if (flag!=-1) *CPtr='\0';
      if (flag==-1 && *CPtr=='\0') flag=-2;
      }
    if (flag>=0) {
      args[ArgCount]=flag;
      argtype[ArgCount++]='B';
      DPtr=strchr(Entry,'=');
      *DPtr='\0';
      return strlen(Entry);
      } else return 0;
    }
   else return 0;
}

int WildMatch(char *src,char *dest)
{
  char flag;
  char buffer[255];
  char class[20];
  char *BPtr;
  int i;
  word wrd;
  int typ;
  char matched;
  char inQuote;
  unsigned char bs,bd;
  byte bitlength;
  word max;
  flag='y';
  matched='N';
  while (flag=='y') {
    argflag[ArgCount]=-1;
    bs=*src++;
    if (bs=='\0') {
      bd=*dest;
      if ((bs |= bd)) matched='N'; else matched='Y';
      flag='n';
      }
      else if (bs<=' ') {
        while (*src!='\0' && *src<=' ') src++;
        bd=*dest;
        if (bd!='\0' && bd<=' ') {
          while (*dest!='\0' && *dest<=' ') dest++;
          } else { matched='N'; flag='n'; }
        }
      else if (bs=='\\') {
        bs=*src++;
        if (bs=='D') {
          BPtr=buffer;
          if ((*dest>='A' && *dest<='Z') || (*dest>='a' && *dest<='z')) {
            while ((*dest>='A' && *dest<='Z') ||
                   (*dest>='a' && *dest<='z') || *dest=='_' ||
                   (*dest>='0' && *dest<='9'))
              *BPtr++ = *dest++;
            *BPtr='\0';
            wrd=get_num(buffer);
            args[ArgCount]=wrd;
            argtype[ArgCount++]='D';
            }
          if ((*dest=='-' || *dest=='+')) {
            while ((*dest>='A' && *dest<='F') ||
                   (*dest>='a' && *dest<='f') ||
                   (*dest>='0' && *dest<='9') || *dest=='+' || *dest=='-')
              *BPtr++ = *dest++;
            *BPtr='\0';
            wrd=get_num(buffer);
            args[ArgCount]=wrd;
            argtype[ArgCount++]='B';
            }
          }
        else if (bs=='N' || bs=='B' || bs=='W' || bs=='(') {
          if (bs=='N') bitlength=4;
          if (bs=='B') bitlength=8;
          if (bs=='W') bitlength=16;
          if (bs=='(') {
            bitlength=char_to_hex(*src++); src++;
            bs='W';
            if (bitlength<9) bs='B';
            if (bitlength<5) bs='N';
            }
          max=1<<bitlength;
          typ=bs;
          BPtr=buffer;
          inQuote = 0;
          while (*dest != 0 && (inQuote != 0 || 
                 *dest != ',' && *dest != ' ')) {
            if (*dest == inQuote) inQuote = 0;
              else if (*dest == '\'' || *dest == '"') inQuote = *dest;
            *BPtr++ = *dest++;
            }
          *BPtr='\0';
/*
          if (*dest == '\'') {
            dest++;
            sprintf(buffer,"%d",*dest++);
            dest++;
            BPtr++;
            }
          else if (*dest == '$') {
            sprintf(buffer,"%d",address);
            BPtr++;
            dest++;
            }
          else if ((*dest>='A' && *dest<='Z') || (*dest>='a' && *dest<='z')) {
            while ((*dest>='A' && *dest<='Z') ||
                   (*dest>='a' && *dest<='z') || *dest=='_' ||
                   (*dest>='0' && *dest<='9'))
              *BPtr++ = *dest++;
            *BPtr='\0';
            }
          else if (*dest>='0' && *dest<='9') {
            while ((*dest>='0' && *dest<='9') ||
                   (*dest>='a' && *dest<='f') ||
                   (*dest>='A' && *dest<='F') || *dest=='H' || *dest=='h')
              *BPtr++ = *dest++;
            *BPtr='\0';
            }
*/
          if (BPtr != buffer) {
            args[ArgCount]=get_num(buffer);
            argtype[ArgCount++]=typ;
            if (abs(args[ArgCount-1])>=max) {
              matched='N'; flag='n'; ArgCount--;
              }
            } else { matched='N'; flag='n'; }
          }
        else if (bs=='L') {
          BPtr=listBuffer;
          while(*dest != '\0') *BPtr++ = *dest++;
          *BPtr='\0';
          if (BPtr != buffer) {
            args[ArgCount]=1;
            argtype[ArgCount++]='L';
            } else { matched='N'; flag='n'; }
          }
        else if (bs=='M') {
          BPtr=listBuffer;
          while(*dest != '\0') *BPtr++ = *dest++;
          *BPtr='\0';
          if (BPtr != buffer) {
            args[ArgCount]=1;
            argtype[ArgCount++]='M';
            } else { matched='N'; flag='n'; }
          }
        else if (bs=='{') {
          BPtr=buffer;
          while (*src!='\0' && *src != '}') *BPtr++ = *src++;
          src++;
          *BPtr='\0';
          i=Class_Match(buffer,dest);
          if (i!=0) {
            dest+=i;
            }
          else { matched='N'; flag='n'; }
          }
        else { matched='N'; flag='n'; }
        }
      else {
        bd=*dest++;
        if (bs != bd) { matched='N'; flag='n'; }
        }


/*
    if (*src == '\\') {
      BPtr=buffer;
      while ((*dest>='A' && *dest<='Z') ||
             (*dest>='a' && *dest<='z') ||
             (*dest>='0' && *dest<='9') ||
              *dest=='_') *BPtr++=*dest++;
      *BPtr='\0'; dest--;
      src++;
      if (*src=='{') {
        src++;
        BPtr=class;
        while ((*src>='A' && *src<='Z') ||
               (*src>='a' && *src<='z') ||
               (*src>='0' && *src<='9') ||
                *src=='_') *BPtr++=*src++;
        *BPtr='\0';
        if (Class_Match(class,buffer)==0) flag='n';
        } else {
        args[ArgCount]=get_num(buffer);
        argtype[ArgCount++]=*src;
        }
      }
    else if (*src != *dest) flag='n';
    if (*src == '\0') flag='n';
    if (flag=='y') {src++; dest++; }
*/
    }
  if (matched=='Y') return 1; else return 0;
}

int Match(int Entry)
{
  int i,m1,m2;
  ArgCount=0;
  m1=WildMatch(DefFile.Command[Entry],command);
  if (m1 != 0) return 1;
  return 0;
}

void Translate(int Entry)
{
  int  labelNum;
  char *Ptr;
  char *Lptr;
  char *BPtr;
  char quotes;
  byte B,i,j;
  word w;
  char flag;
  char noout;
  char buffer[20];
  char temp[1024];
  char *pTemp;
  Ptr=&DefFile.Dest[Entry][0];
  B=0;
  noout=' ';
  while (*Ptr!='\0') {
    if (*Ptr>='0' && *Ptr<='9') B=B*16+(*Ptr-48);
    if (*Ptr>='A' && *Ptr<='F') B=B*16+(*Ptr-55);
    if (*Ptr=='\\' && *(Ptr+1)=='O') {
      Ptr+=2;
      i=*Ptr-49;
      Ptr++;
      address=args[i];
      write_line();
      valid=1;
      column=0;
      outputCount=addressSize;
      noout='Y';
      }
    if (*Ptr=='\\' && *(Ptr+1)=='B') {
      Ptr+=2;
      i=*Ptr-49;
      Ptr++;
      address+=args[i];
      valid=1;
      column=-1;
      noout='Y';
      }
    if (*Ptr=='\\' && *(Ptr+1)=='S') {
      Ptr+=2;
      i=*Ptr-49;
      Ptr++;
      startAddr=args[i];
      valid=1;
      noout='Y';
      }
    if (*Ptr=='\\' && *(Ptr+1)=='E') {
      Ptr+=2;
      i=*Ptr-49;
      Ptr++;
      labelNum = find_label_number(labl);
      if (labelNum > 0) labela[labelNum]=args[i];
      valid=1;
      noout='Y';
      }
    if (*Ptr=='\\' && *(Ptr+1)=='P') {
      Ptr+=2;
      if (pass==1) labelf[nlabel]='P';
      valid=1;
      noout='Y';
      }
    if (*Ptr=='\\' && *(Ptr+1)=='R') {
      Ptr+=2;
      if (pass==2) {
        fprintf(outfile,"\n#%s\n",lastlabel);
        column=0;
        }
      valid=1;
      noout='Y';
      }
    if (*Ptr=='\\' && *(Ptr+1)=='Q') {
      Ptr+=2;
      if (pass==2) {
        fprintf(outfile,"\n#\n");
        column=0;
        }
      valid=1;
      noout='Y';
      }
    if (*Ptr=='\\' && *(Ptr+1)=='X') {
      Ptr+=2;
      if (pass==1) {
        labelf[nlabel]='X';
        labela[nlabel]=0;
        }
      valid=1;
      noout='Y';
      }
    if (*Ptr=='|') {
      Ptr++;
      i=*Ptr-49; j=0;
      Ptr++;
      if (*Ptr=='<') {
        Ptr++; j=char_to_hex(*Ptr);
        B=B | (args[i] & 255)<<j;
        }
      else if (*Ptr=='>') {
        Ptr++; j=char_to_hex(*Ptr);
        B=B | (args[i] & 255)>>j;
        } else {
          Ptr--;
          B=B | args[i];
          }
      noout = ' ';
      }
    if (*Ptr=='&') {
      Ptr++;
      j=char_to_hex(*Ptr++)*16;
      j+=char_to_hex(*Ptr);
      B=B & j;
      noout = ' ';
      }
    if (*Ptr=='%') {
      B=(B >> 4) | ((B & 0xf) << 4);
      noout = ' ';
      }
    if (strncmp(Ptr,"lo(",3)==0) {
      Ptr+=3;
      i=*Ptr-49;
      if (argflag[i]!=-1) {
        fprintf(outfile,"\n?%s\n",labels[argflag[i]]);
        column=0;
        argflag[i]=-1;
        }
      B=(args[i] & 255);
      Ptr++;
      noout=' ';
      }
    if (strncmp(Ptr,"hi(",3)==0) {
      Ptr+=3;
      i=*Ptr-49;
      if (argflag[i]!=-1) {
        fprintf(outfile,"\n?%s\n",labels[argflag[i]]);
        column=0;
        argflag[i]=-1;
        }
      B=((args[i]>>8) & 255);
      Ptr++;
      noout=' ';
      }
    if (*Ptr=='\\') {
      Ptr++;
      i=*Ptr-49;
      if (argtype[i]=='N') {
        B=B*16+(args[i] & 15);
        noout = ' ';
        }
      if (argtype[i]=='B') {
        B=(args[i] & 255);
        noout = ' ';
        }
      if (argtype[i]=='D') {
        B=(args[i]-(address+1)) & 255;
        noout = ' ';
        }
      if (argtype[i]=='L' || argtype[i] == 'M') {
        Lptr=listBuffer;
        quotes=0;
        BPtr=buffer;
        pTemp = temp;
        flag='*';
        while (flag == '*') {
          if ((quotes == 0 && *Lptr == ',') || *Lptr == 0) {
            *pTemp = 0;
            if (pTemp != temp) {
              if (temp[0] == '"' || temp[0] == '\'') {
                quotes = temp[0];
                pTemp = temp+1;
                while (*pTemp != 0 && *pTemp != quotes) {
                  w = *pTemp;
                  if (argtype[i] == 'M') write_byte(w>>8);
                    write_byte(w & 0xff);
                  pTemp++;
                  }
                } else {
                w = get_num(temp);
                if (argtype[i] == 'M') write_byte(w>>8);
                write_byte(w & 0xff);
                }
              pTemp = temp;
              quotes = 0;
              }
            if (*Lptr == ',') Lptr++;
              else flag = ' ';
            } else {
              *pTemp = *Lptr++;
              if (*pTemp == quotes) quotes = 0;
              else if (quotes == 0 && (*pTemp == '"' || *pTemp == '\'')) quotes = *pTemp;
              pTemp++;
              }
          }
/*
          if ((BPtr == buffer || quotes != ' ') && (*Lptr == '"' || *Lptr == '\'')) {
            if (quotes==' ') quotes='*'; else quotes=' ';
            } else if (*Lptr == ',') {
            *BPtr='\0';
            if (BPtr != buffer) {
              w = get_num(buffer);
              if (argtype[i] == 'L') {
                B=(w & 255);
                write_byte(B);
                } else {
                write_byte(w >> 8);
                write_byte(w & 255);
                }
              BPtr=buffer;
              }
            } else {
            if (quotes=='*') {
              if (argtype[i] == 'M') write_byte(0);
              write_byte(*Lptr);
              } else *BPtr++ = *Lptr;
            }
          Lptr++;
          }
        *BPtr='\0';
        if (BPtr != buffer) {
           w = get_num(buffer);
           B=(w & 255);
           write_byte(B);
           }
*/
        noout='Y';
        }  /* *** End of 'L' or 'M' *** */

      }
    Ptr++;
    if (*Ptr==' ') { write_byte(B); B=0; Ptr++; noout='Y'; }
    }
  if (noout!='Y') write_byte(B);
}

void asm_cmd()
{
  char tbuffer[255];
  word i;
  char *src;
  char *dst;
  valid=0;
  for (i=0;i<DefFile.TransCount;i++)
    if (Match(i) != 0) {
      if (showrule=='*') printf("->(%d) %s\n",i,DefFile.Command[i]);
      Translate(i);
      i=9999;
      valid=1;
      }
  if (strcmp(command,"ENDM")==0) 
    {
      fclose(infile[filecount--]);
      inmacro='N';
      valid=1;
    }
  if (strcmp(command,"")==0)  valid=1;
  if (valid==0 && macron>0) {
    i = 0;
    while (command[i] > ' ') {
      tbuffer[i] = command[i];
      i++;
      }
    tbuffer[i] = 0;

    i=1;
    while (i<=macron && strcmp(tbuffer,macros[i])!=0) {
      i++;
      }
    if (i<=macron) {
      strcpy(mbuffer,macros[i]); strcat(mbuffer,".mac");
      filecount++;
      if ((infile[filecount]=fopen(mbuffer,"r"))==NULL) {
        filecount--;
        }
      else {
        fgets(mbuffer,255,infile[filecount]);
        macrovars=0;
        src=mbuffer;
        while (*src!='\0' && *src!=' ') src++;
        while (*src!='\0' && *src==' ') src++;
        while (*src!='\0' && *src!=' ') src++;
        while (*src!='\0' && *src==' ') src++;
        while (*src!='\0') {
          while ((*src==',' || *src<=' ') && *src!='\0') src++;
          if (*src!='\0') {
            macrovars++;
            dst=macrov[macrovars-1];
            while (*src!=',' && *src>' ' && *src!='\0') *dst++=*src++;
            *dst='\0';
            }
          }
        strcpy(arg1,command);
        for (i=0;i<macrovars;i++) strcpy(macror[i],"");
        i=0;
        src=strchr(arg1,':');
        if (src == 0) src = arg1; else ++src;
        while (*src!='\0' && *src==' ') src++;
        while (*src!='\0' && *src!=' ') src++;
        while (*src!='\0' && *src==' ') src++;
        while (*src!='\0') {
          while ((*src==',' || *src<=' ') && *src!='\0') src++;
          if (*src!='\0') {
            i++;
            dst=macror[i-1];
            while (*src!=',' && *src>' ' && *src!='\0') *dst++=*src++;
            *dst='\0';
            }
          }
        inmacro='Y';
        valid=1;
        }
      }
    }
  if ((valid==0) && (pass==2)) 
    { printf("%d: ***ERROR*** Invalid opcode : %s\n",linecount,readline); errs++; }
}


