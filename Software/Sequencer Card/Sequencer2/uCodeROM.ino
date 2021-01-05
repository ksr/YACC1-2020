/*
   change to ptr in write code
*/

//void eep.write(unsigned long,unsigned char);
//unsigned char eep.read(unsigned long);

#define DEBUG 1

void writeCodeToROM(int instruction, unsigned char data[INSTRUCTION_SIZE]) {
  unsigned long location;

#ifdef DEBUG
  char tmp[15];
  Serial.println(instruction);
#endif

  for (unsigned long i = 0; i < INSTRUCTION_SIZE; i++) {
    location = instruction * (unsigned long)INSTRUCTION_SIZE + i;

#ifdef DEBUG
    Serial.println(location);
    eep.write((unsigned long)0x8001, 0x44); delay(50);
    sprintf(tmp, "R1=%02x", eep.read((unsigned long)0x8001));
    Serial.println(tmp);
    //myi2c_eeprom_write_byte(0x57, 0x55, 0x08000 );
    delay(50);
    sprintf(tmp, "R2=%02x", eep.read((unsigned long)0x8001));
    Serial.println(tmp);
    myi2c_eeprom_write_byte(0x57, i , location ); // write to EEPROM
    delay(50);
#endif

    myi2c_eeprom_write_byte(0x57, data[i], location ); // write to EEPROM
    delay(10);
  }
  delay(10);
}

void readCodeFromROM(int instruction, unsigned char *data) {
  byte b;
  unsigned long location;

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
