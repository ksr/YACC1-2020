/*
   change to ptr in write code
*/

//#define DEBUG

void writeCodeToROM(int instruction, unsigned char data[INSTRUCTION_SIZE]) {
  int location;

#ifdef DEBUG
  Serial.println(instruction);
#endif

  for (int i = 0; i < INSTRUCTION_SIZE; i++) {
    location = instruction * INSTRUCTION_SIZE + i;

#ifdef DEBUG
    //Serial.println(location);
#endif

    i2c_eeprom_write_byte(0x57, location, data[i] ); // write to EEPROM
    delay(10);
  }
  delay(10);
}

void readCodeFromROM(int instruction, unsigned char *data) {
  byte b;
  int location;

#ifdef DEBUG
  Serial.print(instruction);
  Serial.print(" ");
#endif
  
  for (int i = 0; i < INSTRUCTION_SIZE; i++) {
    location = instruction * INSTRUCTION_SIZE + i;

#ifdef DEBUG
    Serial.print(location);
    Serial.print(":");
#endif

    b = i2c_eeprom_read_byte(0x57, location); //access an address from the memory
    data[i] = b;

#ifdef DEBUG
    Serial.print(b);
    Serial.print(" ");
#endif
  }
#ifdef DEBUG
  Serial.println();
#endif
}
