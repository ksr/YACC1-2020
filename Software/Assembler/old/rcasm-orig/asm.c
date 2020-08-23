#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "mstrings.h"
#include "header.h"

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
  BPtr=buffer;
  while (*BPtr != '\0' && *BPtr <= ' ') BPtr++;
  strcpy(buffer,BPtr);
  while (buffer[0]!='\0' && buffer[strlen(buffer)-1]<=' ')
    buffer[strlen(buffer)-1]='\0';
  }

void addDefine(char* def,char* value) {
  numDefines++;
  if (numDefines == 1) {
    defines = (char**)malloc(sizeof(char*));
    defValues = (char**)malloc(sizeof(char*));
    } else {
    defines = (char**)realloc(defines,sizeof(char*)*numDefines);
    defValues = (char**)realloc(defValues,sizeof(char*)*numDefines);
    }
  defines[numDefines-1] = (char*)malloc(strlen(def)+1);
  strcpy(defines[numDefines-1],def);
  defValues[numDefines-1] = (char*)malloc(strlen(value)+1);
  strcpy(defValues[numDefines-1],value);
  }

void parse(char *buffer)
{
  int i;
  char inQuote;
  char *BPtr;
  inQuote = 0;
  for (i=0;i<(int)strlen(buffer);i++) {
    if (inQuote != 0 && buffer[i] == inQuote) inQuote = 0;
      else if (buffer[i] == '\'' || buffer[i] == '"') inQuote = buffer[i];
    if (inQuote == 0 && buffer[i]==';') buffer[i]='\0';
    }
  inQuote = 0;
  BPtr = NULL;
  if (buffer[0] >= 'A' && buffer[0] <= 'Z' && strncmp(buffer,"INCLUDE",7) != 0) {
    BPtr=strchr(buffer,':');
    if (BPtr == NULL) {
      BPtr=strchr(buffer,' ');
      }
    }
  if (BPtr == NULL)
  for (i=0; i<(int)strlen(buffer); i++) {
    if (inQuote != 0 && buffer[i] == inQuote) inQuote = 0;
      else if (buffer[i] == '\'' || buffer[i] == '"') inQuote = buffer[i];
    if (inQuote == 0 && buffer[i]==':' && buffer[i+1] <= ' ') {
      BPtr = buffer+i;
      i = (int)strlen(buffer) + 1;
      }
    }
/*
  BPtr=strchr(buffer,':');
*/
  if (BPtr != NULL) {
    *BPtr++ = '\0';
    strcpy(labl,buffer);
    strcpy(buffer,BPtr);
    } else labl[0]='\0';
  trim(buffer);
  strcpy(command,buffer);
}

void make_label()
{
  int i;
  char flag;
  char buffer[60];
#ifdef DEBUG
  doIndent();
  printf("entering: make_label()\n");
  indent += 2;
  doIndent();
  printf("nlabel = %d\n",nlabel);
#endif
  flag=' ';
  for (i=1;i<=nlabel;i++) if (strcmp(labl,labels[i])==0) {
    sprintf(buffer,"Duplicate label : %s (first=%d)",labl,labell[i]);
    errorOut(buffer);
    errs++;
    flag='*';
    }
  if (flag==' ') {
    nlabel++;
    if (nlabel == 1) {
#ifdef DEBUG
  doIndent();
  printf("Initial allocation of label arrays\n");
#endif
      labels = (char**)malloc(sizeof(char*)*2);
      labell = (word*)malloc(sizeof(word)*2);
      labela = (word*)malloc(sizeof(word)*2);
      labelf = (byte*)malloc(sizeof(byte)*2);
      } else {
#ifdef DEBUG
  doIndent();
  printf("Re-allocation of label arrays\n");
#endif
      nlabel++;
      labels = (char**)realloc(labels,sizeof(char*)*nlabel);
      labell = (word*)realloc(labell,sizeof(word)*nlabel);
      labela = (word*)realloc(labela,sizeof(word)*nlabel);
      labelf = (byte*)realloc(labelf,sizeof(byte)*nlabel);
      nlabel--;
      }
#ifdef DEBUG
  doIndent();
  printf("Allocate memory for label\n");
#endif
    labels[nlabel] = (char*)malloc(strlen(labl) + 1);
#ifdef DEBUG
  doIndent();
  printf("Copy label\n");
#endif
    strcpy(labels[nlabel],labl);
    labell[nlabel]=linecount;
    labela[nlabel]=address;
    labelf[nlabel]=' ';
    }
#ifdef DEBUG
  doIndent();
  printf("exiting: make_label()\n");
  indent -= 2;
#endif
}

void sort_labels()
{
  word t;
  char flag;
  char* ctmp;
  int i;
#ifdef DEBUG
  doIndent();
  printf("entering: sort_labels()\n");
  indent += 2;
#endif
  flag='*';
  while (flag=='*') {
    flag=' ';
    for (i=1;i<nlabel;i++) {
      if (strcmp(labels[i],labels[i+1])>0) {
        flag='*';
        t=labell[i]; labell[i]=labell[i+1]; labell[i+1]=t;
        t=labela[i]; labela[i]=labela[i+1]; labela[i+1]=t;
        ctmp = labels[i]; labels[i] = labels[i+1]; labels[i+1] = ctmp;
        }
      }
    }
#ifdef DEBUG
  doIndent();
  printf("exiting: sort_labels()\n");
  indent -= 2;
#endif
}

void sort_labelsa()
{
  word t;
  char flag;
  char* ctmp;
  int i;
#ifdef DEBUG
  doIndent();
  printf("entering: sort_labelsa()\n");
  indent += 2;
#endif
  flag='*';
  while (flag=='*') {
    flag=' ';
    for (i=1;i<nlabel;i++) {
      if (labela[i]>labela[i+1]) {
        flag='*';
        t=labell[i]; labell[i]=labell[i+1]; labell[i+1]=t;
        t=labela[i]; labela[i]=labela[i+1]; labela[i+1]=t;
        ctmp = labels[i]; labels[i] = labels[i+1]; labels[i+1] = ctmp;
        }
      }
    }
#ifdef DEBUG
  doIndent();
  printf("exiting: sort_labelsa()\n");
  indent -= 2;
#endif
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
      if (count==10) { printf("%s%7s",EOL," "); count=0; }
      printf("%6d ",lin); count++; 
      }
    } while (!feof(xref));
  printf("%s",EOL);
}

void macro_replace()
{
  int i;
  for (i=0;i<macrovars;i++) {
makeupper(macrov[i]);
makeupper(macror[i]);
    replace(command,macrov[i],macror[i]);
/*
    replace(arg1,macrov[i],macror[i]);
    replace(arg2,macrov[i],macror[i]);
*/
    }
}

void Pop_Def_File() {
  int i;
  if (defCount == 0) {
    printf("Popped only remaining .def file.  Aborting.%s",EOL);
    exit(1);
    }
  if (DefFile[defCount].ClassCount > 0) {
    for (i=0; i<DefFile[defCount].ClassCount; i++) {
      free(DefFile[defCount].ClassName[i]);
      free(DefFile[defCount].ClassText[i]);
      }
    free(DefFile[defCount].ClassName);
    free(DefFile[defCount].ClassText);
    }
  if (DefFile[defCount].TransCount > 0) {
    for (i=0; i<DefFile[defCount].TransCount; i++) {
      free(DefFile[defCount].Command[i]);
      free(DefFile[defCount].Dest[i]);
      }
    free(DefFile[defCount].Command);
    free(DefFile[defCount].Dest);
    }
  if (defCount == 0) free(DefFile);
  else
    DefFile = (DEFFILE*)realloc(DefFile,sizeof(DEFFILE)*defCount);
  defCount--;
  }

void Read_Def_File(char *FileName)
{
  FILE *InFile;
  char Buffer[255];
  defCount++;
  if (defCount == 0) DefFile = (DEFFILE*)malloc(sizeof(DEFFILE));
    else DefFile = (DEFFILE*)realloc(DefFile,sizeof(DEFFILE)*(defCount+1));
  DefFile[defCount].ClassCount=0;
  DefFile[defCount].TransCount=0;
  addressSize=1;
  if ((InFile=fopen(FileName,"r"))==NULL) {
    strcpy(Buffer,DEF_DIR);
    strcat(Buffer,FileName);
    InFile=fopen(Buffer,"r");
    if (InFile == NULL && getenv("RCASM_DIR") != NULL) {
      strcpy(Buffer,getenv("RCASM_DIR"));
      if (Buffer[strlen(Buffer)-1] != '/') strcat(Buffer,"/");
      strcat(Buffer,FileName);
      InFile=fopen(Buffer,"r");
      }
    }
  if (InFile == NULL) {
    printf("Could not open .def file. aborting\n");
    exit(1);
    }
  fgets(Buffer,80,InFile);
  Buffer[strlen(Buffer)-1]='\0';
  if (verbose == 'Y') printf("%s%s",Buffer,EOL);
  strcpy(DefFile[defCount].Title,Buffer);
  fgets(Buffer,80,InFile);
  Buffer[strlen(Buffer)-1]='\0';
  while (Buffer[0] != '*') {
    if (strcmp(Buffer,"OPTION BIG")==0) endianess = 'B';
    if (strcmp(Buffer,"OPTION LITTLE")==0) endianess = 'L';
    if (strcmp(Buffer,"OPTION 16BIT")==0) {
      if (verbose == 'Y') printf("%s%s",Buffer,EOL);
      addressSize=2;
      }
    if (strncmp(Buffer,"CLASS",5)==0) {
      DefFile[defCount].ClassCount++;
      if (DefFile[defCount].ClassCount == 1) {
        DefFile[defCount].ClassName = (char**)malloc(sizeof(char*));
        DefFile[defCount].ClassText = (char**)malloc(sizeof(char*));
        }
      else {
        DefFile[defCount].ClassName = 
          (char**)realloc(DefFile[defCount].ClassName,
                          sizeof(char*)*DefFile[defCount].ClassCount);
        DefFile[defCount].ClassText = 
          (char**)realloc(DefFile[defCount].ClassText,
                          sizeof(char*)*DefFile[defCount].ClassCount);
        }
      DefFile[defCount].ClassName[DefFile[defCount].ClassCount-1] =
        (char*)malloc(strlen(Buffer+6) + 1);
      strcpy(DefFile[defCount].ClassName[DefFile[defCount].ClassCount-1],&Buffer[6]);
      fgets(Buffer,200,InFile);
      Buffer[strlen(Buffer)-1]='\0';
      DefFile[defCount].ClassText[DefFile[defCount].ClassCount-1] =
        (char*)malloc(strlen(Buffer) + 1);
      strcpy(DefFile[defCount].ClassText[DefFile[defCount].ClassCount-1],Buffer);
      }
    fgets(Buffer,80,InFile);
    Buffer[strlen(Buffer)-1]='\0';
    }
  if (verbose == 'Y')
    printf("%d Classes read.%s",DefFile[defCount].ClassCount,EOL);
  fgets(Buffer,80,InFile);
  while (Buffer[0] != '*') {
    trim(Buffer);
    DefFile[defCount].TransCount++;
      if (DefFile[defCount].TransCount == 1) {
        DefFile[defCount].Command = (char**)malloc(sizeof(char*));
        DefFile[defCount].Dest = (char**)malloc(sizeof(char*));
        }
      else {
        DefFile[defCount].Command = 
          (char**)realloc(DefFile[defCount].Command,
                          sizeof(char*)*DefFile[defCount].TransCount);
        DefFile[defCount].Dest = 
          (char**)realloc(DefFile[defCount].Dest,
                          sizeof(char*)*DefFile[defCount].TransCount);
        }
    DefFile[defCount].Command[DefFile[defCount].TransCount-1] =
      (char*)malloc(strlen(Buffer) + 1);
    strcpy(DefFile[defCount].Command[DefFile[defCount].TransCount-1],Buffer);
    fgets(Buffer,80,InFile);
    DefFile[defCount].Dest[DefFile[defCount].TransCount-1] =
      (char*)malloc(strlen(Buffer) + 1);
    strcpy(DefFile[defCount].Dest[DefFile[defCount].TransCount-1],Buffer);
    fgets(Buffer,80,InFile);
    }
  if (verbose == 'Y')
    printf("%d Patterns read.%s",DefFile[defCount].TransCount,EOL);
  fclose(InFile);
}

void readOptions(char *buffer) {
  char* eq;
  if (buffer[0]!='-') strcpy(workfile,buffer);
  if (strncmp(buffer,"-D",2)==0) {
    eq = strchr(buffer,'=');
    if (eq == NULL) {
      buffer++;
      buffer++;
      makeupper(buffer);
      addDefine(buffer,"");
      }
      else {
      *eq = 0;
      buffer++;
      buffer++;
      eq++;
      makeupper(buffer);
      addDefine(buffer,eq);
      }
    }
  if (strcmp(buffer,"-h")==0) emucode='H';
  if (strcmp(buffer,"-l")==0) showlist='*';
  if (strcmp(buffer,"-s")==0) showsymbols='Y';
  if (strcmp(buffer,"-t")==0) trailer='N';
  if (strcmp(buffer,"-Sl")==0) sorttype='L';
  if (strcmp(buffer,"-Sa")==0) sorttype='A';
  if (strcmp(buffer,"-r")==0) showrule='*';
  if (strcmp(buffer,"-u")==0) outUC='Y';
  if (strcmp(buffer,"-v")==0) verbose='Y';
  if (strcmp(buffer,"-V")==0) {
    printf("RC/asm v%2.1f%s",VERSION,EOL);
    printf("by Michael H. Riley%s%s",EOL,EOL);
    }
  if (strcmp(buffer,"-x")==0) doxref='Y';
  if (strncmp(buffer,"-d",2)==0) {
    if (buffer[2] == ' ') strcpy(DefName,&buffer[3]);
      else strcpy(DefName,&buffer[2]);
    strcat(DefName,".def");
    }
  }

char* readLine(char* buffer) {
  char* ret;
  ret = fgets(buffer,255,infile[filecount]);
  if (ret == NULL) return ret;
  strcpy(readline,buffer);
  makeupper(buffer);
  linecount++;
  return ret;
  }

void preprocessor(char* buffer) {
  char* def;
  char b[255];
  int stack;
  int d;
  if (strncmp(buffer,"#ELSE",5) == 0) {
    stack = 0;
    while (readLine(b) != NULL) {
      if (strncmp(b,"#ENDIF",6) == 0) {
        if (stack == 0) return;
        stack--;
        if (stack < 0) {
          errorOut("Preprocessore #if/#endif nesting error");
          errs++;
          return;
          }
        }
      if (strncmp(b,"#IFDEF",6) == 0) {
        stack++;
        }
      }
    }
  if (strncmp(buffer,"#IFDEF",6) == 0) {
    def = nextWord(buffer);
    d = getDefine(def);
    if (d >= 0) return;
    stack = 0;
    while (readLine(b) != NULL) {
      if (strncmp(b,"#ELSE",5) == 0) {
        if (stack == 0) return;
        }
      if (strncmp(b,"#ENDIF",6) == 0) {
        if (stack == 0) return;
        stack--;
        if (stack < 0) {
          errorOut("Preprocessore #if/#endif nesting error");
          errs++;
          return;
          }
        }
      if (strncmp(b,"#IFDEF",6) == 0) {
        stack++;
        }
      }
    }
  if (strncmp(buffer,"#IFNDEF",7) == 0) {
    def = nextWord(buffer);
    d = getDefine(def);
    if (d < 0) return;
    stack = 0;
    while (readLine(b) != NULL) {
      if (strncmp(b,"#ELSE",5) == 0) {
        if (stack == 0) return;
        }
      if (strncmp(b,"#ENDIF",6) == 0) {
        if (stack == 0) return;
        stack--;
        }
      if (strncmp(b,"#IFDEF",6) == 0) {
        stack++;
        }
      }
    }
  if (strncmp(buffer,"#ENDIF",6) == 0) {
    return;
    }
  if (strncmp(buffer,"#DEFINE",7) == 0) {
    if (pass == 2) return;
    def = nextWord(buffer);
    buffer = findSpace(def);
    if (*buffer == 0) {
      } else {
      *buffer = 0;
      buffer++;
      if (*buffer <= ' ') buffer = nextWord(buffer);
      d = getDefine(def);
      if (d >= 0) {
        sprintf(buffer,"symbol %s already #DEFINEd",def);
        errorOut(buffer);
        errs++;
        return;
        }
      }
    addDefine(def,buffer);
    } /* end of #DEFINE */
  if (strncmp(buffer,"#UNDEF",6) == 0) {
    if (pass == 2) return;
    def = nextWord(buffer);
    d = getDefine(def);
    if (d < 0) {
      sprintf(buffer,"symbol %s not #DEFINEd",def);
      errorOut(buffer);
      errs++;
      return;
      }
    free(defines[d]);
    free(defValues[d]);
    for (i=d; i<numDefines-1; i++) {
      defines[i] = defines[i+1];
      defValues[i] = defValues[i+1];
      }
    numDefines--;
    if (numDefines == 0) {
      free(defines);
      free(defValues);
      } else {
      defines = (char**)realloc(defines,sizeof(char*)*numDefines);
      defValues = (char**)realloc(defValues,sizeof(char*)*numDefines);
      }
    } /* end of #UNDEF */
  }

void asmPass(int p) {
  char ebuffer[255];
  char *pBuffer;
  char defFile[255];
  pass = p;
  outCount = 0;
  outAddress = 0;
  address=0;
  filecount=1;
  inmacro='N';
  linecount=0;
  outputCount=addressSize;
  while (filecount>0) {
    while (fgets(buffer,255,infile[filecount])!=NULL) {
        linecount++;
        strcpy(readline,buffer);
        if (pass == 2) bcount = 0;
        makeupper(buffer);
        if (buffer[0] != '#') processDefs(buffer);
        parse(buffer);
        if (strncmp(command,"INCLUDE",7)==0) {
          filecount++;
          strcpy(arg1,readline);
          pBuffer = arg1;
          while (*pBuffer == ' ' && *pBuffer != 0) pBuffer++;
          while (*pBuffer != ' ' && *pBuffer != 0) pBuffer++;
          while (*pBuffer == ' ' && *pBuffer != 0) pBuffer++;
          for (i=0; i<(int)strlen(pBuffer); i++)
            if (pBuffer[i] < 32) pBuffer[i] = 0;
          if (showlist=='*' && pass == 2) {
            if (inmacro == 'Y') {
              if (outUC == 'Y') printf("      %04X: ",address);
                else printf("      %04x: ",address);
              } else {
              if (outUC == 'Y') printf("%5d %04X: ",linecount,address);
                else printf("%5d %04x: ",linecount,address);
              }
            }
          if ((infile[filecount]=fopen(pBuffer,"r"))==NULL) {
            sprintf(ebuffer,"Include file not found >> %s",pBuffer);
            errorOut(ebuffer);
            filecount--;
            }
          }
        else if (strncmp(command,"MACRO",5)==0) {
          if (pass == 1) {
            strcpy(labl,upper(labl));
            strcpy(macros[++macron],labl);
            strcpy(mbuffer,labl); strcat(mbuffer,".mac");
            macrofile=fopen(mbuffer,"w");
            fputs(readline,macrofile);
            }
          if (showlist=='*' && pass == 2) {
            printf("%5d%19s%s",linecount," ",readline);
            }
          while (strcmp(command,"ENDM")!=0 && !feof(infile[filecount])) {
            fgets(readline,255,infile[filecount]);
            linecount++;
            if (showlist=='*' && pass == 2) {
              printf("%5d%19s%s",linecount," ",readline);
              }
            strcpy(buffer,readline);
            makeupper(buffer);
            if (pass == 1) fputs(readline,macrofile);
            parse(buffer);
            }
          if (pass == 1) fclose(macrofile);
          if (pass == 2) strcpy(readline,"\n");
          }
        else if (command[0] == '#') preprocessor(buffer);
        else if (strchr(command,'{') != NULL) {
          *(strchr(command,'{')) = 0;
          strcpy(defFile,command);
          strcat(defFile,".def");
          Read_Def_File(defFile);
          }
        else if (command[0] == '}') {
          Pop_Def_File();
          }
        else {
          if (showlist=='*' && pass == 2) {
            if (inmacro == 'Y') {
              if (outUC == 'Y') printf("      %04X: ",address);
                else printf("      %04x: ",address);
              } else {
              if (outUC == 'Y') printf("%5d %04X: ",linecount,address);
                else printf("%5d %04x: ",linecount,address);
              }
            }
          if (inmacro=='Y') {
            macro_replace();
            linecount--;
            }
          if (pass == 1 && strlen(labl)>0)  make_label();
          if (pass == 2 && strlen(labl)>0) strcpy(lastlabel,labl);
          asm_cmd();
          }
        if (pass == 2 && showlist=='*') {
          while (bcount<4) { printf("   "); bcount++; }
          printf("%s",readline);
          }

     }
    fclose(infile[filecount]);
    filecount--;
    }
  }

int main(int argc,char** argv,char** env) {
  FILE *inifile;
  char mbuffer[30];
  char ebuffer[255];
  char *pBuffer;
  for (i=0;i<=32767;i++) memory[i]=0;
  numDefines = 0;
  nlabel=0;
  indent = 0;
  showlist=' ';
  emucode='I';
  showsymbols='N';
  sorttype=' ';
  trailer='Y';
  doxref='N';
  showrule=' ';
  defCount = -1;
  outUC = 'N';
  verbose = 'N';
  endianess = 'B';
  strcpy(workfile,"");
  strcpy(DefName,"z80.def");
  inifile=fopen("rcasm.rc","r");
  if (inifile != NULL) {
    while (fgets(buffer,250,inifile)!=0) {
      while (buffer[0]!= '\0' && buffer[strlen(buffer)-1]<33)
        buffer[strlen(buffer)-1]='\0';
      readOptions(buffer);
      }
    fclose(inifile);
    }
  pBuffer = getenv("RCASM_OPTIONS");
  if (pBuffer != NULL) {
    i = 0;
    while (*pBuffer != 0) {
      if (*pBuffer == 0 || *pBuffer == ' ') {
        ebuffer[i] = 0;
        if (strlen(ebuffer) > 0) readOptions(ebuffer);
        i = 0;
        }
      if (*pBuffer > ' ') ebuffer[i++] = *pBuffer;
      pBuffer++;
      }
    ebuffer[i] = 0;
    if (strlen(ebuffer) > 0) readOptions(ebuffer);
    }
  if (argc>0) 
    for (i=1;i<argc;i++)
      {
        readOptions(argv[i]);
     }
  Read_Def_File(DefName);
  strcpy(inname,workfile);
  if (strstr(inname,".") == NULL) strcat(inname,".asm");
  addDefine("_RCASM_","1");
  if ((infile[1]=fopen(inname,"r"))!=0)
    {
      strcpy(outname,workfile);
      if (strstr(outname,".") != NULL)
        *(strstr(outname,".")) = 0;
      if (emucode=='I') strcat(outname,".prg");
      else if (emucode==' ') strcat(outname,".obj");
       else strcat(outname,".hex");
      if (emucode!='I' && emucode!='H') outfile=fopen(outname,"wb");
        else outfile=fopen(outname,"w");
      macron=0;
      errs=0;
      highest=0;
      asmPass(1);
      filecount=1;
      infile[filecount]=fopen(inname,"r");
      total_bytes=0;
      column=0;
      if (doxref=='Y') xref=fopen("xref.tmp","w+");
      asmPass(2);

      write_line();
      if (showlist=='*')  printf("%s",EOL);
      if (emucode == 'I') {
        if (outUC == 'Y') fprintf(outfile,"*%04X%s",startAddr,EOL);
          else fprintf(outfile,"*%04x%s",startAddr,EOL);
        for (i=1;i<=nlabel;i++) {
          if (labelf[i]=='P') {
            if (outUC =='Y') fprintf(outfile,"=%04X %s%s",labela[i],labels[i],EOL);
              else fprintf(outfile,"=%04x %s%s",labela[i],labels[i],EOL);
            }
          }
        }
      if (emucode == 'H') {
        if (outUC == 'Y') fprintf(outfile,":00000001FF%s",EOL);
          else fprintf(outfile,":00000001ff%s",EOL);
        }

      fclose(outfile);
      if (showsymbols=='Y') {
        if (sorttype=='A') sort_labelsa();
        else if (sorttype==' ') sort_labels();
        for (i=1;i<=nlabel;i++) 
          if (outUC=='Y') printf("%04Xh: %-16s - %6d%s",labela[i],labels[i],labell[i],EOL);
            else printf("%04xh: %-16s - %6d%s",labela[i],labels[i],labell[i],EOL);
        }
      if (doxref=='Y') {
        if (sorttype=='A') sort_labelsa();
        else if (sorttype==' ') sort_labels();
        for (i=1;i<=nlabel;i++) {
          if (outUC=='Y') printf("%04Xh: %-16s - <%6d> ",labela[i],labels[i],labell[i]);
            else printf("%04xh: %-16s - <%6d> ",labela[i],labels[i],labell[i]);
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
        printf("%s",EOL);
        printf("%d Lines assembled%s",linecount,EOL);
        printf("%d Errors%s",errs,EOL);
        printf("%d Labels%s",nlabel,EOL);
        printf("Object Code:%d bytes%s",total_bytes,EOL);
        printf("%s",EOL);
        }
   }
 else {
   sprintf(buffer,"File %s does not exist!",workfile);
   errorOut(buffer);
   errs++;
   }
 if (nlabel > 0) {
   for (i=1; i<=nlabel; i++) free(labels[i]);
   free(labels);
   free(labell);
   free(labela);
   free(labelf);
   }
 if (errs==0) return(0); else return(1);
}

