
#define CLOCK 13
#define UCODE_CLOCK 12
#define PULSE_SWITCH 11
#define INSTRUCTION_SWITCH 10
#define TEN_INSTRUCTIONS 9

#define CLOCK_TIME 10

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(CLOCK, OUTPUT);
  pinMode(PULSE_SWITCH, INPUT_PULLUP);
  pinMode(UCODE_CLOCK, INPUT);

  pinMode(INSTRUCTION_SWITCH, INPUT_PULLUP); 
  pinMode(TEN_INSTRUCTIONS, INPUT_PULLUP);
}

void loop() {
  if (!digitalRead(INSTRUCTION_SWITCH)) //no need to debounce
    oneInstruction();

  if (!digitalRead(TEN_INSTRUCTIONS)) //no need to debounce
    for (int i = 0; i < 10; i++)
      oneInstruction();

  if (!digitalRead(PULSE_SWITCH)) {
    lastDebounceTime = millis();
    while ((millis() - lastDebounceTime) < debounceDelay);
    digitalWrite(CLOCK, HIGH);
    delay(CLOCK_TIME);
    digitalWrite(CLOCK, LOW);
    delay(CLOCK_TIME);
  }
}

void oneInstruction() {
  while (digitalRead(UCODE_CLOCK) == 0)
    pulseClock(1);
  pulseClock(1);
}

void pulseClock(int i) {
  for (int count = 0; count < i; count++) {
    digitalWrite(CLOCK, HIGH);
    delay(CLOCK_TIME);
    digitalWrite(CLOCK, LOW);
  }
}
