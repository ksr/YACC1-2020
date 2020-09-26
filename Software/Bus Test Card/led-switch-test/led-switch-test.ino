
/*
   Switch & LED test

   Flash each indicator LED
   then
   Continuous loop
    read IN-SWTICH and DISPLAY on OUT LED
    reads 8 data switches
    adds 1
    displays on 8 data leds
*/

#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <avr/pgmspace.h>
#include "YACC_Common_header.h"

#define OUT_LED 4     // Single OUT LED - Schematic signal OUT-LED
#define IN_SWITCH 5   // Input Switch - Schematic signal IN-SWITCH
#define LEDS_LD 6     // 8 LEDs latch - Schmatic signal LEDS-LD  (LD=Load)
#define SWITCHES_RD 7  // 8 Switches READ - Schmatic signal SWITCHES-RD

//#define DEBUG

Adafruit_MCP23017 mcp[NUMBER_OF_CONTROLLERS];

// Utilities

/*
  Flash an LED
*/
void flash(int led) {
  digitalWrite(led, HIGH);
  delay(100); //milliseconds
  digitalWrite(led, LOW);
}

void switch_led_setup() {
  mcp[5].pinMode(OUT_LED, OUTPUT); // Single OUT LED - Schematic signal OUT-LED
  mcp[5].pinMode(IN_SWITCH, INPUT);  // Input Switch - Schematic signal IN-SWITCH
  mcp[5].pinMode(LEDS_LD, OUTPUT); // 8 LEDs latch - Schmatic signal LEDS-LD  (LD=Load)
  mcp[5].pinMode(SWITCHES_RD, OUTPUT); // 8 Switches READ - Schmatic signal SWITCHES-RD
}

int read_switches() {
  int i, j;

  // SET BIT0-BIT7 to input
  for (j = 8; j < 16; j++) {
    mcp[5].pinMode(j, INPUT);
    mcp[5].pullUp(j, HIGH);  // turn on a 100K pullup internally
  }

  mcp[5].digitalWrite(SWITCHES_RD, 0);  //enable switch buffer
  i = mcp[5].readGPIO(1);               //read switches (all 8 bits)
  mcp[5].digitalWrite(7, 1);            //disable switch buffer
  return (i);
}

void display_led(int i) {
  int j, mask;
  mask = 0x01;

  // SET BIT0-BIT7 to OUTPUT
  for (j = 8; j < 16; j++) {
    mcp[5].pinMode(j, OUTPUT);

    // SET BIT(i)
    if (i & mask) {
      mcp[5].digitalWrite(j, true);
    } else {
      mcp[5].digitalWrite(j, false);
    }
    mask = mask << 1;
  }
  mcp[5].digitalWrite(LEDS_LD, 1); // latch switch DATA in LED Latch
  mcp[5].digitalWrite(LEDS_LD, 0);
}

void setup() {
  int i, j;

  Serial.begin(19200);
  Serial.println("Setup Start");

  //Set all controllers to output, turn on internal pullups and set to low
  for (i = 0; i < NUMBER_OF_CONTROLLERS; i++) {
    mcp[i].begin(i);
    for (j = 0; j < 16; j++) {
      mcp[i].pinMode(j, OUTPUT);
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
      mcp[1].digitalWrite(j, LOW);
    }
  }

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

    mcp[5].digitalWrite(OUT_LED, mcp[5].digitalRead(IN_SWITCH)); // Read IN-SWITCH and Display on OUT-LED

    i = read_switches(); // RED 8 SWITCHES

    i = i + 1; // ADD 1 to value read in

    display_led (i); // Displey on 8 LEDs

  }
}
