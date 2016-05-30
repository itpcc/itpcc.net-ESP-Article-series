#include <AntoIO.h>
#include <dht11.h>
#include "TM1637_custom.h"
#define DHT11PIN 12
#define GPIO15 15
#define LED_CHANGE_STATE_EVENT 7

dht11 DHT11;
tm1637_custom display(4, 5);

uint8_t displayAlpha[3][4] = {
  { 
    //XGFEDCBA
    0b01111000, //t
    0b01111001, //E
    0b01010100, //n
    0b01110011  //P
  },{
    //XGFEDCBA
    0b00000000, // 
    0b01110110, //H
    0b00111110, //U
    0b01011110  //d
  },{
    //XGFEDCBA
    0b00111000, //L
    0b01111001, //E
    0b01011110, //d
    0b00000000, // 
  }
};

// change ssid and password to yours
const char* ssid     = "YOUR_SSID";
const char* password = "SSID_PW";

// use demo token or change to your token
const char* token = "ANTO_TOKEN";
// use demo thing or change to your thing
const char* thing = "ANTO_THING_ID";
// use demo username or change to your username
const char *user = "ANTO_USERNAME";
bool bIsConnected = false;

int32_t tempInt = 0;

unsigned long lastDisplay7Seg = 0;
unsigned long lastRetriveDHT  = 0;
short display7SegMode = 0;

bool LEDtest1State = false;

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  Serial.println("ok");

  Serial.println("DHT11 TEST PROGRAM ");
  Serial.print(  "LIBRARY VERSION: "  );
  Serial.println( DHT11LIB_VERSION    );

  //port output
  pinMode(GPIO15,OUTPUT);

  // register callback functions

  Anto.onConnected(connectedCB);
  Anto.onDisconnected(disconnectedCB);
  Anto.onMsgArrv(msgArrvCB);
  Anto.onPublished(publishedCB);
    
  // Connect to a WiFi network and MQTT broker
  if (!Anto.begin(ssid, password, token, user, thing)) {
    Serial.println("Connection failed!!");
    while (1);
  }

  Serial.println("WiFi connected & Connecting to MQTT broker");
  display.setBrightness(4);
  //                       XGFEDCBA
  display.writeCustom(0, 0b010100000); //r
  display.writeCustom(1, 0b00111110);//U
  display.writeCustom(2, 0b01010100);//n
  display.writeCustom(3, 0b00001000);//_
  delay(2000);
}

void display7seg(){
  int i;
  switch (display7SegMode) {
    case 0: // "TEMP"
    case 2: // " HUd"
      for(i=0; i < 4; i++){
        display.writeCustom(i, displayAlpha[display7SegMode==0?0:1][i]);
      }
    break;
    case 1: // Temperature
    case 3: // Humidity
      i = int(float(display7SegMode==1?DHT11.temperature:DHT11.humidity)*100.0)%10000;
      display.writeTime(int(i/100)%100, i%100, true);
    break;
    case LED_CHANGE_STATE_EVENT:
      for(i=0; i < 3; i++){
        display.writeCustom(i, displayAlpha[2][i]);
      }
      display.writeDigit(3, LEDtest1State?1:0);

    break;
  }  
}

void loop(){
  // put your main code here, to run repeatedly:
  // Wait a few seconds between measurements.
  if(millis() - lastRetriveDHT > 2000){
    Serial.print("Read sensor: ");
    switch (DHT11.read(DHT11PIN)){
      case 0: 
        Serial.print("OK-> ");
        Serial.print("Hud: ");
        Serial.print((float)DHT11.humidity, 2);
      
        Serial.print("   Temp: ");
        Serial.print((float)DHT11.temperature, 2);
        tempInt = (float)DHT11.temperature*100.0;
        Serial.print(" -->"); Serial.print(tempInt);
        //Serial.print(Anto.requestHttp("iotfahsai.itpcc.net", "/DHT11/"+String(tempInt)+"/?millis="+String(millis())));
        Anto.pub("DHT11_temperature", (const char*)String(tempInt).c_str());
        
        Serial.println("");
      break;
      case -1: Serial.println("Checksum error"); break;
      case -2: Serial.println("Time out error"); break;
      default: Serial.println("Unknown error"); break;
    }
    lastRetriveDHT = millis();
  }

  if(millis() - lastDisplay7Seg > 2000){
    display7seg();
    lastDisplay7Seg = millis();
    display7SegMode = (display7SegMode > 3)?0:display7SegMode+1;
  }
}

/*
 * connectedCB(): a callback function called when the connection to the MQTT broker is establised.
 */
void connectedCB(){
    // If the connection is establised, subscribe channels.

    bIsConnected = true;
    Serial.println("Connected to MQTT Broker");
    
    Anto.sub("LEDtest1");
}

/*
* disconnectedCB(): a callback function called when the connection to the MQTT broker is broken.
*/
void disconnectedCB(){
    bIsConnected = false;
    Serial.println("Disconnected to MQTT Broker");
}

/*
 * msgArrvCB(): a callback function called when there a message from the subscribed channel.
 */
void msgArrvCB(String& topic, String& msg){
    uint8_t index = topic.indexOf('/');

    Serial.println("");
    Serial.print(topic);
    Serial.print(" -> :");

    index = topic.indexOf('/', index + 1);
    index = topic.indexOf('/', index + 1);

    topic.remove(0, index + 1);
   
    Serial.print(topic);
    Serial.print(": ");
    Serial.print(msg);

    if(topic.equals("LEDtest1")){
      if(msg.toInt() == 1){
        Serial.println(" ===> LIGHT ON");
        digitalWrite(GPIO15,HIGH);
        if(LEDtest1State == false){
          lastDisplay7Seg = millis() - 2000;
          display7SegMode = LED_CHANGE_STATE_EVENT;
          LEDtest1State = true;
        }
      }else{
        Serial.println(" ---> LIGHT OFF");
        digitalWrite(GPIO15,LOW);
        if(LEDtest1State == true){
          lastDisplay7Seg = millis() - 2000;
          display7SegMode = LED_CHANGE_STATE_EVENT;
          LEDtest1State = false;
        }
      }      
    }else{
      Serial.println("");
    }
}

/*
* publishedCB(): a callback function called when the message is published.
*/
void publishedCB(void){
  Serial.println("published");
}

