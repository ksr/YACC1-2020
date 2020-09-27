
/*
   BUS Test

   Test bus for shorts
*/

#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <avr/pgmspace.h>
#include "YACC_Common_header.h"

//#define DEBUG

Adafruit_MCP23017 mcp[NUMBER_OF_CONTROLLERS];

// Utilities

/*
  Flash an LED
*/
void flash(int led) {
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
}

/*
   Test if the controller/pin combo is connected to the bus or an internal control pin
   Not all MCP23017 lines drive bus, the "last controller" has 12 bins internal to the
   test board
*/
bool is_busline(int controller, int pin) {

  if ( ((controller  * 16) + pin)  < 84) {
    return (true);
  }
  else {
    return (false);
  }
}

/*
   Show status of all bus lines
*/
void show_all_lines() {
  int i, j;
  char tmp[100];

  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    sprintf(tmp, "Controller %d - ", i);
    Serial.print(tmp);
    for (j = 0; j < BITS_PER_CONTROLLER; j++) {
      if ( is_busline(i, j)) {
        sprintf(tmp, "%d ", mcp[i].digitalRead(j));
        Serial.print(tmp);
      }
    }
    sprintf(tmp, "\n");
    Serial.print(tmp);
  }
  sprintf(tmp, "\n\n");
  Serial.print(tmp);
}

bool test_bus_for_low(int controller, int testbit) {
  int i, j;
  unsigned int in;
  char tmp[100];

  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    for (j = 0; j < BITS_PER_CONTROLLER; j++) {
      if (is_busline(i, j)) {

        if ((i != controller) || ( j != testbit)) { //dont test the bus line that was set low

          in = mcp[i].digitalRead(j);

          if (in == 0) { // if line is low there is a short
            sprintf(tmp, "Short between %d %d and %d %d\n", controller, testbit, i, j);
            Serial.print(tmp);
            return (true);
          }
        }
      }

    }
  }
  return (false);
}


void setup() {
  int i, j;

  Serial.begin(19200);
  Serial.println("Setup Start");

  //Set all controllers to input, turn on internal pullups
  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    mcp[i].begin(i);
    for (j = 0; j < 16; j++) {
      mcp[i].pinMode(j, INPUT);
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
    }
  }

  show_all_lines();

  // Flash all onboard leds - 9,10,11,12,13 are direct arduino pins
  for (i = 9; i <= 13; i++) {
    pinMode(i, OUTPUT); /*??? Should 13 actually be i ??*/
    flash(i);
  }
  Serial.println("Setup Done\n\n");
}



void loop() {
  int i, j;
  unsigned int mask;
  char tmp[100];


#ifdef DEBUG
  show_all_lines();
#endif

  /*
     cycle through all bus lines setting a single line to LOW
     then testing if any other lines are low
  */

  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    for (j = 0; j < BITS_PER_CONTROLLER; j++) {


      if (is_busline(i, j)) {
        mcp[i].pinMode(j, OUTPUT);
        mcp[i].digitalWrite(j, LOW);

        if (test_bus_for_low(i, j)) {
          sprintf(tmp, "SHORT FOUND %d %d\n", i, j);
          Serial.print(tmp);
          show_all_lines();
          delay(2000);
          //exit(0);
        }
        mcp[i].pinMode(j, INPUT);
      }
    }
  }

  sprintf(tmp, "Bus test completed - no shorts found\n");
  Serial.print(tmp);
  delay(2000);
  exit(0);
}
