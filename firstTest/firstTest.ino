// This is the first test for control Max7219 LED module
// which has 4 set of modules
// Just on off control program
// made by Vaphio @ Oct.6, 2026

#include "hankaku_v.h"  // alfa character code

int SPI_MOSI = 12;
int SPI_CLK = 11;
int SPI_CS = 10;
int devices = 4;    // No. of modules
byte dspReg[32];    // storage of display data each 8 bytes x 4 modules

void setup() {
  pinMode(SPI_MOSI, OUTPUT);
  pinMode(SPI_CLK, OUTPUT);
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);

  for (int i=0; i<devices; i++) {
    sendCmd(i, 0x0F, 0x00);     //display test mode : normal operation
    sendCmd(i, 0x0B, 0x07);     //scan limit : 0~7 all scan
    sendCmd(i, 0x09, 0x00);     //decode mode : no decode
    sendCmd(i, 0x0C, 0x01);     //shutdown mode : Normal Operation
    sendCmd(i, 0x0A, 0x08);     //intensity mode : level 8

    clearLED(i);
  }
  Serial.begin(9600);
}

void loop() {
  for (int i=0; i<devices; i++) {
    fillLED(i);
    delay(1000);
    clearLED(i);
    delay(1000);
  }
}

void clearLED(int device) {
  int offset;
  offset = device*8;
  for (int i=0; i<8; i++) {
    dspReg[offset+i] = 0x00;
    sendCmd(device, i+1, dspReg[offset+i]);
  }
}

void fillLED(int device) {
  int offset;
  offset = device*8;
  for (int i=0; i<8; i++) {
    dspReg[offset+i] = 0xFF;
    sendCmd(device, i+1, dspReg[offset+i]);
  }
}

void sendCmd(int device, byte cmd, byte data) {
  int offset = device*2;      // set module no. 0 ~ 3 from i/f side
  int maxdev = devices*2;
  byte spidata[maxdev];

  for (int i=0; i<maxdev; i++) {
    spidata[i] = 0x00;
  }

  spidata[offset] = data;
  spidata[offset+1] = cmd;

  digitalWrite(SPI_CS, LOW);

  for (int i=maxdev; i>0; i--) {
    shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, spidata[i-1]);
  }
  digitalWrite(SPI_CS, HIGH);
}