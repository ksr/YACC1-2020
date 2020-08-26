#ifdef SUPPORT
#define prefix
#else
#define prefix extern
#endif

#define VERSION 2.2
#define MAX_LABELS 1000

#define DEF_DIR    "/Users/ken/rcasm/"

typedef unsigned char byte;
typedef unsigned int word;

struct DEFFILE {
  char Title[80];
  char ClassName[20][20];
  char ClassText[20][255];
  int  ClassCount;
  char Command[500][100];
  char Arg1[500][15];
  char Arg2[500][15];
  char Dest[500][40];
  int  TransCount;
  };

typedef struct {
  char typ;
  union {
    int number;
    char chr[32];
    } data;
  } TOKEN;

prefix int   outputCount;
prefix int   addressSize;
prefix TOKEN tokens[1024];
prefix int   tokenCount;
prefix FILE  *infile[10];
prefix FILE  *outfile;
prefix FILE  *xref;
prefix FILE  *macrofile;
prefix int   bcount;
prefix int   filecount;
prefix char  inmacro;
prefix char  inname[80];
prefix char  outname[80];
prefix char  workfile[80];
prefix char  buffer[255];
prefix char  command[1024];
prefix char  labl[1024];
prefix char  lastlabel[1024];
prefix char  arg1[1024];
prefix char  arg2[1024];
prefix word  address;
prefix char  doxref;
prefix int   nlabel;
prefix char  labels[MAX_LABELS][16];
prefix word  labell[MAX_LABELS];
prefix word  labela[MAX_LABELS];
prefix byte  labelf[MAX_LABELS];
prefix char  macror[10][20];
prefix char  macros[100][20];
prefix char  macrov[10][20];
prefix int   macrovars;
prefix int   macron;
prefix char  mbuffer[255];
prefix byte  memory[65536];
prefix int   i;
prefix byte  valid;
prefix char  readline[256];
prefix word  linecount;
prefix int   pass;
prefix char  showlist;
prefix char  showsymbols;
prefix char  emucode;
prefix word  highest;
prefix int   column;
prefix char  sorttype;
prefix word  total_bytes;
prefix word  errs;
prefix char  trailer;
prefix char  DefName[20];
prefix struct DEFFILE DefFile;
prefix word  startAddr;
prefix char  showrule;
prefix char  listBuffer[256];
prefix word args[10];
prefix char argtype[10];
prefix int argflag[10];
prefix int ArgCount;
prefix byte  outRow[16];
prefix byte  outCount;
prefix word  outAddress;

extern word find_label(char* buf);
extern void write_byte(byte i);
extern void write_line();
extern word get_num(char* buf);
extern word find_label_number(char* buf);

