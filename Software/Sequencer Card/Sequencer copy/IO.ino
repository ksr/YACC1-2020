/*
   manage IO control ports

*/


#define INDIR 0
#define OUTDIR 1

int addrDir = -1;
int dataDir = -1;

void setDataOutput() {
  int i, j;

  dataDir = OUTDIR;

  for (i = 1; i < NUMBER_OF_CONTROLERS; i++) {
    for (j = 0; j < 16; j++) {
      mcp[i].pinMode(j, OUTPUT);
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
      mcp[1].digitalWrite(j, LOW);
    }
  }
}

void setAddressOutput() {
  int j;

  addrDir = OUTDIR;

  for (j = 0; j < 16; j++) {
    mcp[ADDRESS_CHIP].pinMode(j, OUTPUT);
    mcp[ADDRESS_CHIP].pullUp(j, HIGH);  // turn on a 100K pullup internally
    mcp[ADDRESS_CHIP].digitalWrite(j, LOW);
  }
}

void setDataInput() {
  int i, j;

  dataDir = INDIR;

  for (i = 1; i < NUMBER_OF_CONTROLERS; i++) {
    for (j = 0; j < 16; j++) {
      mcp[i].pinMode(j, INPUT);
      mcp[i].pullUp(j, HIGH);  // turn on a 100K pullup internally
    }
  }
}

void setAddressInput() {
  int j;

  addrDir = INDIR;

  for (j = 0; j < 16; j++) {
    mcp[ADDRESS_CHIP].pinMode(j, INPUT);
    mcp[ADDRESS_CHIP].pullUp(j, HIGH);  // turn on a 100K pullup internally
  }

}


//FIX TEST TEST TEST use this call in read instruction
void readData(unsigned char *ptr){
  int i;
  
  for (i = 0; i < DATA_CHIPS; i++) {
    *ptr++ = mcp[i + DATA_CHIP_START].readGPIO(0);//read low byte
    *ptr++ = mcp[i + DATA_CHIP_START].readGPIO(1);//read hi byte
  }
}

unsigned int writeData(unsigned char data[]) {
  int i;
  unsigned char *ptr;
  unsigned int dlow, dhigh, dfull;

  ptr = data;

  if (dataDir != OUTDIR)
    doError("IO: Writing Data with data direction set to in");

  //  for (i = 0; i < 4; i++) {
  //    mcp[i + 1].writeGPIOAB(data[i * 2]);
  //    mcp[i + 1].writeGPIOAB(data[i * 2 + 1]);

  for (i = 0; i < DATA_CHIPS; i++) {
    dlow = *ptr;
    ptr++;
    dhigh = *ptr << 8;
    ptr++;
    dfull = dhigh | dlow;
    mcp[i + DATA_CHIP_START].writeGPIOAB(dfull);
  }
}


unsigned int writeAddress(unsigned int address) {

  if (addrDir != OUTDIR)
    doError("IO: Writing address with addr direction set to in");

  mcp[ADDRESS_CHIP].writeGPIOAB(address);
  return (address);
}

unsigned int readAddress(){
  return(mcp[ADDRESS_CHIP].readGPIOAB());
 
}

