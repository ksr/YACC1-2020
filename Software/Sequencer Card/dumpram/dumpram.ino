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
#define TOTAL_INSTRUCTIONS  8  // FIX shoud be 256
#define INSTRUCTION_SIZE  BYTES_PER_LINE*LINES_PER_INSTRUCTION

#define NUMBER_OF_CONTROLERS 5
#define ADDRESS_CHIP 0
#define DATA_CHIP_START 1
#define DATA_CHIPS 4

Adafruit_MCP23017 mcp[NUMBER_OF_CONTROLERS];
unsigned char insData[BYTES_PER_LINE * LINES_PER_INSTRUCTION] = {0,};

/*
   manage IO control ports

*/
#define INDIR 0
#define OUTDIR 1

int addrDir = -1;
int dataDir = -1;

void setDataOutput() {
  int i, j;

  dataDir = OUTDIR;

  for (i = 1; i < NUMBER_OF_CONTROLERS; i++) {
    for (j = 0; j < 16; j++) {
      mcp[i].pinMode(j, OUTPUT);
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
      mcp[1].digitalWrite(j, LOW);
    }
  }
}

void setAddressOutput() {
  int j;

  addrDir = OUTDIR;

  for (j = 0; j < 16; j++) {
    mcp[ADDRESS_CHIP].pinMode(j, OUTPUT);
    mcp[ADDRESS_CHIP].pullUp(j, HIGH);  // turn on a 100K pullup internally
    mcp[ADDRESS_CHIP].digitalWrite(j, LOW);
  }
}

void setDataInput() {
  int i, j;

  dataDir = INDIR;

  for (i = 1; i < NUMBER_OF_CONTROLERS; i++) {
    for (j = 0; j < 16; j++) {
      mcp[i].pinMode(j, INPUT);
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
    }
  }
}

void setAddressInput() {
  int j;

  addrDir = INDIR;

  for (j = 0; j < 16; j++) {
    mcp[ADDRESS_CHIP].pinMode(j, INPUT);
    mcp[ADDRESS_CHIP].pullUp(j, HIGH);  // turn on a 100K pullup internally
  }

}


//FIX TEST TEST TEST use this call in read instruction
void readData(unsigned char ptr[]) {
  int i;
  char tmp[30];

  for (i = 0; i < DATA_CHIPS; i++) {
    //sprintf(tmp, "low = %x  hi=%x both=%x", mcp[i + DATA_CHIP_START].readGPIO(0),mcp[i + DATA_CHIP_START].readGPIO(1),mcp[i + DATA_CHIP_START].readGPIOAB());
    //Serial.println(tmp);

    *ptr++ = mcp[i + DATA_CHIP_START].readGPIO(0);//read low byte
    *ptr++ = mcp[i + DATA_CHIP_START].readGPIO(1);//read hi byte
  }
}


unsigned int writeAddress(unsigned int address) {

  if (addrDir != OUTDIR)
    doError("IO: Writing address with addr direction set to in", 4);

  mcp[ADDRESS_CHIP].writeGPIOAB(address);
  return (address);
}


/*
   manage uCode RAM
*/

void uCodeRamRead(int mode) {
  if (mode == true)
    digitalWrite(MEMRD, LOW);
  else
    digitalWrite(MEMRD, HIGH);
}

void uCodeRamSelect(int mode) {
  if (mode == true)
    digitalWrite(MEMSEL, LOW);
  else
    digitalWrite(MEMSEL, HIGH);
}

unsigned int readInstruction(int instruction, unsigned char *data) {
  int i, j;
  unsigned char *ptr;
  unsigned int address;

  address = instruction * LINES_PER_INSTRUCTION;
  Serial.print("read instruction:"); Serial.print(instruction); Serial.print(" Address:"); Serial.println(address);

  ptr = data;

  for (j = 0; j < LINES_PER_INSTRUCTION; j++) {
    writeAddress(j + address);
    //Serial.print("Address set line:"); Serial.println(j + address);
    ///test test test removal of this code
    for (i = 0; i < BYTES_PER_LINE / 2; i++) {
      //Serial.print("Data:"); Serial.println(mcp[i + DATA_CHIP_START].readGPIO(0));
      *ptr = mcp[i + DATA_CHIP_START].readGPIO(0); //read low byte, +1 IO Chips Data starts at chip 1
      ptr++;
      *ptr = mcp[i + DATA_CHIP_START].readGPIO(1); //read hi byte
      ptr++;
    }
    //readData(ptr);
    //ptr += BYTES_PER_LINE;

  }
}

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
  
  mode = digitalRead(UCODESWITCH);

}

void loop() {

  int address;
  int line;
  int instruction, ins;
  unsigned char data[BYTES_PER_LINE];
  unsigned int counter;
  char tmp[100];
  boolean statusLed = false;
  int i, j;

  if (mode == WRITEMEM) { // DUMP RAM CONTENTS

    digitalWrite(LOADING, HIGH);

    setAddressOutput();
    setDataInput();
    uCodeRamRead(true);
    uCodeRamSelect(true);


    for (instruction = 0; instruction < TOTAL_INSTRUCTIONS; instruction++) {
      digitalWrite(LOADING, statusLed);
      statusLed = !statusLed;

#ifdef JUNK
      address = 0;
      writeAddress(0);
      line = address & 0x1f;
      ins = (address & 0x1fe0) >> 5;

      readData(data);
      sprintf(tmp, "Address=%04x Seq=%05d Instruction=%02x Line=%02x Data=", address & 0x1fff, counter++, ins, line);
      Serial.print(tmp);
      for (int i = 0; i < BYTES_PER_LINE; i++) {
        sprintf(tmp, "%02x:", data[i]);
        Serial.print(tmp);
      }
#endif
      dumpInstruction(instruction);
      Serial.println();


    }

    digitalWrite(LOADING, LOW);
    digitalWrite(READY, HIGH);
    Serial.println("RAM READ COMPLETE!!!");
    while (1)
      flash(READY);
  }
  else { // no - read only - check RAM contents
    while (1)
      flash(FAULT);
  }
}
