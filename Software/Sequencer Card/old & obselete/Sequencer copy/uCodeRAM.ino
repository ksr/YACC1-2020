/*
 * manage uCode RAM
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

void uCodeRamWritePulse() {
  digitalWrite(MEMWR, LOW); //pulse memwrite signal
  digitalWrite(MEMWR, HIGH);
}


unsigned int readInstruction(int instruction, unsigned char data[]) {
  int i, j;
  unsigned char *ptr;
  unsigned int address;

  address = instruction * LINES_PER_INSTRUCTION;
  ptr = data;

  uCodeRamSelect(true);
  uCodeRamRead(true);

  for (j = 0; j < LINES_PER_INSTRUCTION; j++) {
    writeAddress(j + address);
    ///test test test removal of this code
      /*for (i = 0; i < BYTES_PER_LINE / 2; i++) {
      *ptr++ = mcp[i + DATA_CHIP_START].readGPIO(0);//read low byte, +1 IO Chips Data starts at chip 1
      *ptr++ = mcp[i + DATA_CHIP_START].readGPIO(1);//read hi byte
      }*/
    readData(ptr);
    ptr += BYTES_PER_LINE;
    
  }
  uCodeRamSelect(false);
  uCodeRamRead(false);

}

void writeInstruction(int instruction, unsigned char *data) {
  int i, j;
  unsigned int address;
  unsigned char *ptr;

  address = instruction * LINES_PER_INSTRUCTION;
  ptr = data;

  uCodeRamSelect(true);

  for (j = 0; j < LINES_PER_INSTRUCTION; j++) { //
    writeAddress(j + address);
    writeData(ptr);
    ptr += BYTES_PER_LINE;
    uCodeRamWritePulse();
  }
  uCodeRamSelect(false);
}

void blockFillInstructions( int instructions, unsigned char *data) {
  int i, j;
  unsigned int startAddress;
  unsigned char *ptr;
  unsigned int dlow, dhigh, dfull;

  startAddress = 0;
  ptr = data;

  for (j = 0; j < instructions; j++) {
    writeInstruction(j, ptr);
    ptr += INSTRUCTION_SIZE;
  }
}

void fillmem(unsigned char *data, int lines) {
  int i, j;
  unsigned int startAddress;
  unsigned char *ptr;
  unsigned int dlow, dhigh, dfull;

  startAddress = 0;
  ptr = data;

  uCodeRamSelect(true);

  for (j = 0; j < lines; j++) { //
    writeAddress(j + startAddress);

    // Write 2 bytes at a time
    for (i = 0; i < BYTES_PER_LINE / 2; i++) {
      dlow = *ptr;
      ptr++;
      dhigh = *ptr << 8;
      ptr++;
      dfull = dhigh | dlow;
      //sprintf(tmp, "%04x ", dfull); Serial.println(tmp);
      //Serial.println(dfull);
      mcp[i + 1].writeGPIOAB(dfull);
    }
    uCodeRamWritePulse();
  }

  uCodeRamSelect(false);

}


