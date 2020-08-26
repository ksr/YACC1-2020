#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mstrings.h"
#include "header.h"

void asm_cmd();
int unlink(const char *);

void makeupper(char *buffer) {
  char mode;
  mode=' ';
  while (buffer[0] != '\0' && buffer[strlen(buffer)-1]<' ')
     buffer[strlen(buffer)-1]='\0';
  while (*buffer != '\0') {
    if (mode==' ') {
      if (*buffer=='\'') mode='*';
      if (*buffer>='a' && *buffer<='z') *buffer-=32;
      } else {
      if (*buffer=='\'') mode=' ';
      }
    buffer++;
    }
  }

void trim(char *buffer) {
  char *BPtr;
  int i;
    char tmp[100]; // ken
/*
  for (i=0;i<strlen(buffer);i++)
    if (buffer[i]==';') buffer[i]='\0';
*/
  BPtr=buffer;
  while (*BPtr != '\0' && *BPtr <= ' ') BPtr++;
  //ken add if
    if(buffer != BPtr){
        strcpy(tmp,BPtr); //ken
        strcpy(buffer,tmp); //ken
        //ken strcpy(buffer,BPtr);
    }
  while (buffer[0]!='\0' && buffer[strlen(buffer)-1]<=' ')
    buffer[strlen(buffer)-1]='\0';
  }

void parse(char *buffer)
{
  int i;
  char inQuote;
  char *BPtr;
    char tmp[100]; //ken
    
  inQuote = 0;
  for (i=0;i<strlen(buffer);i++) {
    if (inQuote != 0 && buffer[i] == inQuote) inQuote = 0;
      else if (buffer[i] == '\'' || buffer[i] == '"') inQuote = buffer[i];
    if (inQuote == 0 && buffer[i]==';') buffer[i]='\0';
    }
  inQuote = 0;
  BPtr = NULL;
  for (i=0; i<strlen(buffer); i++) {
    if (inQuote != 0 && buffer[i] == inQuote) inQuote = 0;
      else if (buffer[i] == '\'' || buffer[i] == '"') inQuote = buffer[i];
    if (inQuote == 0 && buffer[i]==':') {
      BPtr = buffer+i;
      i = strlen(buffer) + 1;
      }
    }
/*
  BPtr=strchr(buffer,':');
*/
  if (BPtr != NULL) {
    *BPtr++ = '\0';
    strcpy(labl,buffer);
      strcpy(tmp,BPtr); //ken
      strcpy(buffer,tmp); //ken
    //ken strcpy(buffer,BPtr);
    } else labl[0]='\0';
  trim(buffer);
  strcpy(command,buffer);
}

void make_label()
{
  int i;
  char flag;
  flag=' ';
  for (i=1;i<=nlabel;i++) if (strcmp(labl,labels[i])==0) {
    printf("%d:ERR - Duplicate label : %s (first=%d)\n",linecount,labl,
      labell[i]);
    errs++;
    flag='*';
    }
  if (flag==' ') {
    nlabel++;
    strcpy(labels[nlabel],labl);
    labell[nlabel]=linecount;
    labela[nlabel]=address;
    labelf[nlabel]=' ';
    }
}

void sort_labels()
{
  word t;
  char l[20];
  char flag;
  int i;
  flag='*';
  while (flag=='*') {
    flag=' ';
    for (i=1;i<nlabel;i++) {
      if (strcmp(labels[i],labels[i+1])>0) {
        flag='*';
        t=labell[i]; labell[i]=labell[i+1]; labell[i+1]=t;
        t=labela[i]; labela[i]=labela[i+1]; labela[i+1]=t;
        strcpy(l,labels[i]); strcpy(labels[i],labels[i+1]);
        strcpy(labels[i+1],l);
        }
      }
    }
}

void sort_labelsa()
{
  word t;
  char l[20];
  char flag;
  int i;
  flag='*';
  while (flag=='*') {
    flag=' ';
    for (i=1;i<nlabel;i++) {
      if (labela[i]>labela[i+1]) {
        flag='*';
        t=labell[i]; labell[i]=labell[i+1]; labell[i+1]=t;
        t=labela[i]; labela[i]=labela[i+1]; labela[i+1]=t;
        strcpy(l,labels[i]); strcpy(labels[i],labels[i+1]);
        strcpy(labels[i+1],l);
        }
      }
    }
}

void show_xref(char* label)
{
  int count;
  char lab[20];
  int lin;
  count=4;
  rewind(xref);
  do {
    fscanf(xref,"%s %d",lab,&lin);
    if (strcmp(label,lab)==0) {
      if (count==10) { printf("\n%7s"," "); count=0; }
      printf("%6d ",lin); count++; 
      }
    } while (!feof(xref));
  printf("\n");
}

void macro_replace()
{
  int i;
  for (i=0;i<macrovars;i++) {
    replace(command,macrov[i],macror[i]);
    replace(arg1,macrov[i],macror[i]);
    replace(arg2,macrov[i],macror[i]);
    }
}

void Read_Def_File(char *FileName)
{
  FILE *InFile;
  char Buffer[255];
  DefFile.ClassCount=0;
  DefFile.TransCount=0;
  addressSize=1;
  if ((InFile=fopen(FileName,"r"))==NULL) {
    strcpy(Buffer,DEF_DIR);
    strcat(Buffer,FileName);
      printf("KEN %s\n",Buffer);
    InFile=fopen(Buffer,"r");
    }
  fgets(Buffer,80,InFile);
  Buffer[strlen(Buffer)-1]='\0';
  printf("%s\n",Buffer);
  strcpy(DefFile.Title,Buffer);
  fgets(Buffer,80,InFile);
  Buffer[strlen(Buffer)-1]='\0';
  while (Buffer[0] != '*') {
    if (strcmp(Buffer,"OPTION 16BIT")==0) {
      printf("%s\n",Buffer);
      addressSize=2;
      }
    if (strncmp(Buffer,"CLASS",5)==0) {
      strcpy(DefFile.ClassName[DefFile.ClassCount],&Buffer[6]);
      fgets(Buffer,200,InFile);
      Buffer[strlen(Buffer)-1]='\0';
      strcpy(DefFile.ClassText[DefFile.ClassCount++],Buffer);
      }
    fgets(Buffer,80,InFile);
    Buffer[strlen(Buffer)-1]='\0';
    }
  printf("%d Classes read.\n",DefFile.ClassCount);
  fgets(Buffer,80,InFile);
  while (Buffer[0] != '*') {
    trim(Buffer);
    strcpy(DefFile.Command[DefFile.TransCount],Buffer);
    fgets(Buffer,80,InFile);
    strcpy(DefFile.Dest[DefFile.TransCount++],Buffer);
    fgets(Buffer,80,InFile);
    }
  printf("%d Patterns read.\n",DefFile.TransCount);
  fclose(InFile);
}

void readOptions(char *buffer) {
    //printf("ken1 %s\n",buffer);
  if (buffer[0]!='-') strcpy(workfile,buffer);
  if (strcmp(buffer,"-h")==0) emucode='H';
  if (strcmp(buffer,"-l")==0) showlist='*';
  if (strcmp(buffer,"-s")==0) showsymbols='Y';
  if (strcmp(buffer,"-t")==0) trailer='N';
  if (strcmp(buffer,"-Sl")==0) sorttype='L';
  if (strcmp(buffer,"-Sa")==0) sorttype='A';
  if (strcmp(buffer,"-r")==0) showrule='*';
  if (strcmp(buffer,"-v")==0) {
    printf("RC/asm v%2.1f\n",VERSION);
    printf("by Michael H. Riley\n\n");
    }
  if (strcmp(buffer,"-x")==0) doxref='Y';
  if (strncmp(buffer,"-d",2)==0) {
    strcpy(DefName,&buffer[3]);
    strcat(DefName,".def");
    //printf("ken %s\n",DefName);
    i++;
    }
  }

int main(int argc,char** argv)
{
  FILE *inifile;
  char mbuffer[30];
  char *pBuffer;
  for (i=0;i<=32767;i++) memory[i]=0;
  showlist=' ';
  emucode='I';
  showsymbols='N';
  sorttype=' ';
  trailer='Y';
  doxref='N';
  showrule=' ';
  strcpy(workfile,"");
  strcpy(DefName,"1802.def");
  inifile=fopen("rcasm.rc","r");
  if (inifile != NULL) {
    while (fgets(buffer,250,inifile)!=0) {
      while (buffer[0]!= '\0' && buffer[strlen(buffer)-1]<33)
        buffer[strlen(buffer)-1]='\0';
      readOptions(buffer);
      }
    fclose(inifile);
    }
  if (argc>0) 
    for (i=1;i<argc;i++)
      {
        readOptions(argv[i]);
     }
  Read_Def_File(DefName);
  strcpy(inname,workfile);
  strcat(inname,".asm");
  if ((infile[1]=fopen(inname,"r"))!=0)
    {
      strcpy(outname,workfile);
      if (emucode=='I') strcat(outname,".prg");
      else if (emucode==' ') strcat(outname,".obj");
       else strcat(outname,".img");
      if (emucode!='I') outfile=fopen(outname,"wb");
        else outfile=fopen(outname,"w");
      nlabel=0;
      macron=0;
      linecount=0;
      outputCount=addressSize;
      pass=1;
      outCount = 0;
      outAddress = 0;
      address=0;
      highest=0;
      filecount=1;
      inmacro='N';
      errs=0;
      while (filecount>0) {
/*        while (!feof(infile[filecount]))
          {
            fgets(buffer,255,infile[filecount]); */
        while (fgets(buffer,255,infile[filecount])!=NULL) {
            linecount++;
            strcpy(readline,buffer);
            makeupper(buffer);
            parse(buffer);
            if (strncmp(command,"INCLUDE",7)==0) {
              filecount++;
              strcpy(arg1,readline);
              pBuffer = arg1;
              while (*pBuffer == ' ' && *pBuffer != 0) pBuffer++;
              while (*pBuffer != ' ' && *pBuffer != 0) pBuffer++;
              while (*pBuffer == ' ' && *pBuffer != 0) pBuffer++;
              for (i=0; i<strlen(pBuffer); i++)
                if (pBuffer[i] < 32) pBuffer[i] = 0;

              if ((infile[filecount]=fopen(pBuffer,"r"))==NULL) {
                printf("%d:Include file not found - %s\n",linecount,pBuffer);
                filecount--;
                }
              }
            else if (strncmp(command,"MACRO",5)==0) {
              strcpy(labl,upper(labl));
              strcpy(macros[++macron],labl);
              strcpy(mbuffer,labl); strcat(mbuffer,".mac");
              macrofile=fopen(mbuffer,"w");
              fputs(readline,macrofile);
              while (strcmp(command,"ENDM")!=0 && !feof(infile[filecount])) {
                fgets(readline,255,infile[filecount]);
                linecount++;
                fputs(readline,macrofile);
                strcpy(buffer,readline);
                makeupper(buffer);
                parse(buffer);
                }
              fclose(macrofile);
              }
            else {
/*
if (showlist=='*')  printf("%5d %04x: %s",linecount,address,readline);
*/
              if (inmacro=='Y') macro_replace();
              if (strlen(labl)>0)  make_label();
              asm_cmd();
              }
         }
        fclose(infile[filecount]);
        filecount--;
        }
      filecount=1;
      infile[filecount]=fopen(inname,"r");
      pass=2;
      outCount = 0;
      outAddress = 0;
      address=0;
      linecount=0;
      total_bytes=0;
      inmacro='N';
      column=0;
      startAddr=0;
      outputCount=addressSize;
      if (doxref=='Y') xref=fopen("xref.tmp","w+");
      while (filecount>0) {
/*        while (!feof(infile[filecount]))
          {
            fgets(buffer,255,infile[filecount]); */
        while (fgets(buffer,255,infile[filecount])!=NULL) {
            linecount++;
            strcpy(readline,buffer);
            bcount=0;
            makeupper(buffer);
            parse(buffer);
            if (strncmp(command,"INCLUDE",7)==0) {
              strcpy(arg1,readline);
              pBuffer = arg1;
              while (*pBuffer == ' ' && *pBuffer != 0) pBuffer++;
              while (*pBuffer != ' ' && *pBuffer != 0) pBuffer++;
              while (*pBuffer == ' ' && *pBuffer != 0) pBuffer++;
              for (i=0; i<strlen(pBuffer); i++)
                if (pBuffer[i] < 32) pBuffer[i] = 0;
              if (showlist=='*')  printf("%5d %04x: ",linecount,address);
              filecount++;
              if ((infile[filecount]=fopen(pBuffer,"r"))==NULL) {
printf("error opening include file\n");
                filecount--;
                }
              }
            else if (strncmp(command,"MACRO",5)==0) {
              if (showlist=='*') {
                printf("%5d%19s%s",linecount," ",readline);
                }
              while (strcmp(command,"ENDM")!=0 && !feof(infile[filecount])) {
                fgets(readline,255,infile[filecount]);
                linecount++;
                if (showlist=='*') {
                  printf("%5d%19s%s",linecount," ",readline);
                  }
                strcpy(buffer,readline);
                makeupper(buffer);
                parse(buffer);
                }
              strcpy(readline,"\n");
              }
            else {
              if (showlist=='*')  printf("%5d %04x: ",linecount,address);
              if (inmacro=='Y') macro_replace();
              if (strlen(labl)>0) strcpy(lastlabel,labl);
              asm_cmd();
              }
            if (showlist=='*') {
              while (bcount<4) { printf("   "); bcount++; }
              printf("%s",readline);
              }
          }
        fclose(infile[filecount]);
        filecount--;
        }
      write_line();
      if (showlist=='*')  printf("\n");
      if (emucode == 'I') {
        fprintf(outfile,"*%04x\n",startAddr);
        for (i=1;i<=nlabel;i++) {
          if (labelf[i]=='P') {
            fprintf(outfile,"=%04x %s\n",labela[i],labels[i]);
            }
          }
        }
      if (emucode == 'H') {
        fprintf(outfile,":00000001ff\n");
        }

      fclose(outfile);
      if (showsymbols=='Y') {
        if (sorttype=='A') sort_labelsa();
        else if (sorttype==' ') sort_labels();
        for (i=1;i<=nlabel;i++) 
          printf("%04xh: %-16s - %6u\n",labela[i],labels[i],labell[i]);
        }
      if (doxref=='Y') {
        if (sorttype=='A') sort_labelsa();
        else if (sorttype==' ') sort_labels();
        for (i=1;i<=nlabel;i++) {
          printf("%04xh: %-16s - <%6u> ",labela[i],labels[i],labell[i]);
          show_xref(labels[i]);
          }
        }
      if (doxref=='Y') { fclose(xref); unlink("xref.tmp"); }
      if (macron>0)
        for (i=1;i<=macron;i++) {
          strcpy(mbuffer,macros[i]);
          strcat(mbuffer,".mac");
          unlink(mbuffer);
          }
      if (trailer=='Y') {
        printf("\n");
        printf("%d Lines assembled\n",linecount);
        printf("%d Errors\n",errs);
        printf("%d Labels\n",nlabel);
        printf("Object Code:%d bytes\n",total_bytes);
        printf("\n");
        }
   }
 else
    { printf("File does not exist!\n"); errs++; }
 if (errs==0) return(0); else return(1);
}

