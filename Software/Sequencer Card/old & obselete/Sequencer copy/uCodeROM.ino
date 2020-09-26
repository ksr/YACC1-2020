/*
 * change to ptr in write code
 */

void writeCodeToROM(int instruction, unsigned char data[INSTRUCTION_SIZE]) { 
  int location;

  //Serial.println(instruction);
  for (int i = 0; i < INSTRUCTION_SIZE; i++) {
    location = instruction * INSTRUCTION_SIZE + i;
    //Serial.println(location);
    i2c_eeprom_write_byte(0x57, location, data[i] ); // write to EEPROM
    delay(10);
  }
  delay(10);
}

void readCodeFromROM(int instruction, unsigned char *data) {
  byte b;
  int location;

  //Serial.print(instruction);
  //Serial.print(" ");
  for (int i = 0; i < INSTRUCTION_SIZE; i++) {
    location = instruction * INSTRUCTION_SIZE + i;
    //Serial.print(location);
    //Serial.print(":");
    b = i2c_eeprom_read_byte(0x57, location); //access an address from the memory
    data[i] = b;
    //Serial.print(b);
    //Serial.print(" ");
  }
  //Serial.println();
}

