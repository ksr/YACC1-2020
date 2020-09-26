/*
   
    Test EEPROM

    Based on:
    
    Use the I2C bus with EEPROM 24LC64
    Sketch:    eeprom.pde

    Author: hkhijhe
    Date: 01/10/2010


*/

#include <Wire.h> //I2C library

int offset=5000;

void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(rdata);
  Wire.endTransmission();
}


byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress, 1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}


void setup()
{
  char somedata[] = "this is data - eeprom2123456789a"; // data to write
  Wire.begin(); // initialise the connection
  Serial.begin(19200);
  offset = 5000; //random location in EEPROM memory space
  
  for (int i = 0; i < strlen(somedata) + 1; i++) { //+1 for ending null
    i2c_eeprom_write_byte(0x57, i + offset, somedata[i] ); // write to EEPROM
    Serial.println(somedata[i]);
    delay(10);
  }
  delay(10); //add a small delay

  Serial.println("Memory written");
}

void loop()
{
  int addr; //first address
  byte b;

  addr=offset;
  b = i2c_eeprom_read_byte(0x57, addr); // access the first address from the memory

  while (b != 0)
  {
    Serial.print((char)b); //print content to serial port
    addr++; //increase address
    b = i2c_eeprom_read_byte(0x57, addr); //access an address from the memory
  }
  Serial.println(addr);
  delay(2000);
}
