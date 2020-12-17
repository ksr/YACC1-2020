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

void uCodeRamWritePulse() {
  digitalWrite(MEMWR, LOW); //pulse memwrite signal
  digitalWrite(MEMWR, HIGH);
}


unsigned int readInstruction(int instruction, unsigned char *data){
  int i, j;
  unsigned char *ptr;
  unsigned int address;

  address = instruction * LINES_PER_INSTRUCTION *2; // hack tmp *2
  //Serial.print("read instruction:"); Serial.print(instruction); Serial.print(" Address:"); Serial.println(address);

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

void writeInstruction(int instruction, unsigned char *data) {
  int i, j;
  unsigned int address;
  unsigned char *ptr;

  address = instruction * LINES_PER_INSTRUCTION *2 ; // tmp hack *2
  ptr = data;

  uCodeRamSelect(true);

  for (j = 0; j < LINES_PER_INSTRUCTION; j++) { //
    writeAddress(j + address);
    //Serial.print("Address set line:"); Serial.println(j + address);
    writeData(ptr);
    ptr += BYTES_PER_LINE;
    uCodeRamWritePulse();
  }
  uCodeRamSelect(false);
}
