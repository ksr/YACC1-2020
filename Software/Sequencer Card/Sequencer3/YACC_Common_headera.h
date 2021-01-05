#define NUMBER_OF_CONTROLLERS 6
#define PINS_PER_PORT 8
#define BITS_PER_CONTROLLER 16
#define PORTS_PER_CONTROLLER 2
#define ACTIVE_LOW 8 /* add 8 to pin number to signify active low signal */
#define FIRST_BUS_SIGNAL_CONTROLLER 2 /*first controller with bus signal lines skip address and data lines*/
#define ADDRESS_CHIP 0
#define DATA_CHIP 1

#define SPECIAL_OPCODE 9

#ifdef NOTUSED
typedef struct {
  char const *code;
} opcode;

const opcode opcodesPROGMEM[]  PROGMEM = {

  "-REG-FUNC-RD",
  "-REG-FUNC-LD",

  "REG-RD-ID0",
  "REG-RD-ID1",
  "REG-RD-ID2",
  "REG-RD-ID3",

   "REG-LD-ID0",
   "REG-LD-ID1",
   "REG-LD-ID2",
   "REG-LD-ID3",

  "-REG-RD-LO",
  "-REG-LD-LO",
  "-REG-RD-HI",
  "-REG-LD-HI",

  "-REG-DN",
  "-REG-UP",

  "-MEM-RD",
  "-MEM-WR",
  "-IO-RD",
  "-IO-WR",

  "-TMP-REG-RD0",
  "-TMP-REG-LD0",
  "-TMP-REG-RD1",
  "-TMP-REG-LD1",

  "ADDR-REG-ID0",
  "ADDR-REG-ID1",
  "ADDR-REG-ID2",
  "ADDR-REG-ID3", 

  "IOADDR0",
  "IOADDR1",
  "IOADDR2",
  "IOADDR3",

  "-IO-ADDR-LD",
  "-VMA",

  "-INT",
  "-INTA",

  "-ALU-FUNC",
  "ALU0",
  "ALU1",
  "ALU2",
  "ALU3",
  "-AC-LD-INV",
  "-AC-RD",
  "-AC-LD",
  "-SR-LD",
  "BR-COND",
  "-HL-SWAP",
  "IN",
  "OUT",
  "-BUS-EN",
  "-RUN",
  "-RESET",
  "",
};
#endif 
