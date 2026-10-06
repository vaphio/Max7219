// display alphabet pattern for control Max7219 LED module
// which has 4 set of modules
// Just on off control program
// made by Vaphio @ Oct.6, 2026

#include "hankaku_v.h"  // alfa character pattern code

int SPI_MOSI = 12;
int SPI_CLK = 11;
int SPI_CS = 10;
int devices = 4;    // No. of modules
byte dspReg[32];    // storage of display data each 8 bytes x 4 modules
byte charCode[8];   //storage of character pattern

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
  char msg[] = "AFGT";
  for (int i=0; i<devices; i++) {
    getcharCode(charCode, msg[i]);
    rotCode(charCode);
    setLED(i, charCode);
    delay(10);
  }
  delay(1000);
  for (int i=0; i<devices; i++) {
    clearLED(i);
  }
  delay(1000);
  int num = 4567;
  for (int i=0; i<100; i++) {
    set4dig(num++);
    delay(100);
  }
  delay(1000);
  for (int i=0; i<256; i++) {
    set4hex(num++);
    delay(100);
  }
  delay(1000);
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

void set4dig(int num) {
  int buf;
  for (int i=0; i<devices; i++) {
    buf = num % 10;
    getNumCode(charCode, buf);
    rotCode(charCode);
    setLED(devices-i-1, charCode);
    num = num / 10;
  }
}

void set4hex(int num) {
  int buf;
  for (int i=0; i<devices; i++) {
    buf = num % 16;
    getNumCode(charCode, buf);
    rotCode(charCode);
    setLED(devices-i-1, charCode);
    num = num / 16;
  }
}

void setLED(int device, byte* code) {
  int offset;
  offset = device*8;
  for (int i=0; i<8; i++) {
    dspReg[offset+i] = code[i];
    sendCmd(device, i+1, dspReg[offset+i]);
  }
}

void getcharCode(byte* code, char alfa) {
  for (int i=0; i<charWidth; i++) {
    code[i] = HANV[alfa-codeOffset][i];
  }
  for (int i= charWidth; i<8; i++) {
    code[i] = 0x00;
  }
}

void getNumCode(byte* code, int num) {
  for (int i=0; i<charWidth; i++) {
    if (num<10) {
      code[i] = HANV[num+numOffset][i];
    } else {
      code[i] = HANV[num+numOffset+7][i];
    }
  }
  for (int i=charWidth; i<8; i++) {   //fill blank for more than charWidth
    code[i] = 0x00;
  }
}

void rotCode(byte* code) {      // rotate 90deg of the code
  byte buf[8];
  byte stack;
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      stack = (code[i] >> j) & 0x01;
      if (stack == 1) {
        bitSet(buf[j], i);
      } else {
        bitClear(buf[j], i);
      }
    }
  }
  for (int i=0; i<8; i++) {
    code[i] = buf[i];
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