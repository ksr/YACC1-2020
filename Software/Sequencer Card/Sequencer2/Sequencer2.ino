// Sequencer Memory card code
// Mode Determined by UCODESWITCH
//Mode 1 download uCode from serial line and store in EEPROM, press STARTSWITCH to begin (READY and LOADING LEDs ON)
//MODE 2 Write EEPROM into UCode RAM and set READY line high


#include <Wire.h> //I2C library
#include "Adafruit_MCP23017.h"
//#include "YACC_Common_headera.h"

/*
   Format ":AIXXXXXXXX....XXXXX"
   ":"  Start charachter
   "A" Checksum (not including instruction number) (Ignore for now)
   "I" Instruction Number (0-255)- 2 hex ascii chars
   "XXXXXXXX....XXXXX" Data Bytes -  2 hex ascii chars

  Pin 11 FAULT
  Pin 12 Bus-EN READY
  Pin 13 Loading
*/

//RAM Control Lines
#define MEMWR 8
#define MEMRD 9
#define MEMSEL 10

// LEDS
#define FAULT 11
#define READY 12
#define LOADING 13

//Swithces
#define UCODESWITCH A0
#define STARTSWITCH A2 // Default HIGH

//
#define READYLINE 7

//UCODESWITCH Modes
#define DOWNLOAD 0
#define WRITEMEM 1

int mode;
int verbose = 0;

#define LINES_PER_INSTRUCTION 32 // tmp back to 32 HACK  // total number of rows of BYTES_PER_LINE
#define BYTES_PER_LINE 8         // total bytes/row (mem chips) should be 8
#define INSTRUCTION_SIZE  BYTES_PER_LINE*LINES_PER_INSTRUCTION

#define WORKING_INSTRUCTION_SET  256  // FIX shoud be 256
#define ALL_INSTRUCTIONS 256

#define NUMBER_OF_CONTROLERS 5
#define ADDRESS_CHIP 0
#define DATA_CHIP_START 1
#define DATA_CHIPS 4

Adafruit_MCP23017 mcp[NUMBER_OF_CONTROLERS];
unsigned char insData[BYTES_PER_LINE * LINES_PER_INSTRUCTION] = {0,};

void writeCodeToROM(int, unsigned char *);

//opcode myArraySRAM;



void doError(String errorMsg, int code) {
  ///Serial.print("Error: ");
  ///Serial.println(errorMsg);
  while (1) {
    for (int i = 0; i < code; i++) {
      blinkLed(FAULT); 
      delay(1000);
    } 
    delay(5000);
  }
}

void flashLed(int led) {
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
}

void blinkLed(int led) {
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
}

/*
   Test Pattern, for each line of instruction - BYTE 0 instruction #, BYTE 1, LINE #, remaining 0s
*/
void fillTestPattern(int instruction) {
  int i, j;

  if (verbose > 0) {
    Serial.print("Filling Memory - ");
    Serial.println(instruction);
  }

  for (i = 0; i < LINES_PER_INSTRUCTION; i++) {
    insData[i * BYTES_PER_LINE] = instruction;
    insData[i * BYTES_PER_LINE + 1] = i;
    for (j = 2; j < BYTES_PER_LINE; j++) {
      insData[i * BYTES_PER_LINE + j] = 0;
    }
  }
  writeInstruction(instruction, insData);

}

/*
   Read instruction from RAM
*/
void dumpInstruction(int instruction) {
  int i, j;
  char tmp[20];

  Serial.print("Ins=");
  Serial.println(instruction);

  Serial.print("        ");
  for (i = 0; i < BYTES_PER_LINE; i++) {
    sprintf(tmp, "%02x ", i); Serial.print(tmp);
  }
  Serial.println();

  readInstruction(instruction, insData);
  for (j = 0; j < LINES_PER_INSTRUCTION; j++) {
    sprintf(tmp, "LINE:%02d ", j); Serial.print(tmp);
    for (i = 0; i < BYTES_PER_LINE; i++) {
      sprintf(tmp, "%02x ", insData[i + j * BYTES_PER_LINE]); Serial.print(tmp);
    }
    Serial.println();

  }
  Serial.println();
}

void setup() {
  int i;

  Serial.begin(19200);
  Wire.begin();

  pinMode(UCODESWITCH, INPUT);
  mode = digitalRead(UCODESWITCH);


  if (mode == WRITEMEM)
    Serial.println("\n\n\n\nStartup");

  for (i = 0; i < NUMBER_OF_CONTROLERS; i++) {
    mcp[i].begin(i);
  }


  pinMode(FAULT, OUTPUT);
  pinMode(READY, OUTPUT);
  pinMode(LOADING, OUTPUT);
  pinMode(READYLINE, OUTPUT);

  pinMode(MEMRD, OUTPUT);
  pinMode(MEMWR, OUTPUT);
  pinMode(MEMSEL, OUTPUT);

  digitalWrite(MEMRD, HIGH);
  digitalWrite(MEMWR, HIGH);
  digitalWrite(MEMSEL, HIGH);
  digitalWrite(READYLINE, LOW);

  pinMode(UCODESWITCH, INPUT);
  pinMode(STARTSWITCH, INPUT);

  flashLed(FAULT);
  flashLed(READY);
  flashLed(LOADING);

  if (mode == WRITEMEM)
    Serial.println("Startup done\n");
}

void loop() {

  int address;
  int line;
  int instruction;
  unsigned char data[BYTES_PER_LINE];
  unsigned int counter;
  char tmp[100];
  boolean statusLed = false;
  int i, j;

  int previousLine;
  int previousInstruction;
  unsigned char previousData[8];
  unsigned char mask;
  int val;

  //bool printed=false;

  if (mode == WRITEMEM) { // LOADING on while copying, READY on when complete

    Serial.println("Write Memory");

#define FILLRAM
#ifdef FILLRAM
    Serial.println("test pattern");

    setAddressOutput();
    setDataOutput();

    for (instruction = 0; instruction < ALL_INSTRUCTIONS; instruction++) {
      digitalWrite(LOADING, statusLed);
      statusLed = !statusLed;
      fillTestPattern(instruction);
    }
    Serial.println("- done");
#endif

#define ROMTORAM
#ifdef ROMTORAM
    Serial.println("EEPROM to RAM");
    digitalWrite(LOADING, HIGH);
    setAddressOutput();
    setDataOutput();

    for (instruction = 0; instruction < WORKING_INSTRUCTION_SET; instruction++) {
      Serial.print("Copying Instruction ");
      Serial.println(instruction);

      digitalWrite(LOADING, statusLed);
      statusLed = !statusLed;

      readCodeFromROM(instruction, insData);
      writeInstruction(instruction, insData);
    }
    Serial.println("- done");
#endif


#define DISPLAY_MEM
#ifdef DISPLAY_MEM
    // Set all lines to input, therefore can be driven by logic card
    setDataInput();
    setAddressOutput();
    uCodeRamRead(true);
    uCodeRamSelect(true);

    dumpInstruction(0);
    //dumpInstruction(1);
    //dumpInstruction(5);
    //dumpInstruction(6);
    dumpInstruction(124);
#endif

    setAddressInput();
    setDataInput();
    uCodeRamRead(true);
    uCodeRamSelect(true);
    // Set uCode RAM to read enabled and selected


    digitalWrite(LOADING, LOW);
    digitalWrite(READY, HIGH);
    digitalWrite(READYLINE, HIGH);
    Serial.println("RAM copy complete, READY!!!");


    counter = 0;
    previousLine = -1;
    previousInstruction = -1;


    while (1) {
      // Monitor
      delay(1000);
      address = readAddress();
      line = address & 0x1f;
      instruction = (address & 0x1fe0) >> 5;
      if ((previousLine != line) || (previousInstruction != instruction)) {
        readData(data);
        sprintf(tmp, "Addr= % 04x Seq= % 05d Ins= % 02x Line= % 02x Data= ", address & 0x1fff, counter++, instruction, line);
        Serial.print(tmp);
        for (int i = 0; i < BYTES_PER_LINE; i++) {
          sprintf(tmp, " %02x: ", data[i]);
          Serial.print(tmp);
        }
        Serial.println();

        if (line != 0) {
#ifdef JUNK
          sprintf(tmp, "---Addr = % 04x Seq = % 05d Inst = % 02x Line = % 02x Data = ", address & 0x1fff, counter++, instruction, line);
          Serial.print(tmp);
          for (int i = 0; i < 8; i++) {
            sprintf(tmp, " %02x! ", data[i]^previousData[i]);
            Serial.print(tmp);
          }
          Serial.println();
#endif
          for (int bytee = 0; bytee < 8; bytee++) {
            mask = 0x01;
            for (int bit = 0; bit < 8; bit++) {
              if ( ((data[bytee]^previousData[bytee]) & mask) != 0) {
                //memcpy_P( &myArraySRAM, &opcodesPROGMEM[(bytee * 8) + bit], sizeof(opcode));
                //sprintf(tmp, "sig=[%d][%s] ", (bytee * 8) + bit, myArraySRAM.code);
                val = data[bytee] & mask;
                if (val != 0)
                  val = 1;
                sprintf(tmp, "sig=[%d]=%d ", (bytee * 8) + bit, val);
                Serial.print(tmp);
              }
              mask = mask << 1;
            }

          }
          Serial.println();
        }
        previousLine = line;
        previousInstruction = instruction;
        for (i = 0; i < 8; i++)
          previousData[i] = data[i];
      }
    }
  }
  else { // download instructions

    digitalWrite(LOADING, HIGH);
    digitalWrite(READY, HIGH);
    while (digitalRead(STARTSWITCH) == true);
    digitalWrite(READY, LOW);

    while ( waitInstructionBegin()) { // Returns true when instruction downloaded, false when end of instruction list
      downloadInstruction();
      digitalWrite(LOADING, statusLed);
      statusLed = !statusLed;
    }

    digitalWrite(LOADING, LOW);

    while (1)
      delay(1000);
  }
}
