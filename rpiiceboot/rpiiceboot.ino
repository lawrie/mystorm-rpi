
#include <MyStorm.h>
#include <SPI.h>

/*
 * FPGA configuration bitstream built from fpga/readswitches.v
 * by icestorm toolchain (see fpga/makefile).
 */
const byte bitstream[] = {
#include "ice40bitstream.h"
};

const byte syncBytes[] = { 0x7E, 0xAA, 0x99, 0x7E };
// Length of a bitstream file (excluding empty initial comment and marker)
const int bitstreamBodyLength = 135100 - 8;

bool configured;  // true if FPGA has been configured successfully

void setup() {
  Serial1.begin(115200);
  // configure the FPGA
  configured = myStorm.FPGAConfigure(bitstream, sizeof bitstream);
  if (configured) {
    pinMode(PIN_ICE40_CRST, INPUT);
    pinMode(PIN_SPI_SS, INPUT);
    myStorm.muxSelectPi();
  }
}

void loop() {
  if (Serial1.available() > 0) {
    int cmd = Serial1.read();

    if (cmd == 'p') {
      Serial1.println("Raspberry Pi mode");
      pinMode(PIN_ICE40_CRST, INPUT);
      pinMode(PIN_SPI_SS, INPUT);
      myStorm.muxSelectPi();
    }  else if (cmd == 'l') {
      Serial1.println("LED mode");
      myStorm.muxSelectLeds();
    } else if (cmd == 'n') {
      myStorm.muxDisable();
    } else if (cmd == 'c' || cmd == 0xFF) {
        if (cmd != 0xFF) {
          Serial1.println("Waiting for bin file");
          while (Serial1.available()) Serial1.read();
          while (!Serial1.available());
        }
        Serial1.println("Starting configure");
        digitalWrite(LED_BUILTIN, 1);
        if (myStorm.FPGAConfigure(Serial1)) {
          Serial1.println("Configure sucessful");
        } else {
          Serial1.println("Configure failed "); 
        }
        while (Serial1.available()) Serial1.read();
        myStorm.muxSelectLeds(); 
        digitalWrite(LED_BUILTIN, 0);
    } else if (cmd != '\n') {
      Serial1.println("Command not recognized");
    }
    Serial1.flush();
  }
}

