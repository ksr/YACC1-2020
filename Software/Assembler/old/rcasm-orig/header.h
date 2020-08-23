#ifdef SUPPORT
#define prefix
#else
#define prefix extern
#endif

#define VERSION 3.0

#define DEF_DIR    "/usr/lib/rcasm/"

#ifdef WIN32
#define EOL        "\r\n"
#else
#define EOL        "\n"
#endif

typedef unsigned char byte;
typedef unsigned int word;

typedef struct {
  char   Title[80];
  char** ClassName;
  char** ClassText;
  int    ClassCount;
  char** Command;
  char** Dest;
  int    TransCount;
  } DEFFILE;

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
prefix char** labels;
prefix word*  labell;
prefix word*  labela;
prefix byte*  labelf;
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
prefix char  outUC;
prefix char  endianess;
prefix int   indent;
prefix char** defines;
prefix char** defValues;
prefix int    numDefines;
prefix DEFFILE* DefFile;
prefix int      defCount;
prefix char     verbose;

extern word find_label(char* buf);
extern void write_byte(byte i);
extern void write_word(word i);
extern void write_line();
/* extern word get_num(char* buf); */
extern char* get_num(char* buffer,word* ret);
extern word find_label_number(char* buf);
extern void errorOut(char *msg);
extern char* nextWord(char* buffer);
extern char* findSpace(char* buffer);
extern int getDefine(char* buffer);
extern void processDefs(char* buffer);
extern byte char_to_hex(char buffer);
extern void asm_cmd();


