#include <stdio.h>
#include <stdlib.h>

#define OUTPUT_COM 1
#define OUTPUT_CMD 2

#define MAX_OBJECTS 25
#define MAX_SYMBOLS 1000

typedef unsigned char byte;
typedef unsigned short word;

struct CMD_HEADER {
  byte type;
  byte length;
  byte addrLo;
  byte addrHi;
  };

word lastAddress;
word startAddress;
byte outputMode;
FILE *file;
FILE *source;
byte nameIndex;
int  count;
byte buffer[1024];
byte inputLine[1024];
char objects[MAX_OBJECTS][80];
int  numObjects;
char symbols[MAX_SYMBOLS][20];
word symbola[MAX_SYMBOLS];
byte symbolv[MAX_SYMBOLS];
int  numSymbols;

struct CMD_HEADER cmdHeader;

void write_byte_cmd(word addr,byte value) {
  if (addr != lastAddress+1 && count>0) {
    cmdHeader.type=1;
    cmdHeader.length=count+2;
    fwrite(&cmdHeader,1,4,file);
    fwrite(buffer,1,count,file);
    count=0;
    }
  if (count==0) {
    cmdHeader.addrLo=(addr & 255);
    cmdHeader.addrHi=(addr >> 8);
    }
  buffer[count++]=value;
  lastAddress=addr;
  if (count==240) {
    cmdHeader.type=1;
    cmdHeader.length=count+2;
    fwrite(&cmdHeader,1,4,file);
    fwrite(buffer,1,count,file);
    count=0;
    }
  }

void close_cmd() {
  if (count>0) {
    cmdHeader.type=1;
    cmdHeader.length=count+2;
    fwrite(&cmdHeader,1,4,file);
    fwrite(buffer,1,count,file);
    }
  cmdHeader.type=2;
  cmdHeader.length=2;
  cmdHeader.addrLo=startAddress & 255;
  cmdHeader.addrHi=startAddress >> 8;
  fwrite(&cmdHeader,1,4,file);
  fclose(file);
  }

void close_com() {
  fclose(file);
  }

void write_byte_com(word addr,byte value) {
  fseek(file,addr,SEEK_SET);
  fwrite(&value,1,1,file);
  }

void write_byte(word addr,byte value) {
  switch (outputMode) {
    case OUTPUT_COM:write_byte_com(addr,value); break;
    case OUTPUT_CMD:write_byte_cmd(addr,value); break;
    }
  }

void processOptions(char *buffer) {
  if (strcmp(buffer,"-com")==0) outputMode=OUTPUT_COM;
  if (strcmp(buffer,"-cmd")==0) outputMode=OUTPUT_CMD;
  }

char *scanHex(char *buffer,word *dest) {
  *dest=0;
  while ((*buffer>='0' && *buffer<='9') ||
         (*buffer>='a' && *buffer<='f') ||
         (*buffer>='A' && *buffer<='F')) {
    switch (*buffer++) {
      case '0':*dest=*dest*16+0; break;    case '1':*dest=*dest*16+1; break;
      case '2':*dest=*dest*16+2; break;    case '3':*dest=*dest*16+3; break;
      case '4':*dest=*dest*16+4; break;    case '5':*dest=*dest*16+5; break;
      case '6':*dest=*dest*16+6; break;    case '7':*dest=*dest*16+7; break;
      case '8':*dest=*dest*16+8; break;    case '9':*dest=*dest*16+9; break;
      case 'a':*dest=*dest*16+10; break;   case 'b':*dest=*dest*16+11; break;
      case 'c':*dest=*dest*16+12; break;   case 'd':*dest=*dest*16+13; break;
      case 'e':*dest=*dest*16+14; break;   case 'f':*dest=*dest*16+15; break;
      case 'A':*dest=*dest*16+10; break;   case 'B':*dest=*dest*16+11; break;
      case 'C':*dest=*dest*16+12; break;   case 'D':*dest=*dest*16+13; break;
      case 'E':*dest=*dest*16+14; break;   case 'F':*dest=*dest*16+15; break;
      }
    }
  return buffer;
  }

void process_data_line(char *buffer) {
  word addr;
  word wrd;
  buffer++;
  buffer=scanHex(buffer,&addr);
  while (*buffer != '\0' && *buffer<=' ') buffer++;
  while (*buffer != '\0') {
    buffer=scanHex(buffer,&wrd);
    write_byte(addr++,wrd & 255);
    while (*buffer != '\0' && *buffer<=' ') buffer++;
    }
  }

void process_start_line(char *buffer) {
  buffer++;
  buffer=scanHex(buffer,&startAddress);
  }

void doSymbol(char *sym,word addr,byte valid) {
  int i;
  int symb;
  symb=-1;
  i=0;
  while (i<numSymbols) {
    if (strcmp(symbols[i],sym)==0) { symb=i; i=numSymbols; }
    i++;
    }
  if (symb>-1) {
    if (valid!=0) {
      symbola[symb]=addr;
      symbolv[symb]=valid;
      }
    } else {
    strcpy(symbols[numSymbols],sym);
    symbola[numSymbols]=addr;
    symbolv[numSymbols++]=valid;
    }
  }

word getSymbolValue(char *sym) {
  int i;
  int symb;
  symb=-1;
  i=0;
  while (i<numSymbols) {
    if (strcmp(symbols[i],sym)==0) { symb=i; i=numSymbols; }
    i++;
    }
  if (symb>-1) return symbola[symb]; else return 0;
  }

void readPass() {
  int i;
  word addr;
  if ((file=fopen("rclink1.$$$","w"))==NULL) {
    printf("Could not open temp file\n");
    exit(1);
    }
  for (i=0;i<numObjects;i++) {
    strcpy(buffer,objects[i]); strcat(buffer,".prg");
    if ((source=fopen(buffer,"r"))==NULL) {
      printf("Could not open source file: %s\n",buffer);
      exit(1);
      }
    while (fgets(inputLine,1024,source)!=0) {
      while (inputLine[0]!='\0' && inputLine[strlen(inputLine)-1]<32)
        inputLine[strlen(inputLine)-1]='\0';
      if (inputLine[0]=='?') {
        doSymbol(&inputLine[1],0,0);
        fprintf(file,"%s\n",inputLine);
        } else if (inputLine[0]=='=') {
        scanHex(&inputLine[1],&addr);
        doSymbol(&inputLine[6],addr,1);
        }
        else fprintf(file,"%s\n",inputLine);
      }
    fclose(source);
    }
  fclose(file);
  }

void resolvePass() {
  int i;
  word addr;
  if ((source=fopen("rclink1.$$$","r"))==NULL) {
    printf("Could not open temp file\n");
    exit(1);
    }
  if ((file=fopen("rclink2.$$$","w"))==NULL) {
    printf("Could not open temp file\n");
    exit(1);
    }
  while (fgets(inputLine,1024,source)!=0) {
    while (inputLine[0]!='\0' && inputLine[strlen(inputLine)-1]<32)
      inputLine[strlen(inputLine)-1]='\0';
    if (inputLine[0]=='?') {
      addr=getSymbolValue(&inputLine[1]);
      fgets(inputLine,1024,source);
      while (inputLine[0]!='\0' && inputLine[strlen(inputLine)-1]<32)
        inputLine[strlen(inputLine)-1]='\0';
      if (inputLine[0]!=':') {
        printf("Source format error\n");
        fclose(source);
        fclose(file);
        exit(0);
        }
      sprintf(buffer,"%04x",addr);
      inputLine[6]=buffer[2];
      inputLine[7]=buffer[3];
      inputLine[9]=buffer[0];
      inputLine[10]=buffer[1];
      fprintf(file,"%s\n",inputLine);
      }
      else fprintf(file,"%s\n",inputLine);
    }
  fclose(source);
  fclose(file);
  }

void binaryPass() {
  strcpy(buffer,"rclink2.$$$");
  if ((source=fopen(buffer,"r"))==NULL) {
    printf("Could not open source file: %s\n",buffer);
    exit(1);
    }
  strcpy(buffer,objects[numObjects-1]);
  switch (outputMode) {
    case OUTPUT_COM:strcat(buffer,".com"); break;
    case OUTPUT_CMD:strcat(buffer,".cmd"); break;
    }
  if ((file=fopen(buffer,"wb"))==NULL) {
    printf("Could not open output file: %s\n",buffer);
    exit(1);
    }
  while (fgets(inputLine,1023,source)!=0) {
    if (inputLine[0]=='*') process_start_line(inputLine);
    if (inputLine[0]==':') process_data_line(inputLine);
    }
  switch (outputMode) {
    case OUTPUT_COM:close_com(); break;
    case OUTPUT_CMD:close_cmd(); break;
    }
  fclose(source);
  }

main(int argc,char *argv[]) {
  int i;
  lastAddress=0xffff;
  outputMode=OUTPUT_COM;
  count=0;
  cmdHeader.addrLo=0;
  cmdHeader.addrHi=0;
  startAddress=0;
  nameIndex=0;
  numObjects=0;
  numSymbols=0;
  for (i=1;i<argc;i++) {
    if (argv[i][0]=='-') processOptions(argv[i]);
      else strcpy(objects[numObjects++],argv[i]);
    }
  if (numObjects!=0) {
    readPass();
    resolvePass();
    binaryPass();
    unlink("rclink1.$$$");
    unlink("rclink2.$$$");
    } else printf("No input files specified\n");
  }
