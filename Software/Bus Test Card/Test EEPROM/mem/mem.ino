/*
    Test exteranl on board EEPROM connected to AT328 chip 

    Based on:
    
    Use the I2C bus with EEPROM 24LC64
    Sketch:    eeprom.pde

    Author: hkhijhe
    Date: 01/10/2010


*/

#include <Wire.h> //I2C library

#define DEVICE_ADDRESS 0X57
int offset = 5000; //random location in EEPROM memory space


//Write a byte into EEPROM
void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(rdata);
  Wire.endTransmission();
}

//Read a byte from EEPROM
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
  char somedata[] = "this is some sample data - eeprom2123456789a"; // data to write
  int i;
  
  Wire.begin(); // initialise the connection
  Serial.begin(19200);
  Serial.println();
  Serial.println();
  Serial.println();
  
  Serial.println("Write into EEPROM the following string");
  Serial.println(somedata);
  
  for (i = 0; i < strlen(somedata) + 1; i++) { //+1 for ending null, write null into EEPROM
    i2c_eeprom_write_byte(DEVICE_ADDRESS, i + offset, somedata[i] ); // write to EEPROM
    delay(10); //milliseconds
  }
  delay(10); //milliseconds
  Serial.println("SETUP Complete - EEPROM Memory written");
  Serial.println();
  
}

void loop()
{
  int addr; //first address
  byte b;

  addr=offset;
  Serial.println("Read EEPROM");
  b = i2c_eeprom_read_byte(0x57, addr); // access the first address from the memory

  while ((b =  i2c_eeprom_read_byte(0x57, addr)) != 0)
  {
    Serial.print((char)b); //print content to serial port one byte at a time
    addr++; //increase address
  }
  Serial.println();
  Serial.println();
  while(1);
}
