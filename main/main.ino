#include <Wire.h>
#include <ESP8266WiFi.h>

#include "com.h"
#include "sensors.h"
#include "functions.h"
#include "pwds.h"




//**************************************************************//
//                 Gloabals declarations                        //
//**************************************************************//
#define DEBUG true
#define SERVER_PORT 23
#define GPIO_LED_WIFI_CONNECTED 5  
#define GPIO_LED_ALARM 4  

String readTemps();
String readStates();
void checkStates();


//**************************************************************//
//                Gloabals Variables                            //
//**************************************************************//

WiFiServer server(SERVER_PORT);
WifiCom com;

SimulatorSensor st1(2.0,1.,0.,5.);
BasicSensor st2;

BasicSensor* sensors[] = {&st1,&st2};
int nr_sensors = 2;



//**************************************************************//
//                Setup Method                                  //
//**************************************************************//

void setup() {
  bool wifi_searching = true;
  
  // Prepare logger for debug 
  if (DEBUG){
    Serial.begin(115200);
    delay(50);
    Serial.print('Connecting to: ');
    Serial.println(ssid);

  }
  // Configure LED_WIFI
  pinMode(GPIO_LED_WIFI_CONNECTED, OUTPUT);
  digitalWrite(GPIO_LED_WIFI_CONNECTED, LOW);
  
  // Connect to the Wifi
  // TODO: Activate Wifi LED
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  while(true){
    if (i++ < 20){ // Nr of intents 
      if (WiFi.status() == WL_CONNECTED) break;
      //digitalWrite(GPIO_LED_WIFI_CONNECTED, HIGH);
      delay(500);
      //digitalWrite(GPIO_LED_WIFI_CONNECTED, LOW); 
    } 
 
  }
  //digitalWrite(GPIO_LED_WIFI_CONNECTED, HIGH);

  //Start Telnet server
  server.begin();
  server.setNoDelay(true);
  if (DEBUG){
    Serial.print("Ready!\nUse 'telnet ");
    Serial.print(WiFi.localIP());
    Serial.println(" 23' to connect");
  }
  pinMode(GPIO_LED_ALARM, OUTPUT);
  digitalWrite(GPIO_LED_ALARM, LOW);
    
}

//**************************************************************//
//                Main Loop Method                              //
//**************************************************************//

void loop() { 
  bool has_client = false;
  checkStates();

  if (server.hasClient()){
    WiFiClient tmp = server.available();
    com = tmp;
    tmp.stop();
    has_client = true;
  }
  
  
  while(has_client){
    if (com.isConnected()){
      Cmds cmd = com.getCommand();
      if (cmd != NONE)
        switch (cmd){
          case TEMP: 
            com.sendData(readTemps());
            break;
          case CONFIG: 
            com.sendData(String("Not implemented.\n")); 
            break;
          case DI: 
            com.sendData(String("DI1 High; DI2 Low;\n")); 
            break;
          case DO:
            com.sendData(String("Not implemented\n"));
            break;
          case STATE:
            com.sendData(readStates());
            break;           
        }
    }
    else has_client = false;
   }
   
}


//**************************************************************//
//                Functions                                     //
//**************************************************************//

String readTemps(){
  String str_buff;
  StringStream buff = StringStream(str_buff);
  double value;
  for(int i=0; i< nr_sensors; i++){
    buff.print("T");
    buff.print(i+1,DEC);
    buff.print(":");
    value = sensors[i]->getValue();
    buff.print(value, DEC);
    buff.print(";");
    
  }
  buff.print("\n");
  return str_buff;
}

//**************************************************************//
String readStates(){
  String str_buff;
  StringStream buff = StringStream(str_buff);
  for(int i=0; i< nr_sensors; i++){
    buff.print("T");
    buff.print(i+1,DEC);
    buff.print(":");
    const char* value = sensors[i]->isOutOfRange()? "ALARM":"OK";
    buff.print(value);
    buff.print(";");
    
  }
  buff.print("\n");
  return str_buff;
}
//**************************************************************//
void checkStates(){
  digitalWrite(GPIO_LED_ALARM, LOW);
  
  for(int i=0; i< nr_sensors; i++){
    if (sensors[i]->isOutOfRange()){
      if(DEBUG){
        Serial.print("T");
        Serial.print(i+1, DEC);
        Serial.println("OutOfRange");
        Serial.print(readTemps());
      }
      //digitalWrite(GPIO_LED_ALARM, HIGH);
      break;
    } 
  }
}




