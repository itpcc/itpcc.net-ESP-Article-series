#include <SPI.h>
#include <nRF24L01p.h>
#include <IRremote.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include "sony_key.h"
#include "char_display.h"


nRF24L01p transmitter(7,8);//CSN,CE

IRrecv irrecv(A0);
decode_results results;

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
TM1637Display display(CLK, DIO);

String message;
String remoteKey;
int remoteKeyLen = 0;
unsigned long lastPushRemote = 0;

void display7Seg(){
  uint8_t data[1], displayPos = 3;
  char currChar;
  if(remoteKeyLen > 0){
    Serial.print("7SEG ->\t");
    for(int i = remoteKeyLen-1; i >= 0 && displayPos < 4; i--){
      currChar = toupper(remoteKey.charAt(i));
      switch (currChar) {
          case 'E': data[0] = displayChar[0]; break;
          case 'P': data[0] = displayChar[2]; break;
          case 'U': data[0] = displayChar[3]; break;
          case 'D': data[0] = displayChar[4]; break;
          case 'L': data[0] = displayChar[5]; break;
          case 'R': data[0] = displayChar[6]; break;
          case 'I': data[0] = displayChar[7]; break;
          case 'M': data[0] = displayChar[8]; break;
          case 'S': data[0] = displayChar[9]; break;
          case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            display.showNumberDec(currChar - '0', true, 1, displayPos);
            data[0] = 0x80;
          break; 
          default : data[0] = 0x00;                          break;
      }
      if(data[0] != 0x80){
          Serial.print(String("(")+i+")"); Serial.print(data[0], HEX); Serial.print(" ");
          display.setSegments(data, 1, displayPos);
      }
      displayPos--;
    }
  }
  Serial.println(":+:");

  if(displayPos > 0 && displayPos < 4){
    data[0] = 0x00;
    for(; displayPos >= 0; displayPos--){
        display.setSegments(data, 1, displayPos);
        if(displayPos == 0) break;
    }
  }
}

void clear7seg(){
  const uint8_t data[4] = { SEG_D, SEG_D, SEG_D, SEG_D }; // underscore
  display.setBrightness(0x0f);
  display.setSegments(data, 4, 0);
}

int findRemoteKey(unsigned long address){
  for(int i = 0; i < 32; i++){
    //Serial.print("KEY :"); Serial.print(address, HEX); Serial.print(':'); 
    //Serial.println(RemoteCode[i], HEX);
    if(address == RemoteCode[i]){
      Serial.println(String("FOUND AT ")+i);
      return i;
    }
  }
  Serial.println("NOT FOUND!");
  return -1;
}

void clearRemoteBuffer(){
  remoteKey = "";
  remoteKeyLen = 0;
  clear7seg();
  Serial.println("CLEAR!");
}

void sendRemoteKey(){
  char lastCmd = remoteKey.charAt(remoteKeyLen -1);
  if(millis() - lastPushRemote < 500)
    return;
  if(
    remoteKeyLen == 0 || 
    (lastCmd >= '0' && lastCmd <= '9') || 
    lastCmd == 'M' || 
    lastCmd == 'P' || 
    lastCmd == 'I' || 
    lastCmd == 'S' || 
    lastCmd == 'I' || 
    lastCmd == 'R' || 
    lastCmd == 'L' || 
    lastCmd == 'U' || 
    lastCmd == 'D'
  )
    remoteKey += lastCmd;
  transmitter.txPL(String("*-*-*")+remoteKey+String("=-=-="));
  transmitter.send(SLOW);
  Serial.print("SEND! ->");
  Serial.println(String("*-*-*")+remoteKey+String("=-=-="));
  clearRemoteBuffer();
  lastPushRemote += 1000;
}

void appendRemoteKey(char key){
  if((key == 'U' || key == 'D') && remoteKeyLen == 0){
    remoteKey = key;
    remoteKeyLen = 1;
    sendRemoteKey();
    clearRemoteBuffer();
  }else{
    remoteKey += key;
    remoteKeyLen++;
    Serial.println(remoteKey);
    display7Seg();
    Serial.println("OK!");
  }
}

char popRemoteKey(){
  char lastCmd;
  if(remoteKeyLen == 0) return '\0';
  remoteKeyLen--;
  lastCmd = remoteKey.charAt(remoteKeyLen);
  remoteKey.remove(remoteKeyLen);
  display7Seg();
  Serial.print("OK! ->"); Serial.println(lastCmd);
  return lastCmd;
}

void setup(){
  delay(150);
  Serial.begin(115200);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  transmitter.channel(90);
  transmitter.TXaddress("IRPSI");
  transmitter.init();

  irrecv.enableIRIn(); // Start the receiver

  clear7seg();
}

void loop(){
  int isReceiveRemote;
  if(Serial.available()>0){
    char character=Serial.read();
    if(character=='\n'){
      transmitter.txPL(String("*-*-*")+message+String("=-=-="));
      transmitter.send(SLOW);
      Serial.println(String("*-*-*")+message+String("=-=-="));
      message="";
    }else{
      message+=character;
    }
  }else{
    if(irrecv.decode(&results) && results.decode_type == SONY) {
      Serial.print("Remote Received -> "); Serial.println(results.value, HEX);
      if(millis() - lastPushRemote > 400){
        switch (findRemoteKey(results.value)) {
            case 20: clearRemoteBuffer(); break;
            case 1: sendRemoteKey();  break;
            case 2:  appendRemoteKey('0');  break;
            case 3:  appendRemoteKey('1');  break;
            case 4:  appendRemoteKey('2');  break;
            case 5:  appendRemoteKey('3');  break;
            case 6:  appendRemoteKey('4');  break;
            case 7:  appendRemoteKey('5');  break;
            case 8:  appendRemoteKey('6');  break;
            case 9:  appendRemoteKey('7');  break;
            case 10: appendRemoteKey('8');  break;
            case 11: appendRemoteKey('9');  break;
            case 13: appendRemoteKey('U');  break;
            case 14: appendRemoteKey('D');  break;
            case 15: appendRemoteKey('R');  break;
            case 16: appendRemoteKey('L');  break;
            case 12: appendRemoteKey('E');  break;
            case  0: appendRemoteKey('P');  break; //Pause
            case 17: appendRemoteKey('I');  break; //Power
            case 18: appendRemoteKey('M');  break;
            case 19: appendRemoteKey('S');  break;
            case 21: popRemoteKey(); break;
        }
        lastPushRemote = millis();
      }
      Serial.println("Receive end!");
      irrecv.resume(); // Receive the next value
      Serial.println("Receive Resume");
    }
  }
}
