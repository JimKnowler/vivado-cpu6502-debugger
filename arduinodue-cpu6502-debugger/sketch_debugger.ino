#include <SPI.h>

#include "nestest__prg_rom_bank_0.6502.bin.h"

int pin_spi_cs_n = 8;

enum class Command : byte {
  NOP = 0,
  ECHO = 1,
  MEM_WRITE = 2,
  MEM_READ = 3,
  VALUE_WRITE = 4,
  VALUE_READ = 5
};

///////////////////////////////////////////////////////////////////////////
// Utilities

byte hi(uint16_t value) {
  return (value >> 8) & 0xff;
}

byte lo(uint16_t value) {
  return value & 0xff;
}

///////////////////////////////////////////////////////////////////////////
// SPI commands

void nop() {
  SPI.transfer(byte(Command::NOP));
}

byte echo(byte value) {
  SPI.transfer(byte(Command::ECHO));
  SPI.transfer(value);
  byte returnValue = SPI.transfer(0);
  
  return returnValue;
}

void memWrite(uint16_t dst, const byte* src, uint16_t numBytes) {
  SPI.transfer(byte(Command::MEM_WRITE));
  SPI.transfer(hi(dst));
  SPI.transfer(lo(dst));
  SPI.transfer(hi(numBytes));
  SPI.transfer(lo(numBytes));
  for (uint16_t i=0; i<numBytes; i++) {
    SPI.transfer(src[i]);
  }
}

void memRead(uint16_t src, byte* dst, uint16_t numBytes) {
  SPI.transfer(byte(Command::MEM_READ));
  SPI.transfer(hi(src));
  SPI.transfer(lo(src));
  SPI.transfer(hi(numBytes));
  SPI.transfer(lo(numBytes));
  for (uint16_t i=0; i<numBytes; i++) {
    dst[i] = SPI.transfer(0);
  }
}

void valueWrite(uint16_t id, uint16_t value) {
  SPI.transfer(byte(Command::VALUE_WRITE));
  SPI.transfer(hi(id));
  SPI.transfer(lo(id));
  SPI.transfer(hi(value));
  SPI.transfer(lo(value));
}

uint16_t valueRead(uint16_t id) {
  SPI.transfer(byte(Command::VALUE_READ));
  SPI.transfer(hi(id));
  SPI.transfer(lo(id));
  uint8_t valueHi = SPI.transfer(0);
  uint8_t valueLo = SPI.transfer(0);
  
  uint16_t value = (uint16_t(valueHi) << 8) | uint16_t(valueLo);
  
  return value;
}

void enableChipSelect(bool isEnabled) {
  digitalWrite(pin_spi_cs_n, isEnabled ? LOW : HIGH);
}

///////////////////////////////////////////////////////////////////////////
// SPI startup

void syncSPI() {
  // repeatedly try to echo some values from SPI until successful
  // - if you see this failing, then probably need to reset the FPGA

  bool hasSynchronised = false;

  Serial.println("syncSPI - begin");

  while (!hasSynchronised) {
    enableChipSelect(true);
    
    nop();
    
    byte values[3] = { 43, 99, 245 };

    bool hasFailed = false;

    for (int i=0; i<3; i++) {
      byte value = values[i];
      byte returned = echo(value);
      
      if (value != returned) {
        Serial.print("syncSPI failed: echo ");
        Serial.print(value);
        Serial.print(" != ");
        Serial.println(returned);
      
        hasFailed = true;
        break;
      }
    }

    if (hasFailed) {
      delay(1000);
    } else {
      hasSynchronised = true;
    }

    enableChipSelect(false);
  }

  
  Serial.println("syncSPI - complete");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Test Design

void testMemory() {
  // echo
  const byte kEchoSequence[3] = { 43, 99, 245 };

  for (int i=0; i<3; i++) {
    const byte value = kEchoSequence[i];
    byte returned = echo(value);
    Serial.print("echo ");
    Serial.print(value);
    Serial.print(" => ");
    Serial.println(returned);
  }

  // mem write
  const uint16_t kDataSize = 3;
  const byte data[kDataSize] = { 0xAA, 0xBB, 0xCC };
  memWrite(0xABCD, data, kDataSize);

  // mem read 
  byte readData[kDataSize];
  memRead(0xABCD, readData, kDataSize);

  Serial.print("Memory Read: ");
  for (uint16_t i=0; i < kDataSize; i++) {
    Serial.print(readData[i], HEX);
    Serial.print(", ");
  }
  Serial.println("");
}

// works with simulated Values.v - just reading/writing registers 
void testValues() {
  // Value write
  const uint16_t kNumValues = 16;
  const uint16_t values[kNumValues] = { 0xABCD, 0x1234, 0x2345, 0x0987, 0x2345, 0xABDE, 0xFEDC, 0x9864,
                                        0xFEDC, 0x4321, 0x5432, 0x9873, 0x2454, 0xADEF, 0xFEDC, 0x5634 };

  for (uint16_t i=0; i<kNumValues; i++) {
    valueWrite(i, values[i]);
  }

  Serial.print("Values Read: ");
  int numValuesIncorrect = 0;
  for (uint16_t i=0; i<kNumValues; i++) {
    uint16_t value = valueRead(i);
    Serial.print(value, HEX);
    Serial.print(", ");
    if (value != values[i]) {
      numValuesIncorrect += 1;
    }
  }
  Serial.println("");
  if (numValuesIncorrect > 0) {
    Serial.print("VALUES INCORRECT!! x ");
    Serial.println(numValuesIncorrect);
  } else {
    Serial.println("Values are all correct :)");
  }
}

enum ValueID : uint16_t {
  VALUEID_CPU_STEP = 1,
  VALUEID_CPU_RESET_N = 14,

  VALUEID_CPU_ADDRESS = 2,
  VALUEID_CPU_DATA = 3,
  VALUEID_CPU_RW = 4,
  VALUEID_CPU_IRQ_N = 5,
  VALUEID_CPU_NMI_N = 6,
  VALUEID_CPU_SYNC = 7,
  VALUEID_CPU_REG_A = 8,
  VALUEID_CPU_REG_X = 9,
  VALUEID_CPU_REG_Y = 10,
  VALUEID_CPU_REG_S = 11,
  VALUEID_CPU_REG_P = 12,
  VALUEID_CPU_REG_IR = 13
};

void stepCPU() {
  //Serial.println("starting step");
  
  valueWrite(VALUEID_CPU_STEP, 1);

  bool hasFinishedStep = false;
  while (!hasFinishedStep) {
    uint16_t value = valueRead(VALUEID_CPU_STEP);
    if (value == 0) {
      hasFinishedStep = true;
    } else {
      Serial.println("waiting for step to complete");
      delay(10);
    }
  }

  //Serial.println("step complete");
}

void readState(uint16_t valueid, const char* label) {
  uint16_t value = valueRead(valueid);

  Serial.print(label);
  Serial.print(": ");
  Serial.println(value, HEX);
}

void readStateCPU() {
#if 0
  // dump all registers to TTY
  readState(VALUEID_CPU_ADDRESS, "ADDRESS");
  readState(VALUEID_CPU_DATA, "DATA");
  readState(VALUEID_CPU_RW, "RW");
  readState(VALUEID_CPU_IRQ_N, "IRQ_N");
  readState(VALUEID_CPU_NMI_N, "NMI_N");
  readState(VALUEID_CPU_SYNC, "SYNC");
  readState(VALUEID_CPU_REG_A, "A");
  readState(VALUEID_CPU_REG_X, "X");
  readState(VALUEID_CPU_REG_Y, "Y");
  readState(VALUEID_CPU_REG_S, "S");
  readState(VALUEID_CPU_REG_P, "P");
  readState(VALUEID_CPU_REG_IR, "IR");
#else
  uint16_t address = valueRead(VALUEID_CPU_ADDRESS);
  uint16_t data = valueRead(VALUEID_CPU_DATA);
  uint16_t rw = valueRead(VALUEID_CPU_RW);
  uint16_t sync = valueRead(VALUEID_CPU_SYNC);
  uint16_t ir = valueRead(VALUEID_CPU_REG_IR);
  uint16_t a = valueRead(VALUEID_CPU_REG_A);
  uint16_t x = valueRead(VALUEID_CPU_REG_X);
  uint16_t y = valueRead(VALUEID_CPU_REG_Y);
  uint16_t s = valueRead(VALUEID_CPU_REG_S);
  uint16_t p = valueRead(VALUEID_CPU_REG_P);
  
  const uint16_t RW_READ = 1;

# if 0
  // original CPU state reporting
  char output[128];
  sprintf(output, "%04x %c %02x IR:%02x %s", address, (rw == RW_READ) ? 'R' : 'W', data, ir, (sync == 1) ? "SYNC" : "    " );
  Serial.print(output);
  sprintf(output, " - registers A:0x%02X X:0x%02X Y:0x%02X S:0x%02X P:", a, x, y, s);
  Serial.print(output);

  for (int i=0; i<8; i++) {
    uint16_t bitmask = 1 << (7 - i);
    output[i] = ((p & bitmask) != 0) ? '1' : '0';
  }
  output[8] = 0;  
  Serial.println(output);

# else
    // nestest formatting

    // note: we need to let fetch cycle finish executing so that we can get a good snapshot of cpu 
    //       because some opcodes finish executing in next opcode's fetch cycle
    static uint16_t last_sync = 0;
    static uint16_t last_address = 0;

    if (last_sync == 1) {
      char output[128];
      sprintf(output, "%04X  %02X                                        A:%02X X:%02X Y:%02X P:%02X SP:%02X", last_address, ir, a, x, y, p, s);
      Serial.println(output);
    }

    last_sync = sync;
    last_address = address;
# endif

#endif
}

void fillMemory(byte value) {
  const int kBufferSize = 128;
  byte buffer[kBufferSize];
  memset(buffer, value, kBufferSize);
  for (int i=0; i<0x10000; i+= kBufferSize) {
    memWrite(i, buffer, kBufferSize);
  }
}

void resetCPU() {
  valueWrite(VALUEID_CPU_RESET_N, 0);
  stepCPU();
  stepCPU();
  valueWrite(VALUEID_CPU_RESET_N, 1);
}

///////////////////////////////////////////////////////////////////////////
// Arduino lifecycle

void setup() {
  Serial.begin(9600);
  Serial.println("6502 CPU Debugger");
  
  pinMode(pin_spi_cs_n, OUTPUT);
  digitalWrite(pin_spi_cs_n, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));

  syncSPI();

  enableChipSelect(true);
  resetCPU();
  fillMemory(0xEA);     // NOP

  // Ben Eater 6502 test program
  /*
  byte program[] = {
    0xa9, 0xff,         // LDA #$ff
    0x8d, 0x02, 0x60,   // STA $6002
    
    0xa9, 0x55,         // LDA #$55
    0x8d, 0x00, 0x60,   // STA $6000
    
    0xa9, 0xaa,         // LDA #$aa
    0x8d, 0x00, 0x60    // STA $6000
  };
  memWrite(0x8000, program, 15);

  
  byte resetVector[] = {
      0x00, 0x80
  };
  memWrite(0xfffc, resetVector, 2);
  */

  // nes test
  memWrite(0xc000, prg_rom_bank_0_6502_bin, 0x3FFF);

  byte resetVector[] = {
    0x00, 0xc0
  };
  memWrite(0xfffc, resetVector, 2);

  
  enableChipSelect(false);
}


void loop() {
  enableChipSelect(true);

  stepCPU();
  readStateCPU();
  
  enableChipSelect(false);

  //delay(1000);
}
