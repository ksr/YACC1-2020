
/*
   Routines to read/write data to eeprom
*/

/*
   Should deviceaddress be an unsigned int for both routines ???
*/
void i2c_eeprom_write_byte( int deviceaddress, int eeaddress, byte data ) {
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(rdata);
  int res=Wire.endTransmission();
  //Serial.println(res);
  delay(5);
}


/*
   TODO maybe modify to accept 3rd param - pointer to return value location
   AND return 0 if succesful and -1 if fails
*/

byte i2c_eeprom_read_byte( int deviceaddress, int eeaddress ) {
  byte rdata = 0xFF; // Default value;

  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  int res=Wire.endTransmission();
  //Serial.println(res);
  Wire.requestFrom(deviceaddress, 1);
  if (Wire.available()) {
    rdata = Wire.read();
  }
  return rdata;
}

