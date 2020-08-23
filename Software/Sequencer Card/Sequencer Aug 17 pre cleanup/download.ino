/*
   Format "%AIXXXXXXXX....XXXXX-"
   "%"  Start charachter
   "A" Checksum (not including instruction number) (Ignore for now) - 2 hex assci chars
   "I" Instruction Number (0-255)- 2 hex ascii chars
   "XXXXXXXX....XXXXX" Data Bytes -  2 hex ascii chars
   "-" End Instruction data character
   "!" END ALL

*/

#define PROMPT ">>"
#define START_CHARACTER '%'
#define END_CHARACTER '!'

void sendReadyPrompt() {
  Serial.println(PROMPT);
}

void downloadInstruction() {
  unsigned char instructionBytes[INSTRUCTION_SIZE];
  unsigned char currentChecksum;
  unsigned char currentInstruction;
  unsigned char instructionChecksum;
  unsigned char readIns[INSTRUCTION_SIZE];
  char tmp[100];

  for (int i = 0; i < INSTRUCTION_SIZE; i++)
    readIns[i] = 0xff;

  int instruction;
  currentChecksum = getChecksum();
  currentInstruction = getInstructionNumber();
  instructionChecksum = getCode(instructionBytes);

  /*Serial.print("checksum:");
    Serial.print( currentChecksum);
    Serial.print(" ");
    Serial.print(currentInstruction);
    Serial.print(" ");
    for (int i = 0; i < INSTRUCTION_SIZE; i++) {
    Serial.print(instructionBytes[i]);
    Serial.print(":");
    }
    Serial.print(instructionChecksum);
    Serial.println();
  */
  writeCodeToROM(currentInstruction, instructionBytes);
  readCodeFromROM(currentInstruction, readIns); //?? why
  ///sprintf(tmp, "INS=%d Check=%02x ", currentInstruction, instructionChecksum);
  ///Serial.println(tmp);
  for (int i = 0; i < INSTRUCTION_SIZE; i = i + 8) {
    ///Serial.print(i / 8); Serial.print('-');
    for (int j = 0; j < 8; j++) {
      ///sprintf(tmp, "%02x:", readIns[i + j]);
      ///Serial.print(tmp);
    }
    ///Serial.println();
  }
  ///Serial.println();
}

boolean waitInstructionBegin() {
  unsigned char c;

  sendReadyPrompt();
  flash(READY);
  while (Serial.available() < 1)
    delay(1);
  c = Serial.read();

  if (c == START_CHARACTER)
    return (true);
  else if (c == END_CHARACTER)
    return (false);
  else
    doError("Unexpected Character",5);
}

unsigned char getChecksum() {
  return (readHexNumber());
}

unsigned char getInstructionNumber() {
  return (readHexNumber());
}

unsigned char getCode(unsigned char *data) {
  int i, j;
  int tmp;
  unsigned char checksum;

  checksum = 0;
  for (i = 0; i < INSTRUCTION_SIZE; i++) {

    tmp = readHexNumber();
    if (tmp != 256) {
      data[i] = tmp;
      checksum += data[i];
    }
    else {
      for (j = i; j < INSTRUCTION_SIZE; j++) {
        data[j] = 0;
        checksum += 0;
      }
      i=INSTRUCTION_SIZE;
    }
  }
  return (checksum);
}

unsigned int readHexNumber() {
  unsigned char cHigh, cLow;
  while (Serial.available() < 1)
    delay(1);
  cHigh = Serial.read();
  if (cHigh == 'Z') {
    return (256);
  }
  while (Serial.available() < 1)
    delay(1);
  cLow = Serial.read();
  return (convertAsciiToNum(cHigh, cLow));
}

#ifdef ORIG
unsigned char getCode(unsigned char *data) {
  int i;
  unsigned char checksum;

  checksum = 0;
  for (i = 0; i < INSTRUCTION_SIZE; i++) {

    data[i] = readHexNumber();
    checksum += data[i];
  }
  return (checksum);
}

unsigned char readHexNumber() {
  unsigned char cHigh, cLow;
  while (Serial.available() < 1)
    delay(1);
  cHigh = Serial.read();
  while (Serial.available() < 1)
    delay(1);
  cLow = Serial.read();
  return (convertAsciiToNum(cHigh, cLow));
}
#endif

unsigned char convertAsciiToNum(char cHigh, char cLow) {
  unsigned char res;

  /*
    Serial.println("convertAsciitoNum hi lo res");
    Serial.print(cHigh); Serial.print(" ");
    Serial.print(cLow); Serial.print(" ");
    Serial.print(converCharToInt(cHigh)); Serial.print(" ");
    Serial.print(converCharToInt(cLow)); Serial.print(" ");
  */
  res = converCharToInt(cLow) + (converCharToInt(cHigh) << 4);
  //Serial.println(res);
  return (res);

}

unsigned char converCharToInt(char c) {

  //c=toupper(c);
  if (c >= '0' && c <= '9')
    return (byte)(c - '0');
  else
    return (byte)(c - 'A' + 10);
}
