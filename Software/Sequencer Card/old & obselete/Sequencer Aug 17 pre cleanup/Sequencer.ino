// Sequencer Memory card code
// Mode Determined by UCODESWITCH
//Mode 1 download uCode from serial line and store in EEPROM, press STARTSWITCH to begin (READY and LOADING LEDs ON)
//MODE 2 Write EEPROM into UCode RAM and set READY line high


#include <Wire.h> //I2C library
#include "Adafruit_MCP23017.h"

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

//UCODESWITCH Modes
#define DOWNLOAD 0
#define WRITEMEM 1

int mode;

#define LINES_PER_INSTRUCTION 32  // total number of rows of BYTES_PER_LINE
#define BYTES_PER_LINE 8         // total bytes/row (mem chips) should be 8
#define TOTAL_INSTRUCTIONS  10  // FIX shoud be 256
#define INSTRUCTION_SIZE  BYTES_PER_LINE*LINES_PER_INSTRUCTION

/*
  #define LINES_PER_INSTRUCTION 2  // total number of rows of BYTES_PER_LINE
  #define BYTES_PER_LINE 2         // total bytes/row (mem chips) should be 8
  #define TOTAL_INSTRUCTIONS  4
  #define INSTRUCTION_SIZE  BYTES_PER_LINE*LINES_PER_INSTRUCTION
*/

#define NUMBER_OF_CONTROLERS 5
#define ADDRESS_CHIP 0
#define DATA_CHIP_START 1
#define DATA_CHIPS 4

Adafruit_MCP23017 mcp[NUMBER_OF_CONTROLERS];
unsigned char insData[BYTES_PER_LINE * LINES_PER_INSTRUCTION] = {0,};

unsigned char prog1[] = {
  0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,  //blank line
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,  //Out on
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,  //Out off
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,  //Reset ucode counter
};
#define PROG1_SIZE 2

void writeCodeToROM(int, unsigned char *);



void doError(String errorMsg, int code) {
  ///Serial.print("Error: ");
  ///Serial.println(errorMsg);
  while (1) {
    for (int i = 0; i < code; i++)
      flash(FAULT);
    delay(2000);
  }
}

void flash(int led) {
  digitalWrite(led, HIGH);
  delay(250);
  digitalWrite(led, LOW);
  delay(250);
}

/*
   Read instruction from RAM
*/

void dumpInstruction(int instruction) {
  int i, j;
  char tmp[20];

  Serial.print("Instruction=");
  Serial.println(instruction);

  //print header
  for (i = 0; i < BYTES_PER_LINE; i++) {
    sprintf(tmp, "%02x ", i); Serial.print(tmp);
  }
  Serial.println();

  readInstruction(instruction, insData);
  //sprintf(tmp, "%02x ", insData[0]); Serial.println(tmp);
  for (j = 0; j < LINES_PER_INSTRUCTION; j++) {
    for (i = 0; i < BYTES_PER_LINE; i++) {
      sprintf(tmp, "%02x ", insData[i + j * BYTES_PER_LINE]); Serial.print(tmp);
    }
    Serial.println();
  }
}

void setup() {
  int i;

  Serial.begin(19200);
  Wire.begin();

  for (i = 0; i < NUMBER_OF_CONTROLERS; i++) {
    mcp[i].begin(i);
  }


  pinMode(FAULT, OUTPUT);
  pinMode(READY, OUTPUT);
  pinMode(LOADING, OUTPUT);

  pinMode(MEMRD, OUTPUT);
  pinMode(MEMWR, OUTPUT);
  pinMode(MEMSEL, OUTPUT);

  digitalWrite(MEMRD, HIGH);
  digitalWrite(MEMWR, HIGH);
  digitalWrite(MEMSEL, HIGH);

  pinMode(UCODESWITCH, INPUT);
  pinMode(STARTSWITCH, INPUT);

  flash(FAULT);
  flash(READY);
  flash(LOADING);
  flash(FAULT);
  flash(READY);
  flash(LOADING);

  mode = digitalRead(UCODESWITCH);

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

  //unsigned char instructionBytes[INSTRUCTION_SIZE];
  //unsigned char currentChecksum;
  //unsigned char currentInstruction;
  //unsigned char instructionChecksum;
  //unsigned char readIns[INSTRUCTION_SIZE];

  int previousLine;
  int previousInstruction;

  //unsigned char insData[BYTES_PER_LINE * LINES_PER_INSTRUCTION];

  // Select a test program to run, they never return

  //test1();

  if (mode == WRITEMEM) { // LOADING on while copying, READY on when complete

#define FILLRAM
#ifdef FILLRAM //with pattern
    Serial.println("Fill RAM");
    digitalWrite(LOADING, HIGH);
    setAddressOutput();
    setDataOutput();

    for (instruction = 0; instruction < TOTAL_INSTRUCTIONS; instruction++) {
      Serial.print("Filling Instruction Memory - ");
      Serial.println(instruction);

      digitalWrite(LOADING, statusLed);
      statusLed = !statusLed;

      for (i = 0; i < LINES_PER_INSTRUCTION; i++) {
        insData[i * BYTES_PER_LINE] = instruction;
        insData[i * BYTES_PER_LINE + 1] = i;
        for (j = 2; j < BYTES_PER_LINE; j++) {
          insData[i * BYTES_PER_LINE + j] = 0;
        }
      }
      writeCodeToROM(instruction, insData);
    }
#endif

    Serial.println("Copy EEPROM to RAM");
    digitalWrite(LOADING, HIGH);
    setAddressOutput();
    setDataOutput();

    for (instruction = 0; instruction < TOTAL_INSTRUCTIONS; instruction++) {
      Serial.print("Copying Instruction ");
      Serial.println(instruction);

      digitalWrite(LOADING, statusLed);
      statusLed = !statusLed;

      readCodeFromROM(instruction, insData);
      writeInstruction(instruction, insData);
    }

#define DISPLAY_MEM
#ifdef DISPLAY_MEM
    // Set all lines to input, therefore can be driven by logic card
    setDataInput();
    setAddressOutput();
    uCodeRamRead(true);
    uCodeRamSelect(true);

    //digitalWrite(LOADING, HIGH);
    address = 0;
    writeAddress(0);
    line = address & 0x1f;
    instruction = (address & 0x1fe0) >> 5;

    readData(data);
    sprintf(tmp, "----Address=%04x Seq=%05d Instruction=%02x Line=%02x Data=", address & 0x1fff, counter++, instruction, line);
    Serial.print(tmp);
    for (int i = 0; i < BYTES_PER_LINE; i++) {
      sprintf(tmp, "%02x:", data[i]);
      Serial.print(tmp);
    }
    Serial.println();
    // last chance to read RAM
    dumpInstruction(0);
    dumpInstruction(1);
    dumpInstruction(5);
    dumpInstruction(6);
    dumpInstruction(9);

#endif

    setAddressInput();
    setDataInput();
    uCodeRamRead(true);
    uCodeRamSelect(true);
    // Set uCode RAM to read enabled and selected


    digitalWrite(LOADING, LOW);
    digitalWrite(READY, HIGH);
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
        sprintf(tmp, "Address=%04x Seq=%05d Instruction=%02x Line=%02x Data=", address & 0x1fff, counter++, instruction, line);
        Serial.print(tmp);
        for (int i = 0; i < BYTES_PER_LINE; i++) {
          sprintf(tmp, "%02x:", data[i]);
          Serial.print(tmp);
        }
        Serial.println();
        previousLine = line;
        previousInstruction = instruction;
      }
    }


  }
  else { // download instructions

    //FIX maybe wait for swith press STARTSWITCH
    digitalWrite(LOADING, HIGH);
    digitalWrite(READY, HIGH);
    while (digitalRead(STARTSWITCH) == true);
    digitalWrite(READY, LOW);

    digitalWrite(LOADING, HIGH);

    while ( waitInstructionBegin()) { // Returns true when instruction downloaded, false when end of instruction list
      downloadInstruction();
      flash(LOADING);
      digitalWrite(LOADING, HIGH);
    }

    digitalWrite(LOADING, LOW);

    while (1)
      delay(1000);
  }
}