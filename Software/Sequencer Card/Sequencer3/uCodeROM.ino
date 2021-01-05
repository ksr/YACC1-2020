/*
   change to ptr in write code
*/

//#define DEBUG

void writeCodeToROM(int instruction, unsigned char data[INSTRUCTION_SIZE]) {
  unsigned long location;
  int device;

#ifdef DEBUG
  Serial.println(instruction);
#endif

  for (unsigned long i = 0; i < (unsigned long)INSTRUCTION_SIZE; i++) {
    location = (unsigned long)instruction * (unsigned long)INSTRUCTION_SIZE + i;

#ifdef DEBUG
    //Serial.println(location);
#endif
     if (location > (unsigned long)0x0000ffff){
      device = 0x57;
      location=location-(unsigned long)0x00010000;
    }else{
      device = 0x56;
    }
    i2c_eeprom_write_byte(device, location, data[i] ); // write to EEPROM
    delay(5);
  }
  delay(5);
}

void readCodeFromROM(int instruction, unsigned char *data) {
  byte b;
  unsigned long location;
  int device;

#ifdef DEBUG
  Serial.print(instruction);
  Serial.print(" ");
#endif

  for (unsigned long i = 0; i < (unsigned long)INSTRUCTION_SIZE; i++) {
    location = (unsigned long)instruction * (unsigned long)INSTRUCTION_SIZE + i;

#ifdef DEBUG
    Serial.print(location);
    Serial.print(":");
#endif
    if (location > (unsigned long)0x0000ffff){
      device = 0x57;
      location=location-(unsigned long)0x00010000;
    }else{
      device = 0x56;
    }
    b = i2c_eeprom_read_byte(device, location); //access an address from the memory
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
