  /*

    Test EEPROM

    Based on:

    Use the I2C bus with EEPROM 24LC64
    Sketch:    eeprom.pde

    Author: hkhijhe
    Date: 01/10/2010


*/

#include <Wire.h> //I2C library

int offset = 5000;

//#define WRITE_MEM
#define INSTRUCTIONS 8
#define LINES_PER_INSTRUCTION 32
#define BYTES_PER_LINE 8

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
  int i;
  char somedata[] = "this is data from the eeprom2123456789a11223344778899aaddd"; // data to write

  Wire.begin(); // initialise the connection
  Serial.begin(19200);
  offset = 5000;

#ifdef WRITE_MEM
  for (i = 0; i < strlen(somedata) + 1; i++) { //+1 for ending null
    i2c_eeprom_write_byte(0x57, i + offset, somedata[i] ); // write to EEPROM
    Serial.println(somedata[i]);
    delay(10);
  }
  i2c_eeprom_write_byte(0x57, i + offset, 0 );
  delay(10); //add a small delay
  Serial.println("Memory written");
#endif
}

void loop()
{
  int addr; //first address
  byte b;
  int instruction = 0, line = 0, byteinfo = 0;
  char tmp[40];



#ifdef WRITE_MEM
  addr = offset;
  b = i2c_eeprom_read_byte(0x57, addr); // access the first address from the memory
  while (b != 0)
  {
    Serial.print((char)b); //print content to serial port
    addr++; //increase address
    b = i2c_eeprom_read_byte(0x57, addr); //access an address from the memory
  }
  Serial.println(addr);
  delay(2000);
#else

  addr = 0;
  for (instruction = 0; instruction < INSTRUCTIONS; instruction++) {

    for (line = 0; line < LINES_PER_INSTRUCTION; line++) {
      Serial.println("");
      sprintf(tmp, "Instruction=[%02x] LINE[%d] ", instruction, line);
      Serial.print(tmp);

      for (byteinfo = 0; byteinfo < BYTES_PER_LINE; byteinfo++) {
        sprintf(tmp, "%02x ", i2c_eeprom_read_byte(0x57, addr));
        Serial.print(tmp);
        addr++;
      }
    }
  }
  while(1);
#endif
}
