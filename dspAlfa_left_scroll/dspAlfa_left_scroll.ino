// display alphabet pattern for control Max7219 LED module
// which has 4 set of modules
// Just on off control program
// made by Vaphio @ Oct.6, 2026

#include "hankaku_v.h"  // alfa character pattern code

int SPI_MOSI = 12;
int SPI_CLK = 11;
int SPI_CS = 10;
const int devices = 4;    // No. of modules
const int dotWidth = 8;
byte dspReg[devices*dotWidth];    // storage of display data each 8 bytes x 4 modules
byte charCode[dotWidth];   //storage of character pattern

const char msg[] = "Hello, World! ";
const int msgSize = sizeof(msg) / sizeof(msg[0]) -1;
const long codeSize = msgSize * charWidth;
byte strCode[256];

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
    getStrCode(strCode);
  }
  Serial.begin(9600);
}

void loop() {
//  byte strCode[codeSize];
  byte buf[dotWidth];
//  clearCode(strCode, codeSize);
  clearCode(buf, dotWidth);
//  getStrCode(strCode);    // transfer msg[] to strCode[]
//  prnCode(strCode);

  for (int i=0; i<devices; i++) {
    cutCode(strCode, i, buf);
    rotCode(buf);
    setLED(i, buf);
    delay(10);
  }
  shiftCode(strCode);

  delay(50);
}

void clearLED(int device) {
  int offset;
  offset = device*8;
  for (int i=0; i<8; i++) {
    dspReg[offset+i] = 0x00;
    sendCmd(device, i+1, dspReg[offset+i]);
  }
}

void prnCode(byte* code) {
  int size = sizeof(code) / sizeof(code[0]);
  for (int i=0; i<size; i++) {
    Serial.println(code[i], BIN);
  }
}

void clearCode(byte* code, int size) {
  for (int i=0; i<size; i++) {
    code[i] = 0x00;
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
}

void getStrCode(byte* code) {
  byte buf[charWidth];
  int offset = 0;
  for (int i=0; i<msgSize; i++) {
    getcharCode(buf, msg[i]);
    for (int j=0; j<charWidth; j++) {
      code[j+offset] = buf[j];
    }
    offset += charWidth;
  }
}

void cutCode(byte* code, int index, byte* ccode) {
  for (int i=0; i<8; i++) {
    ccode[i] = code[i + index*8];
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

void shiftCode(byte* code) {
  byte stack;
  stack = code[0];
  for (int i=0; i<codeSize-1; i++) {
    code[i] = code[i+1];
  }
  code[codeSize-1] = stack;
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