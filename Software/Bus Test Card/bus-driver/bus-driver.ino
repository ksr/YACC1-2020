
/*
   SET ARDUINO MONITOR TO "No Line Ending"

   Command Format  CMD:OPERAND#
   EX: REG-FUNC:1#

   0 is off
   1 is on

   ????this program adjusts for active low vs active high

   SERIAL MONITOR NO LINE ENDING

*/

#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <avr/pgmspace.h>
#include "YACC_Common_header.h" //switch to YACC_Common_header new3-2 file for new boards

//#define DEBUG

#define READ_INPUT

#define OPCODE_OPERAND_SPLIT ":"
#define PROMPT ">>"

#define BUS_WRITE 1
#define BUS_READ 0

#define BAD_OPCODE 9
#define BAD_PARAMTER 10
#define ADDR_BUS_MODE 11
#define DATA_BUS_MODE 12
#define COMMAND_READY 13

Adafruit_MCP23017 mcp[NUMBER_OF_CONTROLLERS];

String command = "";
boolean commandReady = false;
String result = "";

unsigned int readData();
unsigned int readDataLo();
unsigned int readDataHi();
unsigned int  writeData(unsigned int);
unsigned int readAddress();
unsigned int writeAddress(unsigned int);
int dataBusDir(int);
int addrBusDir(int);
int dataBusMode = BUS_READ;
int addrBusMode = BUS_READ;
int opLookUp(char *codeToLookup);


/*
  LED Routines
*/

void setLed(int led, int state) {
  digitalWrite(led, state);
}

void flashLed(int led) {
  char tmp[20];

#ifdef DEBUG
  sprintf(tmp, "Flash %d\n", led);
  Serial.print(tmp);
#endif

  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
}

void blinkLed(int led) {
  char tmp[20];

#ifdef DEBUG
  sprintf(tmp, "Flash %d\n", led);
  Serial.print(tmp);
#endif

  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
}

void switch_led_setup() {
  int i;
  char tmp[20];

  strcpy(tmp, "OUT-LED");
  i = opLookUp(tmp);
  mcp[opcodes[i].chip].pinMode(opcodes[i].port * PINS_PER_PORT + opcodes[i].pin, OUTPUT);

  strcpy(tmp, "IN-SWITCH");
  i = opLookUp(tmp);
  mcp[opcodes[i].chip].pinMode(opcodes[i].port * PINS_PER_PORT + opcodes[i].pin, OUTPUT); //assumes chip port 0 for these

  strcpy(tmp, "LEDS-LD");
  i = opLookUp(tmp);
  mcp[opcodes[i].chip].pinMode(opcodes[i].port * PINS_PER_PORT + opcodes[i].pin, OUTPUT); //assumes chip port 0 for these

  strcpy(tmp, "SWITCHES-RD");
  i = opLookUp(tmp);
  mcp[opcodes[i].chip].pinMode(opcodes[i].port * PINS_PER_PORT + opcodes[i].pin, OUTPUT); //assumes chip port 0 for these
  mcp[opcodes[i].chip].digitalWrite(opcodes[i].port * PINS_PER_PORT + opcodes[i].pin, 1); // turn off switch bus driver
}

unsigned int read_switches() {
  int j;
  unsigned int i;
  char tmp[20];

  for (j = 8; j < 16; j++) {
    mcp[5].pinMode(j, INPUT);
  }

  mcp[5].digitalWrite(7, 0);
  //i = mcp[5].digitalRead(8);
  i=(mcp[5].readGPIOAB() & 0xff00)>>8;

#ifdef DEBUG
      sprintf(tmp, "sw %x" , i);
      Serial.println(tmp);
#endif

  mcp[5].digitalWrite(7, 1);
  return (i);
}

int display_leds(int i) {
  int j, mask;

  // no idea why switches need to be read first
  //mcp[5].digitalWrite(7, 0);
  //mcp[5].digitalWrite(7, 1);

  mask = 0x01;
  for (j = 8; j < 16; j++) {
    mcp[5].pinMode(j, OUTPUT);

    if (i & mask) {
      mcp[5].digitalWrite(j, HIGH);
    } else {
      mcp[5].digitalWrite(j, LOW);
    }
    mask = mask << 1;
  }

  mcp[5].digitalWrite(6, LOW);
  mcp[5].digitalWrite(6, HIGH);
  mcp[5].digitalWrite(6, LOW);
  return(i);
}

void doError(String errorMsg, int led) {
  Serial.print(F("Error: "));
  Serial.println(errorMsg);
  while (1)
    blinkLed(led);
}

int setCntlPin(int codeIndex, boolean state) {
  int chip, pin;

#ifdef DEBUG
  Serial.println(("SetCntlPin: ") + String(opcodes[codeIndex].code) + " " + String(state));
#endif

  chip = opcodes[codeIndex].chip;
  pin = opcodes[codeIndex].port * PINS_PER_PORT + opcodes[codeIndex].pin;
  if ((opcodes[codeIndex].code[0] == '-') && (chip < NUMBER_OF_CONTROLLERS)) {
    mcp[chip].digitalWrite(pin, !state);
  }
  else {
    mcp[chip].digitalWrite(pin, state);
  }
}


/*
   Return index of opcode in opcodes array or -1 if not found
*/
int opLookUp(char *codeToLookup) {
  int i;

  i = 0;

#ifdef DEBUG
  Serial.print("looking for ");
  Serial.println(codeToLookup);
#endif

  while (strlen(opcodes[i].code) != 0 ) {
    if (!strcmp(codeToLookup, opcodes[i].code)) {
#ifdef DEBUG
      Serial.println("found");
#endif
      return (i);
    }
    i++;
  }
  doError(F("Bad opcode"), BAD_OPCODE);
}


void setup() {
  int i, j, chip, pin;
  char opcode[20];
  char tmp[30];

  Serial.begin(19200);

#ifdef DEBUG
  Serial.println("Setup Start");
#endif

  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {

    mcp[i].begin(i);      // use default address 0 --- SHOULD ADDR & DATA be setup as input?

    for (j = 0; j < 16; j++) {
#ifdef DEBUG
      sprintf(tmp, "chip:%d pin:%d", i, j);
      Serial.println(tmp);
#endif
      mcp[i].pinMode(j, INPUT); // changed start by making inputs not outputs
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
      //mcp[1].digitalWrite(j, LOW); // not needed
    }
  }

  // ob board switch S-A and S-B
  pinMode(14, INPUT); // changed start by making inputs not outputs
  pinMode(15, INPUT); // changed start by making inputs not outputsp


#ifdef DEBUG
  Serial.println("Setup all pins set to input");
#endif

  i = 0;
  while (strlen(opcodes[i].code) > 0) {
    chip = opcodes[i].chip;
    pin = opcodes[i].port * PINS_PER_PORT + opcodes[i].pin;

#ifdef DEBUG
    Serial.print(opcodes[i].code);
    Serial.print(" ");
    Serial.print(opcodes[i].code[0]);
    if (opcodes[i].code[0] == '-')
      Serial.print(F(" Active LOW "));
    else
      Serial.print(F(" Active HIGH "));
    Serial.print(chip);
    Serial.print(" ");
    Serial.println(pin);
#endif

    if (chip < NUMBER_OF_CONTROLLERS) {
      mcp[chip].pinMode(pin, OUTPUT);
      mcp[chip].pullUp(pin, HIGH);  // turn on a 100K pullup internally
      if (opcodes[i].code[0] == '-') {

        mcp[chip].digitalWrite(pin, HIGH);
      }
      else {
        mcp[chip].digitalWrite(pin, LOW);
      }

    }
    i++;
  }

  dataBusDir(BUS_WRITE);
  addrBusDir(BUS_WRITE);

  // set some bus signals to input since driven by other cards

  switch_led_setup();
  strcpy(opcode, "BR-COND");
  i = opLookUp(opcode);

  chip = opcodes[i].chip;
  pin = opcodes[i].port * PINS_PER_PORT + opcodes[i].pin;
  mcp[chip].pinMode(pin, INPUT);

  strcpy(opcode, "-INT");
  i = opLookUp(opcode);

  chip = opcodes[i].chip;
  pin = opcodes[i].port * PINS_PER_PORT + opcodes[i].pin;
  mcp[chip].pinMode(pin, INPUT);

#ifdef READ_INPUT
  strcpy(opcode, "IN");
  i = opLookUp(opcode);

  chip = opcodes[i].chip;
  pin = opcodes[i].port * PINS_PER_PORT + opcodes[i].pin;
  mcp[chip].pinMode(pin, INPUT);
#endif

  // Flash all onboard leds - 9,10,11,12,13 are direct arduino pins
  for (i = 9; i <= 13; i++) {
    pinMode(i, OUTPUT);
    flashLed(i);
  }

#ifdef DEBUG
  Serial.println(F("Setup Done"));
#endif
  digitalWrite(COMMAND_READY, HIGH);
  Serial.println(PROMPT);

}

int command_counter = 0;

void loop() {
  int opcodeIndex;
  char mc[25];
  char opcode[20];
  int val;


  if (commandReady) {
    digitalWrite(COMMAND_READY, LOW);
    display_leds(command_counter++);


#ifdef DEBUG
    Serial.print(F("Command: "));
    Serial.println(command);
#endif

    command.toCharArray(mc, 20);  // convert input of Type String to char array

    int i = command.indexOf(OPCODE_OPERAND_SPLIT); // find opcode/value

#ifdef DEBUG
    Serial.print(i);
#endif

    strncpy(opcode, mc, i);
    opcode[i] = 0;

    val = atoi(&mc[i + 1]);


#ifdef DEBUG
    Serial.print(F("Pin / Val: "));
    Serial.print(opcode);
    Serial.print(F(" / "));
    Serial.print(val);
    Serial.print(F("  code from lookup "));
#endif

    opcodeIndex = opLookUp(opcode);

#ifdef DEBUG
    Serial.println(opcodes[opcodeIndex].code);
#endif

    if (opcodes[opcodeIndex].chip != SPECIAL_OPCODE) {
      setCntlPin(opcodeIndex, val);
#ifdef DEBUG
      Serial.println(F("Complete"));
#endif
    }


    else {
      Serial.print(F("Data: "));
      if (strcmp(opcode, "RD-DATABUS") == 0)
        Serial.println(result + String(readData()));
      else if (strcmp(opcode, "RD-DATABUS-L") == 0)
        Serial.println(result + String(readDataLo()));
      else if (strcmp(opcode, "RD-DATABUS-H") == 0)
        Serial.println(result + String(readDataHi()));
      else if (strcmp(opcode, "RBR-COND") == 0)
        Serial.println(result + String(mcp[4].digitalRead(13)));
      else if (strcmp(opcode, "WR-DATABUS") == 0)
        Serial.println(result + String(writeData(val)));
      else if (strcmp(opcode, "RD-ADDRBUS") == 0)
        Serial.println(result +  String(readAddress()));
      else if (strcmp(opcode, "WR-ADDRBUS") == 0)
        Serial.println(result + String(writeAddress(val)));
      else if (strcmp(opcode, "DATABUS-RD-MODE") == 0)
        Serial.println(result + String(dataBusDir(BUS_READ)));
      else if (strcmp(opcode, "DATABUS-WR-MODE") == 0)
        Serial.println(result + String(dataBusDir(BUS_WRITE)));
      else if (strcmp(opcode, "ADDRBUS-RD-MODE") == 0)
        Serial.println(result + String(addrBusDir(BUS_READ)));
      else if (strcmp(opcode, "ADDRBUS-WR-MODE") == 0)
        Serial.println(result + String(addrBusDir(BUS_WRITE)));
      else if (strcmp(opcode, "RD-IN") == 0)
        Serial.println(result + String(rd_in()));
      else if (strcmp(opcode, "READ-SWITCHES") == 0)
        Serial.println(result + String(read_switches()));
      else if (strcmp(opcode, "SET-LEDS") == 0){
        Serial.println(result + display_leds(val));
      }
      else {
        doError(F("Bad Special Opcode"), BAD_OPCODE);
      }
      Serial.println(F("Complete"));
    }

    command = "";
    commandReady = false;
    Serial.println(PROMPT);
    digitalWrite(COMMAND_READY, HIGH);

  }

}

void serialEvent() {
  while (Serial.available()) {

    // get the new byte:

    char inChar = (char)Serial.read();

    // add it to the inputString:

    command += inChar;

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:

    if (inChar == '#') {
      commandReady = true;
    }
  }
}

//FIX - set direction if not previously set instead of waiting for command to set direction?
unsigned int readAddress() {
  if (addrBusMode == BUS_WRITE)
    doError(F("Addr Bus Mode is Write"), ADDR_BUS_MODE);
  return (mcp[ADDRESS_CHIP].readGPIOAB());
}

unsigned int writeAddress(unsigned int address) {
  if (addrBusMode == BUS_READ)
    doError(F("Addr Bus Mode is READ"), ADDR_BUS_MODE);
  mcp[ADDRESS_CHIP].writeGPIOAB(address);
  return (address);
}

unsigned int readData() {
  if (dataBusMode == BUS_WRITE)
    doError(F("Data Bus Mode is Write"), DATA_BUS_MODE);
  return (mcp[DATA_CHIP].readGPIOAB());
}

unsigned int writeData(unsigned int data) {
  if (dataBusMode == BUS_READ)
    doError(F("Data Bus Mode is Read"), DATA_BUS_MODE);
  mcp[DATA_CHIP].writeGPIOAB(data);
  return (data);
}

unsigned int readDataLo() {

  if (dataBusMode == BUS_WRITE)
    doError(F("data Bus Mode is Write"), DATA_BUS_MODE);
  return (mcp[DATA_CHIP].readGPIO(0));
}

int writeDataLo() {
  doError(F("Write Data Low Dows Not Exist"), DATA_BUS_MODE);
}

unsigned int readDataHi() {
  if (dataBusMode == BUS_WRITE)
    doError(F("Bus Mode is Write"), DATA_BUS_MODE);
  return (mcp[DATA_CHIP].readGPIO(1));
}

/*
   Set the Data bus direction
*/
int dataBusDir(int state) { // 1=wr 0 = rd
  int i;

  if (state == BUS_WRITE) {
    for (i = 0; i < 16; i++)
      mcp[DATA_CHIP].pinMode(i, OUTPUT);
  }
  else {
    for (i = 0; i < 16; i++)
      mcp[DATA_CHIP].pinMode(i, INPUT);
  }
  dataBusMode = state;
  setLed(DATA_BUS_MODE, state);
  return (state);
}

/*
   Set the Address bus direction
*/
int addrBusDir(int state) { // 1=wr 0 = rd
  int i;

  if (state == BUS_WRITE) {
    for (i = 0; i < 16; i++)
      mcp[ADDRESS_CHIP].pinMode(i, OUTPUT);

  }
  else {
    for (i = 0; i < 16; i++)
      mcp[ADDRESS_CHIP].pinMode(i, INPUT);
  }
  addrBusMode = state;
  setLed(ADDR_BUS_MODE, state);
  return (state);
}

//read bus input line
int rd_in()
{
  char opcode[20];
  int chip, pin, i;

  strcpy(opcode, "IN");
  i = opLookUp(opcode);

  chip = opcodes[i].chip;
  pin = opcodes[i].port * PINS_PER_PORT + opcodes[i].pin;
  return (mcp[chip].digitalRead(pin));

}
