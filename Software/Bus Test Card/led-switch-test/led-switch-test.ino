
/*
   BUS Monitor

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
  delay(250);
  digitalWrite(led, LOW);
  delay(250);
}

/*
   Test if the controller/pin combo is connected to the bus or an internal control pin
*/
bool is_busline(int controller, int pin) {
  char tmp[100];

  if ( ((controller  * 16) + pin)  < 84) {

#ifdef DEBUG
    sprintf(tmp, "is busline yes %d %d\n", controller, pin);
    Serial.print(tmp);
#endif

    return (true);
  }
  else {
#ifdef DEBUG1
    sprintf(tmp, "is busline no %d %d\n", controller, pin);
    Serial.print(tmp);
#endif
    return (false);
  }
}

/*
   Show status of all bus lines
*/
void show_all_lines() {
  int i, j;
  char tmp[100];

#ifdef DEBUG
  sprintf(tmp, "\nShow all lines --\n\n");
  Serial.print(tmp);
#endif

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

#ifdef DEBUG
  sprintf(tmp, "\ntest hi start %d %d\n\n", controller, testbit);
  Serial.print(tmp);
#endif

  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    for (j = 0; j < BITS_PER_CONTROLLER; j++) {
      if (is_busline(i, j)) {

#ifdef DEBUG
        sprintf(tmp, "test hi %d %d\n", i, j);
        Serial.print(tmp);
#endif

        if ((i != controller) || ( j != testbit)) { //dont test the bus line that was set low

#ifdef DEBUG
          sprintf(tmp, " do test " );
          Serial.print(tmp);
#endif

          in = mcp[i].digitalRead(j);

#ifdef DEBUG
          sprintf(tmp, "%d\n", in);
          Serial.print(tmp);
#endif

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

void switch_led_setup(){
  mcp[5].pinMode(4, OUTPUT);
  mcp[5].pinMode(5, INPUT);
  mcp[5].pinMode(6, OUTPUT);
  mcp[5].pinMode(7, OUTPUT);
}

int read_switches() {
  int i, j;

  for (j = 8; j < 16; j++) {
    mcp[5].pinMode(j, INPUT);
    mcp[5].pullUp(j, HIGH);  // turn on a 100K pullup internally
  }

  mcp[5].digitalWrite(7, 0);
  i = mcp[5].readGPIO(1);
  mcp[5].digitalWrite(7, 1);
  return (i);
}

void display_led(int i) {
  int j, mask;
  mask = 0x01;
  for (j = 8; j < 16; j++) {
    mcp[5].pinMode(j, OUTPUT);

    if (i & mask) {
      mcp[5].digitalWrite(j, true);
    } else {
      mcp[5].digitalWrite(j, false);
    }
    mask = mask << 1;
  }
  mcp[5].digitalWrite(6, 1);
  mcp[5].digitalWrite(6, 0);
}

void setup() {
  int i, j;

  Serial.begin(19200);
  Serial.println("Setup Start");

  //Set all controllers to input, turn on internal pullups
  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    mcp[i].begin(i);
    for (j = 0; j < 16; j++) {
      mcp[i].pinMode(j, OUTPUT);
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
      mcp[1].digitalWrite(j, LOW);
      //mcp[i].pinMode(j, INPUT);
      //mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
    }
  }
  show_all_lines();
  // Flash all onboard leds - 9,10,11,12,13 are direct arduino pins
  for (i = 9; i <= 13; i++) {
    pinMode(i, OUTPUT); /*??? Should 13 actually be i ??*/
    flash(i);
  }

  switch_led_setup();

  Serial.println("Setup Done\n\n");
}


void loop() {
  int i, j;
  unsigned int mask;
  char tmp[100];

  while (1) {

    mcp[5].digitalWrite(4, mcp[5].digitalRead(5));

    i = read_switches();

    i = i + 1;


    display_led (i);

  }


#ifdef DEBUG
  show_all_lines();
#endif


#ifdef DEBUG
  sprintf(tmp, "Hi Test %d %d\n", i, j);
  Serial.print(tmp);
#endif

  if (is_busline(i, j)) {
    mcp[i].pinMode(j, OUTPUT);
    mcp[i].digitalWrite(j, LOW);
#ifdef DEBUG
    sprintf(tmp, "set low %d %d\n", i, j);
    Serial.print(tmp);
    show_all_lines();
#endif
    if (test_bus_for_low(i, j)) {
      sprintf(tmp, "SHORT FOUND %d %d\n", i, j);
      Serial.print(tmp);
      show_all_lines();
      delay(2000);
      exit(0);
    }
    mcp[i].pinMode(j, INPUT);
  }

  sprintf(tmp, "Bus test completed - no shorts found\n");
  Serial.print(tmp);
  delay(2000);
  exit(0);
}
