
/*
   BUS Monitor

   Loops watching bus for changes.
   Works well when combined with single step mode on sequencer

   Usgae notes:
   Monitor via Arduino IDE Monitor program
   SET ARDUINO MONITOR TO "No Line Ending"

   0 is off
   1 is on

   ***** add logic to only monitor bus lines June 26/20

   this program adjusts for active low vs active high
*/

#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <avr/pgmspace.h>
#include "YACC_Common_header.h"

//#define DEBUG

Adafruit_MCP23017 mcp[NUMBER_OF_CONTROLLERS];

// Hold previous state of bus signal lines (16 bits)
unsigned int previous[NUMBER_OF_CONTROLLERS];

//  Hold cuurent state of bus signal lines (16 bits)
unsigned int current[NUMBER_OF_CONTROLLERS];

boolean first=true;

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

/*
   Given a chip and a pin number return the appropriate signal name string

*/
String  strOpcode(int chip, int pinab) {
  int i;
  int port;
  int pin;
  char tmp[100];

  /* Need to convert a 15 bit port bit pin number (pinab) in range 0-15 to port 0 or 1 and a a 8 bit pin
      Pins 0-7 are on port 0, pint 8-15 are on port 1
  */

  if (pinab >= PINS_PER_PORT) {
    port = 1;
    pin = pinab - PINS_PER_PORT;
  } else {
    port = 0;
    pin = pinab;
  }

  i = 0;

#ifdef DEBUG
  sprintf(tmp, "looking for %d %d %d %d\n", chip, port, pin);
  Serial.print(tmp);
#endif

  while (strlen(opcodes[i].code) != 0 ) {
    if ((opcodes[i].chip == chip) &&
        (opcodes[i].port == port) &&
        (opcodes[i].pin == pin))
      return (opcodes[i].code);
    i++;
  }
  return ("YIKES NO OPCODE FOUND");

}

void setup() {
  int i, j, chip, pin;

  Serial.begin(19200);
  Serial.println("Setup Start");

  /* Set all controllers to input, turn on internal pullups */
  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    mcp[i].begin(i);
    previous[i] = 0;

    for (j = 0; j < BITS_PER_CONTROLLER; j++) {
      mcp[i].pinMode(j, INPUT);
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
    }
  }

  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    previous[i] = mcp[i].readGPIOAB();
  }

  /* Flash all leds
    ??? Looks like 9,10,11,12,13 are Arduino ports
  */
  for (i = 9; i <= 13; i++) {
    pinMode(i, OUTPUT); /*??? Should 13 actually be i ??*/
    flashLed(i);
  }

  Serial.println("Setup Done");
}

void loop() {
  bool changed;
  int i, j;
  char tmp[100];
  unsigned int mask;

  changed = false;

  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    current[i] = mcp[i].readGPIOAB();
    if (current[i] != previous[i]) {
      changed = true;
    }
  }

  if (changed || first) {
    /* print value on address & data bus*/
    sprintf(tmp, "Address=%04x Data Hi=%02x Lo=%02x ",
            current[ADDRESS_CHIP], (current[DATA_CHIP] & 0xff00) >> 8 , current[DATA_CHIP] & 0x00ff);
    Serial.print(tmp);

    /* find changed bus signal within soecific controller */

    for (i = FIRST_BUS_SIGNAL_CONTROLLER; i < NUMBER_OF_CONTROLLERS; i++) {

      mask = 0x0001;
      for (j = 0; j < BITS_PER_CONTROLLER; j++) {
        if (((current[i] & mask) != (previous[i] & mask)) || first) {
          if (current[i] & mask) {
            sprintf(tmp, "(1)");
            Serial.print(tmp);
          }
          else {
            sprintf(tmp, "(0)");
            Serial.print(tmp);
          }
          Serial.print(strOpcode(i, j)); Serial.print(" ");
        }
        mask = mask << 1;
      }
    }
   first = false;
    Serial.println();
    for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
      previous[i] = current[i];
    }
  }

}
