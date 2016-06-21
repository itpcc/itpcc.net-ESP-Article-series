#define INTRO_KEY    "*-*-*"
#define INTRO_LENGTH 5
#define OUTRO_KEY    "=-=-="
#define OUTRO_LENGTH 5

#include <SPI.h>
#include <nRF24L01p.h>

nRF24L01p receiver(7,8);//CSN,CE
String message;

#include <IRremote.h>
#include "psi_key.h"
IRsend irsend;

void setup(){
  delay(150);
  Serial.begin(115200);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  receiver.channel(90);
  receiver.RXaddress("IRPSI");
  receiver.init();
}

void sendIRCode(uint16_t code){
  delay(100);
  irsend.sendNEC(B_prefix | (unsigned long) code, 32);
}

void sendIR(char cmd){
  switch(toupper(cmd)){
    case 'E': sendIRCode(PSIRemoteCode[0]); break;
    case 'O': sendIRCode(PSIRemoteCode[1]); break;
    case '0': sendIRCode(PSIRemoteCode[2]); break;
    case '1': sendIRCode(PSIRemoteCode[3]); break;
    case '2': sendIRCode(PSIRemoteCode[4]); break;
    case '3': sendIRCode(PSIRemoteCode[5]); break;
    case '4': sendIRCode(PSIRemoteCode[6]); break;
    case '5': sendIRCode(PSIRemoteCode[7]); break;
    case '6': sendIRCode(PSIRemoteCode[8]); break;
    case '7': sendIRCode(PSIRemoteCode[9]); break;
    case '8': sendIRCode(PSIRemoteCode[10]); break;
    case '9': sendIRCode(PSIRemoteCode[11]); break;
    case 'P': sendIRCode(PSIRemoteCode[12]); break;
    case 'U': sendIRCode(PSIRemoteCode[13]); break;
    case 'D': sendIRCode(PSIRemoteCode[14]); break;
    case 'L': sendIRCode(PSIRemoteCode[15]); break;
    case 'R': sendIRCode(PSIRemoteCode[16]); break;
    case 'I': sendIRCode(PSIRemoteCode[17]); break;
    case 'M': sendIRCode(PSIRemoteCode[18]); break;
    case 'S': sendIRCode(PSIRemoteCode[19]); break;
  }
}

void sendIRBatch(String str){
  int i, strLen;
  char currCmd;
  str.trim();
  strLen = str.length();
  sendIR('E');
  for(i=0; i < strLen; i++){
    currCmd = str.charAt(i);
    sendIR(currCmd);
  }
  if(currCmd >= '0' && currCmd <= '9') sendIR('O');
}

void loop() {
	int introPos, outroPos;
	if(receiver.available()){
		receiver.read();
		receiver.rxPL(message);
		Serial.println(message);
		introPos = message.indexOf(INTRO_KEY);
		outroPos = message.indexOf(OUTRO_KEY, introPos+1);
		if(introPos > -1 && outroPos > introPos){
			sendIRBatch(message.substring(introPos+INTRO_LENGTH, outroPos-1));
		}
		message="";
	}
}
